#include "x11_sec_runtime.h"

#include <string.h>

#include "x11_sec_client.h"
#include "x11_sec_event.h"
#include "x11_sec_policy.h"
#include "x11_sec_socket.h"
#include "x11_sec_token.h"

int x11_sec_runtime_init(x11_sec_runtime *runtime,
                        const x11_sec_policy *policy,
                        const uint8_t *token,
                        size_t token_length,
                        FILE *stream)
{
    if (runtime == NULL || policy == NULL ||
        (token_length != 0u && token == NULL)) {
        return 0;
    }
    memset(runtime, 0, sizeof(*runtime));
    runtime->policy = *policy;
    if (token_length > X11_SEC_MAX_TOKEN_BYTES) {
        return 0;
    }
    runtime->token_length = token_length;
    if (token_length != 0u) {
        memcpy(runtime->token, token, token_length);
    }
    x11_sec_logger_init(&runtime->logger, stream);
    if (x11_sec_init(&runtime->context,
                     &runtime->policy,
                     runtime->token,
                     runtime->token_length,
                     NULL,
                     NULL) != 0) {
        return 0;
    }
    return 1;
}

enum x11_sec_decision x11_sec_runtime_authorize(x11_sec_runtime *runtime,
                                               const x11_sec_client *client,
                                               const x11_sec_request *request,
                                               uint64_t now_ns,
                                               enum x11_sec_reason *reason)
{
    if (runtime == NULL || client == NULL || request == NULL) {
        if (reason != NULL) {
            *reason = X11_SEC_BAD_ARGUMENT;
        }
        return X11_SEC_DENY;
    }
    if (!x11_sec_client_valid(client)) {
        if (reason != NULL) {
            *reason = X11_SEC_BAD_ARGUMENT;
        }
        return X11_SEC_DENY;
    }
    if (!x11_sec_event_allowed(request->operation, runtime->policy.allowed_operations)) {
        if (reason != NULL) {
            *reason = X11_SEC_OPERATION_DENIED;
        }
        return X11_SEC_DENY;
    }
    if (!x11_sec_socket_allowed(&runtime->policy, client)) {
        if (reason != NULL) {
            *reason = X11_SEC_UNTRUSTED_ORIGIN;
        }
        return X11_SEC_DENY;
    }
    if (!x11_sec_token_equal(client->token, client->token_length,
                             runtime->token, runtime->token_length)) {
        if (reason != NULL) {
            *reason = X11_SEC_BAD_TOKEN;
        }
        return X11_SEC_DENY;
    }
    if (runtime->policy.require_authentication && !client->authenticated) {
        if (reason != NULL) {
            *reason = X11_SEC_NOT_AUTHENTICATED;
        }
        return X11_SEC_DENY;
    }
    return x11_sec_authorize(&runtime->context, client, request, now_ns, reason);
}
