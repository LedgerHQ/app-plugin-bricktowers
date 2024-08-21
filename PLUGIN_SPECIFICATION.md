# Technical Specification

> **Warning**
This documentation describes the smart contracts and functions supported by the Brick Towers plugin.

## About

This documentation describes the smart contracts and functions supported by the boilerplate plugin.

## Smart Contracts

Smart contracts covered by the plugin:

| Network | Version | Smart Contract                             | Address                                      |
|---------|---------|--------------------------------------------| -------------------------------------------- |
| Holesky | v1.0    | Batch Deposit                              | `0x2384a8c0593e4F2Faa5A1e47a618ca801abdB9CD` |
| Holesky | v1.0    | Request Voluntary Exit                     | `0xca541c4C28AeEf7C3E15224ded5bCAA78ccAF13F` |
| Holesky | v3.0    | Stakewise Private Vault                    | `0x8871b38b0c3fe47403aac41121376146d086b6b1` |
| Holesky | v3.0    | Stakewise Public Vault                     | `0x1c135dde70509954a4681ef1dd3aa1c57829b39a` |
| Mainnet | v1.0    | Batch Deposit                              | `0x990E6F7a8863457Dc752e12FabfE572d5704279B` |
| Mainnet | v1.0    | Request Voluntary Exit                     | `0xC0d85F63cD3D280a1Db839Ed7e927FC994E1fabe` |
| Mainnet | v3.0    | Stakewise Swiss Liquid Staking Vault       | `0x302be829c61c287787030888bbcf11115ecd5773` |
| Mainnet | v3.0    | Stakewise Brick Towers Institutional Vault | `0x91804d6d10f2bd4e03338f40dee01cf294085cd1` |

- Stakewise contract implements the same interface / ABI 

## Functions

For the smart contracts implemented, the functions covered by the plugin shall be described here:

| Contract               | Function                            | Selector        | Displayed Parameters                                                         |
|------------------------|-------------------------------------|-----------------|------------------------------------------------------------------------------|
| Batch Deposit          | batchDeposit                        | `0xca0bfcce`    | <table><tbody> <tr><td><code>uint tx.value</code></td></tr> </tbody></table> |
| Request Voluntary Exit | requestVoluntaryExit                | `0x9aa3033a`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | deposit                             | `0xf9609f08`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | multicall                           | `0xac9650d8`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | mintOsToken                         | `0x201b9eb5`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | burnOsToken                         | `0x066055e0`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | enterExitQueue                      | `0x8ceab9aa`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | claimExitedAssets                   | `0x8697d2c2`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | updateState                         | `0x1a7ff553`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | updateStateAndDeposit               | `0x18f72950`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |
| Stakewise Vault        | updateStateAndDepositAndMintOsToken | `0x01d523b6`    | <table><tbody> <tr><td>n/r</td></tr> </tbody></table>                        |



















