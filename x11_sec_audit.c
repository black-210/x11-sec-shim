#include "x11_sec_audit.h"

#include <stdio.h>

int x11_sec_audit_format(const x11_sec_audit_record *record,
                         char *buffer, size_t buffer_size)
{
    int written;

    if (record == NULL || buffer == NULL || buffer_size == 0u) {
        return -1;
    }
    written = snprintf(buffer, buffer_size,
                       "ts=%llu decision=%d reason=%d op=%u client=%s detail=%s",
                       (unsigned long long)record->timestamp_ns,
                       record->decision, record->reason, record->operation,
                       record->client_id, record->detail);
    if (written < 0 || (size_t)written >= buffer_size) {
        buffer[0] = '\0';
        return -1;
    }
    return written;
}
