# Spezifikation für einen LLM-Coding-Agenten: ilic 0.9.10 als erster koordinierter Release einschließlich Web IDE

**Status:** Überarbeitete Implementierungsspezifikation (Version 2)  
**Zieldatum der Spezifikation:** 2026-08-04  
**Zielversion:** `0.9.10`  
**Primärrepository:** `https://github.com/edigonzales/ilic-fork`  
**Betroffene Folgeprojekte:**

- `https://github.com/edigonzales/interlis-language-tools`
- `iox-cpp` aus dem bereitgestellten Repository-Archiv
- `https://github.com/edigonzales/interlis-web-ide`

Diese Spezifikation ist als direkte Arbeitsanweisung für einen autonomen oder halbautonomen Coding-Agenten formuliert. Sie beschreibt nicht nur das gewünschte Endergebnis, sondern auch Reihenfolge, Dateigrenzen, Klassen- und Methodenverträge, Tests, Fehlerszenarien, Release-Gates und die Migration der abhängigen Projekte.

**Änderung gegenüber Version 1:** `interlis-web-ide` ist nun als viertes Repository vollständig in Versionsvertrag, Release-Dispatch, Tarball-Rekonstruktion, Pages-Workflow, Testmatrix, Dateiliste und Definition of Done integriert. Zusätzlich ist der Compiler-Input von `interlis-language-tools` explizit für stabile und Snapshot-Versionen modelliert.

---

## 1. Auftrag

Der Agent soll `ilic-fork` für den ersten bewusst koordinierten Release `0.9.10` vorbereiten und die drei bekannten Folgeprojekte so anpassen, dass sie diese Version eindeutig, reproduzierbar und ohne unterbrochenen Release-Train verwenden können.

Die erste Variante bleibt absichtlich einfach:

1. Der native Compiler wird weiterhin als CLI-Programm über GitHub Releases verteilt.
2. Die JavaScript- und WASM-Artefakte werden als stabile npm-Pakete `0.9.10` publiziert.
3. `iox-cpp` bindet `ilic-core` weiterhin aus dem Quellcode mit CMake ein.
4. `iox-cpp` verwendet dafür den unveränderlichen Git-Tag `v0.9.10` statt eines nicht sprechenden Commit-Pins.
5. `interlis-language-tools` kann sowohl den stabilen Compiler `0.9.10` als auch Compiler-Snapshots auf der Basis `0.9.10` verarbeiten und erzeugt daraus exakte lokale Tarballs.
6. `interlis-web-ide` bleibt das letzte Glied des Release-Trains: Sie baut aus exakt gepinnten Compiler- und Language-Tools-SHAs neu und installiert weiterhin lokale, verifizierte Tarballs statt beweglicher Registry-Tags.
7. Es wird **kein** Conan-Paket erstellt.
8. Es wird **kein** vcpkg-Port erstellt.
9. Es wird **keine** installierbare native SDK-Bibliothek erstellt.
10. Es wird **keine** Shared Library als neues Distributionsartefakt erstellt.
11. Es wird **kein** allgemeines CMake-Install-/`find_package`-Paket verlangt.
12. Es wird **kein** neues abstraktes Modell-Provider-System zwischen `iox-cpp` und `ilic-core` eingeführt.

Der Release muss trotzdem sauber versioniert, testbar, wiederholbar und für Entwickler verständlich dokumentiert sein.

---

## 2. Verbindliche Architekturentscheidung

### 2.1 Distributionskanäle

| Artefakt | Versionierter Kanal | Consumer |
|---|---|---|
| Native CLI `ilic` für macOS ARM64 | GitHub Release `v0.9.10` | Endanwender, Skripte, CI |
| Native CLI `ilic` für Linux x86_64 | GitHub Release `v0.9.10` | Endanwender, Skripte, CI |
| Native CLI `ilic.exe` für Windows x86_64 | GitHub Release `v0.9.10` | Endanwender, Skripte, CI |
| `@ilic/repository-core` | npm `0.9.10`, Dist-Tag `latest` | `@ilic/tools`, JavaScript-Consumer |
| `@ilic/tools` | npm `0.9.10`, Dist-Tag `latest` | `interlis-language-tools`, weitere JS-Consumer |
| `@ilic/compiler-wasm` | npm `0.9.10`, Dist-Tag `latest` | `interlis-language-tools`, Browser, Node.js |
| `ilic-core` für `iox-cpp` | CMake `FetchContent` aus Git-Tag `v0.9.10` | `iox-cpp` |
| Compiler- und Language-Tools-Quellpaarung | `repository_dispatch` mit vollständigen SHAs und exakten Versionen | `interlis-web-ide` |
| Web-IDE-Laufzeit | lokal neu erzeugte und geprüfte `@ilic/*`-Tarballs | GitHub Pages / Browser |

### 2.2 Versionsvertrag

Die folgenden Werte müssen denselben Produktversionswert liefern:

```text
CMake PROJECT_VERSION              0.9.10
Git tag                            v0.9.10
GitHub Release                     v0.9.10
ilic::version()                    0.9.10
ilic_version()                     0.9.10
WASM Compiler.compilerVersion      0.9.10
CompilationResult.compilerVersion  0.9.10
@ilic/repository-core              0.9.10
@ilic/tools                        0.9.10
@ilic/compiler-wasm                0.9.10
iox-cpp expected ilic version      0.9.10
interlis-language-tools compiler input 0.9.10 oder 0.9.10-SNAPSHOT...
interlis-web-ide compiler payload      0.9.10 oder 0.9.10-SNAPSHOT...
```

Die C-ABI-Version bleibt unabhängig davon:

```text
ilic_abi_version()                 1
WASM Compiler.abiVersion           1
```

**Wichtig:** Die Produktversion `0.9.10` darf nicht mit der ABI-Version `1` vermischt werden. Ein Patch-Release des Produkts darf die ABI-Version unverändert lassen, solange der bestehende C-ABI-Vertrag nicht gebrochen wird.

---

## 3. Aktueller Zustand, von dem der Agent ausgehen soll

Der Agent muss den Zustand vor jeder Änderung erneut prüfen. Die folgenden Punkte wurden bei Erstellung dieser Spezifikation festgestellt und dienen als Ausgangshypothesen, nicht als Ersatz für die Pre-Flight-Prüfung.

### 3.1 `ilic-fork`

- `CMakeLists.txt` enthält derzeit `project(ilic VERSION 0.9.9 LANGUAGES C CXX)`.
- `source/core/Compiler.cpp` enthält derzeit eine hart codierte Implementierung:

  ```cpp
  const char *version() { return "0.9.9"; }
  ```

- `include/ilic/Compiler.h` deklariert:

  ```cpp
  const char *version();
  ```

- `source/abi/Capi.cpp` implementiert:

  ```cpp
  std::uint32_t ilic_abi_version(void) { return 1; }
  const char *ilic_version(void) { return ilic::version(); }
  ```

- Die npm-Pakete `@ilic/repository-core`, `@ilic/tools` und `@ilic/compiler-wasm` stehen auf `0.9.9`.
- `@ilic/tools` referenziert `@ilic/repository-core` exakt als `0.9.9`.
- Ein Snapshot-Publish-Workflow existiert bereits und publiziert zeitgestempelte npm-Versionen.
- Ein nativer Release-Workflow existiert bereits und baut CLI-Archive für macOS ARM64, Linux x86_64 und Windows x86_64.
- Der native Release-Workflow publiziert derzeit nur die CLI-Programme, keine native Bibliothek und kein SDK.
- `ilic-core` und `ilic-capi` existieren als statische CMake-Targets.
- Die Aliase `ilic::core` und `ilic::capi` existieren.
- `MetaModelStore` liegt unter `source/metamodel/MetaModelStore.h` und ist derzeit kein separat installiertes öffentliches SDK-Headerpaket.

### 3.2 `iox-cpp`

Die tatsächliche Quellcodeanalyse des bereitgestellten Archivs zeigt:

- Projektversion: `0.2.0`.
- CMake-Mindestversion: `3.20`.
- Optionale direkte Integration über `IOX_ENABLE_ILIC`.
- Lokaler Checkout über `IOX_ILIC_SOURCE_DIR`.
- Remote-Bezug über `IOX_FETCH_ILIC`.
- Der Remote-Bezug pinnt derzeit den Commit:

  ```text
  8582fff47549f8e0ac4d1cd6ec39c66c2bb708b0
  ```

- `iox-ilic` linkt direkt gegen `ilic-core`.
- Der öffentliche Header `include/iox/ilic/IlicModelIndex.h` inkludiert `metamodel/MetaModelStore.h`.
- Dafür wird derzeit `${_IOX_ILIC_SOURCE_DIR}/source` als Build-Include-Pfad exportiert.
- Die konkrete Integration besteht aus:
  - `iox::ilic::IlicModelIndex`
  - `iox::ilic::IlicXtfReader`
  - `iox::ilic::IlicXtfWriter`
- Diese Klassen verwenden direkt die konkreten `metamodel::*`-Typen aus `ilic-core`.
- Es gibt umfangreiche modellbasierte Tests in `test/ilic/ModelBased.test.cpp`.

### 3.3 `interlis-language-tools`

- `packages/language-service/package.json` referenziert derzeit `@ilic/compiler-wasm` auf der Basislinie `0.9.9-SNAPSHOT`.
- `packages/language-server/package.json` referenziert derzeit `@ilic/tools` auf der Basislinie `0.9.9-SNAPSHOT`.
- `pnpm-workspace.yaml` überschreibt die Compilerpakete lokal auf das benachbarte `ilic-fork`-Verzeichnis.
- `scripts/prepare-npm-snapshot.mjs` enthält derzeit eine hart codierte Compiler-Basisversion `0.9.9` und einen Regex, der nur `0.9.9-SNAPSHOT...` akzeptiert.
- Der Release-Train baut den Compiler aus einem exakten Commit neu und prüft die bereits publizierten Compilerpakete als versionierte Eingabe.

### 3.4 `interlis-web-ide`

Die Quellcodeanalyse des öffentlichen Repositories zeigt:

- `package.json` verwendet für die direkten `@ilic/*`-Abhängigkeiten derzeit den beweglichen Registry-Wert `snapshot`.
- Diese Werte sind im koordinierten CI- und Pages-Build **nicht** die effektive Paketquelle, weil `pnpm-workspace.yaml` sämtliche relevanten `@ilic/*`-Pakete auf lokal erzeugte Tarballs unter `../interlis-language-tools/artifacts/npm/` überschreibt.
- Die Tarball-Aliase tragen weiterhin Namen wie `ilic-compiler-wasm-snapshot.tgz`; dieser Dateiname ist ein stabiler lokaler Alias und darf nicht mit der darin enthaltenen Paketversion verwechselt werden.
- `.github/workflows/ci.yml` checkt `ilic-fork`, `interlis-language-tools` und `interlis-web-ide` als Geschwisterverzeichnisse aus, baut Compiler-WASM und Tarballs neu und führt danach `pnpm check` sowie Browser-E2E-Tests aus.
- `.github/workflows/pages.yml` empfängt `repository_dispatch` vom Typ `release-train-published` und pinnt `compiler_sha` sowie `language_tools_sha`.
- Der Pages-Workflow validiert `compiler_version` derzeit hart mit einem Regex für `0.9.9-SNAPSHOT...`.
- Ohne Änderung würde ein Dispatch mit `0.9.10-SNAPSHOT...` bereits vor Build und Tests mit `Invalid compiler snapshot version` abbrechen.
- Der Pages-Workflow verwendet bewusst `pnpm install --no-frozen-lockfile --force --update-checksums`, weil die lokalen Tarball-Dateinamen stabil bleiben, ihre Bytes und Integritätswerte aber vom gepinnten Release-Input abhängen.
- Die eigentliche Web-IDE-Anwendung benötigt voraussichtlich keine fachliche Codeänderung, sofern WASM-ABI `1`, öffentliche Language-Tools-APIs und JSON-Schemata kompatibel bleiben.

---

## 4. Nicht-Ziele

Der Agent darf die folgenden Arbeiten nicht in diesen Auftrag hineinziehen:

- Conan-Rezept oder Conan-Remote.
- vcpkg-Port oder vcpkg-Registry.
- Homebrew-Formula.
- Debian-, RPM-, MSI- oder macOS-PKG-Pakete.
- Installierbares natives `ilicConfig.cmake`-Paket.
- `find_package(ilic)` als Consumer-Schnittstelle.
- Neue Shared Library `libilic.so`, `ilic.dll` oder `libilic.dylib`.
- Binärkompatibilitätsmatrix für Compiler und Standardbibliotheken.
- Umbau des Metamodells in eine stabile C-API.
- Ersetzen der direkten `metamodel::MetaModelStore`-Integration in `iox-cpp`.
- Ersetzen der lokalen Tarball-Overrides in `interlis-web-ide` durch direkte bewegliche Registry-Abhängigkeiten.
- Blindes Umstellen von `interlis-web-ide/package.json` auf `0.9.10`, solange die Web IDE mehrere noch als Snapshot veröffentlichte Language-Tools-Pakete gemeinsam konsumiert.
- Fachlicher Umbau der Web-IDE-Anwendung allein wegen des Compiler-Versionswechsels.
- Neues `ModelProvider`, `TransferModelView`, Plugin-System oder Dependency-Injection-Framework.
- Änderung der fachlichen Semantik von `IlicModelIndex`, `IlicXtfReader` oder `IlicXtfWriter`, sofern kein echter Fehler durch den Versionswechsel sichtbar wird.
- Änderung der C-ABI-Version von `1` auf einen anderen Wert.
- Publikation, Push, Tag-Erstellung oder Release-Auslösung ohne ausdrückliche Freigabe des menschlichen Release-Verantwortlichen.

---

## 5. Agentenregeln

### 5.1 Vor jedem Repository

Der Agent muss in jedem Repository zuerst ausführen und dokumentieren:

