/*******************************************************************************
 *   Plugin Boilerplate
 *   (c) 2023 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#pragma once

#include <string.h>
#include "eth_plugin_interface.h"

#define ADDRESS_STR_LEN                43
#define POOLED_SCREEN_NAME_LEN         32
#define MAX_MULTICALL_ELEMENTS         16
#define ETH1_ADDRESS_WITHDRAWAL_PREFIX 0x01

#define SELECTORS_LIST(X)                                       \
    X(BRICK_TOWERS_DEPOSIT, 0xca0bfcce)                         \
    X(BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT, 0x9aa3033a)          \
    X(BRICK_TOWERS_POOLED_DEPOSIT, 0xf9609f08)                  \
    X(BRICK_TOWERS_POOLED_MULTICALL, 0xac9650d8)                \
    X(BRICK_TOWERS_POOLED_MINT_OS_TOKEN, 0x201b9eb5)            \
    X(BRICK_TOWERS_POOLED_BURN_OS_TOKEN, 0x066055e0)            \
    X(BRICK_TOWERS_POOLED_ENTER_EXIT_QUEUE, 0x8ceab9aa)         \
    X(BRICK_TOWERS_POOLED_CLAIM_EXITED_ASSETS, 0x8697d2c2)      \
    X(BRICK_TOWERS_POOLED_UPDATE_STATE, 0x1a7ff553)             \
    X(BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT, 0x18f72950) \
    X(BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT_AND_MINT_TOKEN, 0x01d523b6)

// Xmacro helpers to define the enum and map
// Do not modify !
#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,

// This enum will be automatically expanded to hold all selector names.
// The value SELECTOR_COUNT can be used to get the number of defined selectors
// Do not modify !
typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

// This array will be automatically expanded to map all selector_t names with the correct value.
// Do not modify !
extern const uint32_t SELECTORS[SELECTOR_COUNT];

// Enumeration used to parse the smart contract data.
typedef enum {
    PUBKEYS_ARRAY = 0,
    WITHDRAWAL_CREDENTIALS_ARRAY,
    WITHDRAWAL_CREDENTIALS_ARRAY_LENGTH,
    WITHDRAWAL_CREDENTIALS_OFFSET,
    WITHDRAWAL_CREDENTIALS_LENGTH,
    WITHDRAWAL_CREDENTIALS,
    MULTICALL_ARRAY,
    MULTICALL_ARRAY_LENGTH,
    MULTICALL_ELEMENT_LENGTH,
    MULTICALL_ARRAY_OFFSET,
    MULTICALL_METHOD_SELECTOR,
    REMAINING_PARAMETERS,
} parameter;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_s {
    // For display.
    uint16_t validators_count;
    uint8_t withdrawal_credentials[PARAMETER_LENGTH];
    uint16_t multicall_count;
    uint16_t current_multicall_index;
    uint16_t multicall_offsets[MAX_MULTICALL_ELEMENTS];
    bool withdrawal_credentials_stored;
    bool withdrawal_credentials_mixed;

    uint8_t withdrawal_address[ADDRESS_LENGTH];
    char pooled_screen_name[POOLED_SCREEN_NAME_LEN];

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

// Check if the context structure will fit in the RAM section ETH will prepare for us
// Do not remove!
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);
