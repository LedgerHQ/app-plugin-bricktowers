#include "plugin.h"

static void set_screen_title(ethQueryContractUI_t *msg, const char *title) {
    if (strlcpy(msg->title, title, msg->titleLength) < strlen(title)) {
        PRINTF("Screen title truncated: %s\n", msg->title);
    }
}

static void set_screen_message(ethQueryContractUI_t *msg, const char *message) {
    if (strlcpy(msg->msg, message, msg->msgLength) < strlen(message)) {
        PRINTF("Screen message truncated: %s\n", msg->msg);
    }
}

static bool set_amount_screen(ethQueryContractUI_t *msg) {
    set_screen_title(msg, "Amount");

    const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
    uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(eth_amount,
                          eth_amount_size,
                          WEI_TO_ETHER,
                          "ETH",
                          msg->msg,
                          msg->msgLength);
}

static bool set_withdrawal_address_warning_screen(ethQueryContractUI_t *msg, context_t *context) {
    if (context->withdrawal_credentials_mixed) {
        set_screen_title(msg, "Funds at risk");
        set_screen_message(msg, "Withdrawal credentials are mixed.");
        return true;
    } else {
        if (context->withdrawal_credentials[0] == ETH1_ADDRESS_WITHDRAWAL_PREFIX) {
            set_screen_title(msg, "Please verify");
            set_screen_message(msg, "Withdrawal address differs from the connected address.");
            return true;
        } else {
            set_screen_title(msg, "Funds at risk");
            set_screen_message(msg, "Withdrawal credentials are set to a BLS key.");
            return true;
        }
    }
}

static bool set_withdrawal_address_screen(ethQueryContractUI_t *msg, context_t *context) {
    set_screen_title(msg, "Withdrawal Address");
    char address_buffer[ADDRESS_STR_LEN];
    getEthDisplayableAddress(context->withdrawal_address, address_buffer, sizeof(address_buffer), 0);
    set_screen_message(msg, address_buffer);
    return true;
}

static bool deposit_ui(ethQueryContractUI_t *msg, context_t *context) {
    bool result = false;
    switch (msg->screenIndex) {
        case 0:
            result = set_amount_screen(msg);
            break;

        case 1:
            result = set_withdrawal_address_warning_screen(msg, context);
            break;

        case 2:
            result = set_withdrawal_address_screen(msg, context);
            break;

        default:
            PRINTF("Received an invalid screenIndex\n");
    }
    return result;
}

bool request_voluntary_exit_ui(ethQueryContractUI_t *msg) {
    bool ret = false;

    switch (msg->screenIndex) {
        case 0:
            set_screen_title(msg, "Request");
            set_screen_message(msg, "Validators Withdrawal");
            ret = true;
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }
    return ret;
}


void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool result = false;

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    switch (context->selectorIndex) {
        case BRICK_TOWERS_DEPOSIT:
            result = deposit_ui(msg, context);
        break;

        case BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT:
            result = request_voluntary_exit_ui(msg);
        break;
    }

    msg->result = result ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}