```sh
git rev-parse --show-toplevel
git branch --show-current
git status --short
git log -1 --oneline
```

Bei einem nicht sauberen Arbeitsbaum:

- vorhandene Benutzeränderungen nicht überschreiben;
- Änderungen inventarisieren;
- nur Dateien ändern, die sicher zum Auftrag gehören;
- keine fremden Änderungen stagen oder committen;
- bei Konflikten abbrechen und den Konflikt exakt melden.

### 5.2 Keine Veröffentlichung während der Implementierung

Der Agent darf während der normalen Implementierungsphase nicht:

- `npm publish` gegen das öffentliche npm-Registry ausführen;
- einen Git-Tag pushen;
- GitHub Releases erzeugen;
- `repository_dispatch` an Folgeprojekte senden;
- Marketplace- oder Open-VSX-Publikationen auslösen.

Er darf und soll dagegen:

- `npm pack` ausführen;
- lokale Tarballs installieren;
- Workflows statisch prüfen;
- alle Release-Gates lokal simulieren;
- eine Release-Checkliste vorbereiten.

### 5.3 Keine erfundenen APIs

Vor jeder Änderung an `ilic-core` oder `iox-cpp` muss der Agent die realen Typen und Signaturen lesen. Insbesondere darf er keine vermuteten Metamodellklassen erfinden. Die in dieser Spezifikation aufgeführten aktuellen Signaturen sind verbindlich, solange die erneute Quellcodeprüfung keine Abweichung zeigt.

### 5.4 CMake-3.20-Kompatibilität

Die Projekte verlangen CMake 3.20. Der Agent darf deshalb nicht ungeprüft Variablen oder Befehle verwenden, die erst in neueren CMake-Versionen eingeführt wurden.

Insbesondere soll für die Top-Level-Erkennung weiterhin das kompatible Muster verwendet werden:

```cmake
set(_ilic_is_top_level OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(_ilic_is_top_level ON)
endif()
```

Der Agent soll nicht `PROJECT_IS_TOP_LEVEL` verwenden, solange `cmake_minimum_required` bei `3.20` bleibt.

---

# Teil A – Änderungen in `ilic-fork`

## 6. Phase A0: Baseline und Versionsinventar

### 6.1 Vollständige Suche

Vor Änderungen:

```sh
git grep -n '0\.9\.9'
git grep -n '0.9.9-SNAPSHOT'
git grep -n 'project(ilic VERSION'
git grep -n 'ilic_abi_version'
git grep -n 'ilic_version'
git grep -n 'const char \*version'
```

Der Agent muss die Treffer in Kategorien einteilen:

1. produktive Versionsquelle;
2. Paketmanifest;
3. Testfixture;
4. Dokumentation;
5. historische Changelog-Einträge;
6. absichtlich kompatible Altversion.

Historische Changelog-Einträge dürfen nicht blind umgeschrieben werden.

### 6.2 Baseline-Build

Mindestens auf der verfügbaren Entwicklungsplattform:

```sh
cmake -S . -B build/baseline -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON
cmake --build build/baseline --parallel
ctest --test-dir build/baseline --output-on-failure
```

Zusätzlich:

```sh
./scripts/build-wasm.sh
npm test --prefix packages/repository-core
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
node --test test/npm/PrepareNpmSnapshotTest.mjs
```

Der Agent dokumentiert alle Baseline-Fehler. Er darf bestehende Fehler nicht als Ergebnis seiner Änderung verschleiern.

---

## 7. Phase A1: Produktversion auf `0.9.10` anheben und zentralisieren

### 7.1 `CMakeLists.txt`

Ändern:

```cmake
project(ilic VERSION 0.9.10 LANGUAGES C CXX)
```

`PROJECT_VERSION` ist danach die maßgebliche Produktversionsquelle für C++-Build und Release-Staging.

### 7.2 Generierter interner Versionsheader

Neue Datei:

```text
cmake/IlicVersionConfig.h.in
```

Inhalt:

```cpp
#pragma once

#define ILIC_VERSION_STRING "@PROJECT_VERSION@"
#define ILIC_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define ILIC_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define ILIC_VERSION_PATCH @PROJECT_VERSION_PATCH@
```

In `CMakeLists.txt` unmittelbar nach der Projekt- und Optionskonfiguration:

```cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/IlicVersionConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ilic/IlicVersionConfig.h"
    @ONLY
)
```

Beim Target `ilic-core` ergänzen:

```cmake
target_include_directories(ilic-core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
```

Falls der bestehende Block bereits Generator-Expressions oder weitere Pfade enthält, diese erhalten und nur den privaten generierten Include-Pfad ergänzen.

### 7.3 `source/core/Compiler.cpp`

Die hart codierte Version entfernen.

Ergänzen:

```cpp
#include "ilic/IlicVersionConfig.h"
```

Implementierung:

```cpp
const char *version() noexcept {
    return ILIC_VERSION_STRING;
}
```

Falls die Deklaration bisher kein `noexcept` hat, gibt es zwei erlaubte Varianten:

1. Deklaration und Definition gemeinsam auf `noexcept` erweitern; oder
2. `noexcept` in der Definition weglassen, um die bestehende Signatur exakt zu erhalten.

Für diesen Patch-Release ist Variante 2 konservativer. Der Agent soll keine unnötige öffentliche Signaturänderung vornehmen.

Verbindliches Endergebnis:

```cpp
const char *version() { return ILIC_VERSION_STRING; }
```

### 7.4 Keine Änderung am C-ABI-Vertrag

In `source/abi/Capi.cpp` muss erhalten bleiben:

```cpp
std::uint32_t ilic_abi_version(void) { return 1; }
const char *ilic_version(void) { return ilic::version(); }
```

Nicht zulässig:

- `ilic_abi_version()` auf `10`, `9`, `0x00090010` oder einen Produktversionswert ändern;
- zweite unabhängige Versionskonstante in der C-API einführen;
- `ilic_version()` erneut hart codieren.

### 7.5 npm-Manifeste

Folgende Dateien auf exakt `0.9.10` ändern:

```text
packages/repository-core/package.json
packages/tools/package.json
packages/compiler-wasm/package.json
```

In `packages/tools/package.json` zusätzlich:

```json
"dependencies": {
  "@ilic/repository-core": "0.9.10"
}
```

Keine Versionsbereiche wie `^0.9.10` oder `~0.9.10` für interne Compilerpakete verwenden. Die drei Pakete bilden einen koordinierten Release und sollen exakt aufeinander zeigen.

### 7.6 Versionsvertragstest für C++

Neue Datei:

```text
test/core/VersionContractTest.cpp
```

Minimaler, frameworkunabhängiger Test:

```cpp
#include "ilic/Compiler.h"

#include <iostream>
#include <string>

#ifndef ILIC_EXPECTED_VERSION
#error ILIC_EXPECTED_VERSION must be defined by CMake
#endif

int main() {
    const std::string actual = ilic::version();
    const std::string expected = ILIC_EXPECTED_VERSION;
    if (actual != expected) {
        std::cerr << "expected ilic version " << expected
                  << ", got " << actual << '\n';
        return 1;
    }
    return 0;
}
```

In `CMakeLists.txt` innerhalb des Testblocks:

```cmake
add_executable(ilic_version_contract_test
    test/core/VersionContractTest.cpp
)
target_link_libraries(ilic_version_contract_test PRIVATE ilic::core)
target_compile_definitions(ilic_version_contract_test PRIVATE
    ILIC_EXPECTED_VERSION="${PROJECT_VERSION}"
)
add_test(NAME ilic_version_contract COMMAND ilic_version_contract_test)
```

### 7.7 Versionsvertragstest für C-ABI

Neue Datei:

```text
test/abi/CapiVersionContractTest.c
```

Inhalt:

```c
#include "ilic/capi.h"

#include <stdio.h>
#include <string.h>

#ifndef ILIC_EXPECTED_VERSION
#error ILIC_EXPECTED_VERSION must be defined by CMake
#endif

int main(void) {
    if (ilic_abi_version() != 1U) {
        fprintf(stderr, "expected ilic ABI 1, got %u\n",
                (unsigned)ilic_abi_version());
        return 1;
    }
    if (strcmp(ilic_version(), ILIC_EXPECTED_VERSION) != 0) {
        fprintf(stderr, "expected ilic version %s, got %s\n",
                ILIC_EXPECTED_VERSION, ilic_version());
        return 1;
    }
    return 0;
}
```

CMake:

```cmake
add_executable(ilic_capi_version_contract_test
    test/abi/CapiVersionContractTest.c
)
set_target_properties(ilic_capi_version_contract_test PROPERTIES
    LINKER_LANGUAGE CXX
)
target_link_libraries(ilic_capi_version_contract_test PRIVATE ilic::capi)
target_compile_definitions(ilic_capi_version_contract_test PRIVATE
    ILIC_EXPECTED_VERSION="${PROJECT_VERSION}"
)
add_test(NAME ilic_capi_version_contract
         COMMAND ilic_capi_version_contract_test)
```

### 7.8 Akzeptanzkriterien Phase A1

- `git grep -n 'const char \*version() { return "'` findet keine produktive hart codierte Version mehr.
- `ilic::version()` liefert `0.9.10`.
- `ilic_version()` liefert `0.9.10`.
- `ilic_abi_version()` liefert weiterhin `1`.
- Alle drei npm-Manifeste enthalten exakt `0.9.10`.
- `@ilic/tools` referenziert `@ilic/repository-core` exakt als `0.9.10`.
- Native Tests bestehen.
- WASM-Build besteht.

---

## 8. Phase A2: Saubere Verwendung als CMake-Unterprojekt

`iox-cpp` bindet `ilic-fork` als Unterprojekt ein. Deshalb darf ein Consumer-Build nicht unnötig die `ilic`-CLI und die komplette `ilic`-Testsuite als Teil seines normalen `all`-Targets bauen.

### 8.1 Neue Optionen

In `CMakeLists.txt` nach `include(CTest)` und vor den Targets:

```cmake
set(_ilic_is_top_level OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(_ilic_is_top_level ON)
endif()

option(ILIC_BUILD_CLI
    "Build ilic and ilic-format command-line programs"
    ${_ilic_is_top_level}
)

option(ILIC_BUILD_TESTS
    "Build ilic tests when BUILD_TESTING is enabled"
    ${_ilic_is_top_level}
)
```

Danach `_ilic_is_top_level` wieder entfernen:

```cmake
unset(_ilic_is_top_level)
```

### 8.2 CLI-Targets kapseln

Die Targets `ilic` und `ilic-format` nur erzeugen, wenn `ILIC_BUILD_CLI=ON`:

```cmake
if(ILIC_BUILD_CLI)
    add_executable(ilic source/main/ilic.cpp)
    target_link_libraries(ilic PRIVATE ilic-capi)

    add_executable(ilic-format source/main/ilic_format.cpp)
    target_link_libraries(ilic-format PRIVATE ilic-core)
endif()
```

Alle späteren CMake-Blöcke, die direkt das Target `ilic` verändern, müssen zusätzlich `ILIC_BUILD_CLI` berücksichtigen.

Beispiel:

```cmake
if(ILIC_BUILD_CLI AND ILIC_ENABLE_NATIVE_REPOSITORY AND NOT EMSCRIPTEN)
    # bestehende Repository-CLI-Integration
endif()
```

Bibliothekstargets wie `ilic-core`, `ilic-json`, `ilic-capi`, `ilic-repository-core` und `ilic-repository` dürfen nur soweit verändert werden, wie es für korrekte Abhängigkeiten nötig ist.

### 8.3 Testblock kapseln

Ändern:

```cmake
if(BUILD_TESTING AND ILIC_BUILD_TESTS)
    # bestehende Tests
endif()
```

Der Agent darf nicht im Parent-Projekt global `BUILD_TESTING` umschalten, um `ilic`-Tests zu unterdrücken.

### 8.4 Verhalten als Top-Level-Projekt

Top-Level-Aufrufe bleiben kompatibel:

```sh
cmake -S . -B build/native -DBUILD_TESTING=ON
```

müssen standardmäßig weiterhin:

- `ilic` bauen;
- `ilic-format` bauen;
- Tests bauen und registrieren.

### 8.5 Verhalten als Unterprojekt

Ein Parent-Projekt mit `BUILD_TESTING=ON` soll `ilic` standardmäßig so sehen:

```text
ILIC_BUILD_CLI=OFF
ILIC_BUILD_TESTS=OFF
```

Es soll trotzdem `ilic::core` und `ilic::capi` verwenden können.

### 8.6 Netzwerkfreier Subproject-Smoke-Test

Neue Struktur:

```text
test/consumer/source-subproject/
├── CMakeLists.txt
└── main.cpp
```

`main.cpp`:

```cpp
#include "ilic/Compiler.h"

#include <string>

int main() {
    return std::string(ilic::version()).empty() ? 1 : 0;
}
```

Consumer-`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(ilic_source_consumer LANGUAGES CXX)

set(ILIC_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(ILIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ILIC_ENABLE_NATIVE_REPOSITORY OFF CACHE BOOL "" FORCE)

add_subdirectory("${ILIC_SOURCE_DIR}" ilic-build)

if(TARGET ilic)
    message(FATAL_ERROR "ilic CLI target must not exist in library-only consumer mode")
endif()
if(NOT TARGET ilic::core)
    message(FATAL_ERROR "ilic::core target is missing")
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE ilic::core)
```

Ein CMake-Skripttest soll dieses Projekt in einem temporären Buildverzeichnis konfigurieren, bauen und ausführen. Er darf kein Netzwerk verwenden.

### 8.7 Akzeptanzkriterien Phase A2

- Top-Level-Build verhält sich wie bisher.
- Unterprojekt-Build erzeugt standardmäßig keine CLI- und keine `ilic`-Testtargets.
- `ilic::core` bleibt vorhanden.
- `iox-cpp` muss `BUILD_TESTING` nicht mehr temporär manipulieren.
- CMake 3.20 bleibt unterstützt.

