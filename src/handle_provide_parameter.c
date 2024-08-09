#include "plugin.h"

static bool withdrawal_credentials_match(ethPluginProvideParameter_t *msg, context_t *context) {
    return memcmp(context->withdrawal_credentials, msg->parameter, PARAMETER_LENGTH) == 0;
}

static void handle_deposit(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case PUBKEYS_ARRAY:
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY:
            context->offset =
                SELECTOR_SIZE + U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY_LENGTH;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY_LENGTH:
            context->validators_count =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->validators_count));

            context->next_param = WITHDRAWAL_CREDENTIALS_OFFSET;
            break;

        case WITHDRAWAL_CREDENTIALS_OFFSET:
            context->offset = msg->parameterOffset +
                              U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_LENGTH;
            break;

        case WITHDRAWAL_CREDENTIALS_LENGTH:
            if (U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(uint16_t)) != 32) {
                PRINTF("Invalid withdrawal credentials length\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }

            context->next_param = WITHDRAWAL_CREDENTIALS;
            break;

        case WITHDRAWAL_CREDENTIALS:
            if (!context->withdrawal_credentials_stored) {
                copy_parameter(context->withdrawal_credentials,
                               msg->parameter,
                               sizeof(context->withdrawal_credentials));

                context->withdrawal_credentials_stored = true;
            } else if (!withdrawal_credentials_match(msg, context)) {
                context->withdrawal_credentials_mixed = true;
            }

            context->validators_count--;

            if (context->validators_count != 0) {
                context->next_param = WITHDRAWAL_CREDENTIALS_LENGTH;
            } else {
                context->next_param = REMAINING_PARAMETERS;
            }
            break;

        case REMAINING_PARAMETERS:
            // Skip the remaining parameters
            break;

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_pooled_multicall(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    // function multicall(
    //    bytes[] calldata data
    // ) external
    //
    // **************************************************************************
    // example for a multicall with 2 function calls
    // [  0] selector (0xac9650d8)
    // [  4] data_offset
    // [ 36] data_length
    // [ 68] data_0_offset
    // [100] data_1_offset
    // [132] data_0
    //        [132] data_0_length (size in bytes)
    //        [164] data_0_contents (first call's function selector and arguments)
    //             [164] function_selector (first 4 bytes)
    //             [168] arg_1_offset_or_value
    //             [200] arg_2_offset_or_value
    //             ...
    // [456] data_1
    //        [456] data_1_length (size in bytes)
    //        [488] data_1_contents (second call's function selector and arguments)
    //             [488] function_selector (first 4 bytes)
    //             [492] arg_1_offset_or_value
    //             [524] arg_2_offset_or_value
    //             ...
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    size_t index;
    switch (context->next_param) {
        case MULTICALL_ARRAY:
            context->offset =
                SELECTOR_SIZE + U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = MULTICALL_ARRAY_LENGTH;
            break;
        case MULTICALL_ARRAY_LENGTH:
            context->multicall_count =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->multicall_count));

            // Check if the number of elements exceeds the maximum allowed
            if (context->multicall_count > MAX_MULTICALL_ELEMENTS) {
                PRINTF("Too many elements in multicall array: %d\n", context->multicall_count);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            // Initialize the current multicall index to 0
            context->current_multicall_index = 0;

            context->next_param = MULTICALL_ARRAY_OFFSET;
            break;
        case MULTICALL_ARRAY_OFFSET:
            context->multicall_offsets[context->current_multicall_index] =
                msg->parameterOffset - (context->current_multicall_index * PARAMETER_LENGTH) +
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            // Move to the next element offset or go to the first method selector
            context->current_multicall_index++;

            if (context->current_multicall_index < context->multicall_count) {
                context->next_param = MULTICALL_ARRAY_OFFSET;
            } else {
                context->current_multicall_index = 0;
                context->next_param = MULTICALL_ELEMENT_LENGTH;
            }
            break;

        case MULTICALL_ELEMENT_LENGTH:
            if (context->current_multicall_index < context->multicall_count) {
                // Jump to the stored offset of the current element
                context->offset =
                    context->multicall_offsets[context->current_multicall_index] + PARAMETER_LENGTH;
                context->go_to_offset = true;
                context->next_param = MULTICALL_METHOD_SELECTOR;
            }
            break;

        case MULTICALL_METHOD_SELECTOR:
            if (find_selector(U4BE(msg->parameter, 0), SELECTORS, SELECTOR_COUNT, &index)) {
                switch (index) {
                    case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT:
                    case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT_AND_MINT_TOKEN:
                    case BRICK_TOWERS_POOLED_DEPOSIT:
                        strlcpy(context->pooled_screen_name, "Deposit", POOLED_SCREEN_NAME_LEN);
                        break;
                    case BRICK_TOWERS_POOLED_MINT_OS_TOKEN:
                        strlcpy(context->pooled_screen_name,
                                "Mint osETH Token",
                                POOLED_SCREEN_NAME_LEN);
                        break;
                    case BRICK_TOWERS_POOLED_BURN_OS_TOKEN:
                        strlcpy(context->pooled_screen_name,
                                "Burn osETH Token",
                                POOLED_SCREEN_NAME_LEN);
                        break;
                    case BRICK_TOWERS_POOLED_ENTER_EXIT_QUEUE:
                        strlcpy(context->pooled_screen_name,
                                "Enter Exit Queue",
                                POOLED_SCREEN_NAME_LEN);
                        break;
                    case BRICK_TOWERS_POOLED_CLAIM_EXITED_ASSETS:
                        strlcpy(context->pooled_screen_name,
                                "Claim Exited Assets",
                                POOLED_SCREEN_NAME_LEN);
                        break;
                    case BRICK_TOWERS_POOLED_UPDATE_STATE:
                        if (context->pooled_screen_name[0] == '\0') {
                            strlcpy(context->pooled_screen_name,
                                    "Update State",
                                    POOLED_SCREEN_NAME_LEN);
                        }
                        break;
                    default:
                        PRINTF("Error: selector not found!\n");
                        break;
                }
            } else {
                // Selector not found, continue to the next element
                PRINTF("Error: selector not found!\n");
            }

            // Increment to move to the next element's method selector
            context->current_multicall_index++;

            if (context->current_multicall_index < context->multicall_count) {
                // Proceed to the next element offset and jump to its location
                context->next_param = MULTICALL_ELEMENT_LENGTH;
            } else {
                context->next_param = REMAINING_PARAMETERS;
            }

            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        case REMAINING_PARAMETERS:
            // Skip or handle remaining parameters if needed
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case BRICK_TOWERS_DEPOSIT:
            handle_deposit(msg, context);
            break;
        case BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT:
            // no parameters to handle
            break;
        case BRICK_TOWERS_POOLED_MULTICALL:
            handle_pooled_multicall(msg, context);
            break;
        case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT:
        case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT_AND_MINT_TOKEN:
        case BRICK_TOWERS_POOLED_DEPOSIT:
            // strlcpy(context->pooled_screen_name, "Deposit", POOLED_SCREEN_NAME_LEN);
            break;
        case BRICK_TOWERS_POOLED_MINT_OS_TOKEN:
            strlcpy(context->pooled_screen_name, "Mint osETH Token", POOLED_SCREEN_NAME_LEN);
            break;
        case BRICK_TOWERS_POOLED_BURN_OS_TOKEN:
            strlcpy(context->pooled_screen_name, "Burn osETH Token", POOLED_SCREEN_NAME_LEN);
            break;
        case BRICK_TOWERS_POOLED_ENTER_EXIT_QUEUE:
            strlcpy(context->pooled_screen_name, "Enter Exit Queue", POOLED_SCREEN_NAME_LEN);
            break;
        case BRICK_TOWERS_POOLED_CLAIM_EXITED_ASSETS:
            strlcpy(context->pooled_screen_name, "Claim Exited Assets", POOLED_SCREEN_NAME_LEN);
            break;
        case BRICK_TOWERS_POOLED_UPDATE_STATE:
            strlcpy(context->pooled_screen_name, "Update State", POOLED_SCREEN_NAME_LEN);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}