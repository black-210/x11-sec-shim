#include "x11_sec_health.h"
#include "x11_sec_config.h"

#include <stdio.h>
#include <string.h>

static unsigned audits;

static int audit_record(const x11_sec_audit_record *record, void *context)
{
    (void)context;
    if (record == NULL) {
        return -1;
    }
    ++audits;
    return 0;
}

static int failing_audit(const x11_sec_audit_record *record, void *context)
{
    (void)record;
    (void)context;
    return -1;
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void make_request(x11_sec_request *request, uint32_t operation)
{
    memset(request, 0, sizeof(*request));
    request->operation = operation;
    request->payload_length = 32u;
    request->resource_count = 1u;
    request->has_display = 1u;
    request->has_resource = 1u;
}

static void make_client(x11_sec_client *client, const uint8_t *token, size_t length)
{
    memset(client, 0, sizeof(*client));
    client->authenticated = 1u;
    client->unix_socket = 1u;
    client->token = token;
    client->token_length = length;
    (void)snprintf(client->client_id, sizeof(client->client_id), "%s", "test-client");
    (void)snprintf(client->resource_id, sizeof(client->resource_id), "%s", "window-1");
}

int main(void)
{
    const uint8_t token[] = { 0x42u, 0x43u, 0x44u, 0x45u };
    uint8_t bad_token[] = { 0x42u, 0x43u, 0x44u, 0x46u };
    x11_sec_policy policy;
    x11_sec_context context;
    x11_sec_client client;
    x11_sec_request request;
    enum x11_sec_reason reason;
    char detail[64];
    char summary[256];

    x11_sec_policy_default(&policy);
    CHECK(x11_sec_policy_validate(&policy, detail, sizeof(detail)) == 0);
    CHECK(strstr(detail, "valid") != NULL);
    CHECK(x11_sec_policy_summary(&policy, summary, sizeof(summary)) == 0);
    CHECK(strstr(summary, "auth=1") != NULL);
    CHECK(x11_sec_init(&context, &policy, token, sizeof(token), audit_record, NULL) == 0);

    make_client(&client, token, sizeof(token));
    make_request(&request, X11_SEC_OP_READ_PROPERTIES);
    CHECK(x11_sec_authorize(&context, &client, &request, 1000u, &reason) == X11_SEC_ALLOW);
    CHECK(reason == X11_SEC_OK);

    client.token = bad_token;
    CHECK(x11_sec_authorize(&context, &client, &request, 1001u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_BAD_TOKEN);
    client.token = token;

    client.authenticated = 0u;
    CHECK(x11_sec_authorize(&context, &client, &request, 1002u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_NOT_AUTHENTICATED);
    client.authenticated = 1u;

    request.operation = X11_SEC_OP_WRITE_PROPERTIES;
    CHECK(x11_sec_authorize(&context, &client, &request, 1003u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_OPERATION_DENIED);
    request.operation = X11_SEC_OP_READ_PROPERTIES;

    client.unix_socket = 0u;
    CHECK(x11_sec_authorize(&context, &client, &request, 1004u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_UNTRUSTED_ORIGIN);
    client.unix_socket = 1u;

    request.payload_length = X11_SEC_MAX_PAYLOAD_BYTES + 1u;
    CHECK(x11_sec_authorize(&context, &client, &request, 1005u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_MALFORMED_REQUEST);

    policy.requests_per_window = 1u;
    CHECK(x11_sec_init(&context, &policy, token, sizeof(token), audit_record, NULL) == 0);
    make_client(&client, token, sizeof(token));
    make_request(&request, X11_SEC_OP_READ_PROPERTIES);
    CHECK(x11_sec_authorize(&context, &client, &request, 2000u, &reason) == X11_SEC_ALLOW);
    CHECK(x11_sec_authorize(&context, &client, &request, 2001u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_RATE_LIMITED);
    CHECK(audits >= 4u);

    CHECK(x11_sec_init(&context, &policy, token, sizeof(token), failing_audit, NULL) == 0);
    CHECK(x11_sec_authorize(&context, &client, &request, 3000u, &reason) == X11_SEC_ALLOW);
    CHECK(x11_sec_authorize(&context, &client, &request, 3001u, &reason) == X11_SEC_DENY);
    CHECK(reason == X11_SEC_AUDIT_FAILURE || reason == X11_SEC_RATE_LIMITED);

    policy.allowed_operations = 0u;
    CHECK(x11_sec_policy_validate(&policy, detail, sizeof(detail)) == 1);
    CHECK(x11_sec_policy_summary(NULL, summary, sizeof(summary)) == -1);
    puts("x11-sec-shim: all tests passed");
    return 0;
}