---

## 9. Phase A3: WASM-Wrapper mit expliziter Laufzeitversion

Die erzeugten JSON-Ergebnisse enthalten bereits `compilerVersion`. Zusätzlich soll das geladene Compilerobjekt seine Laufzeitversion direkt exponieren. Damit kann ein Consumer sofort erkennen, ob JavaScript-Wrapper und WASM-Binary zusammenpassen.

### 9.1 `packages/compiler-wasm/wasm-memory.js`

Neue Funktion:

```js
export function readCString(module, pointer) {
  if (!pointer) return "";
  let end = pointer;
  while (end < module.HEAPU8.length && module.HEAPU8[end] !== 0) end += 1;
  if (end >= module.HEAPU8.length) {
    throw new Error("ilic returned an unterminated C string");
  }
  return decoder.decode(module.HEAPU8.subarray(pointer, end));
}
```

Eigenschaften:

- Nullpointer ergibt leeren String.
- Kein Lesen außerhalb `HEAPU8`.
- Nicht terminierter String führt zu einem klaren Fehler.
- Kein Einsatz von Emscripten-internen Hilfsmethoden, die nicht explizit exportiert sind.

### 9.2 `packages/compiler-wasm/compiler.js`

Import ergänzen:

```js
import { readCString } from "./wasm-memory.js";
```

Im Konstruktor von `Compiler`, direkt nach der ABI-Ermittlung:

```js
this.abiVersion = module._ilic_abi_version();
if (this.abiVersion !== 1) {
  throw new Error(`unsupported ilic ABI ${this.abiVersion}`);
}
this.compilerVersion = readCString(module, module._ilic_version());
if (!/^\d+\.\d+\.\d+(?:[-+].+)?$/.test(this.compilerVersion)) {
  throw new Error(`invalid ilic compiler version ${this.compilerVersion}`);
}
```

Für den stabilen Release muss der Wert exakt `0.9.10` sein.

Keine Änderung an:

```js
createSession()
createCompiler(...)
```

außer soweit notwendig, um `compilerVersion` zu testen.

### 9.3 `packages/compiler-wasm/index.d.ts`

`EmscriptenIlicModule` ergänzen:

```ts
_ilic_version(): number;
```

`Compiler` ergänzen:

```ts
readonly compilerVersion: string;
```

### 9.4 Tests

Mindestens folgende Tests ergänzen:

1. `readCString` liest einen nullterminierten UTF-8-String.
2. `readCString` lehnt einen nicht terminierten String ab.
3. `Compiler.compilerVersion` entspricht der nativen WASM-Version.
4. `Compiler.abiVersion` bleibt `1`.
5. Ein echter Compile-Aufruf liefert `CompilationResult.compilerVersion === compiler.compilerVersion`.

### 9.5 Keine breaking API

Die Änderung ist additiv. Bestehende Exporte bleiben:

```text
Compiler
CompilerSession
createCompiler
```

Die Top-Level-Exportliste des npm-Pakets darf sich nicht versehentlich verändern.

---

## 10. Phase A4: Gemeinsames npm-Staging für Snapshot und stabilen Release

Der aktuelle Snapshot-Stager enthält bereits wichtige Sicherheitsprüfungen. Diese Logik darf nicht dupliziert und später auseinanderentwickelt werden.

### 10.1 Neue gemeinsame Datei

Neue Datei:

```text
scripts/npm-package-staging.mjs
```

Sie enthält die gemeinsame Paketdefinition:

```js
export const PACKAGE_SPECS = Object.freeze([
  {
    id: "repository_core",
    name: "@ilic/repository-core",
    source: "packages/repository-core",
    destination: "repository-core"
  },
  {
    id: "tools",
    name: "@ilic/tools",
    source: "packages/tools",
    destination: "tools"
  },
  {
    id: "compiler_wasm",
    name: "@ilic/compiler-wasm",
    source: "packages/compiler-wasm",
    destination: "compiler-wasm"
  }
]);
```

### 10.2 Erforderliche Funktionen

#### `readProjectVersion(projectRoot)`

- liest `CMakeLists.txt`;
- extrahiert `project(ilic VERSION X.Y.Z ...)`;
- akzeptiert nur drei numerische Komponenten;
- wirft einen klaren Fehler, wenn die Version fehlt oder mehrdeutig ist;
- liefert einen String.

#### `validateOutputRoot(projectRoot, outputRoot, allowedProjectDirectory)`

- übernimmt die bestehenden Schutzregeln;
- verweigert Dateisystemwurzel;
- verweigert Quellpaketverzeichnisse;
- erlaubt nur ein explizites Build-Unterverzeichnis im Projekt oder ein externes temporäres Verzeichnis.

#### `readAndValidateSourcePackages({ projectRoot, baseVersion })`

Für jedes `PACKAGE_SPECS`-Element:

- `manifest.name` exakt prüfen;
- `manifest.version === baseVersion` prüfen;
- explizite, nicht leere `files`-Liste verlangen;
- alle Dateien vor dem Löschen des Staging-Verzeichnisses prüfen;
- Lizenz und notwendige Notices prüfen;
- bei `@ilic/tools` die interne Quellabhängigkeit `@ilic/repository-core === baseVersion` prüfen.

#### `rewriteInternalDependencies(manifest, targetVersion)`

- kopiert das Manifest;
- ersetzt nur bekannte interne `@ilic/*`-Compilerabhängigkeiten;
- verwendet exakte Versionen;
- mutiert das Quellmanifest nicht;
- lässt externe Dependencies unverändert.

#### `stageCompilerPackages({ projectRoot, outputRoot, targetVersion })`

- löscht ausschließlich das validierte Staging-Verzeichnis;
- kopiert nur Dateien aus der Manifest-`files`-Liste;
- schreibt pro Paket ein neues `package.json`;
- setzt `version` exakt auf `targetVersion`;
- setzt interne Compilerabhängigkeiten exakt auf `targetVersion`;
- liefert:

  ```js
  {
    baseVersion,
    targetVersion,
    outputRoot,
    directories: {
      repository_core,
      tools,
      compiler_wasm
    }
  }
  ```

### 10.3 Snapshot-Wrapper erhalten

`scripts/prepare-npm-snapshot.mjs` bleibt als öffentlicher interner Einstiegspunkt bestehen, damit `interlis-language-tools` und bestehende Tests nicht unnötig brechen.

Die exportierten Funktionen müssen mindestens erhalten bleiben:

```js
formatUtcTimestamp
validateBuildId
prepareNpmSnapshot
```

`prepareNpmSnapshot()` berechnet weiterhin:

```text
0.9.10-SNAPSHOT.YYYYMMDDHHmmss[.buildId]
```

und delegiert das eigentliche Kopieren und Umschreiben an `stageCompilerPackages()`.

### 10.4 Neuer stabiler Wrapper

Neue Datei:

```text
scripts/prepare-npm-release.mjs
```

Exportierte Funktion:

```js
export async function prepareNpmRelease({
  projectRoot,
  outputRoot,
  expectedVersion,
  expectedTag
} = {})
```

Verhalten:

1. `baseVersion` aus CMake lesen.
2. Wenn `expectedVersion` gesetzt ist, exakte Gleichheit verlangen.
3. Wenn `expectedTag` gesetzt ist:
   - muss Format `vX.Y.Z` haben;
   - muss `expectedTag.slice(1) === baseVersion` sein.
4. Zielversion ist exakt `baseVersion`, ohne `SNAPSHOT`.
5. Staging standardmäßig unter `build/npm-release`.
6. Quellmanifeste nicht verändern.
7. GitHub-Output optional schreiben:

   ```text
   base_version=0.9.10
   release_version=0.9.10
   repository_core_directory=...
   tools_directory=...
   compiler_wasm_directory=...
   ```

### 10.5 Tests

Neue Datei:

```text
test/npm/PrepareNpmReleaseTest.mjs
```

Pflichttests:

- Basisversion `0.9.10` wird exakt gestaged.
- Alle drei Paketversionen sind `0.9.10`.
- `@ilic/tools` zeigt im Staging exakt auf `@ilic/repository-core@0.9.10`.
- Quellmanifeste bleiben byte-identisch.
- `expectedVersion=0.9.9` wird abgelehnt.
- `expectedTag=v0.9.9` wird abgelehnt.
- Tag ohne `v` wird abgelehnt.
- fehlendes `ilic.wasm` wird abgelehnt.
- fehlendes `ilic.mjs` wird abgelehnt.
- gefährliches Output-Verzeichnis wird abgelehnt.
- unbekannte interne Version wird nicht stillschweigend akzeptiert.

Bestehende Snapshot-Tests werden auf die Basisversion `0.9.10` aktualisiert, ohne ihre Snapshot-Semantik zu verlieren.

---

## 11. Phase A5: npm-Tarball- und Consumer-Prüfung verallgemeinern

### 11.1 `scripts/test-npm-packages.mjs`

Neue CLI-Argumente:

```text
--expected-version <version>
--version-kind stable|snapshot
```

`--expected-version` ist in Release-Workflows verpflichtend.

### 11.2 `verifyPackList(...)`

Signatur ändern auf:

```js
async function verifyPackList(
  directory,
  expectedName,
  expectedVersion,
  versionKind
)
```

Prüfungen:

- `manifest.version === expectedVersion`;
- bei `stable`: `/^\d+\.\d+\.\d+$/`;
- bei `snapshot`: gültiges Snapshotformat;
- Repository, Lizenz, Autor, PublishConfig erhalten;
- Tarball enthält exakt die erwarteten Dateien;
- `@ilic/tools` referenziert `@ilic/repository-core` exakt als `expectedVersion`;
- alle drei Pakete haben dieselbe Version.

### 11.3 Consumer-Smoke

Das generierte `smoke.mjs` muss zusätzlich prüfen:

```js
assert.equal(compiler.compilerVersion, process.env.ILIC_EXPECTED_VERSION);
assert.equal(compiler.abiVersion, 1);
```

Nach einer Compilation:

```js
assert.equal(
  compilation.compilerVersion,
  process.env.ILIC_EXPECTED_VERSION
);
```

Die Umgebung beim Aufruf:

```js
env: {
  ILIC_REPOSITORY_FIXTURE: ...,
  ILIC_EXPECTED_VERSION: expectedVersion
}
```

### 11.4 Tarballs als verifizierte Release-Eingabe

Der Prüfschritt soll Tarballs unter einem deterministischen Verzeichnis erzeugen:

```text
build/npm-release/tarballs/
```

Nur diese erfolgreich installierten Tarballs dürfen später an `npm publish` übergeben werden. Der Publish-Job darf die Paketverzeichnisse nicht erneut unabhängig packen.

### 11.5 Release-Manifest

Neue Datei im Staging:

```text
build/npm-release/release-manifest.json
```

Schema:

```json
{
  "schemaVersion": 1,
  "version": "0.9.10",
  "packages": {
    "@ilic/repository-core": {
      "version": "0.9.10",
      "tarball": "ilic-repository-core-0.9.10.tgz"
    },
    "@ilic/tools": {
      "version": "0.9.10",
      "tarball": "ilic-tools-0.9.10.tgz"
    },
    "@ilic/compiler-wasm": {
      "version": "0.9.10",
      "tarball": "ilic-compiler-wasm-0.9.10.tgz"
    }
  }
}
```

Der Agent darf reale npm-generierte Dateinamen verwenden. Das Manifest muss exakt auf diese Dateien zeigen.

---

## 12. Phase A6: Stabiler koordinierter Release-Workflow

### 12.1 Grundsatz

Vor der ersten unveränderlichen Veröffentlichung müssen **alle** Artefakte gebaut und geprüft sein:

- native macOS-CLI;
- native Linux-CLI;
- native Windows-CLI;
- WASM;
- drei npm-Tarballs;
- Installation der Tarballs in ein leeres Consumer-Projekt.

Erst danach darf ein Publish-Job beginnen.

### 12.2 Bestehenden nativen Workflow erweitern

Bevorzugte einfache Lösung: den bestehenden Workflow

```text
.github/workflows/build-native-release.yml
```

zu einem koordinierten Compiler-Release erweitern, statt zwei unkoordiniert publizierende Tag-Workflows zu erzeugen.

Der manuelle Snapshot-Pfad darf erhalten bleiben. Stabile npm-Publikation erfolgt nur bei einem gepushten `v*`-Tag.

### 12.3 Neuer Job `validate-release`

Bei Tag-Ausführung:

1. Checkout ohne persistierte Credentials.
2. Node einrichten.
3. `node scripts/check-release-version.mjs` oder `prepare-npm-release.mjs` im Validate-Modus ausführen.
4. Prüfen:
   - Tagformat `v0.9.10`;
   - CMake-Version `0.9.10`;
   - Paketversionen `0.9.10`;
   - interne Abhängigkeiten `0.9.10`;
   - kein produktiver `0.9.9`-Rest in den definierten Versionsquellen.

### 12.4 Native Build-Jobs

Die drei bestehenden nativen Jobs hängen von `validate-release` ab, wenn das Ereignis ein stabiler Tag ist.

Sie müssen weiterhin:

- Tests ausführen;
- Laufzeitabhängigkeiten prüfen;
- Smoke-Tests ausführen;
- Archive mit stabilen Dateinamen erstellen;
- Checksummen im Publish-Job erzeugen.

Keine native Library in die Archive aufnehmen.

### 12.5 Neuer Job `build-npm-release`

Nur für stabile Tags.

Schritte:

1. Checkout des exakten Tags.
2. Node 24 einrichten.
3. npm-Version wie im Snapshot-Workflow pinnen.
4. gepinnte Emscripten-Version installieren.
5. `./scripts/build-wasm.sh` ausführen.
6. Quellpakete testen.
7. Staging-Tests ausführen.
8. Stabil stagen:

   ```sh
   node scripts/prepare-npm-release.mjs \
     --expected-version "${GITHUB_REF_NAME#v}" \
     --expected-tag "$GITHUB_REF_NAME" \
     --github-output "$GITHUB_OUTPUT"
   ```

