#ifndef X11_SEC_POLICY_H
#define X11_SEC_POLICY_H

#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

void x11_sec_policy_apply_defaults(x11_sec_policy *policy);
int x11_sec_policy_validate(const x11_sec_policy *policy);
int x11_sec_policy_is_strict(const x11_sec_policy *policy);

#ifdef __cplusplus
}
#endif

#endif
