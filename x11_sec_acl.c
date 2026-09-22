#include "x11_sec_acl.h"

int x11_sec_acl_operation_allowed(const x11_sec_policy *policy,
                                  uint32_t operation)
{
    if (policy == NULL || operation == 0u ||
        (operation & ~policy->allowed_operations) != 0u) {
        return 0;
    }
    return 1;
}

int x11_sec_acl_origin_allowed(const x11_sec_policy *policy,
                               const x11_sec_client *client)
{
    if (policy == NULL || client == NULL) {
        return 0;
    }
    if (client->unix_socket != 0u) {
        return policy->allow_local_unix != 0u;
    }
    if (client->loopback != 0u) {
        return policy->allow_tcp_loopback != 0u;
    }
    return 0;
}
