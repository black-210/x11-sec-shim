#ifndef X11_SEC_EVENT_H
#define X11_SEC_EVENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int x11_sec_event_known(uint32_t operation);
int x11_sec_event_allowed(uint32_t operation, uint32_t allowed_mask);

#ifdef __cplusplus
}
#endif

#endif
