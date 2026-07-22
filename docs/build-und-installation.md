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

## ANTLR-Runtime

Die ANTLR-4.7.1-C++-Runtime ist im Repository enthalten. Unter
`lib/antlr4/include` liegen sowohl die Header als auch die zugehörigen
`.cpp`-Quelldateien. Der Name des Verzeichnisses ist historisch; es handelt
sich nicht nur um Header-Dateien und es wird keine ANTLR-Runtime aus dem
Internet geladen.

Der normale CMake-Build kompiliert diese Quelldateien selbst als statische
Bibliothek `antlr4-runtime`. Das daraus entstehende `libantlr4-runtime.a`
beziehungsweise `antlr4-runtime.lib` ist ein Buildartefakt; zur Laufzeit muss
keine separate ANTLR-DLL installiert werden.

`lib/antlr4/bin.zip` enthält die zum ursprünglichen Visual-Studio-Build
gehörigen vorgebauten statischen Bibliotheken sowie den passenden
ANTLR-4.7.1-Java-Generator. Für einen normalen CMake-Build wird die
vorgebaute Runtime daraus nicht verwendet. Das Target
`check-parser-regeneration` extrahiert bei Bedarf nur den passenden Generator
in ein temporäres Verzeichnis. Dadurch bleiben die eingecheckten generierten
Parserdateien unverändert.

Die Header deklarieren öffentliche ANTLR-Symbole über `ANTLR4CPP_PUBLIC`.
`ANTLR4CPP_STATIC` teilt ihnen mit, dass diese Symbole aus einer statischen
Bibliothek kommen und nicht als `__declspec(dllimport)` aus einer DLL importiert
werden sollen. Die Definition wird in CMake als öffentliche
Target-Compile-Definition weitergereicht, damit Runtime und alle abhängigen
Targets dieselbe Symbolsemantik verwenden. Das Makro schaltet den statischen
Link nicht selbst ein; dafür sorgt `add_library(antlr4-runtime STATIC ...)`.

`ANTLR4CPP_STATIC` ist außerdem unabhängig von der statischen MSVC-Runtime:
Das ANTLR-Makro steuert nur die Export-/Import-Deklarationen der ANTLR-Symbole,
während `/MT` die Microsoft-C++-Runtime in das Programm einbindet.

## Windows-Build-Stack

Der aktuelle Fork baut Windows x86_64 über CMake und ein daraus generiertes
Visual-Studio-Projekt. Die GitHub-Actions konfigurieren mit `cmake -A x64` und
verwenden die MSVC-Toolchain des Windows-Runners. Für die statische
Distribution setzt CMake die statische MSVC-Runtime (`/MT`), baut curl mit
Windows Schannel und prüft anschließend mit
`scripts/check-windows-runtime-deps.ps1`, dass keine curl-, XML- oder
MSVC-Runtime-DLL neben dem Programm benötigt wird.
Die Prüfung benötigt kein vorher initialisiertes Visual-Studio-Terminal:
`dumpbin.exe` wird zuerst über `PATH` und andernfalls über `vswhere.exe` aus
dem Visual-Studio-Installer gesucht. Dafür müssen die Visual-Studio-C++-Tools
(`Microsoft.VisualStudio.Component.VC.Tools.x86.x64`) installiert sein.

Das ist ein anderer Buildpfad als im ursprünglichen
[`infogrips/ilic`](https://github.com/infogrips/ilic). Dort beschreibt
`build/vc2019/ilic.vcxproj` ein handgepflegtes Visual-Studio-Projekt mit dem
`ClangCL`-Toolset und referenziert vorgebaute
`antlr4-runtime.lib`-Dateien aus einem `Static`-Verzeichnis. Auch dieser
Upstream-Pfad ist statisch gelinkt; der Unterschied liegt in der Bereitstellung
der Runtime und in der Toolchain. Der Fork baut die bereits eingecheckten
ANTLR-Quellen reproduzierbar selbst und validiert den Windows-Build in der
CI.

Die Windows-CI hat dadurch Portabilitätsprobleme sichtbar gemacht, die im
vorbereiteten Upstream-Build nicht auftraten. `C2491` entstand, weil MSVC beim
Übersetzen der statischen ANTLR-Runtime ohne `ANTLR4CPP_STATIC` fälschlich
`dllimport`-Deklarationen sah. Der Fehler bei `std::replace` in
`RepositoryUri.cpp` war eine separate MSVC-Typüberladung zwischen `char` und
`wchar_t`. Beide Korrekturen betreffen die Kompatibilität des neuen
CMake/MSVC-Buildpfads und ändern nicht die öffentliche Compiler- oder
Repository-API.

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
