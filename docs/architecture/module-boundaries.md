# Modulgrenzen

Die öffentlichen Fassaden bleiben klein und delegieren an Komponenten mit
einer klaren Zuständigkeit. Diese Grenzen sind wichtiger als ein abstraktes
Zeilenbudget: Eine Fassade darf wachsen, wenn sie weiterhin nur übersetzt und
koordiniert, aber keine fremde Fachlogik übernimmt.

## Nativer Compiler

`CompilerSession` ist die öffentliche Lebensdauer- und Synchronisationsgrenze.
Der veränderliche Zustand liegt in `CompilerSessionState`, Quellenänderungen
koordiniert `SourceUpdateCoordinator`, Snapshots erzeugt `SnapshotService` und
Kompilationsläufe führen `CompilationOrchestrator` und `CompilationRun` aus.
`CompilationTranscript` formatiert die protokollierten Ereignisse. Parser,
Checker und Generatoren erhalten konkrete Referenzen auf ihre Abhängigkeiten;
es gibt keinen Service-Locator und keinen globalen aktiven Context.

## C-ABI und WebAssembly

`Capi.cpp` übersetzt ausschliesslich Handles, Requests und Fehler auf die
stabile C-ABI. Registry, Request-Decodierung und JSON-Projektion liegen in
`CapiRegistries`, `CapiRequestDecoder` und `CapiJsonProjectors`. Das
Emscripten-Target kompiliert dieselben C-ABI-Komponenten wie die native
Bibliothek.

Im npm-Paket ist `index.js` die öffentliche Exportfassade. Session-Lebensdauer,
WASM-Speicherzugriffe und die explizite Projektion älterer Editor-Snapshots
liegen in eigenen Modulen. Repository-I/O bleibt ausserhalb des WASM-Moduls und
wird über den Host bereitgestellt.

## Dauerhafte Prüfung

Der repository-lokale Guard prüft wenige, fachliche Eigentumsgrenzen statt
Dateiexistenz oder Dateigrösse:

```sh
python3 scripts/check-architecture-boundaries.py .
```

Er verhindert insbesondere generierte Parserintegration in `Compiler.cpp`,
JSON-Projektion in `Capi.cpp` und Legacy-Projektion in der WASM-Exportfassade.
Ein integrierter Selbsttest stellt bei jedem Lauf sicher, dass der Matcher eine
repräsentative Grenzverletzung erkennt.
Zusätzliche Regeln sollen nur aufgenommen werden, wenn sie eine dauerhafte
Verantwortungsgrenze ausdrücken.
