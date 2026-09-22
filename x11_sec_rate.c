#include "x11_sec_rate.h"

void x11_sec_rate_reset(x11_sec_rate *rate, uint64_t now_ns)
{
    if (rate == NULL) {
        return;
    }
    rate->started_ns = now_ns;
    rate->count = 0u;
}

int x11_sec_rate_take(x11_sec_rate *rate, uint64_t now_ns,
                      uint64_t window_ns, uint32_t limit)
{
    if (rate == NULL || window_ns == 0u || limit == 0u) {
        return 0;
    }
    if (now_ns < rate->started_ns ||
        now_ns - rate->started_ns >= window_ns) {
        x11_sec_rate_reset(rate, now_ns);
    }
    if (rate->count >= limit) {
        return 0;
    }
    ++rate->count;
    return 1;
}
