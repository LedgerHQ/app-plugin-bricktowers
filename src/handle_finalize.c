#include "plugin.h"

static bool withdrawal_address_matches_sender(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    return memcmp(context->withdrawal_address, msg->address, ADDRESS_LENGTH) == 0;
}

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    switch (context->selectorIndex) {
        case BRICK_TOWERS_DEPOSIT:
            msg->numScreens = 1;

            if (context->withdrawal_credentials[0] == ETH1_ADDRESS_WITHDRAWAL_PREFIX) {
                copy_address(context->withdrawal_address,
                             context->withdrawal_credentials,
                             sizeof(context->withdrawal_address));
            }

            if (context->withdrawal_credentials_mixed) {
                msg->numScreens += 1;
            } else if (!withdrawal_address_matches_sender(msg)) {
                if (context->withdrawal_credentials[0] == ETH1_ADDRESS_WITHDRAWAL_PREFIX) {
                    msg->numScreens += 2;
                } else {
                    msg->numScreens += 1;
                }
            }
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;

        case BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT:
            msg->numScreens = 1;
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
    }
}