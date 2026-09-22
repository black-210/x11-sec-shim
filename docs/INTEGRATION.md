# Integration guide

## Adapter responsibilities

`x11-sec-shim` intentionally knows nothing about Xlib or XCB. An adapter is responsible for:

1. Parsing and validating the incoming X11 event or request.
2. Establishing the client identity and authentication state.
3. Setting exactly one trusted origin flag (`unix_socket` or `loopback`) where applicable.
4. Mapping the operation to one `x11_sec_operation` bit.
5. Supplying bounded payload and resource counts.
6. Calling `x11_sec_authorize()` immediately before the protected action.

Do not treat an omitted field as safe. Zero values are rejected for required display/resource metadata and unknown operation bits are denied.

## Initialization

```c
x11_sec_policy policy;
x11_sec_context context;
const uint8_t token[] = { 0x42, 0x43, 0x44, 0x45 };

x11_sec_policy_default(&policy);
if (x11_sec_policy_validate(&policy, NULL, 0) != 0) {
    /* Configuration is unsafe or incomplete. Stop startup. */
}
if (x11_sec_init(&context, &policy, token, sizeof(token), audit_fn, audit_ctx) != 0) {
    /* Do not start the adapter with a partially initialized context. */
}
```

The expected token is copied into the context. The caller still owns the input buffer and should clear temporary secret material when appropriate.

## Authorization loop

```c
enum x11_sec_reason reason;
if (x11_sec_authorize(&context, &client, &request, monotonic_now_ns, &reason)
        != X11_SEC_ALLOW) {
    log_denial(x11_sec_reason_string(reason));
    return DENY;
}
perform_already_validated_x11_operation();
```

Use a monotonic clock. The library does not read the clock, so callers can test deterministic timestamps and avoid wall-clock rollback issues.

## Policy diagnostics

At startup, call `x11_sec_policy_summary()` and record the result with the application configuration. The function never allocates memory and always NUL-terminates a sufficiently sized output buffer.

A policy validation failure must stop startup. Validation checks include a non-empty operation mask, non-zero rate limits, a non-zero time window, and flags containing only `0` or `1`.

## Audit callback rules

Audit callbacks should be fast, non-blocking, and safe when passed a denied request. They must not mutate the record. Returning non-zero signals an audit failure; with the default policy this remains fail-closed.

## Scope reminder

The shim is a policy gate, not a complete X11 security architecture. Use OS credentials, transport protections, privilege separation, and protocol-specific validation in the adapter.
