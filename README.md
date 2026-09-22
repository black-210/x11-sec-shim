# x11-sec-shim

A strict X11-oriented security shim implemented in pure C.

This project is intentionally C-only. No C#, no managed runtime, and no non-C build targets.

Build:

  make

Run tests:

  ./x11_sec_shim_test

The shim enforces a deny-by-default model for X11 client operations and is designed to be used as a policy gate around X11 requests, windows, and client resources.
