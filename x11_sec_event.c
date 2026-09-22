#include "x11_sec_event.h"

int x11_sec_event_known(uint32_t operation)
{
    return operation != 0u;
}

int x11_sec_event_allowed(uint32_t operation, uint32_t allowed_mask)
{
    if (!x11_sec_event_known(operation)) {
        return 0;
    }
    return (operation & ~allowed_mask) == 0u;
}
