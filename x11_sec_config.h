# Conservative protocol-side limits for the C-only X11 security boundary.
#ifndef X11_SEC_CONFIG_H
#define X11_SEC_CONFIG_H

#define X11_SEC_MAX_PAYLOAD_BYTES 1048576u
#define X11_SEC_MAX_RESOURCES_PER_REQUEST 256u
#define X11_SEC_DEFAULT_REQUEST_LIMIT 100u
#define X11_SEC_DEFAULT_WINDOW_NS 1000000000ULL
#define X11_SEC_BUILD_LANGUAGE "C11"

#endif
