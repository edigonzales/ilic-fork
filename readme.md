# ilic - INTERLIS Compiler
Validates [INTERLIS](https://www.interlis.ch) data models. 
ilic can generate output in several formats (.log, .imd, .ili, .xsd, .gml) from INTERLIS 1.0, 2.3 and 2.4 input models.
ilic is maintained by [infoGrips Ltd. Zürich](https://www.infogrips.ch).

## License
Source files and binaries can be copied, changed, integrated in other software (commercial or non commercial) without any license restrictions. 
On foreign libraries contained in folder /lib some license restrictions may apply (see folder for details).

## Release platforms

The native release workflow produces self-contained compiler archives for
macOS ARM64, Linux x86_64 (fully static musl), and Windows x86_64. Runtime
dependency checks are part of each release job. macOS x86_64 is not a release
target.

## Installation
In order to install ilic, extract the [.zip](https://www.infogrips.ch/products/ilic.zip) file into a directory.

Current source builds and the three reproducible static-distribution modes are
documented in [Build und Installation](./docs/build-und-installation.md).

## Building on macOS

To build ilic from source on macOS, the following dependencies are required:

+ Xcode Command Line Tools, including Apple Clang and the C++ standard library
+ CMake 3.20 or newer

The ANTLR 4.7.1 C++ runtime is included in `lib/antlr4/include` and is compiled
statically as part of the build. No ANTLR runtime library is required when the
resulting `ilic` executable is run.

The checked-in generated parser sources are used for a normal build, so Java is
not required. A Java runtime or JDK is only needed when regenerating the parser
sources from the grammars. The matching ANTLR 4.7.1 generator is included in
`lib/antlr4/bin.zip`; do not use a different ANTLR version for this step.

For example, with the CMake app installed in `/Applications/CMake.app`:

```sh
/Applications/CMake.app/Contents/bin/cmake -S . -B build/macos \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON
/Applications/CMake.app/Contents/bin/cmake --build build/macos -j
/Applications/CMake.app/Contents/bin/ctest \
  --test-dir build/macos \
  --output-on-failure
build/macos/ilic -v
```

The CTest suite checks startup/version reporting, validation of a simple model,
model lookup through `-ilidirs`, XSD output generation, semantic crash
regressions, and `TRANSLATION OF` validation. The last command should report the
detected platform, for example `platform=macos64` on Apple Silicon.

The static library `libantlr4-runtime.a` is a link-time build artifact. It is
embedded into the `ilic` executable and is not required at runtime. The normal
build uses only the checked-in generated parser sources and therefore does not
require Java or an extracted ANTLR JAR.

Native repository support uses statically embedded pugixml instead of libxml2.
Normal developer builds link the system libcurl; release builds compile a
pinned HTTP(S)-only curl and select the platform TLS backend. Repository support
can be excluded completely with `-DILIC_ENABLE_NATIVE_REPOSITORY=OFF`.

### Checking parser regeneration

The parser sources in `source/input/parser/generated` were generated with the
bundled ANTLR 4.7.1 generator. Parser regeneration is deliberately not part of
the normal build: it requires Java and can change many generated source files.

To regenerate into a temporary build directory and compare the result with the
checked-in files, run:

```sh
/Applications/CMake.app/Contents/bin/cmake --build build/macos \
  --target check-parser-regeneration
```

The check extracts only `java/antlr-4.7.1-complete.jar` from
`lib/antlr4/bin.zip`, writes regenerated files below
`build/macos/parser-regeneration`, and never overwrites files in the source
tree. A difference is reported with the affected files; use the exact bundled
ANTLR version when a grammar change requires an intentional regeneration.

`test/ili23/Roads/RoadsExgm2ien.ili` is included as a regular regression test
for object-path resolution in graphics and views.

## Documentation

+ current user and developer documentation (German):
  - start with the [documentation index](./docs/README.md)
  - use the verified [CLI reference](./docs/cli.md) for command line options
  - see the [native APIs](./docs/native-api.md) and [WASM SDK](./docs/wasm.md) for embedding
  - see the [language-tooling snapshots](./docs/language-tooling-snapshots.md) for editor integrations
  - see the [build and publication pipeline](./docs/build-und-publikationspipeline.md) for CI and the coordinated release train
  - see [npm snapshot publication](./docs/npm-publikation.md) for package builds and authentication
+ reference material:
  - consult the [changelog](./doc/changelog.txt) for historical changes
  - INTERLIS manuals and legacy documentation remain in `doc/`

+ historical developer notes can be found [here](./doc/dev/readme.md)
