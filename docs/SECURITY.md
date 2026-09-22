# Security notes

## Threat model

The library is designed to reduce accidental or unauthorized execution of selected X11-facing operations after an adapter has produced request metadata. It helps with policy consistency, bounded inputs, origin restrictions, authentication state, and auditability.

It does not defend against a compromised process that bypasses the library, a malicious adapter that supplies false metadata, memory corruption elsewhere, or a stolen authentication token.

## Secure-by-default rules

- Start from `x11_sec_policy_default()`.
- Validate custom policies before installing them.
- Keep TCP loopback disabled unless there is a documented reason to enable it.
- Keep write, grab, input-injection, and resource-management operations disabled unless each has a reviewed use case.
- Use a strong, protected token provisioned by the embedding application.
- Keep audit failure fail-closed.
- Use monotonic timestamps and retain denial records.
- Do not log token bytes or other secrets.

## Reporting

When reporting a vulnerability, include the affected commit, compiler/platform, minimal reproducer, and whether the issue requires a malicious adapter or can be reached through the public API. Never include live credentials or private display data.

## Hardening checklist

- [ ] Compile with `-Wall -Wextra -Wpedantic`.
- [ ] Run `make SANITIZE=1 check` where supported.
- [ ] Run the complete regression suite in CI.
- [ ] Review every enabled operation bit.
- [ ] Confirm the adapter rejects malformed X11 fields before calling the shim.
- [ ] Confirm logs redact tokens and sensitive resource names.
- [ ] Confirm the process has the least privileges needed.
