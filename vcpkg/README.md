# ilic vcpkg support

This directory contains the native vcpkg packaging boundary for ilic. It is
additive: existing `add_subdirectory()` and FetchContent consumers do not need
to change.

## Current scope

The `vcpkg/ports/ilic` port builds the native library package only:

- `ilic::core`
- `ilic::capi`
- bundled ANTLR runtime and JSON implementation details

The CLI and native INTERLIS repository support are disabled because the first
downstream target (`iox-cpp`) only needs the compiler library.

The in-tree port is also the template used for registry publication. Its
checked-in source revision remains immutable so local overlay builds are
reproducible, but published snapshot and release versions are rendered from
this template with their own immutable source commit and archive SHA512.

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
ilic registry continue to come from the pinned builtin vcpkg registry.

Published versions are immutable. Snapshot versions use
`X.Y.Z-snapshot.<short-source-sha>` and stable tags `vX.Y.Z` publish the vcpkg
version `X.Y.Z`. Older entries remain in `versions/i-/ilic.json` when the
baseline advances.

`.github/workflows/vcpkg-version-publish.yml` runs only after a successful
`Release native compiler` workflow. It:

1. resolves the exact released source commit and snapshot or stable version,
2. computes the GitHub source archive SHA512,
3. renders `ports/ilic` on the `vcpkg-registry` branch,
4. uses `vcpkg x-add-version` to add the new version and advance the baseline,
5. refuses to rewrite an already published version, and
6. dispatches binary-cache publication for that exact registry version.

This means registry publication follows the same successful snapshot/tag
release boundary as the native release artifacts without creating commits on
`main` and without introducing a release loop.

## Shared binary cache

`.github/workflows/vcpkg-binary-cache.yml` publishes binary packages for:

- `x64-linux`
- `arm64-osx`
- `x64-windows`

The workflow is normally started by the registry publisher through the
`vcpkg-version-published` repository-dispatch event. A manual dispatch remains
available for repairing or republishing a known immutable version by supplying
its version and registry baseline.

Each build consumes the requested version through the Git registry rather than
through the mutable in-tree overlay. The top-level temporary manifest uses a
vcpkg override to pin that exact version.

The cache uses the GitHub Packages NuGet feed for the `edigonzales` namespace.
Publishing from this repository uses the workflow `GITHUB_TOKEN` with
`packages: write`. After all three platform packages have been published, a
fresh Linux job runs vcpkg with `--only-binarycaching`; the verification fails
instead of falling back to a source build when the remote package is missing.

For a downstream repository, read access can later be granted to its GitHub
Actions workflow through the package's Actions access settings. A classic PAT
with `read:packages` can be used instead when cross-repository package access
cannot or should not be granted directly.

ilic is licensed under the MIT License. The vcpkg port declares `MIT` and
installs the corresponding copyright file.
