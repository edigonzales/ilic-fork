# ilic vcpkg support

This directory contains the first vcpkg packaging boundary for native ilic.
It is intentionally additive: existing `add_subdirectory()` and FetchContent
consumers do not need to change.

## Current scope

The `vcpkg/ports/ilic` overlay builds the native library package only:

- `ilic::core`
- `ilic::capi`
- bundled ANTLR runtime and JSON implementation details

The CLI and native INTERLIS repository support are disabled for this initial
port because the first downstream target (`iox-cpp`) only needs the compiler
library.

The source revision is pinned to an immutable ilic commit. This is important
for reproducible builds and for vcpkg binary-cache ABI keys.

## Local use

With a vcpkg checkout in `$VCPKG_ROOT`:

```sh
$VCPKG_ROOT/vcpkg install ilic:x64-linux \
  --overlay-ports="$PWD/vcpkg/ports"
```

Use `arm64-osx` on Apple Silicon macOS and `x64-windows` on 64-bit Windows.
A CMake consumer can then use the normal vcpkg toolchain and:

```cmake
find_package(ilic CONFIG REQUIRED)
target_link_libraries(app PRIVATE ilic::core)
```

## Next packaging step

The overlay is the recipe-validation stage. The intended next step is to move
this recipe into a small git-backed INTERLIS vcpkg registry and publish vcpkg
binary-cache packages for the supported triplets. That will let downstream
repositories restore matching ilic binaries instead of rebuilding ilic.

The repository currently has no project-level `LICENSE` file. The overlay
therefore does not claim an SPDX license and temporarily skips vcpkg's
copyright-file post-build check. This must be resolved before treating the
port as a generally redistributable registry package.
