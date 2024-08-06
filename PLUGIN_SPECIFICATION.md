# Technical Specification

> **Warning**
This documentation describes the smart contracts and functions supported by the Brick Towers plugin.

## About

This documentation describes the smart contracts and functions supported by the boilerplate plugin.

## Smart Contracts

Smart contracts covered by the plugin:

| Network | Version | Smart Contract                        | Address                                      |
|---------|---------|---------------------------------------| -------------------------------------------- |
| Holesky | v1.0    | Batch Deposit                         | `0x2384a8c0593e4F2Faa5A1e47a618ca801abdB9CD` |
| Holesky | v1.0    | Request Voluntary Exit                | `0xca541c4C28AeEf7C3E15224ded5bCAA78ccAF13F` |
| Mainnet | v1.0    | Batch Deposit                         | `0x990E6F7a8863457Dc752e12FabfE572d5704279B` |
| Mainnet | v1.0    | Request Voluntary Exit                | `0xC0d85F63cD3D280a1Db839Ed7e927FC994E1fabe` |


## Functions

For the smart contracts implemented, the functions covered by the plugin shall be described here:

| Contract               | Function             | Selector        | Displayed Parameters                                                         |
|------------------------|----------------------|-----------------|------------------------------------------------------------------------------|
| Batch Deposit          | batchDeposit         | `0xca0bfcce`    | <table><tbody> <tr><td><code>uint tx.value</code></td></tr> </tbody></table> |
| Request Voluntary Exit | requestVoluntaryExit | `0x9aa3033a`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |