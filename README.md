# x11-sec-shim

> A small, dependency-free C11 policy gate for X11-facing clients.

[![Language](https://img.shields.io/badge/language-C11-00599C?logo=c)](https://en.cppreference.com/w/c/11)
[![Policy](https://img.shields.io/badge/policy-deny--by--default-8A2BE2)](#security-model)
[![Build](https://img.shields.io/badge/build-make-2E8B57?logo=gnu)](#build)
[![License](https://img.shields.io/badge/license-see%20repository-555)](LICENSE)

`x11-sec-shim` is a transport-agnostic security boundary written in portable C. An adapter can translate X11 connection and request metadata into the shim's small data structures, then allow an operation only when every configured check succeeds.

It does **not** replace Xlib/XCB, implement an X11 server, or open a display by itself. It is deliberately a policy component that can sit between an X11 integration and the action it is about to perform.

## Highlights

- **Deny by default** with explicit operation allowlists.
- **Authentication and constant-time token comparison** for bounded tokens.
- **Origin checks** for local Unix sockets and loopback TCP connections.
- **Request bounds** for payloads and resource counts.
- **Per-context rate limiting** with a monotonic timestamp supplied by the caller.
- **Audit callbacks** with fail-closed behavior when configured.
- **No runtime or external library dependency** beyond the C standard library.
- **Small public API** that is straightforward to wrap from another language or transport.

## Build

Requirements: a C11 compiler, `make`, and `ar`.

```sh
make
make test
```

The build produces a static library (`libx11_sec_shim.a`) and the executable test runner (`x11_sec_shim_test`). Strict warnings are enabled by default.

Useful targets:

```sh
make clean                 # remove generated objects and binaries
make test                  # build and run the test suite
make check                 # clean, build, and run tests
make SANITIZE=1 check     # run with AddressSanitizer/UBSan when supported
```

## Quick start

```c
#include "x11_sec_shim.h"

x11_sec_policy policy;
x11_sec_context context;
uint8_t token[] = { 0x42, 0x43, 0x44, 0x45 };

auto /* replace with your C declarations */;
```

The actual integration sequence is:

1. Call `x11_sec_policy_default()` and narrow or expand the operation mask deliberately.
2. Call `x11_sec_init()` once for the policy context.
3. Translate each request into `x11_sec_client` and `x11_sec_request`.
4. Call `x11_sec_authorize()` before performing the operation.
5. Proceed only when it returns `X11_SEC_ALLOW`.
6. Treat every other result as a denial and retain the audit record.

See [`x11_sec_shim.h`](x11_sec_shim.h) and [`docs/INTEGRATION.md`](docs/INTEGRATION.md) for the complete API contract.

## Security model

The default policy permits only property reads and input selection from authenticated local Unix clients. Loopback TCP is disabled. Requests are bounded to 1 MiB and 256 resources, and the default rate limit is 100 requests per one-second window.

The shim is intentionally **not** a cryptographic protocol. Tokens must be provisioned and protected by the embedding application; transport confidentiality, replay prevention, credential verification, and X11 protocol parsing remain adapter responsibilities.

### Important boundaries

- This project does not transmit input, capture keystrokes, or access an X11 display on its own.
- `X11_SEC_OP_INJECT_INPUT`, grabs, writes, and resource creation are not enabled by the default policy.
- A successful decision means that the supplied metadata passed policy checks; it is not proof that an operation is safe in every environment.
- Integrators must validate their X11 protocol fields before constructing a request.
- Use only with displays and clients you own or are authorized to administer.

## API surface

| API | Purpose |
| --- | --- |
| `x11_sec_policy_default` | Initialize conservative defaults |
| `x11_sec_init` | Create an authorization context |
| `x11_sec_authorize` | Evaluate one client request |
| `x11_sec_reason_string` | Convert a denial reason to stable text |
| `x11_sec_policy_validate` | Validate a policy before deployment |
| `x11_sec_policy_summary` | Produce a bounded human-readable policy summary |

The policy health helpers are implemented in [`x11_sec_health.c`](x11_sec_health.c) and are useful for startup diagnostics without exposing internal transport details.

## Project layout

```text
.
├── x11_sec_shim.[ch]       core authorization API
├── x11_sec_health.[ch]     policy validation and diagnostics
├── x11_sec_*.[ch]           focused ACL, audit, client, token, and runtime helpers
├── x11_sec_config.h         shared conservative limits
├── x11_sec_shim_test.c      self-contained regression suite
├── docs/INTEGRATION.md      adapter and embedding guidance
├── docs/SECURITY.md         threat model and security checklist
└── Makefile                 reproducible native build
```

## Testing

The test runner covers the allow path and important fail-closed paths: invalid initialization, missing authentication, bad tokens, untrusted origins, denied operations, malformed bounds, rate limiting, audit failures, policy validation, and summary output.

```sh
make check
```

For memory and undefined-behavior checks:

```sh
make SANITIZE=1 check
```

Sanitizer availability depends on the compiler. The normal build remains dependency-free.

## Contributing

Keep changes small and reviewable. New checks should be fail-closed, bounded, deterministic, and covered by a regression test. Do not add display access, network listeners, input injection, or transmission behavior to the policy library.

Please read [`docs/SECURITY.md`](docs/SECURITY.md) before reporting a security issue or integrating the shim into a privileged process.

## License

See [`LICENSE`](LICENSE). The repository is provided as a focused security component, not as a guarantee that an embedding application is secure.
