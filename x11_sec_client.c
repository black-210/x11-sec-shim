#include "x11_sec_client.h"

#include <string.h>

int x11_sec_client_init(x11_sec_client *client,
                        const char *client_id,
                        const char *resource_id,
                        const uint8_t *token,
                        size_t token_length,
                        uint8_t authenticated,
                        uint8_t unix_socket,
                        uint8_t loopback)
{
    if (client == NULL || client_id == NULL || resource_id == NULL) {
        return 0;
    }
    memset(client, 0, sizeof(*client));
    if (snprintf(client->client_id, sizeof(client->client_id), "%s", client_id) < 0) {
        return 0;
    }
    if (snprintf(client->resource_id, sizeof(client->resource_id), "%s", resource_id) < 0) {
        return 0;
    }
    client->token = token;
    client->token_length = token_length;
    client->authenticated = authenticated;
    client->unix_socket = unix_socket;
    client->loopback = loopback;
    return 1;
}

int x11_sec_client_valid(const x11_sec_client *client)
{
    if (client == NULL) {
        return 0;
    }
    if (client->client_id[0] == '\0' || client->resource_id[0] == '\0') {
        return 0;
    }
    return 1;
}