9. Tarballs und Consumer prüfen:

   ```sh
   node scripts/test-npm-packages.mjs \
     --staging-root build/npm-release \
     --expected-version "${GITHUB_REF_NAME#v}" \
     --version-kind stable
   ```

10. `build/npm-release/tarballs/*.tgz` und `release-manifest.json` als Workflow-Artefakt hochladen.

### 12.6 Publish-Reihenfolge

Nach allen grünen Build-Jobs:

1. GitHub Release und CLI-Assets publizieren oder idempotent bestätigen.
2. npm-Pakete in Abhängigkeitsreihenfolge publizieren:
   1. `@ilic/repository-core`
   2. `@ilic/tools`
   3. `@ilic/compiler-wasm`
3. npm-Dist-Tag `latest` verifizieren.

### 12.7 Idempotenz

Ein erneuter Workflow-Lauf für denselben Tag darf nicht blind scheitern und darf nichts überschreiben.

Für jedes npm-Paket:

- Wenn `name@0.9.10` nicht existiert: verifizierten Tarball publizieren.
- Wenn es existiert:
  - `gitHead` muss dem Release-Commit entsprechen;
  - Paketversion muss exakt sein;
  - dann Publish überspringen;
  - bei abweichendem `gitHead` sofort fehlschlagen.

Für den GitHub Release:

- Wenn Release noch nicht existiert: erzeugen.
- Wenn Release existiert:
  - Tag muss identisch sein;
  - bereits vorhandene Assets dürfen nur übersprungen werden, wenn ihre Prüfsumme mit dem lokal gebauten Asset übereinstimmt;
  - abweichende Assets nicht ersetzen;
  - bei Abweichung fehlschlagen.

Stabile Releases und npm-Versionen sind unveränderlich.

### 12.8 Kein automatischer stabiler Consumer-Dispatch

Der stabile `0.9.10`-Workflow soll in dieser ersten Variante **keinen** automatischen `repository_dispatch` an `interlis-language-tools` senden.

Begründung:

- Der bestehende Dispatch ist auf Snapshot-Release-Trains ausgerichtet.
- Die stabile Consumer-Migration soll als sichtbare, überprüfbare Änderung im Folgeprojekt erfolgen.
- Dadurch bleibt die erste stabile Compilerpublikation einfacher und kontrollierter.

Der vorhandene Snapshot-Workflow darf weiterhin Snapshot-Dispatches senden. Nach dem stabilen Compiler-Publish darf ein bewusst gestarteter Language-Tools-Release-Train den stabilen Wert `compiler_version=0.9.10` an `interlis-web-ide` weiterreichen. Der Web-IDE-Pfad muss deshalb beide Versionsarten beherrschen, obwohl der Compiler-Workflow selbst keinen automatischen stabilen Dispatch auslöst.

---

## 13. Phase A7: Release-Version-Prüfskript

Neue Datei:

```text
scripts/check-release-version.mjs
```

### 13.1 Exportierte Funktionen

```js
export function readCMakeProjectVersion(cmakeText)
export function normalizeReleaseTag(tag)
export async function collectReleaseVersionState(projectRoot)
export function validateReleaseVersionState(state, expectedVersion, expectedTag)
```

### 13.2 `collectReleaseVersionState`

Liefert mindestens:

```js
{
  cmakeVersion,
  repositoryCoreVersion,
  toolsVersion,
  compilerWasmVersion,
  toolsRepositoryCoreDependency
}
```

### 13.3 Validierungsregeln

- alle Werte exakt gleich;
- Version exakt `X.Y.Z`;
- Tag exakt `vX.Y.Z`;
- keine Snapshotversion für stabilen Release;
- interne Dependency exakt;
- klare Fehlermeldung pro abweichendem Feld.

### 13.4 CLI

Unterstützte Argumente:

```text
--project-root <path>
--expected-version <X.Y.Z>
--expected-tag <vX.Y.Z>
```

Exitcode ungleich null bei jeder Abweichung.

### 13.5 Tests

Neue Datei:

```text
test/release/CheckReleaseVersionTest.mjs
```

Testfälle:

- gültiger Zustand `0.9.10`;
- CMake `0.9.9`, Pakete `0.9.10`;
- tools-interne Dependency `0.9.9`;
- Tag `0.9.10` ohne `v`;
- Tag `v0.9.11`;
- Snapshotmanifest im stabilen Check;
- fehlende Versionsquelle.

---

## 14. Phase A8: Entwickler- und Release-Dokumentation

### 14.1 Neue Datei `docs/versioning.md`

Inhalt:

- Produktversion versus C-ABI-Version;
- CMake als primäre Versionsquelle;
- generierter Header;
- npm-Paketgleichlauf;
- Snapshotformat;
- stabiler Tag;
- Unveränderlichkeit publizierter Versionen;
- Regel für nächste Version:
  - Bugfix ohne API-Bruch: `0.9.11`;
  - bedeutende API-/Verhaltensänderung: bewusst entscheiden, z. B. `0.10.0`.

### 14.2 Neue Datei `docs/releasing.md`

Muss ein Entwickler ohne Vorwissen Schritt für Schritt durchführen können.

Pflichtkapitel:

1. Voraussetzungen.
2. Benötigte Berechtigungen und Trusted-Publisher-Konfiguration.
3. Baseline und sauberer Arbeitsbaum.
4. Versionsänderung.
5. lokale Native-Gates.
6. lokale WASM-Gates.
7. lokales npm-Staging.
8. Installation der Tarballs in leeren Consumer.
9. Consumer-Migration vorbereiten.
10. Tag erstellen.
11. GitHub Actions beobachten.
12. GitHub Release prüfen.
13. npm-Pakete prüfen.
14. `iox-cpp` auf Tag umstellen.
15. `interlis-language-tools` finalisieren.
16. Verhalten bei Teilpublikation.
17. Verhalten bei nachträglich entdecktem Fehler.

### 14.3 Exakte lokale Befehle

Die Dokumentation muss mindestens enthalten:

```sh
cmake -S . -B build/release-candidate -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_BUILD_TESTS=ON \
  -DILIC_BUILD_CLI=ON
cmake --build build/release-candidate --parallel
ctest --test-dir build/release-candidate --output-on-failure
```

```sh
./scripts/build-wasm.sh
npm test --prefix packages/repository-core
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
node --test test/npm/*.test.mjs
```

```sh
node scripts/check-release-version.mjs \
  --expected-version 0.9.10 \
  --expected-tag v0.9.10
```

```sh
node scripts/prepare-npm-release.mjs \
  --expected-version 0.9.10 \
  --expected-tag v0.9.10
```

```sh
node scripts/test-npm-packages.mjs \
  --staging-root build/npm-release \
  --expected-version 0.9.10 \
  --version-kind stable
```

### 14.4 Tag-Erstellung

Nur nach Freigabe:

```sh
git tag -s v0.9.10 -m "ilic 0.9.10"
git push origin v0.9.10
```

Falls signierte Tags in der Umgebung nicht möglich oder nicht etabliert sind, soll die Dokumentation eine bewusst entschiedene Alternative beschreiben. Der Agent darf nicht stillschweigend auf einen unsignierten Tag wechseln.

### 14.5 Fehler nach Veröffentlichung

Dokumentieren:

- Tag nicht verschieben.
- npm-Version nicht unpublishen und neu verwenden.
- GitHub-Assets nicht austauschen.
- Fehler beheben und `0.9.11` veröffentlichen.
- Bei schwerem Fehler `latest` kontrolliert auf eine bekannte gute Version setzen oder Deprecation-Hinweis verwenden, aber Version nicht überschreiben.

---

# Teil B – Änderungen in `iox-cpp`

## 15. Architekturvertrag mit `ilic-core`

Die direkte Integration bleibt bestehen.

### 15.1 Unveränderte öffentliche Klassen

Die folgenden Signaturen dürfen durch diesen Release nicht fachlich umgebaut werden.

#### `iox::ilic::IlicModelIndex`

```cpp
explicit IlicModelIndex(const metamodel::MetaModelStore& models);
~IlicModelIndex();

std::optional<std::string> modelLanguage(
    std::string_view modelName) const;
std::optional<ModelEntry> transferModel(
    std::string_view modelName, XtfVersion version) const;
std::optional<IomName> resolveTopic(
    const IomName& observed,
    std::string_view targetModel,
    XtfVersion version) const;
std::optional<IomName> resolveClass(
    const IomName& observed,
    std::string_view targetModel,
    XtfVersion version) const;
std::optional<IomName> resolveProperty(
    const IomName& owner,
    const IomName& observed,
    std::string_view targetModel,
    XtfVersion version) const;
std::vector<IomName> transferProperties(
    const IomName& owner,
    std::string_view targetModel,
    XtfVersion version) const;
std::optional<IomName> referenceTargetClass(
    const IomName& owner,
    const IomName& property,
    std::string_view targetModel,
    XtfVersion version) const;
std::optional<std::string> translateEnumeration(
    const IomName& owner,
    const IomName& property,
    std::string_view lexicalValue,
    std::string_view targetModel) const;
bool isTopLevelTransferable(const IomName& className) const;
bool isTransientProperty(
    const IomName& owner,
    const IomName& property) const;
bool isEmbeddedRole(
    const IomName& owner,
    const IomName& property) const;
```

#### `iox::ilic::IlicXtfReader`

```cpp
IlicXtfReader(
    const metamodel::MetaModelStore& models,
    IlicXtfReaderOptions options = {});
~IlicXtfReader() override;

ReadOutcome next() override;
void feed(ByteView data) override;
void finish() override;
bool isFinished() const noexcept override;
std::vector<Diagnostic> takeDiagnostics() override;
```

#### `iox::ilic::IlicXtfWriter`

```cpp
IlicXtfWriter(
    const metamodel::MetaModelStore& models,
    std::shared_ptr<OutputSink> output,
    IlicXtfWriterOptions options = {});
~IlicXtfWriter() override;

void write(const IoxEvent& event) override;
void flush() override;
void close() override;
bool isClosed() const noexcept override;
std::vector<Diagnostic> takeDiagnostics() override;
```

### 15.2 Eigentumsvertrag

`IlicModelIndex` erstellt beim Konstruktor einen kompakten internen Index und hält den übergebenen `MetaModelStore` nicht fest. Dieser Vertrag bleibt bestehen.

`IlicXtfReader` und `IlicXtfWriter` dürfen intern ihren eigenen `IlicModelIndex` erzeugen. Der Agent darf nicht ohne Not rohe Metamodellpointer über die Lebensdauer des Stores hinaus speichern.

### 15.3 Source-Level-Vertrag

Für `0.9.10` wird ausdrücklich akzeptiert:

- `iox-cpp` inkludiert `metamodel/MetaModelStore.h` aus dem `ilic-fork`-Source Tree;
- `iox-ilic` ist eine Source-Build-Integration;
- es gibt keine Zusage einer separat installierbaren nativen Binär-ABI;
- die Kompatibilität wird durch den exakten Release-Tag und Cross-Repository-Tests abgesichert.

Dieser Umstand muss dokumentiert, aber nicht in diesem Auftrag architektonisch beseitigt werden.

---

## 16. Phase B1: CMake-Abhängigkeit auf `v0.9.10` umstellen

### 16.1 `cmake/IoxDependencies.cmake`

Die ilic-Abhängigkeit soll neben Expat und yyjson zentral dokumentiert werden.

Neue Konstanten:

```cmake
set(IOX_ILIC_VERSION "0.9.10")
set(IOX_ILIC_GIT_TAG "v${IOX_ILIC_VERSION}")
```

Neue Funktion:

```cmake
function(iox_make_ilic_available out_source_dir)
    set(ILIC_BUILD_CLI OFF CACHE BOOL "" FORCE)
    set(ILIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ILIC_ENABLE_NATIVE_REPOSITORY OFF CACHE BOOL "" FORCE)

    if(IOX_ILIC_SOURCE_DIR)
        if(NOT EXISTS "${IOX_ILIC_SOURCE_DIR}/CMakeLists.txt")
            message(FATAL_ERROR
                "IOX_ILIC_SOURCE_DIR does not contain ilic-fork: ${IOX_ILIC_SOURCE_DIR}")
        endif()
        add_subdirectory(
            "${IOX_ILIC_SOURCE_DIR}"
            "${CMAKE_BINARY_DIR}/ilic-core-build"
            EXCLUDE_FROM_ALL
        )
        set(_iox_ilic_source_dir "${IOX_ILIC_SOURCE_DIR}")
    elseif(IOX_FETCH_ILIC)
        FetchContent_Declare(ilic
            GIT_REPOSITORY https://github.com/edigonzales/ilic-fork.git
            GIT_TAG "${IOX_ILIC_GIT_TAG}"
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(ilic)
        set(_iox_ilic_source_dir "${ilic_SOURCE_DIR}")
    else()
        message(FATAL_ERROR
            "IOX_ENABLE_ILIC requires IOX_ILIC_SOURCE_DIR or IOX_FETCH_ILIC")
    endif()

    if(NOT TARGET ilic::core)
        message(FATAL_ERROR
            "ilic ${IOX_ILIC_VERSION} must provide target ilic::core")
    endif()
    if(NOT EXISTS "${_iox_ilic_source_dir}/source/metamodel/MetaModelStore.h")
        message(FATAL_ERROR
            "ilic ${IOX_ILIC_VERSION} does not provide the expected metamodel source API")
    endif()

    set(${out_source_dir} "${_iox_ilic_source_dir}" PARENT_SCOPE)
endfunction()
```

Der Agent darf die genaue Funktionsstruktur an CMake-Scope-Regeln anpassen. Das funktionale Ergebnis ist verbindlich.

### 16.2 `CMakeLists.txt`

Den bisherigen eingebetteten FetchContent-/add_subdirectory-Block vereinfachen:

