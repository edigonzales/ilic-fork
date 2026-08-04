# Versionierungsvertrag

Die einzige stabile Produktbasis steht in `project(ilic VERSION …)` in
`CMakeLists.txt`. Für diesen Release ist sie `0.9.10`. C++, C-ABI, native CLI
und die drei eingecheckten npm-Manifeste müssen diese Basis exakt melden;
`@ilic/tools` hängt exakt von derselben Version von `@ilic/repository-core` ab.

Stabile Artefakte verwenden `0.9.10`. Snapshots verwenden
`0.9.10-SNAPSHOT.YYYYMMDDHHmmss` mit optionaler numerischer Build-ID. Ein
Snapshot-WASM wird mit `ILIC_WASM_VERSION` gebaut, damit C-ABI,
`Compiler.compilerVersion`, Compilation-Ergebnis und npm-Paketversion dieselbe
unveränderliche Identität tragen. Der Präfix muss weiterhin der CMake-Basis
entsprechen.

`scripts/check-release-version.mjs` prüft die stabilen Quellen und das Tag.
`prepare-npm-snapshot.mjs` beziehungsweise `prepare-npm-release.mjs` stagen
nur explizit erlaubte Dateien und verändern keine Quellmanifeste.
