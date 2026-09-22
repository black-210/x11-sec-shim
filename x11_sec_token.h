#ifndef X11_SEC_TOKEN_H
#define X11_SEC_TOKEN_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int x11_sec_token_equal(const uint8_t *actual, size_t actual_length,
                        const uint8_t *expected, size_t expected_length);
int x11_sec_token_valid_length(size_t length);

#ifdef __cplusplus
}
#endif

#endif
