#ifndef X11_SEC_SOCKET_H
#define X11_SEC_SOCKET_H

#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum x11_sec_socket_kind {
    X11_SEC_SOCKET_KIND_INVALID = 0,
    X11_SEC_SOCKET_KIND_LOCAL,
    X11_SEC_SOCKET_KIND_TCP
} x11_sec_socket_kind;

int x11_sec_socket_kind(const x11_sec_client *client, x11_sec_socket_kind *kind);
int x11_sec_socket_allowed(const x11_sec_policy *policy, const x11_sec_client *client);

#ifdef __cplusplus
}
#endif

#endif
