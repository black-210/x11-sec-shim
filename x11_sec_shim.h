#ifndef X11_SEC_SHIM_H
#define X11_SEC_SHIM_H

/*
 * Strict, dependency-free policy layer for an X11 security boundary.
 *
 * This header deliberately does not expose Xlib internals.  A transport adapter
 * can translate X11 connection and request metadata into these types, then deny
 * by default unless x11_sec_authorize() returns ALLOW.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum x11_sec_decision {
    X11_SEC_DENY = 0,
    X11_SEC_ALLOW = 1
};

enum x11_sec_reason {
    X11_SEC_OK = 0,
    X11_SEC_BAD_ARGUMENT,
    X11_SEC_NOT_AUTHENTICATED,
    X11_SEC_BAD_TOKEN,
    X11_SEC_UNTRUSTED_ORIGIN,
    X11_SEC_POLICY_DISABLED,
    X11_SEC_OPERATION_DENIED,
    X11_SEC_MALFORMED_REQUEST,
    X11_SEC_RATE_LIMITED,
    X11_SEC_AUDIT_FAILURE
};

enum x11_sec_operation {
    X11_SEC_OP_READ_PROPERTIES = 1u << 0,
    X11_SEC_OP_WRITE_PROPERTIES = 1u << 1,
    X11_SEC_SELECT_INPUT = 1u << 2,
    X11_SEC_SEND_EVENT = 1u << 3,
    X11_SEC_GRAB = 1u << 4,
    X11_SEC_CREATE_RESOURCE = 1u << 5,
    X11_SEC_DESTROY_RESOURCE = 1u << 6,
    X11_SEC_INJECT_INPUT = 1u << 7
};

#define X11_SEC_MAX_TOKEN_BYTES 128u
#define X11_SEC_MAX_CLIENT_ID 64u
#define X11_SEC_MAX_RESOURCE_ID 32u
#define X11_SEC_MAX_DETAIL 160u
#define X11_SEC_MAX_AUDIT 256u

typedef struct x11_sec_policy {
    uint32_t allowed_operations;
    uint32_t requests_per_window;
    uint64_t window_ns;
    uint8_t require_authentication;
    uint8_t allow_local_unix;
    uint8_t allow_tcp_loopback;
    uint8_t fail_closed_on_audit_error;
    uint8_t reserved[4];
} x11_sec_policy;

typedef struct x11_sec_client {
    char client_id[X11_SEC_MAX_CLIENT_ID];
    char resource_id[X11_SEC_MAX_RESOURCE_ID];
    const uint8_t *token;
    size_t token_length;
    uint8_t authenticated;
    uint8_t unix_socket;
    uint8_t loopback;
    uint8_t reserved;
} x11_sec_client;

typedef struct x11_sec_request {
    uint32_t operation;
    uint32_t payload_length;
    uint32_t resource_count;
    uint8_t has_display;
    uint8_t has_resource;
    uint8_t reserved[2];
} x11_sec_request;

typedef struct x11_sec_audit_record {
    uint64_t timestamp_ns;
    enum x11_sec_decision decision;
    enum x11_sec_reason reason;
    uint32_t operation;
    char client_id[X11_SEC_MAX_CLIENT_ID];
    char detail[X11_SEC_MAX_DETAIL];
} x11_sec_audit_record;

typedef int (*x11_sec_audit_fn)(const x11_sec_audit_record *record, void *context);

typedef struct x11_sec_context {
    x11_sec_policy policy;
    uint8_t expected_token[X11_SEC_MAX_TOKEN_BYTES];
    size_t expected_token_length;
    uint64_t window_started_ns;
    uint32_t requests_in_window;
    x11_sec_audit_fn audit;
    void *audit_context;
} x11_sec_context;

void x11_sec_policy_default(x11_sec_policy *policy);
int x11_sec_init(x11_sec_context *context,
                 const x11_sec_policy *policy,
                 const uint8_t *expected_token,
                 size_t expected_token_length,
                 x11_sec_audit_fn audit,
                 void *audit_context);

/* Returns ALLOW only when every configured security check succeeds. */
enum x11_sec_decision x11_sec_authorize(x11_sec_context *context,
                                         const x11_sec_client *client,
                                         const x11_sec_request *request,
                                         uint64_t now_ns,
                                         enum x11_sec_reason *reason);

const char *x11_sec_reason_string(enum x11_sec_reason reason);

#ifdef __cplusplus
}
#endif

#endif
