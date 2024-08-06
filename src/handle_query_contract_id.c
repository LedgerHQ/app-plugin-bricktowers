#include "plugin.h"

void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;

    strlcpy(msg->name, APPNAME, msg->nameLength);

    if (context->selectorIndex == BRICK_TOWERS_DEPOSIT) {
        strlcpy(msg->version, "Native Staking", msg->versionLength);
        msg->result = ETH_PLUGIN_RESULT_OK;
    } else if (context->selectorIndex == BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT) {
        strlcpy(msg->version, "Native Staking", msg->versionLength);
        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Selector index: %d not supported\n", context->selectorIndex);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}