#ifndef X11_SEC_RATE_H
#define X11_SEC_RATE_H

#include <stdint.h>

typedef struct x11_sec_rate {
    uint64_t started_ns;
    uint32_t count;
} x11_sec_rate;

void x11_sec_rate_reset(x11_sec_rate *rate, uint64_t now_ns);
int x11_sec_rate_take(x11_sec_rate *rate, uint64_t now_ns,
                      uint64_t window_ns, uint32_t limit);

#endif
