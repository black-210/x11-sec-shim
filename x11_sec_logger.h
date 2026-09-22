#ifndef X11_SEC_LOGGER_H
#define X11_SEC_LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct x11_sec_logger {
    FILE *stream;
} x11_sec_logger;

void x11_sec_logger_init(x11_sec_logger *logger, FILE *stream);
void x11_sec_logger_log(x11_sec_logger *logger,
                        const char *component,
                        const char *message);

#ifdef __cplusplus
}
#endif

#endif
