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

The overlay is the recipe-validation stage. The next step is to publish this
port in a small git-backed INTERLIS vcpkg registry and use a shared GitHub
Packages/NuGet binary cache for the supported triplets. That will let downstream
repositories restore matching ilic binaries instead of rebuilding ilic.

ilic is licensed under the MIT License. The vcpkg port declares `MIT` and
installs the corresponding copyright file, so it no longer relies on a
copyright-check bypass.
