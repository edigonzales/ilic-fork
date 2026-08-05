# Versionierungsvertrag

Die numerische Basisversion steht zentral in `project(ilic VERSION …)` in
`CMakeLists.txt`. Die aktuelle Entwicklungszeile ergänzt den Qualifier
`SNAPSHOT`; C++, C-ABI, native CLI und die drei eingecheckten npm-Manifeste
melden deshalb `0.10.0-SNAPSHOT`. `@ilic/tools` hängt exakt von derselben
Source-Version von `@ilic/repository-core` ab.

Ein konkretes Snapshot-Artefakt verwendet
`0.10.0-SNAPSHOT.YYYYMMDDHHmmss` mit optionaler numerischer Build-ID. Ein
Snapshot-WASM wird mit `ILIC_WASM_VERSION` gebaut, damit der Wrapper und das
Artefakt dieselbe unveränderliche Identität tragen. Der Präfix muss weiterhin
der CMake-Basis `0.10.0-SNAPSHOT` entsprechen. Es gibt in dieser
Entwicklungszeile kein finales `0.10.0` und keinen Release-Tag.

`scripts/check-release-version.mjs` prüft die stabilen Quellen und das Tag.
`prepare-npm-snapshot.mjs` beziehungsweise `prepare-npm-release.mjs` stagen
nur explizit erlaubte Dateien und verändern keine Quellmanifeste.
