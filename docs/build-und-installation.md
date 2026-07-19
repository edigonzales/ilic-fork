# Build und Installation

[Dokumentationsindex](README.md) · [Beispiele](examples/README.md) · [WASM](wasm.md)

## Voraussetzungen für den nativen Build

- CMake 3.20 oder neuer;
- ein C11- und C++17-Compiler;
- libcurl und libxml2 für die native Repository-Unterstützung;
- Git nur zum Beziehen des Quellcodes.

Der ANTLR-4.7.1-C++-Runtime und die generierten Parser sind eingecheckt. Ein
normaler Build und der laufende Compiler benötigen weder Java noch ein
installiertes ANTLR. Java ist nur für eine absichtliche Parser-Regeneration und
für die externe Conformance-Suite nötig.

## Nativer Debug-Build

```sh
cmake -S . -B build/macos \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON
cmake --build build/macos --parallel
ctest --test-dir build/macos --output-on-failure
build/macos/ilic -version
```

Wichtige Artefakte:

| Artefakt | Zweck |
| --- | --- |
| `build/macos/ilic` | Compiler-CLI |
| `build/macos/ilic-format` | Formatter-CLI |
| `build/macos/libilic-core.a` | C++-Compiler-Core |
| `build/macos/libilic-capi.a` | statische C-ABI |
| `build/macos/libilic-repository.a` | native Repository-Library |

Der Ordnername `build/macos` ist keine CMake-Vorgabe. Auf Linux kann
beispielsweise `build/linux` verwendet werden.

## Release-Build

```sh
cmake -S . -B build/release \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF
cmake --build build/release --parallel
```

Repository-Unterstützung lässt sich für einen Core-only-Build deaktivieren:

```sh
cmake -S . -B build/core-only \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF
cmake --build build/core-only --parallel
```

Dann fehlen `-repositories`, `-models` und `ilic-repository`; die lokale Suche
mit `-ilidirs` bleibt Bestandteil des Compilers.

## Parser-Regeneration

Die Grammatiken liegen unter `source/input/parser/grammar`, die erzeugten
Dateien unter `source/input/parser/generated`. Die normale Buildkette verändert
diese Dateien nicht. Der reproduzierbare Vergleich verwendet exakt den
mitgelieferten ANTLR-Generator:

```sh
cmake --build build/macos --target check-parser-regeneration
```

Der Target generiert in einem temporären Buildverzeichnis und meldet Diffs,
ohne eingecheckte Parserdateien zu überschreiben.

## WebAssembly

Die unterstützte Emscripten-Version steht in `.emscripten-version` und ist
aktuell `3.1.64`.

### Emscripten SDK einmalig installieren

Das offizielle `emsdk` kann an einem beliebigen Ort ausserhalb dieses
Repositories installiert werden. Im folgenden Beispiel müssen nur die beiden
Pfade angepasst werden:

```sh
cd /pfad/zu/ilic-fork
export ILIC_EMSDK_DIR=/pfad/zu/emsdk

git clone https://github.com/emscripten-core/emsdk.git "$ILIC_EMSDK_DIR"

ILIC_EMSCRIPTEN_VERSION="$(tr -d '[:space:]' < .emscripten-version)"
"$ILIC_EMSDK_DIR/emsdk" install "$ILIC_EMSCRIPTEN_VERSION"
"$ILIC_EMSDK_DIR/emsdk" activate "$ILIC_EMSCRIPTEN_VERSION"
source "$ILIC_EMSDK_DIR/emsdk_env.sh"

emcc --version
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

`emsdk install` lädt die gepinnte Toolchain herunter. `emsdk activate` wählt
sie innerhalb der SDK-Installation aus. `source emsdk_env.sh` ergänzt unter
anderem `PATH` nur für das aktuelle Terminal. In jedem neu geöffneten Terminal
muss die Umgebung deshalb erneut geladen werden:

```sh
cd /pfad/zu/ilic-fork
export ILIC_EMSDK_DIR=/pfad/zu/emsdk
source "$ILIC_EMSDK_DIR/emsdk_env.sh"
emcc --version
```

Eine dauerhaft globale Aktivierung ist nicht erforderlich. Dadurch können
andere Projekte weiterhin eine andere Emscripten-Version verwenden.

### WASM bauen

Mit aktivierter Emscripten-Umgebung wird der Build aus dem Projektverzeichnis
gestartet:

```sh
cd /pfad/zu/ilic-fork
source /pfad/zu/emsdk/emsdk_env.sh

./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

`scripts/build-wasm.sh` vergleicht `emcc --version` mit
`.emscripten-version` und bricht ab, falls eine andere Version aktiv ist.

Erzeugt werden:

- `build/wasm/ilic.mjs`;
- `build/wasm/ilic.wasm`;
- lokale Kopien beider Dateien in `packages/compiler-wasm`.

Die WASM-Dateien sind Buildartefakte und werden nicht eingecheckt.

## Lokale npm-Pakete

Für lokale und publizierbare Tarballs müssen zuerst die WASM-Artefakte gebaut
werden. Das Snapshot-Staging liest die CMake-Projektversion, ergänzt einen
gemeinsamen UTC-Zeitstempel und verändert die Quell-Manifeste nicht:

```sh
./scripts/build-wasm.sh
node scripts/prepare-npm-snapshot.mjs
node scripts/test-npm-packages.mjs
```

Die geprüften Tarballs liegen danach unter `build/npm/tarballs`. Ein anderes
Projekt kann sie direkt installieren:

```sh
npm install /pfad/zu/build/npm/tarballs/ilic-compiler-wasm-0.9.9-SNAPSHOT.ZEITSTEMPEL.tgz
npm install /pfad/zu/build/npm/tarballs/ilic-tools-0.9.9-SNAPSHOT.ZEITSTEMPEL.tgz
```

Die öffentliche Snapshot-Publikation, der einmalige Bootstrap und die
OIDC-Authentisierung sind unter [npm-Publikation](npm-publikation.md)
dokumentiert. Bis dieser Bootstrap auf npmjs.com durchgeführt wurde, bleiben
die lokalen Tarballs der verlässliche Installationsweg.

Für die Entwicklung im Checkout importieren die
[JavaScript-Beispiele](examples/README.md#webassembly-und-javascript) direkt aus
`packages/`.

## Testgruppen

```sh
# Native Unit-, Regression-, Repository- und Dokumentationstests
ctest --test-dir build/macos --output-on-failure

# Repository-Hostbibliothek
npm test --prefix packages/tools

# Reales WASM und dokumentierte JS-Beispiele
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Die externe 571-Fälle-Messung ist getrennt und wird unter
[Compiler-Conformance](conformance.md) beschrieben.
