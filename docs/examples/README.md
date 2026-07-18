# Ausführbare Beispiele

[Dokumentationsindex](../README.md) · [Build](../build-und-installation.md)

Diese Beispiele werden von CTest beziehungsweise den Tests von
`@ilic/compiler-wasm` ausgeführt. Remote-Beispiele in den Textseiten sind
bewusst nicht Teil der automatischen Abnahme; die Tests verwenden lokale
Repository-Fixtures und benötigen kein Netzwerk.

## Modelle

| Datei | Zweck |
| --- | --- |
| [`models/Legacy.ili`](models/Legacy.ili) | minimales gültiges INTERLIS-1-Modell |
| [`models/Base.ili`](models/Base.ili) | importiertes INTERLIS-2.3-Basismodell |
| [`models/Example.ili`](models/Example.ili) | Root-Modell mit Import, `!!`-Kommentar und Metaattribut |
| [`models/Invalid.ili`](models/Invalid.ili) | absichtlich unbekannter Typ für Diagnostics |

```sh
build/macos/ilic -silent models/Legacy.ili
build/macos/ilic -silent -ilidirs models models/Example.ili
build/macos/ilic models/Invalid.ili
```

Die Befehle funktionieren aus diesem `docs/examples`-Verzeichnis. Vom
Repository-Root müssen die Pfade mit `docs/examples/` beginnen.

## CLI und Formatter

[`cli.sh`](cli.sh) prüft:

- INTERLIS 1.0 und 2.3;
- lokale Imports mit und ohne Auto-Search;
- ein lokales `ilimodels.xml`-Repository;
- stille Konsole mit Logdatei;
- `-werror` und einen konfigurierten Erfolgsstatus;
- ILI 1.0, ILI 2.3, ILI 2.4, IMD, XSD und GML in einem vorhandenen Zielverzeichnis;
- Formatter-Ausgabe und `--check`.

Manueller Aufruf:

```sh
docs/examples/cli.sh \
  "$PWD/build/macos/ilic" \
  "$PWD/build/macos/ilic-format" \
  "$PWD" \
  /tmp/ilic-docs-example
```

## C++

[`cpp-session.cpp`](cpp-session.cpp) registriert zwei virtuelle Dateien,
kompiliert einen Import und formatiert den Root-Text.

```sh
cmake --build build/macos --target ilic_docs_cpp_example
build/macos/ilic_docs_cpp_example
```

## C

[`c-api.c`](c-api.c) ist eine reine C11-Quelldatei. Sie übergibt Source und
Compilation Request an `ilic/capi.h`, liest das längenbegrenzte JSON-Resultat
und gibt alle Handles wieder frei.

```sh
cmake --build build/macos --target ilic_docs_c_example
build/macos/ilic_docs_c_example
```

Der Target verwendet den C-Compiler für die Quelldatei und den C++-Linker für
die intern in C++ implementierte statische Bibliothek.

## WebAssembly und JavaScript

[`wasm-session.mjs`](wasm-session.mjs) kompiliert und formatiert eine virtuelle
Quelle über das reale WASM-Modul:

```sh
source /pfad/zu/emsdk/emsdk_env.sh
./scripts/build-wasm.sh
node docs/examples/wasm-session.mjs
```

[`wasm-repository.mjs`](wasm-repository.mjs) löst standardmäßig das lokale
Test-Repository mit `@ilic/tools` auf, kopiert den resultierenden Workspace in
WASM und kompiliert das Root-Modell:

```sh
node docs/examples/wasm-repository.mjs
```

Beide Programme exportieren zusätzlich eine Funktion und werden direkt aus den
Node-Tests importiert.

## Alle Dokumentationsbeispiele testen

```sh
cmake -S . -B build/macos -DBUILD_TESTING=ON
cmake --build build/macos --parallel
ctest --test-dir build/macos -R '^ilic_docs_' --output-on-failure

source /pfad/zu/emsdk/emsdk_env.sh
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Der Test `ilic_docs_links` prüft zusätzlich, dass alle relativen Links der neuen
Dokumentation auf vorhandene Dateien zeigen.
