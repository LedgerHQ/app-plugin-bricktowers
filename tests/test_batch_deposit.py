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

BATCH_DEPOSIT_SELECTOR = bytes.fromhex("ca0bfcce")

with open("%s/0x2384a8c0593e4f2faa5a1e47a618ca801abdb9cd.abi.json" % (ABIS_FOLDER)) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        # Get address from filename
        address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
    )

def generate_batch_deposit_data(contract, withdrawal_credentials, num_validators):
    pubkeys = [os.urandom(48) for _ in range(num_validators)]
    signatures = [os.urandom(96) for _ in range(num_validators)]
    deposit_data_roots = [os.urandom(32) for _ in range(num_validators)]

    data = contract.encode_abi("batchDeposit", [
        pubkeys,
        withdrawal_credentials,
        signatures,
        deposit_data_roots
    ])

    return data

def generate_batch_deposit_transaction(contract, withdrawal_credentials, num_validators):
    data = generate_batch_deposit_data(contract, withdrawal_credentials, num_validators)

    tx_params = {
        "nonce": NONCE,
        "gasPrice": GAS_PRICE,
        "gas": GAS,
        "to": contract.address,
        "value": Web3.to_wei(VALIDATOR_PRICE * num_validators, "ether"),
        "chainId": ChainId.ETH,
        "data": data
    }

    return tx_params

def sign_and_verify_transaction(client, firmware, navigator,  test_name, transaction, wallet_addr):
    # Sign the transaction
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

def reject_transaction(backend, firmware, navigator, test_name, transaction, wallet_addr):
    client = EthAppClient(backend)
    # Sign the transaction
    try:
        with client.sign(DERIVATION_PATH, transaction):

            # Navigate and compare depending on the device type
            if firmware.is_nano:
                navigator.navigate_and_compare(
                    ROOT_SCREENSHOT_PATH,
                    test_name,
                    [NavInsID.BOTH_CLICK]
                )
            else:
                navigator.navigate_and_compare(
                    ROOT_SCREENSHOT_PATH,
                    test_name,
                    [NavInsID.USE_CASE_CHOICE_CONFIRM]
                )
    except ExceptionRAPDU as e:
        assert e.status == StatusWord.INVALID_DATA
    else:
        assert False

def test_single_validator(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("010000000000000000000000" + wallet_addr.get().hex())

    transaction = generate_batch_deposit_transaction(contract, [withdrawal_credential], 1)

    client.set_external_plugin(PLUGIN_NAME,contract.address,BATCH_DEPOSIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)

def test_single_validator_other_address(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("0100000000000000000000001384a8c0593e4f2faa5a1e47a618ca801abdb9cd")

    transaction = generate_batch_deposit_transaction(contract, [withdrawal_credential], 1)

    client.set_external_plugin(PLUGIN_NAME,contract.address,BATCH_DEPOSIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)


def test_single_validator_bls_credential(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("00efa784baf17175e94d9e338ab3b5c9bc3ce9bc9832939dd6756080812942c3")

    transaction = generate_batch_deposit_transaction(contract, [withdrawal_credential], 1)

    client.set_external_plugin(PLUGIN_NAME,contract.address,BATCH_DEPOSIT_SELECTOR)

    reject_transaction(backend, firmware, navigator, test_name, transaction, wallet_addr)


def test_multiple_validators(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("010000000000000000000000" + wallet_addr.get().hex())

    withdrawal_credentials = [withdrawal_credential for _ in range(3)]

    transaction = generate_batch_deposit_transaction(contract, withdrawal_credentials, 3)

    client.set_external_plugin(PLUGIN_NAME, contract.address,BATCH_DEPOSIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)

def test_multiple_validators_different_withdrawal(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("0100000000000000000000002384a8c0593e4f2faa5a1e47a618ca801abdb9cd")
    withdrawal_credentials = [withdrawal_credential for _ in range(3)]

    transaction = generate_batch_deposit_transaction(contract, withdrawal_credentials, 3)

    client.set_external_plugin(PLUGIN_NAME, contract.address,BATCH_DEPOSIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)

def test_multiple_validators_different_withdrawal_bls(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    withdrawal_credential = bytes.fromhex("00efa784baf17175e94d9e338ab3b5c9bc3ce9bc9832939dd6756080812942c3")
    withdrawal_credentials = [withdrawal_credential for _ in range(3)]

    transaction = generate_batch_deposit_transaction(contract, withdrawal_credentials, 3)

    client.set_external_plugin(PLUGIN_NAME, contract.address,BATCH_DEPOSIT_SELECTOR)

    reject_transaction(backend, firmware, navigator, test_name, transaction, wallet_addr)

def test_multiple_validators_multiple_mixed_withdrawal_addresses(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    address1 = bytes.fromhex("0100000000000000000000001384a8c0593e4f2faa5a1e47a618ca801abdb9cd")
    address2 = bytes.fromhex("0100000000000000000000002384a8c0593e4f2faa5a1e47a618ca801abdb9cd")
    walletAddress = bytes.fromhex("010000000000000000000000" + wallet_addr.get().hex())
    address4 = bytes.fromhex("0100000000000000000000003384a8c0593e4f2faa5a1e47a618ca801abdb9cd")

    withdrawal_credentials = [address1, address2, walletAddress, address4]

    transaction = generate_batch_deposit_transaction(contract, withdrawal_credentials, 4)

    client.set_external_plugin(PLUGIN_NAME,contract.address,BATCH_DEPOSIT_SELECTOR)

    reject_transaction(backend, firmware, navigator, test_name, transaction, wallet_addr)

def test_multiple_validators_multiple_mixed_withdrawal_addresses_bls_key(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    address1 = bytes.fromhex("0100000000000000000000001384a8c0593e4f2faa5a1e47a618ca801abdb9cd")
    address2 = bytes.fromhex("0100000000000000000000002384a8c0593e4f2faa5a1e47a618ca801abdb9cd")
    walletAddress = bytes.fromhex("010000000000000000000000" + wallet_addr.get().hex())
    address4 = bytes.fromhex("00efa784baf17175e94d9e338ab3b5c9bc3ce9bc9832939dd6756080812942c3")

    withdrawal_credentials = [address1, address2, walletAddress, address4]

    transaction = generate_batch_deposit_transaction(contract, withdrawal_credentials, 4)

    client.set_external_plugin(PLUGIN_NAME,contract.address,BATCH_DEPOSIT_SELECTOR)

    reject_transaction(backend, firmware, navigator, test_name, transaction, wallet_addr)