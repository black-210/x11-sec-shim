#include "x11_sec_logger.h"

#include <string.h>

void x11_sec_logger_init(x11_sec_logger *logger, FILE *stream)
{
    if (logger == NULL) {
        return;
    }
    logger->stream = stream == NULL ? stderr : stream;
}

void x11_sec_logger_log(x11_sec_logger *logger,
                        const char *component,
                        const char *message)
{
    if (logger == NULL || logger->stream == NULL || component == NULL || message == NULL) {
        return;
    }
    fprintf(logger->stream, "[%s] %s\n", component, message);
}