```cmake
if(IOX_ENABLE_ILIC)
    iox_make_ilic_available(_IOX_ILIC_SOURCE_DIR)

    add_library(iox-ilic STATIC
        source/ilic/IlicModelIndex.cpp
        source/ilic/IlicXtf.cpp
    )

    target_include_directories(iox-ilic
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${_IOX_ILIC_SOURCE_DIR}/source>
            $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(iox-ilic
        PUBLIC
            iox-xtf
            iox-core
            ilic::core
    )
    iox_set_warnings(iox-ilic)
endif()
```

### 16.3 Globales `BUILD_TESTING` nicht mehr verändern

Der bisherige Ablauf speichert und überschreibt `BUILD_TESTING`. Das muss entfernt werden.

Nicht mehr zulässig:

```cmake
set(_iox_parent_build_testing "${BUILD_TESTING}")
set(BUILD_TESTING OFF)
...
set(BUILD_TESTING "${_iox_parent_build_testing}")
```

Stattdessen werden die neuen ilic-spezifischen Optionen gesetzt:

```cmake
set(ILIC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ILIC_BUILD_CLI OFF CACHE BOOL "" FORCE)
```

### 16.4 Kein Fallback auf rohes Target

Nach der Migration soll `iox-cpp` den stabilen Alias `ilic::core` verwenden. Kein neuer Fallback auf `ilic-core` ist nötig, weil `v0.9.10` diesen Alias vertraglich bereitstellt.

### 16.5 Cache-Optionen dokumentieren

`cmake/IoxOptions.cmake` behält:

```cmake
option(IOX_ENABLE_ILIC "Build direct ilic-core integration" OFF)
option(IOX_FETCH_ILIC "Fetch the pinned ilic-fork dependency" OFF)
set(IOX_ILIC_SOURCE_DIR "" CACHE PATH "Path to ilic-fork source directory")
```

Beschreibungen präzisieren:

- `IOX_FETCH_ILIC` lädt `v0.9.10`.
- `IOX_ILIC_SOURCE_DIR` ist für lokale gemeinsame Entwicklung.
- beide Optionen gleichzeitig: `IOX_ILIC_SOURCE_DIR` hat Vorrang oder die Kombination wird klar abgelehnt. Das Verhalten muss dokumentiert und getestet sein.

---

## 17. Phase B2: Expliziter Versionsvertragstest in `iox-cpp`

### 17.1 Neue Datei

```text
test/ilic/IlicDependencyVersion.test.cpp
```

Inhalt im Stil des bestehenden Testframeworks:

```cpp
#include "ilic/Compiler.h"
#include "iox/test/Test.h"

#include <string>

#ifndef IOX_EXPECTED_ILIC_VERSION
#error IOX_EXPECTED_ILIC_VERSION must be defined
#endif

IOX_TEST(ilic_dependency_version_matches_release_contract) {
    IOX_CHECK_EQ(
        std::string(IOX_EXPECTED_ILIC_VERSION),
        std::string(ilic::version())
    );
}

#include "iox/test/TestMain.h"
```

### 17.2 `test/CMakeLists.txt`

Im `if(IOX_ENABLE_ILIC)`-Block neues Testtarget:

```cmake
add_executable(iox-test-ilic-version
    ilic/IlicDependencyVersion.test.cpp
)
target_link_libraries(iox-test-ilic-version PRIVATE
    ilic::core
    iox-test-support
)
target_compile_definitions(iox-test-ilic-version PRIVATE
    IOX_EXPECTED_ILIC_VERSION="${IOX_ILIC_VERSION}"
)
add_test(NAME iox.test.ilic.version COMMAND iox-test-ilic-version)
```

### 17.3 Bestehender modellbasierter Test

`test/ilic/ModelBased.test.cpp` bleibt fachlich unverändert und muss vollständig gegen `v0.9.10` bestehen.

Der Agent darf Testanpassungen nur vornehmen, wenn:

- ein echter, beabsichtigter API-Unterschied in `ilic 0.9.10` vorliegt;
- die Änderung fachlich begründet und dokumentiert wird;
- keine Assertion abgeschwächt oder entfernt wird.

### 17.4 Konfigurationsmatrix

Mindestens testen:

#### Ohne ilic

```sh
cmake -S . -B build/no-ilic -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DIOX_ENABLE_ILIC=OFF
cmake --build build/no-ilic --parallel
ctest --test-dir build/no-ilic --output-on-failure
```

#### Mit lokalem ilic-Checkout

```sh
cmake -S . -B build/ilic-local -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DIOX_ENABLE_ILIC=ON \
  -DIOX_ILIC_SOURCE_DIR=/path/to/ilic-fork
cmake --build build/ilic-local --parallel
ctest --test-dir build/ilic-local --output-on-failure
```

#### Mit Release-Tag

Erst nachdem `v0.9.10` existiert:

```sh
cmake -S . -B build/ilic-fetch -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DIOX_ENABLE_ILIC=ON \
  -DIOX_FETCH_ILIC=ON
cmake --build build/ilic-fetch --parallel
ctest --test-dir build/ilic-fetch --output-on-failure
```

### 17.5 Akzeptanzkriterien

- `IOX_FETCH_ILIC=ON` lädt `v0.9.10`.
- `iox.test.ilic.version` prüft exakt `0.9.10`.
- `iox.test.ilic.model_based` besteht vollständig.
- Build ohne ilic bleibt vollständig funktionsfähig.
- `iox-core` und `iox-xtf` erhalten keine neue ilic-Abhängigkeit.
- Nur `iox-ilic` linkt `ilic::core`.

---

## 18. Phase B3: Dokumentation in `iox-cpp`

### 18.1 `README.md`

Zwei klar getrennte Entwicklerwege dokumentieren.

#### Lokale gemeinsame Entwicklung

```sh
cmake -S . -B build/ilic \
  -DBUILD_TESTING=ON \
  -DIOX_ENABLE_ILIC=ON \
  -DIOX_ILIC_SOURCE_DIR=/path/to/ilic-fork
```

#### Reproduzierbarer Release-Bezug

```sh
cmake -S . -B build/ilic-release \
  -DBUILD_TESTING=ON \
  -DIOX_ENABLE_ILIC=ON \
  -DIOX_FETCH_ILIC=ON
```

Erklären:

- `IOX_FETCH_ILIC` verwendet `v0.9.10`.
- `ilic-core` wird aus Source gebaut.
- es wird keine vorkompilierte native Bibliothek heruntergeladen.
- `iox-ilic` ist optional.

### 18.2 `docs/architecture.md`

Festhalten:

```text
iox-core  -> keine ilic-Abhängigkeit
iox-xtf   -> keine ilic-Abhängigkeit
iox-ilic  -> direkte Source-Level-Abhängigkeit auf ilic::core v0.9.10
```

### 18.3 `docs/conformance.md`

Eintragen:

- semantischer Dependency-Name: `ilic 0.9.10`;
- Git-Tag: `v0.9.10`;
- nach Tag-Erstellung der aufgelöste vollständige Commit-SHA;
- Grund für Source-Level-Einbindung;
- Hinweis auf `iox.test.ilic.version`.

### 18.4 Bestehende Agentenspezifikation

Falls `iox-cpp-llm-coding-spec.md` noch den alten Commit-Pin oder ein zukünftiges installiertes Paket als aktuellen Weg beschreibt, die reale Implementierung aktualisieren. Historische Architekturbegründungen nicht unnötig umschreiben.

---

# Teil C – Änderungen in `interlis-language-tools`

## 19. Ziel der Consumer-Migration

`interlis-language-tools` muss drei Dinge sauber können:

1. stabil auf `@ilic/compiler-wasm@0.9.10` und `@ilic/tools@0.9.10` zeigen;
2. neue Compiler-Snapshots im Format `0.9.10-SNAPSHOT...` akzeptieren;
3. aus einem exakten Compiler-SHA sowohl stabile Compilerpakete als auch Snapshot-Compilerpakete lokal rekonstruieren, damit `interlis-web-ide` denselben Release-Input reproduzierbar nachbauen kann.

Während der Übergangsphase kann das aktuell publizierte npm-Dist-Tag `snapshot` noch auf eine `0.9.9-SNAPSHOT...`-Version zeigen. Diese Übergangsannahme darf nur an explizit dokumentierten Eingangsgrenzen bestehen und darf nicht dazu führen, dass ein `0.9.10`-Checkout als `0.9.9` gestaged wird.

---

## 20. Phase C1: Compiler-Versionsmodell auf stabile und Snapshot-Eingaben vorbereiten

### 20.1 Ziel

Der Release-Train darf die Compiler-Version nicht mehr als Spezialfall `0.9.9-SNAPSHOT...` behandeln. Er benötigt ein explizites Modell mit zwei zulässigen Arten:

```text
stable:   0.9.10
snapshot: 0.9.10-SNAPSHOT.YYYYMMDDHHmmss[.buildId]
```

Temporär darf die Parserfunktion auch `0.9.9-SNAPSHOT...` erkennen, um einen bereits publizierten alten Snapshot kontrolliert zu diagnostizieren oder während der Migrationsreihenfolge noch zu konsumieren. Eine lokale Rekonstruktion mit einem `ilic-fork`-Checkout auf `0.9.10` darf daraus jedoch niemals fälschlich ein Paket mit Basis `0.9.9` erzeugen.

### 20.2 Gemeinsamer Parser

In `scripts/prepare-npm-snapshot.mjs` oder einer neuen fokussierten Hilfsdatei eine Funktion vorsehen:

```js
export function parseCompilerVersion(version) {
  const stable = version.match(/^(\d+\.\d+\.\d+)$/);
  if (stable) {
    return {
      kind: "stable",
      baseVersion: stable[1],
      version
    };
  }

  const snapshot = version.match(
    /^(\d+\.\d+\.\d+)-SNAPSHOT\.(\d{14})(?:\.(\d+))?$/
  );
  if (!snapshot) {
    throw new Error(
      `Compiler version must be X.Y.Z or X.Y.Z-SNAPSHOT.YYYYMMDDHHmmss[.buildId], received ${version}`
    );
  }
  validateTimestamp(snapshot[2]);
  return {
    kind: "snapshot",
    baseVersion: snapshot[1],
    timestamp: snapshot[2],
    buildId: snapshot[3],
    version
  };
}
```

Der genaue Dateiname darf an bestehende Konventionen angepasst werden. Die Funktion muss separat testbar und frei von GitHub-Actions-spezifischer Logik sein.

### 20.3 Unterstützte Basisversionen

Die aktive Compiler-Basis ist:

```js
const COMPILER_BASE_VERSION = "0.9.10";
```

Während der geordneten Migration darf eine explizite Menge existieren:

```js
const TEMPORARILY_ACCEPTED_SNAPSHOT_BASES = new Set([
  "0.9.9",
  "0.9.10"
]);
```

Regeln:

- Neue Snapshot-Versionen werden immer auf Basis `0.9.10` erzeugt.
- Stabile Eingaben müssen exakt `0.9.10` sein.
- `0.9.9-SNAPSHOT...` darf nur als temporäre externe Eingabe akzeptiert werden.
- Die Basisversion der Eingabe muss vor lokaler Rekonstruktion mit der aus dem ausgecheckten `ilic-fork/CMakeLists.txt` gelesenen Basisversion übereinstimmen.
- Eine Nichtübereinstimmung ist ein harter Fehler mit beiden Werten in der Fehlermeldung.

### 20.4 Lokale Rekonstruktion nach Versionsart

Die Funktion `prepareNpmSnapshot(...)` im Language-Tools-Repository erhält weiterhin den exakten Parameter `compilerVersion`.

Nach `parseCompilerVersion(compilerVersion)` gilt:

#### Snapshot-Compiler

- `ilic-fork/scripts/prepare-npm-snapshot.mjs` importieren;
- `timestamp` und `buildId` aus der geparsten Version übergeben;
- Ergebnisversion muss exakt `compilerVersion` sein.

#### Stabiler Compiler

- `ilic-fork/scripts/prepare-npm-release.mjs` importieren;
- `expectedVersion: compilerVersion` übergeben;
- kein künstliches Snapshot-Suffix ergänzen;
- Ergebnisversion muss exakt `compilerVersion` sein.

Der Language-Tools-Stager darf die Compilerpakete nicht aus dem öffentlichen npm-Registry erneut beziehen. Er soll sie aus dem exakt ausgecheckten Compiler-SHA reproduzieren und als lokale Tarballs prüfen.

### 20.5 Rewrite der Language-Tools-Manifeste

`rewriteLanguageManifest(manifest, { snapshotVersion, compilerVersion })` behält seine Verantwortung:

- interne Language-Tools-Abhängigkeiten auf die exakte Language-Tools-Snapshot-Version setzen;
- `@ilic/compiler-wasm`, `@ilic/tools` und `@ilic/repository-core` auf die exakte übergebene `compilerVersion` setzen;
- weder `workspace:*` noch `file:` noch Dist-Tags in publizierte Manifeste übernehmen.

Dies muss für beide Beispiele funktionieren:

```text
compilerVersion = 0.9.10
compilerVersion = 0.9.10-SNAPSHOT.20260804120000.123456
```

### 20.6 Release-Manifest

`snapshot-manifest.json` und `release-manifest.json` müssen mindestens enthalten:

```json
{
  "compilerVersion": "0.9.10",
  "compilerVersionKind": "stable",
  "compilerBaseVersion": "0.9.10",
  "compilerSha": "<vollständiger SHA>"
}
```

Für Snapshots entsprechend:

```json
{
  "compilerVersion": "0.9.10-SNAPSHOT.20260804120000.123456",
  "compilerVersionKind": "snapshot",
  "compilerBaseVersion": "0.9.10"
}
```

Bestehende Schemas nur additiv erweitern oder eine Schema-Version bewusst erhöhen. Consumer und Tests gleichzeitig anpassen.

### 20.7 Pflichttests

Mindestens testen:

