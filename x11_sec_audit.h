#ifndef X11_SEC_AUDIT_H
#define X11_SEC_AUDIT_H

#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

int x11_sec_audit_format(const x11_sec_audit_record *record,
                         char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
