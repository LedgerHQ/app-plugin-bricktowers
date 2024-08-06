from pathlib import Path
import json
import os

from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient
import ledger_app_clients.ethereum.response_parser as ResponseParser
from ledger_app_clients.ethereum.utils import recover_transaction
from ragger.navigator import NavInsID

from .utils import get_appname_from_makefile, DERIVATION_PATH

ROOT_SCREENSHOT_PATH = Path(__file__).parent
ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))
NONCE = 21
GAS_PRICE = 13
GAS = 173290
PLUGIN_NAME = get_appname_from_makefile()

REQUEST_VOLUNTARY_EXIT_SELECTOR = bytes.fromhex("9aa3033a")

with open("%s/0xca541c4c28aeef7c3e15224ded5bcaa78ccaf13f.abi.json" % (ABIS_FOLDER)) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        # Get address from filename
        address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
    )

def generate_request_voluntary_exit_data(contract, num_validators):
    pubkeys = [os.urandom(48) for _ in range(num_validators)]

    data = contract.encode_abi("requestVoluntaryExit", [
        pubkeys
    ])

    return data

def generate_request_voluntary_exit_transaction(contract, num_validators):
    data = generate_request_voluntary_exit_data(contract, num_validators)

    tx_params = {
        "nonce": NONCE,
        "gasPrice": GAS_PRICE,
        "gas": GAS,
        "to": contract.address,
        "value": Web3.to_wei(0, "ether"),
        "chainId": ChainId.ETH,
        "data": data
    }

    return tx_params

def sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr):
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


def test_request_single_validator(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    transaction = generate_request_voluntary_exit_transaction(contract, 1)

    client.set_external_plugin(PLUGIN_NAME,contract.address,REQUEST_VOLUNTARY_EXIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)


def test_request_multiple_validator(backend, firmware, navigator, test_name, wallet_addr):
    client = EthAppClient(backend)

    transaction = generate_request_voluntary_exit_transaction(contract, 3)

    client.set_external_plugin(PLUGIN_NAME,contract.address,REQUEST_VOLUNTARY_EXIT_SELECTOR)

    sign_and_verify_transaction(client, firmware, navigator, test_name, transaction, wallet_addr)