- stabile Version `0.9.10` wird als `stable` geparst;
- `0.9.10-SNAPSHOT.20260804090000` wird als `snapshot` geparst;
- `0.9.10-SNAPSHOT.20260804090000.123` wird korrekt geparst;
- malformed Version wird abgelehnt;
- unmöglicher Zeitstempel wird abgelehnt;
- stabile Version `0.9.9` wird für den neuen Release abgelehnt;
- temporärer `0.9.9-SNAPSHOT...`-Input wird nur an der vorgesehenen Migrationsgrenze akzeptiert;
- Compiler-Checkout-Basis und Compiler-Version müssen übereinstimmen;
- stabiler Compiler ruft den Stable-Stager auf;
- Snapshot-Compiler ruft den Snapshot-Stager auf;
- gestagte Compiler-Version stimmt exakt mit dem Input überein;
- alle publizierten Language-Tools-Manifeste enthalten exakte Compiler-Versionen.

### 20.8 Spätere Bereinigung

Nach erfolgreichem `0.9.10`-Snapshot-, Stable- und Pages-Deploy kann die temporäre Akzeptanz von `0.9.9-SNAPSHOT...` in einem separaten Cleanup entfernt werden. Diese Bereinigung ist nicht Teil des kritischen ersten Release-Pfads, muss aber als technischer Folgepunkt dokumentiert sein.

---

## 21. Phase C2: Stabile Compilerabhängigkeiten setzen

Nach erfolgreicher npm-Publikation von `0.9.10`:

### 21.1 `packages/language-service/package.json`

Ändern:

```json
"dependencies": {
  "@ilic/compiler-wasm": "0.9.10"
}
```

### 21.2 `packages/language-server/package.json`

Ändern:

```json
"dependencies": {
  "@ilic/tools": "0.9.10"
}
```

### 21.3 Weitere Manifeste

Der Agent muss repositoryweit suchen:

```sh
git grep -n '0\.9\.9'
git grep -n '@ilic/compiler-wasm'
git grep -n '@ilic/tools'
git grep -n '@ilic/repository-core'
```

Jede produktive Compilerabhängigkeit muss bewusst klassifiziert und auf die neue Basislinie gebracht werden.

### 21.4 Lokale Overrides behalten

`pnpm-workspace.yaml` darf die lokalen `file:../ilic-fork/...`-Overrides behalten. Diese sind für gemeinsame Entwicklung nützlich.

Die veröffentlichten Tarballs dürfen jedoch nie `file:`, `workspace:*` oder einen beweglichen Dist-Tag für Compilerabhängigkeiten enthalten.

### 21.5 Lockfile

Nach Manifeständerungen:

```sh
pnpm install
```

`pnpm-lock.yaml` nur mit den tatsächlich erzeugten, nachvollziehbaren Änderungen committen.

### 21.6 Kein unnötiger Runtime-Umbau

Da die Compiler-WASM-API für `0.9.10` kompatibel bleibt, sollen folgende Komponenten nicht fachlich umgebaut werden:

- Language Service;
- Monaco Adapter;
- Diagramm-Paket;
- DOCX-Paket;
- Language Server;
- VS-Code-Extension.

Anpassungen sind nur nötig, wenn Tests eine echte Versionsannahme oder neue additive Eigenschaft `compilerVersion` verwenden sollen.

---

## 22. Phase C3: Consumer-Version sichtbar prüfen

Mindestens ein Integrationstest im Language Service oder Release-Tarball-Consumer soll nach dem Laden prüfen:

```ts
expect(compiler.compilerVersion).toBe(expectedCompilerVersion);
expect(compiler.abiVersion).toBe(1);
```

Bei einem Compile-/Analyze-Aufruf zusätzlich:

```ts
expect(result.compilerVersion).toBe(compiler.compilerVersion);
```

Der erwartete Wert darf im Snapshot-Release-Train aus dem exakten `compilerVersion`-Input kommen und nicht aus einem beweglichen npm-Dist-Tag neu abgeleitet werden.

---

## 23. Phase C4: Dokumentation

Aktualisieren:

- `docs/build-und-publikationspipeline.md`;
- `docs/release.md`, falls vorhanden;
- README-Stellen mit Compilerbasis;
- Beispiele mit `0.9.9-SNAPSHOT`.

Dokumentieren:

- stabile Entwicklungsbasis `0.9.10`;
- Snapshotformat `0.9.10-SNAPSHOT...`;
- lokale Workspace-Overrides;
- exakte Version im Release-Manifest;
- Übergangskompatibilität für `0.9.9` nur temporär.

---


# Teil D – Änderungen in `interlis-web-ide`

## 24. Ziel der Web-IDE-Migration

`interlis-web-ide` bleibt das letzte Glied des koordinierten Release-Trains. Der Release `0.9.10` darf dieses Glied nicht durch eine veraltete Versionsannahme unterbrechen.

Der Agent muss sicherstellen:

1. `release-train-published` akzeptiert Compiler-Snapshots auf Basis `0.9.10`.
2. Der Workflow kann zusätzlich den stabilen Compilerwert `0.9.10` verarbeiten.
3. `compiler_version` wird gegen den tatsächlich ausgecheckten Compiler-SHA geprüft.
4. Die Web IDE baut weiterhin Compiler-WASM und alle Cross-Repository-Tarballs lokal aus gepinnten Quellen neu.
5. `package.json` und `pnpm-workspace.yaml` werden nicht unnötig auf ein anderes Abhängigkeitsmodell umgestellt.
6. Die eigentliche Browseranwendung bleibt unverändert, sofern die bestehenden Integrations- und E2E-Tests keine Inkompatibilität zeigen.

---

## 25. Architekturvertrag der Web IDE

### 25.1 Verbindliche Quellpaarung

Bei `repository_dispatch` erhält die Web IDE mindestens:

```json
{
  "compiler_sha": "<40-stelliger SHA>",
  "language_tools_sha": "<40-stelliger SHA>",
  "compiler_version": "0.9.10 oder 0.9.10-SNAPSHOT...",
  "language_tools_version": "0.1.0-SNAPSHOT...",
  "language_timestamp": "YYYYMMDDHHmmss",
  "language_build_id": "<numerische Run-ID>",
  "release_run_id": "<Run-ID>"
}
```

`compiler_sha` und `compiler_version` bilden einen Vertrag. Dasselbe gilt für Language Tools.

Ein Workflow darf nicht nur die Syntax der Version prüfen. Er muss nach Checkout bestätigen, dass die Version zur tatsächlichen Compiler-Basis des ausgecheckten SHAs passt.

### 25.2 Paketauflösung

Die effektive Paketauflösung bleibt:

```text
interlis-language-tools/artifacts/npm/*.tgz
```

über `pnpm-workspace.yaml`.

Die Registry-Werte `snapshot` in `package.json` sind ein Fallback beziehungsweise eine deklarative Entwicklungsbasis, aber nicht die Quelle des reproduzierbaren Pages-Builds.

Folglich gilt:

- `package.json` nicht allein wegen `ilic 0.9.10` auf einzelne stabile Versionswerte umstellen;
- lokale Tarball-Overrides beibehalten;
- Tarball-Aliasnamen nicht als Paketversion interpretieren;
- Paketversionen aus den enthaltenen `package.json`-Dateien und dem Release-Manifest prüfen.

### 25.3 Lockfile

Der Runner darf weiterhin ausführen:

```sh
pnpm install --no-frozen-lockfile --force --update-checksums
```

weil lokal erzeugte Tarballs bei stabilen Aliasnamen neue Integritätswerte besitzen.

Regeln:

- Lockfile-Änderungen im Runner nicht zurück ins Repository schreiben;
- `pnpm-lock.yaml` lokal nur committen, wenn Manifeständerungen eine echte, dauerhafte Lockfile-Änderung erzeugen;
- reine Tarball-Checksum-Änderungen nicht blind committen.

---

## 26. Phase D1: Release-Metadatenvalidierung aus dem Workflow extrahieren

### 26.1 Problem

`.github/workflows/pages.yml` enthält derzeit eine Inline-Prüfung, die nur Folgendes akzeptiert:

```text
0.9.9-SNAPSHOT.YYYYMMDDHHmmss[.buildId]
```

Das ist eine produktive Blockade für `0.9.10` und muss vor dem ersten `0.9.10`-Dispatch behoben sein.

### 26.2 Bevorzugte Implementierung

Neue Datei, beispielsweise:

```text
scripts/validate-release-metadata.mjs
```

Exportierte, rein funktionale Hilfen:

```js
export function parseSemanticVersion(value)
export function parseCompilerVersion(value)
export function parseLanguageToolsVersion(value)
export function validateFullSha(value, fieldName)
export function readIlicProjectVersion(cmakeText)
export function validateReleaseMetadata(input)
```

`validateReleaseMetadata(input)` erhält mindestens:

```ts
{
  compilerSha: string;
  languageToolsSha: string;
  compilerVersion: string;
  languageToolsVersion: string;
  languageTimestamp: string;
  languageBuildId: string;
  checkedOutCompilerSha: string;
  checkedOutLanguageToolsSha: string;
  checkedOutCompilerBaseVersion: string;
}
```

und liefert normalisierte Metadaten zurück oder wirft einen präzisen Fehler.

### 26.3 Compiler-Version

Zulässig sind nur:

```text
<checkedOutCompilerBaseVersion>
<checkedOutCompilerBaseVersion>-SNAPSHOT.YYYYMMDDHHmmss[.buildId]
```

Für den Release-Checkout `0.9.10` also:

```text
0.9.10
0.9.10-SNAPSHOT.20260804120000
0.9.10-SNAPSHOT.20260804120000.123456
```

Nicht zulässig:

```text
0.9.9-SNAPSHOT....
0.9.11
snapshot
v0.9.10
0.9.10-SNAPSHOT.invalid
```

### 26.4 CMake-Version lesen

Die Basisversion aus `ilic-fork/CMakeLists.txt` lesen. Keine zweite harte Konstante `0.9.10` im Web-IDE-Workflow einführen.

Der Parser muss das reale Muster `project(ilic VERSION X.Y.Z LANGUAGES ...)` unterstützen und bei fehlender oder mehrdeutiger Version fehlschlagen.

### 26.5 SHA-Prüfung

- beide Payload-SHAs müssen exakt 40 hexadezimale Kleinbuchstaben/Ziffern enthalten;
- nach Checkout `git rev-parse HEAD` mit dem Payload vergleichen;
- keine stillen Fallbacks bei einem `repository_dispatch`;
- `main`-Fallback nur beim expliziten manuellen `workflow_dispatch` beibehalten und als nicht release-gepinnten Recovery-Build dokumentieren.

### 26.6 Workflow-Anbindung

Der Inline-Shell-Block in `.github/workflows/pages.yml` soll nur noch:

1. Event-Felder sammeln;
2. tatsächliche SHAs und CMake-Basisversion lesen;
3. das getestete Skript aufrufen;
4. normalisierte Werte in `$GITHUB_OUTPUT` schreiben.

Komplexe Regex- und Migrationslogik darf nicht ungetestet im YAML verbleiben.

---

## 27. Phase D2: Stabilen und Snapshot-Compiler reproduzierbar bauen

### 27.1 Übergabe an `interlis-language-tools`

Der Pages-Workflow setzt weiterhin:

```text
COMPILER_VERSION
LANGUAGE_TOOLS_VERSION
SNAPSHOT_TIMESTAMP
SNAPSHOT_BUILD_ID
```

`COMPILER_VERSION` darf nun stabil oder Snapshot sein.

### 27.2 Erwartetes Verhalten von `pnpm pack:verify`

Bei stabilem Compiler:

```text
COMPILER_VERSION=0.9.10
```

muss `interlis-language-tools` aus dem ausgecheckten `ilic-fork` die drei Compiler-Tarballs mit Paketversion `0.9.10` erzeugen und die Language-Tools-Tarballs auf exakt diese Version verweisen lassen.

Bei Snapshot-Compiler:

```text
COMPILER_VERSION=0.9.10-SNAPSHOT.20260804120000.123456
```

müssen die Compiler-Tarballs exakt diese Version tragen.

### 27.3 Keine Registry-Auflösung als Release-Wahrheit

Der Pages-Build darf nicht auf Folgendes umgestellt werden:

```sh
pnpm add @ilic/compiler-wasm@latest
```

oder:

```sh
pnpm add @ilic/compiler-wasm@snapshot
```

Die Release-Wahrheit sind:

- exakter Compiler-SHA;
- exakter Language-Tools-SHA;
- exakte Versionsfelder;
- lokal reproduzierte und geprüfte Tarballs.

### 27.4 Manifest- und Laufzeitprüfung

Vor dem Vite-Build mindestens prüfen:

- installiertes `@ilic/compiler-wasm/package.json` trägt `compiler_version`;
- installiertes `@ilic/tools/package.json` trägt dieselbe Compiler-Version;
- `@ilic/language-service` und andere direkte Compiler-Consumer referenzieren exakt `compiler_version`;
- keine interne `@ilic/*`-Abhängigkeit enthält `workspace:*`, `file:` oder `snapshot` als publizierten Versionswert;
- ein echter Browser- oder Node-Smoke lädt den Compiler und sieht ABI `1` sowie die erwartete Produktversion.

---

## 28. Phase D3: Manifeste, Anwendungscode und Dokumentation

### 28.1 `package.json`

Die direkten Werte wie:

```json
"@ilic/tools": "snapshot"
```

müssen nicht allein wegen dieses Releases geändert werden.

Der Agent darf sie nur ändern, wenn die vollständige Paketstrategie der Web IDE bewusst von lokalen Cross-Repository-Tarballs auf stabile Registry-Pakete umgestellt wird. Das ist **nicht** Teil dieses Auftrags.

### 28.2 `pnpm-workspace.yaml`

Die lokalen Overrides für folgende Pakete bleiben erhalten:

- `@ilic/repository-core`;
- `@ilic/tools`;
- `@ilic/compiler-wasm`;
- `@ilic/language-service`;
- `@ilic/monaco-adapter`;
- `@ilic/diagram`;
- `@ilic/docx`.

