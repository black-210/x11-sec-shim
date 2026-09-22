#include "x11_sec_policy.h"

#include <string.h>

void x11_sec_policy_apply_defaults(x11_sec_policy *policy)
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

int x11_sec_policy_validate(const x11_sec_policy *policy)
{
    if (policy == NULL) {
        return 0;
    }
    if (policy->allowed_operations == 0u) {
        return 0;
    }
    if (policy->requests_per_window == 0u || policy->window_ns == 0u) {
        return 0;
    }
    return 1;
}

int x11_sec_policy_is_strict(const x11_sec_policy *policy)
{
    if (policy == NULL) {
        return 0;
    }
    return policy->require_authentication != 0u &&
           policy->allow_tcp_loopback == 0u &&
           policy->fail_closed_on_audit_error != 0u;
}
