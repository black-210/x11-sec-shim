#ifndef X11_SEC_HEALTH_H
#define X11_SEC_HEALTH_H

#include <stddef.h>
#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 0 for a deployable policy, -1 for invalid arguments, and 1 for an invalid policy. */
int x11_sec_policy_validate(const x11_sec_policy *policy,
                            char *detail,
                            size_t detail_size);

/* Writes a bounded, human-readable summary. Returns 0 on success, -1 on bad arguments. */
int x11_sec_policy_summary(const x11_sec_policy *policy,
                           char *output,
                           size_t output_size);

#ifdef __cplusplus
}
#endif

#endif
