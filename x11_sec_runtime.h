#ifndef X11_SEC_RUNTIME_H
#define X11_SEC_RUNTIME_H

#include "x11_sec_logger.h"
#include "x11_sec_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct x11_sec_runtime {
    x11_sec_context context;
    x11_sec_policy policy;
    x11_sec_logger logger;
    uint8_t token[X11_SEC_MAX_TOKEN_BYTES];
    size_t token_length;
} x11_sec_runtime;

int x11_sec_runtime_init(x11_sec_runtime *runtime,
                        const x11_sec_policy *policy,
                        const uint8_t *token,
                        size_t token_length,
                        FILE *stream);

enum x11_sec_decision x11_sec_runtime_authorize(x11_sec_runtime *runtime,
                                               const x11_sec_client *client,
                                               const x11_sec_request *request,
                                               uint64_t now_ns,
                                               enum x11_sec_reason *reason);

#ifdef __cplusplus
}
#endif

#endif