Die Dateinamen dürfen weiterhin `-snapshot.tgz` enthalten, solange dokumentiert ist, dass dies Aliasnamen sind und die interne Paketversion aus dem Release-Input stammt.

### 28.3 Anwendungscode

Kein pauschaler Umbau der Web-IDE-Quellen.

Nur bei tatsächlichem Testfehler ändern, insbesondere in Bereichen:

- Compiler-Erzeugung;
- Language-Service-Initialisierung;
- Monaco-Adapter;
- Diagramm;
- DOCX;
- Worker-Bootstrap;
- Offline-/PWA-Build.

Jede solche Änderung benötigt einen gezielten Unit- oder E2E-Regressionstest.

### 28.4 Dokumentation

`docs/build-und-publikationspipeline.md` aktualisieren:

- Beispiele `0.9.9-SNAPSHOT...` auf `0.9.10`-Basis bringen;
- stabile und Snapshot-Compiler-Version erklären;
- Version-SHA-Vertrag dokumentieren;
- lokale Tarball-Rekonstruktion beschreiben;
- Unterschied zwischen Registry-Deklaration und effektivem Workspace-Override erklären;
- manuellen Pages-Build als nicht release-gepinnten Recovery-Build kennzeichnen;
- typische Fehlermeldung bei Basisversionsabweichung dokumentieren.

---

## 29. Verbindliche Testmatrix `interlis-web-ide`

### 29.1 Unit-Test der Metadatenvalidierung

Mindestens folgende Fälle:

- `0.9.10` mit Checkout-Basis `0.9.10` akzeptieren;
- `0.9.10-SNAPSHOT.20260804120000` akzeptieren;
- Snapshot mit numerischer Build-ID akzeptieren;
- `0.9.9-SNAPSHOT...` gegen Checkout `0.9.10` ablehnen;
- stabile Version `0.9.9` ablehnen;
- Version mit führendem `v` ablehnen;
- malformed Timestamp ablehnen;
- unmöglichen Timestamp ablehnen;
- nicht vollständige SHAs ablehnen;
- Payload-SHA und Checkout-SHA bei Abweichung ablehnen;
- fehlende Language-Timestamp/Build-ID beim Dispatch ablehnen.

### 29.2 Vollständiger lokaler Build

Mit Geschwisterverzeichnissen:

```sh
cd ../ilic-fork
./scripts/build-wasm.sh

cd ../interlis-language-tools
corepack pnpm install --frozen-lockfile
COMPILER_VERSION=0.9.10 \
SNAPSHOT_TIMESTAMP=20260804120000 \
SNAPSHOT_BUILD_ID=123456 \
corepack pnpm pack:verify

cd ../interlis-web-ide
corepack pnpm install --no-frozen-lockfile --force --update-checksums
corepack pnpm check
```

Zusätzlich denselben Pfad mit einer `0.9.10-SNAPSHOT...`-Compiler-Version ausführen.

### 29.3 Browser-E2E

```sh
corepack pnpm exec playwright install chromium firefox
corepack pnpm e2e --project chromium --project firefox
```

WebKit auf einer unterstützten macOS-Umgebung:

```sh
corepack pnpm exec playwright install webkit
corepack pnpm e2e --project webkit
```

### 29.4 Workflow-Prüfung

- YAML syntaktisch validieren;
- Eventpfad `repository_dispatch` mit Testfixtures simulieren;
- manuellen `workflow_dispatch`-Fallback separat testen;
- sicherstellen, dass ein ungültiger Payload vor Emscripten-Installation und Build fehlschlägt;
- sicherstellen, dass ein gültiger `0.9.10`- oder `0.9.10-SNAPSHOT`-Payload bis zum Tarball-Build gelangt.

### 29.5 Akzeptanzkriterien

- kein produktiver `0.9.9`-Regex im Pages-Workflow;
- Compiler-Version wird aus dem Payload validiert und gegen CMake-Basis geprüft;
- vollständige SHAs bleiben verbindlich;
- lokale Tarball-Overrides bleiben aktiv;
- Web-IDE-Build und Browser-E2E sind grün;
- keine unnötige Anwendungscodeänderung;
- Pages-Dokumentation beschreibt beide Compiler-Versionstypen.

---

# Teil E – Reihenfolge über alle Repositories

## 30. Implementierungsreihenfolge ohne Publikation

### Schritt 1: `interlis-language-tools` versionsfähig machen

Zuerst Parser, Staging und Tests so erweitern, dass stabile `0.9.10`-Eingaben und `0.9.10-SNAPSHOT...`-Eingaben unterstützt werden. Die temporäre Akzeptanz eines bereits publizierten `0.9.9-SNAPSHOT...` darf nur kontrolliert und getestet erfolgen.

### Schritt 2: `interlis-web-ide` vor dem ersten neuen Dispatch vorbereiten

- hart codierte `0.9.9`-Validierung entfernen;
- getestete dynamische Release-Metadatenvalidierung einführen;
- stabile und Snapshot-Compiler-Versionen unterstützen;
- lokale Tarball-Overrides unverändert lassen;
- Unit-Tests der Payload-Validierung ausführen.

Dieser Schritt muss auf `main` sein, bevor ein automatischer `0.9.10-SNAPSHOT...`-Release-Train bis zur Web IDE durchlaufen kann.

### Schritt 3: `ilic-fork` implementieren

- Version zentralisieren;
- auf `0.9.10` anheben;
- CMake-Unterprojektverhalten verbessern;
- WASM-Laufzeitversion ergänzen;
- stabiles npm-Staging und Workflow vorbereiten;
- alle lokalen Gates ausführen.

### Schritt 4: `iox-cpp` lokal gegen den vorbereiteten Checkout testen

Mit:

```sh
-DIOX_ILIC_SOURCE_DIR=/path/to/ilic-fork
```

Noch nicht zwingend den Remote-Tag verwenden, solange `v0.9.10` nicht existiert.

### Schritt 5: vollständigen Cross-Repository-Build ausführen

Mit vier Geschwister-Working-Copies:

```text
ilic-fork/
interlis-language-tools/
interlis-web-ide/
iox-cpp/
```

Mindestens:

- Compiler nativ und WASM;
- Compiler-Tarballs stabil und Snapshot;
- Language-Tools-Pack-Verifikation;
- Web-IDE-`pnpm check` und E2E;
- `iox-cpp` gegen lokalen Compiler-Checkout.

### Schritt 6: Release-Freigabe einholen

Der Agent liefert:

- vollständige Testresultate;
- Diff-Zusammenfassung pro Repository;
- bekannte Risiken;
- Liste der noch nicht ausführbaren Tag-basierten Tests;
- exakte Release-Befehle.

Er publiziert nicht selbst ohne Freigabe.

---

## 31. Release-Ausführungsreihenfolge

Nach menschlicher Freigabe:

1. `interlis-language-tools`-Änderungen für stabile und `0.9.10`-Snapshot-Compiler-Versionen auf `main` mergen.
2. `interlis-web-ide`-Änderungen für dynamische Release-Metadatenvalidierung auf `main` mergen und dessen CI abwarten.
3. `ilic-fork`-Release-Änderungen auf `main` mergen.
4. Erfolgreiche `ilic-fork`-CI abwarten.
5. Den automatisch erzeugten `0.9.10-SNAPSHOT...`-Pfad und den nachgelagerten Language-Tools-Train prüfen.
6. Prüfen, dass der `release-train-published`-Dispatch die Web IDE erreicht und der Pages-Build den `0.9.10-SNAPSHOT...`-Wert akzeptiert.
7. Web-IDE-Pages-Deploy sowie Browser-Smoke prüfen.
8. Lokale Release-Gates auf dem exakten Compiler-Release-Commit erneut ausführen.
9. `v0.9.10` erstellen und pushen.
10. Koordinierten Compiler-Release-Workflow vollständig grün abwarten.
11. GitHub-Release-Assets und SHA256 prüfen.
12. npm-Pakete `0.9.10` und Dist-Tag `latest` prüfen.
13. `interlis-language-tools`-Quellmanifeste auf stabile `0.9.10` setzen und testen.
14. Einen Language-Tools-Snapshot mit `COMPILER_VERSION=0.9.10` bauen und publizieren beziehungsweise den vorgesehenen manuellen Release-Train starten.
15. Den daraus folgenden Web-IDE-Dispatch mit stabilem `compiler_version=0.9.10` prüfen.
16. `iox-cpp`-Remote-Pin auf `v0.9.10` mergen.
17. `iox-cpp` mit `IOX_FETCH_ILIC=ON` vollständig testen.
18. Dokumentierten aufgelösten Commit-SHA des Tags in `iox-cpp/docs/conformance.md` eintragen.
19. Temporäre `0.9.9`-Migrationsunterstützung als separaten Cleanup-Punkt erfassen.

Wichtig: Der stabile Compiler-Workflow darf gemäss Phase A6 weiterhin ohne automatischen Consumer-Dispatch bleiben. In diesem Fall erfolgt Schritt 14 bewusst über den kontrollierten Language-Tools-Release-Train. Die Web IDE muss trotzdem stabile Compiler-Versionen verarbeiten können.


# Teil F – Tests und Qualitätsgates

## 32. Verbindliche Testmatrix `ilic-fork`

### 32.1 Native Debug oder Developer Build

```sh
cmake -S . -B build/dev -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_BUILD_TESTS=ON \
  -DILIC_BUILD_CLI=ON
cmake --build build/dev --parallel
ctest --test-dir build/dev --output-on-failure
```

### 32.2 Native Release Build

```sh
cmake -S . -B build/release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_BUILD_TESTS=ON \
  -DILIC_BUILD_CLI=ON
cmake --build build/release --parallel
ctest --test-dir build/release --output-on-failure
build/release/ilic -version
```

Erwartete Ausgabe muss `0.9.10` enthalten und darf keine alte Version melden.

### 32.3 Library-only Subproject

Der neue netzwerkfreie Consumer-Smoke muss bestehen.

### 32.4 WASM

```sh
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

### 32.5 npm-Source-Pakete

```sh
npm test --prefix packages/repository-core
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
```

### 32.6 Snapshot-Staging

```sh
node --test test/npm/PrepareNpmSnapshotTest.mjs
node scripts/prepare-npm-snapshot.mjs \
  --timestamp 20260804090000 \
  --build-id 123
node scripts/test-npm-packages.mjs \
  --staging-root build/npm \
  --expected-version 0.9.10-SNAPSHOT.20260804090000.123 \
  --version-kind snapshot
```

### 32.7 Stable-Staging

```sh
node --test test/npm/PrepareNpmReleaseTest.mjs
node scripts/prepare-npm-release.mjs \
  --expected-version 0.9.10 \
  --expected-tag v0.9.10
node scripts/test-npm-packages.mjs \
  --staging-root build/npm-release \
  --expected-version 0.9.10 \
  --version-kind stable
