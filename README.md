# INTERLIS vcpkg registry

This branch is a Git-backed vcpkg registry for native INTERLIS packages.

Current package:

- `ilic` `0.10.0-snapshot.bb284a67`

The initial ilic port is library-only and exposes `ilic::core` and `ilic::capi`.
It supports `x64-linux`, `arm64-osx`, and `x64-windows` and is licensed under MIT.

Consumers should reference this repository as a Git registry and pin a specific
registry baseline commit in `vcpkg-configuration.json`.
