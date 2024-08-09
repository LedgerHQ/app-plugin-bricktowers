from pathlib import Path
import json
import os

from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
import ledger_app_clients.ethereum.response_parser as ResponseParser
from ledger_app_clients.ethereum.utils import recover_transaction
from ragger.navigator import NavInsID
from ragger.error import ExceptionRAPDU

from .utils import get_appname_from_makefile, DERIVATION_PATH

ROOT_SCREENSHOT_PATH = Path(__file__).parent
ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))
NONCE = 21
GAS_PRICE = 13
GAS = 173290
VALIDATOR_PRICE = 32
PLUGIN_NAME = get_appname_from_makefile()

POOLED_DEPOSIT_SELECTOR = bytes.fromhex("f9609f08")
POOLED_UPDATE_STATE_SELECTOR = bytes.fromhex("1a7ff553")
POOLED_MULTICALL_SELECTOR = bytes.fromhex("ac9650d8")
POOLED_CLAIM_EXITED_SELECTOR = bytes.fromhex("8697d2c2")

with open("%s/0x8871b38b0c3fe47403aac41121376146d086b6b1.abi.json" % (ABIS_FOLDER)) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        # Get address from filename
        address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
    )


def generate_deposit_data(contract, receiver, referrer):
    data = contract.encode_abi("deposit", [
        receiver,
        referrer
    ])
    return data

def generate_claim_exited_assets(contract):
    data = contract.encode_abi("claimExitedAssets", [
        Web3.to_int(1),
        Web3.to_int(2),
        Web3.to_int(3)
    ])
    return data


def generate_update_state_data(contract):
    data = contract.encode_abi(
        fn_name="updateState",
        args=[(
            bytes.fromhex("5688d11893321bbbac4e9921030e9ba9c2f3a75341dcb9c8b74531ec10b1a6c9"),  # rewardsRoot
            Web3.to_int(56911233836981101840),  # reward
            Web3.to_int(310305157213160780),  # unlockedMevReward
            [  # proof
                bytes.fromhex("5a7670cf6edda845d9992770f985d73915e4254528b062cacca4ef324ca65a18"),
                bytes.fromhex("ff47cf6f1001589d6a6f8500db751899de22429d5ffd89ffc24f93d2600840bb"),
                bytes.fromhex("fb49a69bbfb26493a7185580a78dfa565a26599e0a655672ca23d8c93827d2d9"),
                bytes.fromhex("953c2c671f409f63c13244bcdd0dc5060229b1b1f769af1bf4ff6e9ecbaa7e9b"),
                bytes.fromhex("272be4a951bb2b3273aa6275652c0bcb77cafe1835f4b18492b71d38b127829b"),
                bytes.fromhex("b8ffbc39f6957246ad898dcc0ff14fdfcd3b5e7318b8f2114a392dc9ebed37eb")
            ]
        )]
    )
    return data


def generate_multicall_data(contract, transactions):
    data = contract.encode_abi("multicall", transactions)
    return data


def generate_tx_params(data, amount):
    tx_params = {
        "nonce": NONCE,
        "gasPrice": GAS_PRICE,
        "gas": GAS,
        "to": contract.address,
        "value": Web3.to_wei(amount, "ether"),
        "chainId": ChainId.ETH,
        "data": data
    }
    return tx_params


def generate_deposit_transaction(contract, receiver, referrer, amount):
    data = generate_deposit_data(contract, receiver, referrer)
    return generate_tx_params(data, amount)


def sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr, selector):
    # Sign the transaction
    client.set_external_plugin(PLUGIN_NAME, contract.address, selector)

    with client.sign(DERIVATION_PATH, transaction):

        # Navigate and compare depending on the device type
        if firmware.is_nano:
            navigator.navigate_until_text_and_compare(
                NavInsID.RIGHT_CLICK,
                [NavInsID.BOTH_CLICK],
                "Accept",
                ROOT_SCREENSHOT_PATH,
                test_name
            )
        else:
            navigator.navigate_until_text_and_compare(
                NavInsID.SWIPE_CENTER_TO_LEFT,
                [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS],
                "Hold to sign",
                ROOT_SCREENSHOT_PATH,
                test_name
            )

    # Parse the signature and recover the transaction address
    vrs = ResponseParser.signature(client.response().data)
    addr = recover_transaction(transaction, vrs)

    # Assert that the recovered address matches the wallet address
    assert addr == wallet_addr.get()


def test_pooled_deposit(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    address1 = bytes.fromhex("1384a8c0593e4f2faa5a1e47a618ca801abdb9cd")

    transaction = generate_deposit_transaction(contract, wallet_addr.get(), address1, 1.2)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr,
                                POOLED_DEPOSIT_SELECTOR)


def test_pooled_simple_update_state(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    data = generate_update_state_data(contract)

    transaction = generate_tx_params(data, 0)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr,
                                POOLED_UPDATE_STATE_SELECTOR)

# https://holesky.etherscan.io/tx/0xb0d0d48faed9011826efec4e4f3e34221c6986b2e040a37eab8dfb1920a9ad97
def test_pooled_simple_multicall_deposit(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)
    address1 = bytes.fromhex("1384a8c0593e4f2faa5a1e47a618ca801abdb9cd")

    data1 = "0x" + POOLED_UPDATE_STATE_SELECTOR.hex() + generate_update_state_data(
        contract)[2:]
    data2 = "0x" + POOLED_DEPOSIT_SELECTOR.hex() + generate_deposit_data(
        contract, address1, address1)[2:]

    data = contract.encode_abi("multicall", [[data1, data2]])
    transaction = generate_tx_params(data, 1.2)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr,
                                POOLED_MULTICALL_SELECTOR)

def test_pooled_simple_multicall_claim_exit(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    data1 = "0x" + POOLED_UPDATE_STATE_SELECTOR.hex() + generate_update_state_data(
        contract)[2:]
    data2 = "0x" + POOLED_CLAIM_EXITED_SELECTOR.hex() + generate_claim_exited_assets(contract)[2:]

    data = contract.encode_abi("multicall", [[data1, data2]])
    transaction = generate_tx_params(data, 1.2)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr,
                                POOLED_MULTICALL_SELECTOR)
