#include "x11_sec_token.h"

#include "x11_sec_shim.h"

int x11_sec_token_valid_length(size_t length)
{
    return length <= X11_SEC_MAX_TOKEN_BYTES ? 1 : 0;
}

int x11_sec_token_equal(const uint8_t *actual, size_t actual_length,
                        const uint8_t *expected, size_t expected_length)
{
    size_t i;
    uint8_t difference;

    if (!x11_sec_token_valid_length(actual_length) ||
        !x11_sec_token_valid_length(expected_length) ||
        (actual_length != 0u && actual == NULL) ||
        (expected_length != 0u && expected == NULL)) {
        return 0;
    }
    difference = (uint8_t)(actual_length ^ expected_length);
    for (i = 0; i < X11_SEC_MAX_TOKEN_BYTES; ++i) {
        const uint8_t a = i < actual_length ? actual[i] : 0u;
        const uint8_t b = i < expected_length ? expected[i] : 0u;
        difference = (uint8_t)(difference | (uint8_t)(a ^ b));
    }
    return difference == 0u;
}
