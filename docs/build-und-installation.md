# Build und Installation

[Dokumentationsindex](README.md) · [Beispiele](examples/README.md) · [Repositories](repositories.md)

## Voraussetzungen

Der native Build benötigt CMake 3.20 oder neuer sowie einen C11- und
C++17-Compiler. Die ANTLR-4.7.1-Runtime, generierten Parser und Sources liegen
im Repository. Java ist nur für eine absichtliche Parser-Regeneration nötig.

Bei aktivierter nativer Repository-Unterstützung wird pugixml 1.14 aus einem
per SHA256 gepinnten Quellarchiv statisch gebaut. Im normalen Modus wird eine
installierte libcurl verwendet; libxml2 wird nicht benötigt. Im Static-Modus
wird auch curl 8.10.1 aus einem gepinnten Quellarchiv gebaut.

## Normaler Build

```sh
cmake -S . -B build/native \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON
cmake --build build/native --parallel
ctest --test-dir build/native --output-on-failure
build/native/ilic -version
```

Wichtige Artefakte sind `ilic`, `ilic-format`, `libilic-core.a`,
`libilic-capi.a` und bei nativer Repository-Unterstützung
`libilic-repository.a`.

## Core-only

```sh
cmake -S . -B build/core-only \
  -DCMAKE_BUILD_TYPE=Release \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF
cmake --build build/core-only --parallel
```

Dieser Modus konfiguriert weder curl noch pugixml und baut kein
`ilic-repository`. Compiler-Core, C-API, Formatter, lokale `-ilidirs`-Suche und
die zugehörigen Tests bleiben verfügbar. Repository-Optionen stehen in diesem
Binary nicht als Funktion zur Verfügung.

## Statische Distribution

```sh
cmake -S . -B build/static \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_STATIC_DISTRIBUTION=ON
cmake --build build/static --parallel
ctest --test-dir build/static --output-on-failure
```

Der Modus baut HTTP(S)-only curl statisch und deaktiviert unnötige Protokolle
und optionale Kompressions-/Netzwerkabhängigkeiten. TLS verwendet auf macOS
Secure Transport, auf Windows Schannel und im Linux-Release statische OpenSSL-
Libraries. Die MSVC-Runtime wird über `CMAKE_MSVC_RUNTIME_LIBRARY` statisch
gelinkt; Linux erhält einen vollständig statischen Link.

Unterstützte Release-Artefakte:

| Plattform | Artefakt | Prüfung |
| --- | --- | --- |
| macOS ARM64 | `ilic-macos-arm64.tar.gz` | nur Apple-Systemframeworks und `/usr/lib` laut `otool -L` |
| Linux x86_64 (musl) | `ilic-linux-x86_64.tar.gz` | kein dynamischer Interpreter und keine `NEEDED`-Einträge |
| Windows x86_64 | `ilic-windows-x86_64.zip` | keine curl-, XML- oder MSVC-Runtime-DLL |

Die Prüfskripte sind `scripts/check-macos-runtime-deps.sh`,
`scripts/check-linux-static.sh` und `scripts/check-windows-runtime-deps.ps1`.
Der Workflow `.github/workflows/build-native-release.yml` baut, testet,
smoke-testet und paketiert alle drei Plattformen. macOS x86_64 gehört nicht zur
Release-Matrix.

## Parser-Regeneration

```sh
cmake --build build/native --target check-parser-regeneration
```

Das Target verwendet den mitgelieferten ANTLR-4.7.1-Generator in einem
temporären Buildverzeichnis und überschreibt keine eingecheckten Parserdateien.

## Emscripten SDK einmalig installieren

Die unterstützte Version steht in `.emscripten-version`. Das SDK kann außerhalb
des Repositories installiert und für das aktuelle Terminal aktiviert werden:

```sh
export ILIC_EMSDK_DIR=/pfad/zu/emsdk
git clone https://github.com/emscripten-core/emsdk.git "$ILIC_EMSDK_DIR"
ILIC_EMSCRIPTEN_VERSION="$(tr -d '[:space:]' < .emscripten-version)"
"$ILIC_EMSDK_DIR/emsdk" install "$ILIC_EMSCRIPTEN_VERSION"
"$ILIC_EMSDK_DIR/emsdk" activate "$ILIC_EMSCRIPTEN_VERSION"
source "$ILIC_EMSDK_DIR/emsdk_env.sh"
```

Die Aktivierung verändert nur die aktuelle Shell und muss in einem neuen
Terminal wiederholt werden.

## WebAssembly

Mit aktivierter, zu `.emscripten-version` passender Umgebung:

```sh
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Erzeugt werden `build/wasm/ilic.mjs`, `build/wasm/ilic.wasm` und Kopien im
Paket `packages/compiler-wasm`. Der WASM-Build hängt nicht von der nativen
Repository-Library ab; Repository-Zugriffe übernimmt `@ilic/tools` im Host.

## npm-Pakete

Nach dem WASM-Build lassen sich Snapshot-Tarballs reproduzierbar vorbereiten
und als echte Konsumenten testen:

```sh
node scripts/prepare-npm-snapshot.mjs
node scripts/test-npm-packages.mjs
```

Details zu Versionierung, OIDC und Veröffentlichung stehen unter
[npm-Publikation](npm-publikation.md).

## Testgruppen

```sh
ctest --test-dir build/native --output-on-failure
npm test --prefix packages/tools
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Die normale CTest-Suite benötigt für Repository-Tests kein externes Netzwerk;
sie verwendet lokale gemeinsame Fixtures und Fake-Transporte.
