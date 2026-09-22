#ifndef X11_SEC_ACL_H
#define X11_SEC_ACL_H

#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

int x11_sec_acl_operation_allowed(const x11_sec_policy *policy,
                                  uint32_t operation);
int x11_sec_acl_origin_allowed(const x11_sec_policy *policy,
                               const x11_sec_client *client);

#ifdef __cplusplus
}
#endif

#endif
