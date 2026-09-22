#include "x11_sec_shim.h"

#include <stdio.h>
#include <string.h>

static int bounded_cstring(const char *value, size_t limit)
{
    size_t i;

    if (value == NULL) {
        return 0;
    }
    for (i = 0; i < limit; ++i) {
        if (value[i] == '\0') {
            return 1;
        }
    }
    return 0;
}

static int secure_equal(const uint8_t *left, size_t left_length,
                        const uint8_t *right, size_t right_length)
{
    size_t i;
    uint8_t difference;

    /* Compare the complete configured token length without early exit. */
    difference = (uint8_t)(left_length ^ right_length);
    for (i = 0; i < X11_SEC_MAX_TOKEN_BYTES; ++i) {
        uint8_t a = i < left_length ? left[i] : 0u;
        uint8_t b = i < right_length ? right[i] : 0u;
        difference = (uint8_t)(difference | (uint8_t)(a ^ b));
    }
    return difference == 0u;
}

static void copy_client_id(char destination[X11_SEC_MAX_CLIENT_ID],
                           const char source[X11_SEC_MAX_CLIENT_ID])
{
    size_t i;
    for (i = 0; i + 1u < X11_SEC_MAX_CLIENT_ID && source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

static enum x11_sec_decision reject(x11_sec_context *context,
                                    const x11_sec_client *client,
                                    const x11_sec_request *request,
                                    uint64_t now_ns,
                                    enum x11_sec_reason reason,
                                    enum x11_sec_reason *out_reason)
{
    x11_sec_audit_record record;

    if (out_reason != NULL) {
        *out_reason = reason;
    }
    if (context->audit == NULL) {
        return X11_SEC_DENY;
    }

    memset(&record, 0, sizeof(record));
    record.timestamp_ns = now_ns;
    record.decision = X11_SEC_DENY;
    record.reason = reason;
    record.operation = request == NULL ? 0u : request->operation;
    if (client != NULL && bounded_cstring(client->client_id, X11_SEC_MAX_CLIENT_ID)) {
        copy_client_id(record.client_id, client->client_id);
    }
    (void)snprintf(record.detail, sizeof(record.detail), "%s",
                   x11_sec_reason_string(reason));
    if (context->audit(&record, context->audit_context) != 0 &&
        context->policy.fail_closed_on_audit_error) {
        if (out_reason != NULL) {
            *out_reason = X11_SEC_AUDIT_FAILURE;
        }
    }
    return X11_SEC_DENY;
}

void x11_sec_policy_default(x11_sec_policy *policy)
{
    if (policy == NULL) {
        return;
    }
    memset(policy, 0, sizeof(*policy));
    policy->allowed_operations = X11_SEC_OP_READ_PROPERTIES |
                                 X11_SEC_OP_SELECT_INPUT;
    policy->requests_per_window = 100u;
    policy->window_ns = 1000000000ULL;
    policy->require_authentication = 1u;
    policy->allow_local_unix = 1u;
    policy->allow_tcp_loopback = 0u;
    policy->fail_closed_on_audit_error = 1u;
}

int x11_sec_init(x11_sec_context *context,
                 const x11_sec_policy *policy,
                 const uint8_t *expected_token,
                 size_t expected_token_length,
                 x11_sec_audit_fn audit,
                 void *audit_context)
{
    if (context == NULL || policy == NULL ||
        expected_token_length > X11_SEC_MAX_TOKEN_BYTES ||
        (expected_token_length != 0u && expected_token == NULL)) {
        return -1;
    }
    if (policy->window_ns == 0u || policy->requests_per_window == 0u) {
        return -1;
    }
    memset(context, 0, sizeof(*context));
    context->policy = *policy;
    context->expected_token_length = expected_token_length;
    if (expected_token_length != 0u) {
        memcpy(context->expected_token, expected_token, expected_token_length);
    }
    context->audit = audit;
    context->audit_context = audit_context;
    return 0;
}

enum x11_sec_decision x11_sec_authorize(x11_sec_context *context,
                                         const x11_sec_client *client,
                                         const x11_sec_request *request,
                                         uint64_t now_ns,
                                         enum x11_sec_reason *reason)
{
    if (reason != NULL) {
        *reason = X11_SEC_OK;
    }
    if (context == NULL || client == NULL || request == NULL ||
        !bounded_cstring(client->client_id, X11_SEC_MAX_CLIENT_ID) ||
        !bounded_cstring(client->resource_id, X11_SEC_MAX_RESOURCE_ID)) {
        return X11_SEC_DENY;
    }
    if (context->policy.allowed_operations == 0u) {
        return reject(context, client, request, now_ns,
                      X11_SEC_POLICY_DISABLED, reason);
    }
    if ((request->operation == 0u) ||
        (request->operation & ~context->policy.allowed_operations) != 0u ||
        request->payload_length > 1048576u || request->resource_count > 256u ||
        request->has_display == 0u || request->has_resource == 0u) {
        return reject(context, client, request, now_ns,
                      X11_SEC_MALFORMED_REQUEST, reason);
    }
    if ((client->unix_socket == 0u && client->loopback == 0u) ||
        (client->unix_socket != 0u && !context->policy.allow_local_unix) ||
        (client->loopback != 0u && !context->policy.allow_tcp_loopback)) {
        return reject(context, client, request, now_ns,
                      X11_SEC_UNTRUSTED_ORIGIN, reason);
    }
    if (context->policy.require_authentication && !client->authenticated) {
        return reject(context, client, request, now_ns,
                      X11_SEC_NOT_AUTHENTICATED, reason);
    }
    if (!secure_equal(client->token, client->token_length,
                      context->expected_token, context->expected_token_length)) {
        return reject(context, client, request, now_ns,
                      X11_SEC_BAD_TOKEN, reason);
    }
    if (now_ns < context->window_started_ns ||
        now_ns - context->window_started_ns >= context->policy.window_ns) {
        context->window_started_ns = now_ns;
        context->requests_in_window = 0u;
    }
    if (context->requests_in_window >= context->policy.requests_per_window) {
        return reject(context, client, request, now_ns,
                      X11_SEC_RATE_LIMITED, reason);
    }
    ++context->requests_in_window;
    if (reason != NULL) {
        *reason = X11_SEC_OK;
    }
    return X11_SEC_ALLOW;
}

const char *x11_sec_reason_string(enum x11_sec_reason reason)
{
    switch (reason) {
    case X11_SEC_OK: return "ok";
    case X11_SEC_BAD_ARGUMENT: return "bad argument";
    case X11_SEC_NOT_AUTHENTICATED: return "not authenticated";
    case X11_SEC_BAD_TOKEN: return "bad token";
    case X11_SEC_UNTRUSTED_ORIGIN: return "untrusted origin";
    case X11_SEC_POLICY_DISABLED: return "policy disabled";
    case X11_SEC_OPERATION_DENIED: return "operation denied";
    case X11_SEC_MALFORMED_REQUEST: return "malformed request";
    case X11_SEC_RATE_LIMITED: return "rate limited";
    case X11_SEC_AUDIT_FAILURE: return "audit failure";
    default: return "unknown reason";
    }
}
