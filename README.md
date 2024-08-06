[![Ensure compliance with Ledger guidelines](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml)


# app-plugin-bricktowers

This plug-in is compatible with Nano S / X, Flex and Stax devices, it enables to
interact in a secure way with the Brick Towers staking contracts.

## Documentation

The documentation is defined in [PLUGIN_SPECIFICATON.md](https://github.com/LedgerHQ/app-plugin-boilerplate/blob/develop/PLUGIN_SPECIFICATION.md).

## Clearsigning

### Brick Towers - Batch Deposit

Function: `deposit()`
Clearsigned:

- tx.value

![](/tests/snapshots/nanosp/test_single_validator/00000.png) ![](/tests/snapshots/nanosp/test_single_validator/00001.png) ![](/tests/snapshots/nanosp/test_single_validator/00002.png) ![](/tests/snapshots/nanosp/test_single_validator/00003.png) ![](/tests/snapshots/nanosp/test_single_validator/00004.png) ![](/tests/snapshots/nanosp/test_single_validator/00005.png)

![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00000.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00001.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00002.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00003.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00004.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00005.png) ![](/tests/snapshots/nanosp/test_multiple_validators_different_withdrawal/00006.png)


### Brick Towers - Request Voluntary Exit

Function: `requestVoluntaryExit()`
Clearsigned:

- none as BLS key is not digest to display on device and only owner of the validator can withdraw on its behalf so no attack vector possible.

![](/tests/snapshots/nanosp/test_request_single_validator/00000.png) ![](/tests/snapshots/nanosp/test_request_single_validator/00001.png) ![](/tests/snapshots/nanosp/test_request_single_validator/00002.png) ![](/tests/snapshots/nanosp/test_request_single_validator/00003.png) ![](/tests/snapshots/nanosp/test_request_single_validator/00004.png) ![](/tests/snapshots/nanosp/test_request_single_validator/00005.png)

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
