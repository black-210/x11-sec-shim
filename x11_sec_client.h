#ifndef X11_SEC_CLIENT_H
#define X11_SEC_CLIENT_H

#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

int x11_sec_client_init(x11_sec_client *client,
                        const char *client_id,
                        const char *resource_id,
                        const uint8_t *token,
                        size_t token_length,
                        uint8_t authenticated,
                        uint8_t unix_socket,
                        uint8_t loopback);
int x11_sec_client_valid(const x11_sec_client *client);

#ifdef __cplusplus
}
#endif

#endif
