# ilic-Dokumentation

Diese Dokumentation beschreibt den implementierten Stand des aktuellen
`ilic-fork`. Historische Dokumente unter `../doc/` gehören zum ursprünglichen
infoGrips-Projekt und sind nicht massgeblich für Build oder Release dieses
Forks.

## Einstieg und Betrieb

| Thema | Dokument |
| --- | --- |
| Abhängigkeiten und Artefaktfluss aller fünf Repositories | [Ökosystem](ecosystem.md) |
| Lokale, native und WASM-Builds | [Build und Installation](build-und-installation.md) |
| Versionierung, npm, GitHub Releases und vcpkg | [Release](release.md) |
| Befehlszeile | [CLI-Referenz](cli.md) |
| Modell-Repositories und Cache | [Repositories](repositories.md) |
| Diagnostik und strukturierte Logs | [Diagnostik und Logging](diagnostik-und-logging.md) |
| Unterstützte Sprachversionen und Features | [Funktionsumfang](funktionsumfang.md) |
| Externe Compiler-Conformance-Suite | [Compiler-Conformance](conformance.md) |

## Einbettung und Architektur

| Thema | Dokument |
| --- | --- |
| C++-API, C-ABI und JSON-Protokoll | [Native APIs](native-api.md) |
| Browser, Node, Worker und LSP | [WASM](wasm.md) |
| Editor-Snapshots | [Language-Tooling-Snapshots](language-tooling-snapshots.md) |
| Standardformatierung | [Formatter](formatter.md) |
| Modulverantwortlichkeiten | [Modulgrenzen](architecture/module-boundaries.md) |
| Inkrementelle Sessions | [Inkrementelle Kompilation](architecture/incremental-compilation.md) |
| Vollständige Programme | [Beispiele](examples/README.md) |

Die versionierten JSON-Schemas liegen unter `../schemas/`. Bewusste aktuelle
Einschränkungen bleiben in den jeweiligen Fachdokumenten sichtbar.
