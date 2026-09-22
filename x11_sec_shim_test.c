#include "x11_sec_shim.h"

#include <stdio.h>
#include <string.h>

static int audit_record(const x11_sec_audit_record *record, void *context)
{
    (void)context;
    if (record == NULL) {
        return -1;
    }
    printf("audit decision=%d reason=%d op=%u client=%s\n",
           record->decision,
           record->reason,
           record->operation,
           record->client_id);
    return 0;
}

int main(void)
{
    x11_sec_policy policy;
    x11_sec_context ctx;
    x11_sec_client client;
    x11_sec_request request;
    uint8_t token[] = {0x42u, 0x43u, 0x44u, 0x45u};
    enum x11_sec_reason reason;
    enum x11_sec_decision decision;

    memset(&client, 0, sizeof(client));
    memset(&request, 0, sizeof(request));

    x11_sec_policy_default(&policy);
    client.authenticated = 1u;
    client.unix_socket = 1u;
    client.loopback = 0u;
    client.token = token;
    client.token_length = sizeof(token);
    snprintf(client.client_id, sizeof(client.client_id), "client-01");
    snprintf(client.resource_id, sizeof(client.resource_id), "win-01");

    request.operation = X11_SEC_OP_READ_PROPERTIES;
    request.payload_length = 32u;
    request.resource_count = 1u;
    request.has_display = 1u;
    request.has_resource = 1u;

    if (x11_sec_init(&ctx, &policy, token, sizeof(token), audit_record, NULL) != 0) {
        fprintf(stderr, "x11_sec_init failed\n");
        return 1;
    }

    decision = x11_sec_authorize(&ctx, &client, &request, 1000ULL, &reason);
    if (decision != X11_SEC_ALLOW) {
        fprintf(stderr, "authorization failed: %s\n", x11_sec_reason_string(reason));
        return 2;
    }

    printf("authorization accepted\n");
    return 0;
}