```

---

## 33. Verbindliche Testmatrix `iox-cpp`

### 33.1 Ohne ilic

Vollständige Tests müssen unverändert bestehen.

### 33.2 Mit lokalem `ilic-fork`

- `iox.test.ilic.version`;
- `iox.test.ilic.model_based`;
- öffentliche Header;
- Coverage-Konfiguration, sofern verfügbar;
- Sanitizer-Build, sofern in bestehender CI vorgesehen.

### 33.3 Mit FetchContent-Tag

Nach Tag-Erstellung vollständiger Build mit `IOX_FETCH_ILIC=ON`.

### 33.4 CMake-Zielkontrolle

In Consumer-Konfiguration:

- `ilic::core` vorhanden;
- `ilic`-CLI nicht als unnötiges Default-Target gebaut;
- `ilic`-Tests nicht in `iox-cpp` registriert;
- `iox-cpp`-eigene Tests bleiben registriert.

---

## 34. Verbindliche Testmatrix `interlis-language-tools`

```sh
pnpm install --frozen-lockfile
pnpm build
pnpm lint
pnpm typecheck
pnpm test
pnpm test:snapshot
pnpm pack:verify
pnpm licenses:check
pnpm security:check
```

Zusätzlich:

- echter Compiler meldet die exakte erwartete Version;
- Compilerparser akzeptiert stabil `0.9.10`;
- Snapshotparser akzeptiert `0.9.10-SNAPSHOT...`;
- temporäre `0.9.9`-Kompatibilität ist getestet;
- stabiler Input verwendet den Stable-Stager von `ilic-fork`;
- Snapshot-Input verwendet den Snapshot-Stager von `ilic-fork`;
- Tarballs enthalten exakte interne Versionen;
- VSIX-Build bleibt funktionsfähig, falls im bestehenden Gate vorgesehen.

---

# Teil G – Klassen- und Methodenkompatibilität

## 35. `ilic-core`

### 35.1 `ilic::CompilerSession`

Keine Änderung an den öffentlichen Methoden:

```cpp
CompilerSession();
explicit CompilerSession(IncrementalCacheOptions cacheOptions);
~CompilerSession();
void putSource(...);
SourceUpdateResult updateSource(...);
bool removeSource(...);
SourceManager& sources();
const SourceManager& sources() const;
SyntaxSnapshot parse(...);
EditorSnapshot editorSnapshot(...);
SemanticSnapshot analyze(...);
CompilationAnalysisResult compileAndAnalyze(...);
CompilationResult compile(...);
IncrementalStats incrementalStats() const;
IncrementalTrace lastIncrementalTrace() const;
IncrementalCacheSnapshot incrementalCacheSnapshot() const;
void resetIncrementalStats();
void clearIncrementalCaches();
```

Dieser Auftrag betrifft Versions- und Veröffentlichungsinfrastruktur, nicht Compilersemantik.

### 35.2 `ilic::version()`

Einzige funktionale Änderung:

- Wert kommt aus CMake-generiertem Header;
- Rückgabewert `0.9.10`;
- Lebensdauer weiterhin statisch;
- keine Allokation;
- kein Besitztransfer;
- threadsicher durch Compile-Time-Konstante.

### 35.3 C-API

Keine Änderung an Speicher-, Session- oder Result-Handles.

`ilic_version()` bleibt ein dünner Forwarder.

### 35.4 `metamodel::MetaModelStore`

Keine Änderung an:

- `make<T>()`;
- `owns()`;
- `objectCount()`;
- `discardUnlinked()`;
- `clear()`;
- `addModel()` und weiteren Registrierungsfunktionen;
- Pointer-Lebensdauervertrag.

`iox-cpp` baut weiterhin direkt gegen diese konkrete API.

---

## 36. WASM-API

### 36.1 Additive Eigenschaft

`Compiler.compilerVersion` wird ergänzt.

### 36.2 Bestehende Methoden unverändert

```ts
Compiler.createSession()
CompilerSession.putSource(...)
CompilerSession.putWorkspace(...)
CompilerSession.removeSource(...)
CompilerSession.compile(...)
CompilerSession.parse(...)
CompilerSession.editorSnapshot(...)
CompilerSession.analyze(...)
CompilerSession.compileAndAnalyze(...)
CompilerSession.format(...)
CompilerSession.dispose()
```

### 36.3 Konsistenz

Für jede Operation mit Ergebnisobjekt:

```text
result.abiVersion      == compiler.abiVersion      == 1
result.compilerVersion == compiler.compilerVersion == 0.9.10
```

---

## 37. `iox-cpp`

Kein funktionaler Umbau der Modelltransformation.

Insbesondere erhalten bleiben:

- Zykluserkennung in Übersetzungsbeziehungen;
- Behandlung mehrdeutiger Modellnamen;
- Auflösung übersetzter Topics, Klassen und Properties;
- normative Transferreihenfolge;
- transiente Properties;
- Rollen und eingebettete Associations;
- Enumerationstranslation einschließlich `OTHERS`;
- terminaler Fehlerzustand des Writers;
- Diagnosecodes `ilic.unknown_name` und `ilic.model_mismatch`.

Wenn der Versionswechsel einen Compile-Fehler in diesen Bereichen auslöst, soll der Agent zuerst prüfen, ob `ilic 0.9.10` versehentlich eine bisher intern genutzte API verändert hat. Er darf nicht vorschnell Verhalten aus `iox-cpp` entfernen.

---

# Teil H – Release-Sicherheit

## 38. Vorabprüfung externer Zustände

Unmittelbar vor Veröffentlichung:

```sh
npm view @ilic/repository-core@0.9.10 version
npm view @ilic/tools@0.9.10 version
npm view @ilic/compiler-wasm@0.9.10 version
```

Erwartung vor Erstpublish: nicht vorhanden.

Zusätzlich prüfen, ob `v0.9.10` bereits existiert. Ein vorhandener unerwarteter Tag oder Paketstand ist ein harter Stop.

## 39. Supply-Chain-Regeln

- Checkout mit `persist-credentials: false` in reinen Build-Jobs.
- Schreibrechte nur in Publish-Jobs.
- npm-Publish über Trusted Publishing/OIDC.
- Keine langlebigen npm-Tokens, wenn bestehende OIDC-Konfiguration funktioniert.
- Gepinnte Emscripten-Version.
- Exakte Release-Commit-SHAs in Manifeste und Summaries schreiben.
- Nur verifizierte Tarballs publizieren.
- Keine Neukompilation im Publish-Job.

## 40. Teilpublikation

Da drei npm-Pakete nicht atomar publiziert werden können:

- Reihenfolge strikt einhalten;
- bestehende identische Versionen bei Wiederholung überspringen;
- abweichende Versionen oder `gitHead` hart ablehnen;
- Workflow-Summary muss pro Paket `published`, `already present` oder `failed` zeigen.

## 41. Keine Retagging-Strategie

`v0.9.10` ist nach Push unveränderlich.

Nicht zulässig:

```sh
git tag -f v0.9.10
git push --force origin v0.9.10
```

Fehler führen zu `0.9.11`.

---

# Teil I – Erforderliche Dateiliste

## 42. `ilic-fork`: voraussichtlich zu ändern

```text
CMakeLists.txt
source/core/Compiler.cpp
packages/repository-core/package.json
packages/tools/package.json
packages/compiler-wasm/package.json
packages/compiler-wasm/compiler.js
packages/compiler-wasm/wasm-memory.js
packages/compiler-wasm/index.d.ts
scripts/prepare-npm-snapshot.mjs
scripts/test-npm-packages.mjs
.github/workflows/build-native-release.yml
docs/build-und-publikationspipeline.md
docs/npm-publikation.md
readme.md
```

## 43. `ilic-fork`: neu anzulegen

```text
cmake/IlicVersionConfig.h.in
test/core/VersionContractTest.cpp
test/abi/CapiVersionContractTest.c
test/consumer/source-subproject/CMakeLists.txt
test/consumer/source-subproject/main.cpp
scripts/npm-package-staging.mjs
scripts/prepare-npm-release.mjs
scripts/check-release-version.mjs
test/npm/PrepareNpmReleaseTest.mjs
test/release/CheckReleaseVersionTest.mjs
docs/versioning.md
docs/releasing.md
```

Der Agent darf Namen leicht an bestehende Konventionen anpassen, muss aber dieselben Verantwortlichkeiten klar trennen.

## 44. `iox-cpp`: voraussichtlich zu ändern

```text
CMakeLists.txt
cmake/IoxDependencies.cmake
cmake/IoxOptions.cmake
test/CMakeLists.txt
README.md
docs/architecture.md
docs/conformance.md
iox-cpp-llm-coding-spec.md
```

## 45. `iox-cpp`: neu anzulegen

```text
test/ilic/IlicDependencyVersion.test.cpp
```

## 46. `interlis-language-tools`: voraussichtlich zu ändern

```text
scripts/prepare-npm-snapshot.mjs
test/npm/*Snapshot*.test.mjs
packages/language-service/package.json
packages/language-server/package.json
pnpm-lock.yaml
docs/build-und-publikationspipeline.md
docs/release.md
```

Die tatsächliche Suche entscheidet über weitere Dateien.

### `interlis-web-ide`: voraussichtlich zu ändern

```text
.github/workflows/pages.yml
docs/build-und-publikationspipeline.md
```

Bevorzugt neu anzulegen:

```text
scripts/validate-release-metadata.mjs
test/release/ValidateReleaseMetadataTest.mjs
```

Nur bei tatsächlich notwendiger Regression zusätzlich Anwendungscode oder E2E-Fixtures ändern. `package.json`, `pnpm-workspace.yaml` und `pnpm-lock.yaml` zunächst prüfen, aber nicht automatisch ändern.

---

# Teil J – Commit- und PR-Strategie

## 47. Empfohlene Commits

### `interlis-language-tools`

1. `build: accept ilic 0.9.10 compiler snapshots`
2. Nach stabilem Publish: `deps: use ilic compiler 0.9.10`

### `ilic-fork`

1. `build: centralize ilic version and prepare 0.9.10`
2. `build: support clean source subproject consumption`
3. `release: add stable npm package staging and verification`
4. `release: coordinate native and npm publication`
5. `docs: document ilic versioning and release operation`

### `iox-cpp`

1. Vor Tag lokal vorbereitbar: `build: prepare ilic 0.9.10 source integration`
2. Nach Tag: `deps: pin ilic v0.9.10`

### `interlis-web-ide`

1. `release: validate ilic 0.9.10 release metadata dynamically`
2. `docs: document stable and snapshot compiler inputs`

Commits dürfen zusammengelegt werden, wenn das Repository kleinere atomare PRs bevorzugt. Jeder Commit muss für sich verständlich sein und darf keine absichtlich kaputte Zwischenstufe erzeugen.

---

# Teil K – Definition of Done

## 48. Funktionale Fertigstellung

Der Auftrag ist erst abgeschlossen, wenn alle folgenden Punkte erfüllt sind:

- [ ] `ilic-fork` verwendet `PROJECT_VERSION 0.9.10`.
- [ ] `ilic::version()` ist nicht mehr hart codiert.
- [ ] `ilic_version()` liefert `0.9.10`.
- [ ] `ilic_abi_version()` bleibt `1`.
- [ ] alle drei Compiler-npm-Pakete stehen auf `0.9.10`.
- [ ] interne npm-Abhängigkeiten sind exakt `0.9.10`.
- [ ] stabile npm-Tarballs können lokal erzeugt werden.
- [ ] Tarballs werden in einem leeren Consumer-Projekt installiert und ausgeführt.
- [ ] WASM-`Compiler.compilerVersion` liefert `0.9.10`.
- [ ] Result-`compilerVersion` stimmt mit der geladenen WASM-Version überein.
- [ ] `ilic-fork` kann sauber als Library-Unterprojekt eingebunden werden.
- [ ] Top-Level-CLI- und Testverhalten bleibt erhalten.
- [ ] nativer GitHub-Release-Workflow prüft vor Publikation auch WASM/npm.
- [ ] stabiler npm-Publish ist idempotent und OIDC-basiert.
- [ ] `iox-cpp` verwendet `v0.9.10` statt des alten Commit-Pins.
- [ ] `iox-cpp` linkt `ilic::core`.
- [ ] `iox-cpp` verändert `BUILD_TESTING` nicht mehr global.
- [ ] `iox.test.ilic.version` prüft exakt `0.9.10`.
- [ ] alle modellbasierten `iox-cpp`-Tests bestehen.
- [ ] `interlis-language-tools` akzeptiert `0.9.10-SNAPSHOT...`.
- [ ] `interlis-language-tools` kann den stabilen Compilerwert `0.9.10` lokal rekonstruieren und in Language-Tools-Tarballs verwenden.
- [ ] `interlis-web-ide` akzeptiert `0.9.10` und `0.9.10-SNAPSHOT...` nur bei passender ausgecheckter Compiler-Basis.
- [ ] der produktive `0.9.9`-Regex wurde aus dem Pages-Workflow entfernt.
- [ ] `interlis-web-ide` verwendet weiterhin gepinnte SHAs und lokale verifizierte Tarballs.
- [ ] `pnpm check` sowie Chromium-/Firefox-E2E der Web IDE bestehen; WebKit ist auf unterstützter macOS-Umgebung geprüft.
- [ ] die stabile Consumerbasis ist `0.9.10`.
- [ ] alle Entwickler- und Release-Abläufe sind dokumentiert.
- [ ] keine Conan-, vcpkg- oder native SDK-Arbeit wurde eingeschoben.

## 49. Release-Fertigstellung

Nach ausdrücklicher Freigabe zusätzlich:

- [ ] Tag `v0.9.10` zeigt auf den freigegebenen Commit.
- [ ] GitHub Release `v0.9.10` existiert.
- [ ] alle drei nativen CLI-Archive existieren.
- [ ] `SHA256SUMS` ist korrekt.
- [ ] `@ilic/repository-core@0.9.10` existiert.
- [ ] `@ilic/tools@0.9.10` existiert.
- [ ] `@ilic/compiler-wasm@0.9.10` existiert.
- [ ] npm `latest` zeigt bei allen drei Paketen auf `0.9.10`.
- [ ] npm-`gitHead` entspricht dem Release-Commit.
- [ ] `interlis-language-tools` baut und testet gegen `0.9.10`.
- [ ] ein Language-Tools-Release-Train mit `compiler_version=0.9.10` ist erfolgreich.
- [ ] `interlis-web-ide` rekonstruiert diesen Input aus exakten SHAs und deployed erfolgreich nach Pages.
- [ ] die ausgelieferte Web IDE lädt Compiler-Produktversion `0.9.10` bei ABI-Version `1`.
- [ ] `iox-cpp` baut per `IOX_FETCH_ILIC=ON` gegen `v0.9.10`.
- [ ] der aufgelöste Tag-Commit ist in der Consumer-Dokumentation festgehalten.

---

# Teil L – Abschlussbericht des Agenten

## 50. Erforderliches Berichtsformat

Der Agent beendet seine Arbeit mit einem Bericht in dieser Struktur:

```text
1. Repositories und geprüfte Ausgangs-Commits
2. Geänderte Dateien je Repository
3. Versionierungsänderungen
4. CMake-Unterprojektänderungen
5. WASM- und npm-Änderungen
6. iox-cpp-Migration
7. interlis-language-tools-Migration
8. interlis-web-ide-Migration und Pages-Release-Vertrag
9. Ausgeführte Testbefehle mit Ergebnis
10. Nicht ausführbare Tests und genauer Grund
11. Noch ausstehende Release-Handlungen
12. Bekannte Risiken
13. Empfohlene nächste Aktion
```

Er darf nicht lediglich „alle Tests grün“ schreiben. Für jeden Gate-Bereich muss der konkrete Befehl und das Ergebnis genannt werden.

---

## 51. Schlussentscheidung

Für `0.9.10` gilt bewusst folgendes einfache Modell:

```text
                         ilic-fork v0.9.10
                                 |
          +----------------------+----------------------+
          |                      |                      |
   GitHub Release             npm 0.9.10          Git source dependency
   native CLI only         WASM and JS tools         CMake FetchContent
          |                      |                      |
       Anwender        interlis-language-tools          iox-cpp
                                  |
                     exact SHAs + exact versions
                                  |
                         interlis-web-ide
                                  |
                    local verified tarballs
                                  |
                           GitHub Pages
```

Diese Variante ist ausreichend für den ersten koordinierten Release, weil sie die vorhandenen Projektstrukturen nutzt, die bekannten Consumer eindeutig versioniert, den vollständigen Release-Train bis GitHub Pages erhält und keine vorzeitige native Paketierungsinfrastruktur einführt.

Eine spätere Version kann auf dieser Grundlage ein installierbares natives CMake-Paket, Conan oder eine stabile Shared-Library-ABI ergänzen. Diese spätere Erweiterung darf den Release `0.9.10` nicht verzögern oder unnötig verkomplizieren.
