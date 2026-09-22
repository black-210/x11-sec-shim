#include "x11_sec_socket.h"

int x11_sec_socket_kind(const x11_sec_client *client, x11_sec_socket_kind *kind)
{
    if (client == NULL || kind == NULL) {
        return 0;
    }
    if (client->unix_socket != 0u) {
        *kind = X11_SEC_SOCKET_KIND_LOCAL;
        return 1;
    }
    if (client->loopback != 0u) {
        *kind = X11_SEC_SOCKET_KIND_TCP;
        return 1;
    }
    *kind = X11_SEC_SOCKET_KIND_INVALID;
    return 0;
}

int x11_sec_socket_allowed(const x11_sec_policy *policy, const x11_sec_client *client)
{
    x11_sec_socket_kind kind;

    if (policy == NULL || client == NULL) {
        return 0;
    }
    if (!x11_sec_socket_kind(client, &kind)) {
        return 0;
    }
    if (kind == X11_SEC_SOCKET_KIND_LOCAL) {
        return policy->allow_local_unix != 0u;
    }
    if (kind == X11_SEC_SOCKET_KIND_TCP) {
        return policy->allow_tcp_loopback != 0u;
    }
    return 0;
}
