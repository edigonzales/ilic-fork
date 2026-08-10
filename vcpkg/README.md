# ilic vcpkg support

This directory contains the native vcpkg packaging boundary for ilic. It is
additive: existing `add_subdirectory()` and FetchContent consumers do not need
to change.

## Current scope

The `vcpkg/ports/ilic` port builds the native library package only:

- `ilic::core`
- `ilic::capi`
- bundled ANTLR runtime and JSON implementation details

The CLI and native INTERLIS repository support are disabled for this initial
port because the first downstream target (`iox-cpp`) only needs the compiler
library.

The source revision is pinned to an immutable ilic commit. This is important
for reproducible builds and for vcpkg binary-cache ABI keys.

## Local overlay use

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

## Git registry

The repository branch `vcpkg-registry` is a git-backed vcpkg registry. A
consumer selects that branch with the registry `reference` field and pins an
immutable registry commit in the `baseline` field. Packages not owned by the
ilic registry continue to come from a pinned builtin vcpkg registry.

This is the intended consumption path for downstream repositories once they
move away from embedding ilic source directly.

## Shared binary cache

`.github/workflows/vcpkg-binary-cache.yml` publishes vcpkg binary packages for:

- `x64-linux`
- `arm64-osx`
- `x64-windows`

The cache uses the GitHub Packages NuGet feed for the `edigonzales` namespace.
Publishing from this repository uses the workflow `GITHUB_TOKEN` with
`packages: write`. The workflow also performs a fresh Linux restore after the
three publish jobs, proving that the binary package can be downloaded from the
remote feed instead of rebuilt locally.

For a downstream repository, read access can later be granted to its GitHub
Actions workflow through the package's Actions access settings. A classic PAT
with `read:packages` can be used instead when cross-repository package access
cannot or should not be granted directly.

ilic is licensed under the MIT License. The vcpkg port declares `MIT` and
installs the corresponding copyright file.
