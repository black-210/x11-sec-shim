#include "x11_sec_health.h"
#include "x11_sec_config.h"

#include <stdio.h>
#include <string.h>

static int write_detail(char *detail, size_t size, const char *message)
{
    if (detail != NULL && size != 0u) {
        (void)snprintf(detail, size, "%s", message);
    }
    return 1;
}

int x11_sec_policy_validate(const x11_sec_policy *policy,
                            char *detail,
                            size_t detail_size)
{
    if (policy == NULL) {
        return -1;
    }
    if (policy->allowed_operations == 0u) {
        return write_detail(detail, detail_size, "no operations are enabled");
    }
    if (policy->requests_per_window == 0u) {
        return write_detail(detail, detail_size, "request limit must be non-zero");
    }
    if (policy->window_ns == 0u) {
        return write_detail(detail, detail_size, "time window must be non-zero");
    }
    if (policy->require_authentication > 1u ||
        policy->allow_local_unix > 1u ||
        policy->allow_tcp_loopback > 1u ||
        policy->fail_closed_on_audit_error > 1u) {
        return write_detail(detail, detail_size, "policy flags must be zero or one");
    }
    if (detail != NULL && detail_size != 0u) {
        (void)snprintf(detail, detail_size, "%s", "policy is valid");
    }
    return 0;
}

int x11_sec_policy_summary(const x11_sec_policy *policy,
                           char *output,
                           size_t output_size)
{
    if (policy == NULL || output == NULL || output_size == 0u) {
        return -1;
    }
    (void)snprintf(output, output_size,
                   "ops=0x%08x auth=%u unix=%u loopback=%u limit=%u window_ns=%llu audit_fail_closed=%u",
                   (unsigned)policy->allowed_operations,
                   (unsigned)policy->require_authentication,
                   (unsigned)policy->allow_local_unix,
                   (unsigned)policy->allow_tcp_loopback,
                   (unsigned)policy->requests_per_window,
                   (unsigned long long)policy->window_ns,
                   (unsigned)policy->fail_closed_on_audit_error);
    return 0;
}
