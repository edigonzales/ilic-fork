# P7-Abschlussbericht

Stand: 2026-08-03. Die bestehenden Arbeitsänderungen wurden beibehalten.

## Ziel und Toolchain

P7 migriert Zustands- und Ablaufhoheit in interne Komponenten. Die öffentliche Compiler-, C-ABI-, WASM-, Language-Service- und Workbench-API bleibt erhalten.

Die WASM-Abnahme verwendet Emscripten 3.1.64. Die Toolchain wird bei Bedarf über `scripts/build-wasm.sh` eingerichtet.

## Native Snapshotpipeline

`SnapshotPipeline.cpp` ist eine Delegationsfassade mit 28 Non-empty-Zeilen. Die Snapshotbildung liegt in `ParsedSourceArtifactFactory`. Sprachdetektion, Strict-/Tolerant-Dispatch, Recovery-Tokenstream, Diagnoseordnung, Syntaxprojektion sowie Editorprojektion sind in getrennten internen Komponenten organisiert. Ili1 und Ili2 besitzen eigene Editorprojektionsadapter.

Das Parserartefakt hält die ANTLR-Strukturen intern. Syntax- und Editorprojektion werden aus demselben Parserlauf erzeugt.

## LanguageService

`service.ts` ist eine öffentliche Fassade mit 4 Non-empty-Zeilen. Die Runtime delegiert Source-Lifecycle, Diagnosekoordination, Semantic-Snapshot-Verwaltung, Dependency-Index, Repositoryauflösung, Run-Koordination und Scheduling an interne Komponenten. Save-/Current-/Last-Good-/Freshness-Verträge und Diagnoseevents bleiben behavior-kompatibel.

Der Scheduler besitzt Queue, Prioritäten, Coalescing, Run-IDs, Cancellation und Promise-Lifecycle. Der Run-Coordinator führt Compilerlauf und Missing-Model-Auflösung in derselben Session aus.

## Worker

`compiler-worker.ts` ist auf Re-Exports mit 12 Non-empty-Zeilen reduziert. Der gemeinsame RPC-Client besitzt Portbindung, Request-IDs, Pending-Settlement, Fehler- und Restart-Verhalten. Source-Mirror und Lifecycle-Tracker besitzen Replay, Bytezählung, Fallback- und Queue-Statistiken.

Compiler- und Editorbackend bleiben getrennte Adapter mit jeweils eigener Fallbackpolitik. Replay wird nur beim Attach/Restart ausgeführt; normale Sourceupdates werden direkt gespiegelt.

## Web-IDE

`workbench.ts` ist eine öffentliche Exportfassade mit 3 Non-empty-Zeilen. Die Runtime verwendet Tab-, Save- und Recovery-Controller. Ergänzende Controller kapseln View, Workspace, Compilation, Problems, Outline, Diagramm, Layout, Commands und Suggestions.

Save, Auto-Save, Recovery und Compile-after-save verwenden definierte Controller-Lifecycle. Controller-Dispose ist idempotent und wird vor dem Entfernen der Workbench-Subscriptions ausgeführt.

## Guards und Abnahme

Der P7-Guard prüft die Budgets:

- `SnapshotPipeline.cpp <= 180`
- `Compiler.cpp <= 300`
- `Capi.cpp <= 300`
- `packages/compiler-wasm/index.js <= 100`
- `service.ts <= 500`
- `compiler-worker.ts <= 200`
- `workbench.ts <= 650`

Zusätzlich werden Legacy-State in den Fassaden, Worker-Transport in der Fassade, Komponenten-Grenzen und der negative Projector-Canary geprüft.

## Verifikation

Release-Native: 146/146 CTest grün. Repository-disabled: 129/129 CTest grün.
ASan/UBSan: 146/146 CTest grün. TSan: 146/146 CTest grün.

Language Tools: kompletter Workspace-Check grün, einschließlich 215 Language-Service-Tests.

Web-IDE: Lint, Typecheck, 34 Unit-Tests und PWA-Build grün. Die Browser-E2E-Suite lief mit 84 bestandenen und 18 planmäßig übersprungenen Tests in Chromium, Firefox und WebKit; der isolierte Chromium-Performance-Lock bestand zusätzlich 3/3-mal.

WASM-Build über `scripts/build-wasm.sh` mit Emscripten 3.1.64 und WASM-NPM-Suite 14/14 grün; `pnpm pack --dry-run` ebenfalls grün. Damit sind Release, Repository-disabled, Sanitizer, TSan, WASM-Pack/NPM, Language-Service, Web-IDE und die Browser-Behavior-Locks ausgeführt.
