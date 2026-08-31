# ilic – INTERLIS-Compiler

`ilic` validiert INTERLIS-1.0-, -2.3- und -2.4-Modelle und erzeugt unter
anderem ILI-, IMD-, XSD- und GML-Ausgaben. Der Compiler steht als CLI,
C++-Bibliothek, C-ABI und WebAssembly-Paket zur Verfügung.

Dieses Repository ist ein weiterentwickelter Fork des von
[infoGrips](https://www.infogrips.ch/) geschaffenen und gepflegten
Originalprojekts. Die historischen Handbücher und Entwicklernotizen unter
[`doc/`](doc/) stammen teilweise aus diesem Ursprung. Build-, API- und
Release-Angaben für den Fork stehen ausschliesslich unter [`docs/`](docs/).

## Schnellstart

```sh
cmake -S . -B build/local -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/local --parallel
ctest --test-dir build/local --output-on-failure
build/local/ilic -silent docs/examples/models/Legacy.ili
```

Für reproduzierbare Builds und plattformspezifische Voraussetzungen siehe
[Build und Installation](docs/build-und-installation.md).

## Artefakte

- Native CLI-Archive: [GitHub Releases](https://github.com/edigonzales/ilic-fork/releases)
- JavaScript/WASM: `@ilic/repository-core`, `@ilic/tools` und
  `@ilic/compiler-wasm` auf npm
- Native Bibliotheken: gemeinsames vcpkg-Registry und GitHub-Packages-
  Binary-Cache

Die Beziehungen zu `iox-cpp`, `duckdb-interlis`,
`interlis-language-tools` und `interlis-web-ide` sind in der
[Ökosystemübersicht](docs/ecosystem.md) beschrieben. Versionen, Snapshots und
Releases stehen im [Release-Runbook](docs/release.md).

## Dokumentation

Der [Dokumentationsindex](docs/README.md) führt zu CLI-, API-, WASM-,
Repository-, Architektur- und Conformance-Dokumentation. Historische Angaben
unter `doc/` sind Referenzmaterial und kein aktueller Build- oder
Releasevertrag.

## Lizenz

Der ilic-Quellcode und die Binärdateien dürfen ohne Einschränkung kopiert,
verändert und in kommerzielle oder nichtkommerzielle Software integriert
werden. Für eingebundene Fremdbibliotheken können die jeweils dort
aufgeführten Lizenzen gelten.
