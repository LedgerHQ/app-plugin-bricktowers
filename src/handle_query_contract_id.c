#include "plugin.h"

void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;

    strlcpy(msg->name, APPNAME, msg->nameLength);

    switch (context->selectorIndex) {
        case BRICK_TOWERS_POOLED_DEPOSIT:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_MULTICALL:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_MINT_OS_TOKEN:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_BURN_OS_TOKEN:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_ENTER_EXIT_QUEUE:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_CLAIM_EXITED_ASSETS:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_UPDATE_STATE:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_POOLED_UPDATE_STATE_AND_DEPOSIT_AND_MINT_TOKEN:
            strlcpy(msg->version, "Pooled Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_DEPOSIT:
            strlcpy(msg->version, "Native Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        case BRICK_TOWERS_REQUEST_VOLUNTARY_EXIT:
            strlcpy(msg->version, "Native Staking", msg->versionLength);
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}