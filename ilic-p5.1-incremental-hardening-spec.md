# P5.1-Spezifikation: Hardening, Nachweisbarkeit und Vereinfachung der inkrementellen Compilerarchitektur

**Projekt:** `ilic-fork`  
**Abhängige Folgeprojekte:** `../interlis-language-tools`, `../interlis-web-ide`  
**Status:** normative Implementierungsspezifikation für einen LLM-Coding-Agenten  
**Stand der Analyse:** 1. August 2026  
**Referenzstände bei Erstellung:**

```text
ilic-fork:
  e3d7f666c3340527b6b860a20349bde96e342cc6

interlis-language-tools:
  4f4fe2690e226f81f88172cfae0d36737467e392

interlis-web-ide:
  ad92df63089f7df9b65519fb34a5b3f891c0cffe
```

Die genannten Commits beschreiben den untersuchten Ausgangszustand. Bei der tatsächlichen Umsetzung ist der aktuelle lokale Repositoryzustand maßgeblich. Der Agent MUSS Abweichungen gegenüber den Referenzständen inventarisieren und darf neuere funktionale Korrekturen nicht zurückbauen.

---

## 1. Auftrag

P5 hat eine sessionlokale inkrementelle Compilerarchitektur eingeführt:

- Source-Update-Klassifikation;
- wiederverwendbare Parserartefakte;
- einen Parsercache;
- einen Rootanalyse-Cache;
- inkrementelle Statistiken;
- C-ABI- und WASM-Zugriff;
- Worker-Integration;
- Repository-Nachladen ohne normalen Session-Neustart;
- inkrementelle Workspace-Synchronisierung in der Web-IDE.

P5.1 ist **keine weitere Featurephase**. P5.1 muss die vorhandene P5-Architektur korrigieren, beweisbar machen, vereinfachen und gegen fachliche sowie lebensdauerbezogene Fehler härten.

Der Coding-Agent MUSS P5.1 vollständig über alle drei Repositories implementieren:

```text
../ilic-fork
../interlis-language-tools
../interlis-web-ide
```

P5.1 hat sechs gleichrangige Hauptziele:

1. **Striktes Compilerparsing und tolerantes Editorparsing fachlich trennen.**
2. **Sourceänderungen einschließlich neu hinzugefügter Modelle korrekt auf Rootanalysen abbilden.**
3. **Parser- und Rootcache mit korrekter Ownership, zentraler Löschlogik und konsistenter Bytebuchhaltung implementieren.**
4. **Speicherbudgets realistisch und konservativ messen, insbesondere für WASM.**
5. **Statistiken und Traces semantisch korrekt, reproduzierbar und als Testorakel verwendbar machen.**
6. **Cold-/Incremental-Äquivalenz über Mutationssequenzen und Consumer-E2E beweisen.**

Daneben MUSS P5.1 die Codebasis vereinfachen:

- Inkrementalitätslogik aus `Compiler.cpp` herauslösen;
- Parseraufbau und Snapshotprojektionen aus `SnapshotPipeline.cpp` entflechten;
- duplizierte Cachelookup- und Insertpfade entfernen;
- C-ABI-Serialisierung modularisieren, soweit dies ohne unnötiges Risiko möglich ist;
- die Zuständigkeit des TypeScript-Layers gegenüber dem nativen Compiler dokumentieren und testen;
- die Web-IDE von „vollständiger Scan plus Diff“ in Richtung ereignisbasierter Sourceupdates weiterentwickeln.

P5.1 darf die fachliche Bedeutung bestehender öffentlicher APIs nicht brechen.

---

## 2. Normative Sprache

Die Schlüsselwörter werden normativ verwendet:

- **MUSS / DARF NICHT:** zwingend;
- **SOLL / SOLL NICHT:** starke Vorgabe; Abweichung nur mit dokumentierter Begründung;
- **DARF:** ausdrücklich zulässig;
- **KANN:** optional.

Eine Anforderung gilt erst als umgesetzt, wenn:

1. produktiver Code vorhanden ist;
2. ein sinnvoller automatisierter Test vorhanden ist;
3. der Test tatsächlich ausgeführt wurde;
4. keine relevante Regression in den drei Repositories besteht;
5. der Abschlussbericht den Nachweis enthält.

Kommentare oder Dokumentation allein erfüllen keine Muss-Anforderung.

---

## 3. Verbindlicher Arbeitsbereich

### 3.1 Primärrepository `ilic-fork`

P5.1 betrifft mindestens:

```text
include/ilic/Compiler.h
include/ilic/Incremental.h
include/ilic/SourceManager.h
include/ilic/capi.h

source/core/Compiler.cpp
source/core/CompilerContext.*
source/core/CompilationSourceStore.*
source/core/ParsedSourceArtifact.*
source/core/SnapshotPipeline.*
source/core/SourceManager.*
source/core/Semantic.*
source/core/SourceRangeMapper.*

source/input/ili1/Ili1Input.*
source/input/ili2/Ili2Input.*
source/util/IliFile.*

source/abi/Capi.cpp
source/abi/Json.*

packages/compiler-wasm/
packages/tools/

test/core/
test/abi/
test/npm/
test/data/
scripts/
cmake/
.github/workflows/
docs/
conformance/
```

### 3.2 Folgeprojekt `interlis-language-tools`

Mindestens:

```text
packages/language-service/src/types.ts
packages/language-service/src/compiler.ts
packages/language-service/src/compiler-worker.ts
packages/language-service/src/service.ts
packages/language-service/src/cache.ts
packages/language-service/src/live-analysis.ts
packages/language-service/src/features.ts
packages/language-service/src/repository.ts

packages/language-service/src/*.test.ts
packages/language-server/src/
packages/monaco-adapter/src/
packages/diagram/src/
packages/docx/src/

scripts/
docs/
.github/workflows/
```

### 3.3 Folgeprojekt `interlis-web-ide`

Mindestens:

```text
src/workbench/workbench.ts
src/workspace/source-synchronizer.ts
src/workspace/
src/language-worker.ts
src/interlis-compiler.worker.ts
src/language-repository.ts
src/main.ts

test/
tests/
e2e/
playwright.config.*
vite.config.*
.github/workflows/
docs/
```

Der Agent MUSS reale Aufrufketten verfolgen und darf sich nicht auf die obige Dateiliste beschränken.

---

## 4. Voraussetzungen P0 bis P5

P5.1 setzt die folgenden erreichten Eigenschaften voraus und MUSS sie erhalten.

### 4.1 P0 – Testhärtung

Erhalten bleiben müssen:

- Release- und Debug-Tests;
- Assertions, die auch unter `NDEBUG` funktionieren;
- ASan;
- UBSan;
- Leak Detection;
- TSan;
- CTest-Integration;
- Plattformmatrix Linux/macOS/Windows;
- reproduzierbare Testtargets;
- negative Architecture-Guard-Canaries.

### 4.2 P1 – externe Compiler-Conformance

Erhalten bleiben müssen:

- gepinnter externer Testsuite-Commit;
- explizite Baseline;
- Reportartefakte;
- nachgelagerte Exitcode-Durchsetzung;
- keine Erweiterung der Baseline zur Kaschierung einer P5.1-Regression.

P5.1 MUSS die Baseline nicht vollständig abbauen. P5.1 DARF aber keine neue Abweichung erzeugen.

### 4.3 P2 – Sessionisolation

Erhalten bleiben müssen:

- `CompilerContext` pro Compilerlauf;
- keine veränderlichen Compiler-Globals;
- keine globale Compiler-Mutex;
- keine globalen Parser- oder Rootcaches;
- keine `thread_local` Compilerzustände;
- unabhängige parallele Sessions;
- klare Ownership von Source, Parser, Metamodell und Resultaten.

### 4.4 P3 – ein autoritativer nativer Parser

Erhalten bleiben müssen:

- kein produktiver JavaScript-Quelltextparser;
- native Syntax- und Editor-Snapshots;
- Native-/WASM-Parität;
- UTF-8-/UTF-16-korrekte Ranges;
- tolerante Editoranalyse;
- stabile vorhandene Snapshotverträge.

P5.1 präzisiert P3:

> „Ein autoritativer Parser“ bedeutet nicht, dass strikte Compileranalyse und tolerante Editor-Recovery denselben mutierten Tokenstrom verwenden dürfen. Beide Modi müssen dieselbe Grammatik und denselben nativen Parsercode verwenden, aber Recovery darf die fachliche Compilerquelle nicht verändern.

### 4.5 P4 – Repository- und Sourcegrenzen

Erhalten bleiben müssen:

- autoritative `uri + source`-Identität;
- Repositoryquellen ohne Tempfile-Zwang;
- modulare Repositoryports;
- Node-/Browser-Kompatibilität;
- direkter Workspace-Installationspfad;
- sichere XML-Verarbeitung;
- stabile Repository-URIs.

### 4.6 P5 – sessionlokale Inkrementalität

Erhalten bleiben müssen:

- `updateSource`;
- Updateklassifikation;
- Parserartefakte;
- Parsercache;
- Rootcache;
- Statistikzugriff;
- Cache-Clear;
- additive C-ABI- und WASM-Funktionen;
- Repository-Nachladen ohne normalen Session-Neustart;
- Consumerkompatibilität;
- Last-Good-Verhalten;
- Worker-Recovery.

P5.1 darf P5 intern wesentlich refaktorieren, solange öffentliche Verträge kompatibel bleiben.

---

## 5. Aktueller Problemzustand

Dieser Abschnitt beschreibt die bei Erstellung bekannte Ausgangslage. Der Agent MUSS sie gegen den tatsächlichen Code verifizieren.

### 5.1 Vermischung von striktem und tolerantem Parsermodus

Der aktuelle Parsercache wird über eine Snapshotpipeline aufgebaut, die Editor-Recovery aktivieren kann. Der tolerante Tokenstream kann synthetische Tokens ergänzen, beispielsweise bei unvollständigen Modellköpfen.

Dasselbe gecachte Parserartefakt kann anschließend vom vollständigen Compiler-Visitor verwendet werden.

Risiko:

```text
unvollständiger Quelltext
  -> tolerante Token-Recovery
  -> synthetisch veränderter Parse Tree
  -> Artefakt wird gecacht
  -> vollständige Kompilation besucht denselben Tree
  -> Compiler prüft nicht mehr exakt die ursprünglichen Bytes
```

Dies ist unabhängig davon problematisch, ob aktuelle Tests zufällig grün sind.

### 5.2 Unvollständige Invalidierung bei `Added`

Ein neu hinzugefügtes Dokument kann die Modellauflösung verändern:

- einen vorher fehlenden Import erfüllen;
- einen neuen Kandidaten für einen Modellnamen einführen;
- eine Mehrdeutigkeit erzeugen;
- eine bisherige Auflösungsentscheidung verändern;
- eine andere Sprachversion desselben Modells bereitstellen.

Eine Invalidierung nur bei `ContentChanged`, `Reintroduced` und `Removed` ist daher nicht hinreichend.

### 5.3 Verteilte und duplizierte Cachelogik

Cachelookup, Keybildung, Hit-Zählung, LRU-Aktualisierung, Snapshotmaterialisierung und Insertlogik sind teilweise mehrfach implementiert:

- Parserzugriff für Compiler;
- `parse`;
- `editorSnapshot`;
- Syntaxsammlung in `compileAndAnalyze`.

Dadurch können Zähler, Bytewerte und Evictionverhalten auseinanderlaufen.

### 5.4 Fehlerhafte oder unvollständige Root-Bytebuchhaltung

Root-Einträge können über verschiedene Pfade gelöscht werden:

- LRU-Eviction;
- Ersetzung;
- Sourceinvalidierung;
- Sourceentfernung;
- Cache-Clear.

Jeder Pfad MUSS dieselbe Bytebuchhaltung und dieselben Invarianten verwenden. Direktes `map.erase()` außerhalb des Cachetyps ist nicht zulässig.

### 5.5 Flache Speicherschätzung

`sizeof` auf Snapshot- oder Resultatobjekten erfasst nicht:

- dynamische Stringinhalte;
- Vector-/Map-Capacities;
- Diagnosen;
- Related Information;
- Tokenobjekte;
- Parse-Tree-Knoten;
- Parserzustand;
- Diagramm;
- Dokumentation;
- Logs;
- Transkript.

Die vorhandenen nominalen Speicherbudgets sind daher nicht zuverlässig.

### 5.6 Irreführende Statistiken

Zähler wie „reused closure sources“ oder „reparsed closure sources“ müssen die tatsächliche Zahl betroffener Sources abbilden. Ein Root-Cache-Hit darf nicht pauschal als „eine wiederverwendete Source“ gezählt werden.

Ein API-Aufruf und ein tatsächlich ausgeführter Compilelauf sind unterschiedliche Ereignisse.

### 5.7 Zu kleiner Inkrementalitätstest

Ein einzelner Smoke-Test beweist keine Korrektheit eines inkrementellen Compilers. Insbesondere fehlen systematische Mutationssequenzen und ein Fresh-Session-Orakel.

### 5.8 Web-IDE scannt weiterhin vollständig

Der `WorkspaceSourceSynchronizer` verhindert zwar unnötige `putWorkspaceSource`-Aufrufe. Der Workbenchpfad kann aber weiterhin alle `.ili`-Dateien rekursiv lesen und erst danach den Diff bestimmen.

P5.1 MUSS bekannte Einzeldateioperationen direkt abbilden.

---

## 6. Ziele

### 6.1 Fachliche Korrektheit

Für jeden finalen Sourcezustand gilt:

```text
inkrementelle Session
  == fachlich ==
frische Session mit denselben Sources und Optionen
```

Dies gilt unabhängig von:

- Aufrufreihenfolge;
- vorherigen fehlerhaften Zwischenständen;
- Editor-Snapshot-Anfragen;
- Parsercache-Hits;
- Rootcache-Hits;
- Evictions;
- Repository-Nachladungen;
- Worker-Recovery.

### 6.2 Transparente Architektur

Ein Entwickler muss anhand von Klassen- und Methodennamen erkennen können:

- wo strikt geparst wird;
- wo Editor-Recovery stattfindet;
- wer Parserartefakte besitzt;
- wer Cachekeys bildet;
- wer Rootauswirkungen berechnet;
- wer Bytebudgets verwaltet;
- was ein Counter bedeutet;
- wann ein Worker replayt;
- wann die Web-IDE einen Vollscan durchführt.

### 6.3 Begrenzter Speicher

Der tatsächliche Retained-Heap soll durch konservative Budgets begrenzt werden. Exakte Bytegenauigkeit ist nicht erforderlich, aber systematische starke Unterschätzung ist unzulässig.

### 6.4 Stabile öffentliche Verträge

Bestehende Consumer sollen keine Pflichtmigration benötigen.

### 6.5 Consumerreife

Node-LSP, Monaco und Web-IDE müssen nach P5.1:

- dieselbe fachliche Semantik behalten;
- Last-Good behalten;
- Completion während nachlaufender Editoranalyse behalten;
- keine zusätzlichen Worker-Restarts erhalten;
- keine unnötigen Full Replays erhalten;
- keine unnötigen Workspace-Vollscans erhalten.

---

## 7. Nichtziele

P5.1 umfasst ausdrücklich nicht:

- partielles Relexing innerhalb einer Datei;
- Token-Splicing;
- Parse-Tree-Patching;
- persistente On-Disk-Semantikcaches;
- Compiler-Daemon;
- verteilte Caches;
- Änderung des INTERLIS-Metamodells;
- vollständigen Abbau aller Conformance-Known-Failures;
- Umstellung auf C++20 oder C++23;
- neue LSP-Features;
- neues Diagrammlayout;
- neue Repositoryprotokolle;
- Breaking Major Release;
- parallele Operationen innerhalb derselben `CompilerSession`;
- SharedArrayBuffer-basierte WASM-Parallelisierung.

---

## 8. Verbindliche Invarianten

### 8.1 Strict-Compiler-Invariante

Die vollständige Kompilation MUSS ausschließlich einen Parse Tree verwenden, der aus den unveränderten registrierten Sourcebytes im Modus `StrictCompiler` erzeugt wurde.

Synthetische Editor-Recovery-Tokens dürfen diesen Tree nicht beeinflussen.

### 8.2 Tolerant-Editor-Invariante

`editorSnapshot(uri)` SOLL auch für unvollständige Dokumente verwertbare Deklarationen, Referenzen und Diagnosen liefern.

Recovery MUSS ausdrücklich markiert sein:

```text
recovered = true
complete = false
```

### 8.3 Modus-Key-Invariante

Falls strikter und toleranter Modus eigene Parserartefakte besitzen, MUSS `ParseMode` Bestandteil des Cachekeys sein.

### 8.4 Source-Impact-Invariante

Jede akzeptierte Sourceoperation MUSS entweder:

- nachweislich keine fachliche Auswirkung haben; oder
- alle möglicherweise betroffenen Rootanalysen invalidieren.

Ein falscher Cachehit ist schwerer als eine konservative Überinvalidierung.

### 8.5 Bytebuchhaltungs-Invariante

Für jeden Cache gilt nach jeder Operation:

```text
retainedBytes
  ==
Summe der geschätzten Bytes aller im Cache registrierten Einträge
```

Debug- und Testbuilds MÜSSEN diese Invariante prüfen können.

### 8.6 Lebensdauer-Invariante

Eviction oder Cache-Clear dürfen bereits an Aufrufer zurückgegebene Snapshots nicht ungültig machen.

### 8.7 Cold-/Incremental-Invariante

Alle canonical fachlichen Resultate müssen zwischen inkrementellem und kaltem Lauf übereinstimmen.

### 8.8 Determinismus-Invariante

Listen, Traces, Schlüssel und Testreports müssen deterministisch sortiert sein.

### 8.9 Consumer-Invariante

Native, C-ABI, WASM, Language Service, Worker, LSP und Web-IDE dürfen keine voneinander abweichende fachliche Cacheentscheidung treffen.

### 8.10 Keine verborgene Fallbacksemantik

Ein Compatibility-Fallback darf nicht still eine andere fachliche Semantik liefern. Ein alter WASM-Build muss entweder explizit unterstützt oder klar abgelehnt werden.

---

## 9. Zielarchitektur

Die Zielstruktur SOLL mindestens folgende Komponenten besitzen:

```text
CompilerSession
  |
  +-- SourceManager
  |
  +-- ParsedSourceCache
  |     +-- StrictSourceParser
  |     +-- TolerantEditorParser
  |     +-- SyntaxSnapshotProjector
  |     +-- EditorSnapshotProjector
  |
  +-- SourceModelIndex
  |
  +-- SourceDependencyIndex
  |
  +-- IncrementalPlanner
  |
  +-- RootAnalysisCache
  |
  +-- IncrementalStatistics
  |
  +-- IncrementalMemoryEstimator
```

Empfohlene native Dateistruktur:

```text
source/core/parsing/
  ParseMode.h
  ParsedSourceArtifact.h
  ParsedSourceArtifact.cpp
  StrictSourceParser.h
  StrictSourceParser.cpp
  TolerantEditorParser.h
  TolerantEditorParser.cpp
  SyntaxSnapshotProjector.h
  SyntaxSnapshotProjector.cpp
  EditorSnapshotProjector.h
  EditorSnapshotProjector.cpp

source/core/incremental/
  ParsedSourceCache.h
  ParsedSourceCache.cpp
  SourceModelIndex.h
  SourceModelIndex.cpp
  SourceDependencyIndex.h
  SourceDependencyIndex.cpp
  IncrementalPlanner.h
  IncrementalPlanner.cpp
  RootAnalysisCache.h
  RootAnalysisCache.cpp
  IncrementalMemoryEstimator.h
  IncrementalMemoryEstimator.cpp
  IncrementalStatistics.h
  IncrementalStatistics.cpp
  IncrementalFingerprint.h
  IncrementalFingerprint.cpp
```

Die exakten Pfade dürfen an bestehende Konventionen angepasst werden. Die Verantwortungen MÜSSEN jedoch getrennt sein.

---

## 10. `ParseMode`

### 10.1 Typ

Es MUSS ein expliziter Modus existieren:

```cpp
namespace ilic::detail {

enum class ParseMode : std::uint8_t {
    StrictCompiler,
    TolerantEditor
};

const char* toString(ParseMode mode) noexcept;

}
```

### 10.2 Semantik

#### `StrictCompiler`

- keine synthetischen Grammatik-Tokens;
- kein Einfügen von `AT`, `VERSION`, `STRING` oder vergleichbaren Tokens;
- Standard-ANTLR-Fehlerbehandlung beziehungsweise explizit dokumentierte strikte Recovery;
- Parserdiagnosen entsprechen dem realen Sourceinhalt;
- Artefakt darf für `buildMetaModel` verwendet werden.

#### `TolerantEditor`

- darf gezielte, dokumentierte Recovery verwenden;
- darf zusätzliche Editorhinweise erzeugen;
- darf `recovered=true` setzen;
- darf nicht für `buildMetaModel` einer vollständigen Kompilation verwendet werden;
- darf keine erfolgreiche Kompilation vortäuschen.

### 10.3 Cachekey

```cpp
struct ParsedSourceKey {
    std::string uri;
    std::uint64_t contentRevision = 0;
    std::string contentHash;
    std::size_t byteLength = 0;
    ParseMode mode = ParseMode::StrictCompiler;
    std::string grammarFingerprint;
};
```

`operator==` und eine deterministische Ordnungs- oder Hashfunktion MÜSSEN vorhanden sein.

Der Schlüssel MUSS tatsächliche Bytegleichheit zusätzlich prüfen, falls ein nicht kollisionssicherer Hash verwendet wird.

---

## 11. Strict Parser

### 11.1 Klasse

```cpp
namespace ilic::detail {

class StrictSourceParser {
public:
    ParsedSourceArtifactPtr parse(const SourceBuffer& source) const;
};

}
```

### 11.2 Anforderungen

`StrictSourceParser::parse` MUSS:

1. `SourceBuffer` wertmäßig oder über eine unveränderliche besessene Kopie im Artefakt sichern;
2. `SourceRangeMapper` auf dem exakt registrierten Inhalt aufbauen;
3. Sprache deterministisch erkennen;
4. Lexer und Parser mit strikten Listenern konfigurieren;
5. den Root-Parse-Tree erzeugen;
6. Parserdiagnosen canonical sortieren;
7. Headerdaten aus demselben Tree extrahieren;
8. einen Compiler-Visitor-Zugriff bereitstellen;
9. keine Editor-Recovery durchführen;
10. bei internen Ausnahmen einen kontrollierten Fehler liefern.

### 11.3 Sprachversionen

Unterstützte Sprachversionen müssen aus der tatsächlichen Grammatik und vorhandenen Compilerlogik stammen:

```text
INTERLIS 1.0
INTERLIS 2.3
INTERLIS 2.4
```

Unbekannte Versionen dürfen keinen Crash erzeugen.

### 11.4 `buildMetaModel`

Nur ein striktes Artefakt darf:

```cpp
artifact->buildMetaModel(builder, logger);
```

ausführen.

Empfohlener Schutz:

```cpp
virtual bool supportsMetaModelBuild() const noexcept = 0;
```

oder getrennte Interfaces:

```cpp
class ParsedSourceArtifact;
class StrictParsedSourceArtifact : public ParsedSourceArtifact;
```

Bevorzugt wird eine Compile-Time- oder klar sichtbare Runtime-Grenze. Ein bloßer Kommentar reicht nicht.

---

## 12. Toleranter Editorparser

### 12.1 Klasse

```cpp
namespace ilic::detail {

class TolerantEditorParser {
public:
    EditorParseArtifactPtr parse(const SourceBuffer& source) const;
};

}
```

Alternativ darf ein gemeinsamer Parserbuilder verwendet werden:

```cpp
class SourceParser {
public:
    ParsedSourceArtifactPtr parse(
        const SourceBuffer& source,
        ParseMode mode
    ) const;
};
```

Die Implementierung MUSS intern klar getrennte Moduspfade besitzen.

### 12.2 Recovery

Jede Recoveryregel MUSS:

- einen Namen besitzen;
- eine klar begrenzte Triggerbedingung besitzen;
- im Code isoliert sein;
- über einen Test mit minimaler Source belegt sein;
- `recovered=true` bewirken;
- `complete=false` bewirken;
- den Strict-Compiler nicht beeinflussen.

Beispiel:

```cpp
enum class EditorRecoveryKind {
    BareModelHeader,
    MissingEndName,
    TruncatedContainer,
    UnexpectedEof
};
```

### 12.3 Recovery-Trace

Der Editor-Snapshot KANN intern Recoverydetails besitzen:

```cpp
struct EditorRecoveryEvent {
    EditorRecoveryKind kind;
    SourceRange range;
    std::string description;
};
```

Diese Details müssen nicht zwingend Teil der stabilen öffentlichen ABI werden. Sie sollen im Testzugriff und Debugtrace verfügbar sein.

---

## 13. Parserartefakte

### 13.1 Gemeinsame Basisschnittstelle

```cpp
class ParsedSourceArtifact {
public:
    virtual ~ParsedSourceArtifact() = default;

    virtual ParseMode mode() const noexcept = 0;
    virtual const SourceIdentity& identity() const noexcept = 0;
    virtual const ParsedSourceHeader& header() const noexcept = 0;
    virtual const std::vector<Diagnostic>& parserDiagnostics() const noexcept = 0;
    virtual std::size_t tokenCount() const noexcept = 0;
    virtual std::size_t parseTreeNodeCount() const noexcept = 0;
    virtual std::size_t estimatedRetainedBytes() const noexcept = 0;
};
```

### 13.2 Striktes Artefakt

```cpp
class StrictParsedSourceArtifact : public ParsedSourceArtifact {
public:
    virtual void reportParserDiagnostics(util::Logger&) const = 0;
    virtual void buildMetaModel(
        metamodel::MetaModelBuilder&,
        util::Logger&
    ) const = 0;
};
```

### 13.3 Editorartefakt

```cpp
class EditorParsedSourceArtifact : public ParsedSourceArtifact {
public:
    virtual const std::vector<EditorRecoveryEvent>& recoveryEvents() const noexcept = 0;
};
```

### 13.4 Ownership

Das Artefakt MUSS mindestens besitzen:

- unveränderliche `SourceBuffer`-Kopie oder gleichwertige besessene Bytes;
- normalisierten Sourcepuffer, falls `SourceRangeMapper` ihn benötigt;
- ANTLR-Inputstream;
- Lexer;
- Tokenstream;
- Parser;
- Root-Kontext;
- Diagnosen;
- Headerdaten;
- Node-/Tokencounter;
- Deep-Size-Schätzung.

Die Zerstörungsreihenfolge MUSS sicher sein. Abhängige Objekte müssen vor den Objekten zerstört werden, auf die sie zeigen.

### 13.5 Unveränderlichkeit

Nach Veröffentlichung über `shared_ptr<const ...>` darf das Artefakt fachlich nicht mehr mutiert werden.

Lazy Materialization ist nur erlaubt, wenn sie:

- thread-safe ist;
- fachlich deterministisch ist;
- keine Veränderung des Parse Trees durchführt;
- in der Bytebuchhaltung berücksichtigt wird.

P5.1 SOLL Lazy Materialization vermeiden, wenn sie die Architektur unnötig verkompliziert.

---

## 14. Snapshotprojektionen

### 14.1 `SyntaxSnapshotProjector`

```cpp
class SyntaxSnapshotProjector {
public:
    SyntaxSnapshot project(
        const StrictParsedSourceArtifact& artifact,
        std::uint64_t visibleDocumentVersion
    ) const;
};
```

Syntaxsnapshots für die öffentliche `parse()`-API SOLLEN aus dem strikten Artefakt entstehen.

Begründung:

- `parse()` ist eine fachliche Syntax-API;
- sie darf nicht still Editor-Recovery in die Compilersemantik tragen;
- `editorSnapshot()` bleibt die explizit tolerante API.

### 14.2 `EditorSnapshotProjector`

```cpp
class EditorSnapshotProjector {
public:
    EditorSnapshot project(
        const EditorParsedSourceArtifact& artifact,
        std::uint64_t visibleDocumentVersion
    ) const;
};
```

### 14.3 Version-only Materialization

Parserartefakte sind inhaltsgebunden. Dokumentversionen sind sichtbare Metadaten.

Daher MUSS die Projektion eine aktuelle sichtbare Version einsetzen können, ohne das Parserartefakt neu zu bauen.

### 14.4 Keine Snapshotkopie im internen Cachelookup

Der Cache SOLL Referenzen oder `shared_ptr<const Entry>` zurückgeben.

Nicht erwünscht:

```cpp
SnapshotBundle ensureParsed(...); // kopiert großes Bundle
```

Bevorzugt:

```cpp
std::shared_ptr<const ParsedSourceCacheEntry> getOrBuild(...);
```

---

## 15. `ParsedSourceCache`

### 15.1 Klasse

```cpp
class ParsedSourceCache {
public:
    explicit ParsedSourceCache(ParsedSourceCacheOptions options);

    std::shared_ptr<const ParsedSourceCacheEntry> getOrBuild(
        const SourceBuffer& source,
        const SourceIdentity& identity,
        ParseMode mode,
        const SourceParser& parser,
        IncrementalOperationRecorder& recorder
    );

    void invalidateUri(
        std::string_view uri,
        InvalidationReason reason,
        IncrementalOperationRecorder& recorder
    );

    void clear(IncrementalOperationRecorder& recorder);

    ParsedSourceCacheSnapshot snapshot() const;

    bool checkInvariants(std::string* error = nullptr) const;
};
```

### 15.2 Entry

```cpp
struct ParsedSourceCacheEntry {
    ParsedSourceKey key;
    std::string sourceBytes;
    ParsedSourceArtifactPtr artifact;
    std::optional<SyntaxSnapshotTemplate> syntaxTemplate;
    std::optional<EditorSnapshotTemplate> editorTemplate;
    std::size_t retainedBytes = 0;
    std::uint64_t lastUsedTick = 0;
};
```

Die konkrete Trennung zwischen strikten und toleranten Artefakten darf typisiert werden.

### 15.3 Lookup

Ein Hit liegt nur vor, wenn:

- URI gleich;
- Modus gleich;
- Grammar-Fingerprint gleich;
- Byte-Länge gleich;
- Hash gleich;
- Sourcebytes gleich.

Die Inhaltsrevision darf als schneller Schlüsselbestandteil verwendet werden, ist aber nicht der alleinige Korrektheitsnachweis.

### 15.4 Invalidierung

Bei `ContentChanged` muss die aktuelle Zuordnung der URI invalidiert werden.

Historische Artefakte dürfen weiterleben, solange externe `shared_ptr` existieren. Sie dürfen aber nicht mehr über den Cache als aktuell gefunden werden.

### 15.5 Zentraler Erase-Pfad

Alle Löschungen MÜSSEN über interne Methoden laufen:

```cpp
iterator erase(
    iterator position,
    CacheRemovalReason reason,
    IncrementalOperationRecorder& recorder
);

void evictUntilWithinBudget(
    IncrementalOperationRecorder& recorder
);
```

Direktes `entries_.erase()` außerhalb dieser Methoden ist verboten.

### 15.6 LRU

LRU muss deterministisch sein. Bei gleichem Tick muss ein stabiler Tie-Breaker verwendet werden, beispielsweise der canonical Key.

### 15.7 Oversized Entry

Wenn ein einzelnes Artefakt größer als `maxRetainedBytes` ist:

- die Operation darf das Artefakt für den aktuellen Request verwenden;
- der Cache SOLL es nicht dauerhaft halten;
- bestehende Einträge dürfen nach klarer Policy evicted werden;
- Statistik und Trace müssen `NotRetainedOversized` ausweisen.

### 15.8 Optionen

```cpp
struct ParsedSourceCacheOptions {
    std::size_t maxEntries = 512;
    std::size_t maxRetainedBytes = 256 * 1024 * 1024;
    bool retainOversizedEntry = false;
};
```

Neue Felder müssen additiv sein.

---

## 16. `SourceManager`

### 16.1 Verantwortung

`SourceManager` ist verantwortlich für:

- aktuelle Sources;
- Dokumentversion;
- Content Revision;
- Source Generation;
- Byteidentität;
- Remove-/Reintroduce-Historie;
- Positionmapping.

Er ist nicht verantwortlich für:

- Parserartefakte;
- Rootcache;
- Dependencygraph;
- Workerstate.

### 16.2 Update-API

```cpp
SourceUpdateResult update(
    std::string uri,
    std::string utf8,
    std::uint64_t documentVersion = 0
);
```

### 16.3 Remove-API

P5.1 SOLL `remove` vereinheitlichen:

```cpp
SourceUpdateResult remove(std::string_view uri);
```

Die bestehende öffentliche `CompilerSession::removeSource()` darf weiterhin `bool` liefern. Intern soll sie den vollständigen Update-Result verwenden.

### 16.4 `version == 0`

Der Vertrag MUSS explizit werden.

Empfohlene Regel:

- `0` bedeutet „keine neue externe Dokumentversion angegeben“;
- bei bestehender Source bleibt die bisherige sichtbare Dokumentversion erhalten;
- bei neuer Source darf `0` gespeichert werden;
- bei verändertem Inhalt und `version == 0` wird nur Content Revision erhöht;
- `0` darf eine bekannte höhere Version nicht zurücksetzen.

Alternativ darf `std::optional<std::uint64_t>` intern verwendet werden.

### 16.5 Stale Version

Bei Byteänderung mit kleinerer expliziter Nicht-Null-Version MUSS der Update abgelehnt werden.

Bei identischen Bytes und kleinerer Version MUSS ebenfalls kein Downgrade stattfinden.

### 16.6 Ergebnis

```cpp
struct SourceUpdateResult {
    SourceUpdateKind kind;
    SourceIdentity previous;
    SourceIdentity current;
    bool accepted = false;
    bool parserInvalidated = false;
    bool analysesInvalidated = false;
    std::vector<std::string> affectedModels;
};
```

`affectedModels` ist optional für die öffentliche API, aber intern für Tracing sinnvoll.

---

## 17. `SourceModelIndex`

### 17.1 Zweck

Der Index ist die Grundlage korrekter `Added`-Auswirkungen.

### 17.2 Daten

```cpp
struct SourceModelRecord {
    std::string uri;
    std::string iliVersion;
    std::vector<std::string> declaredModels;
    std::vector<std::string> importedModels;
    std::vector<std::string> requiredModels;
    SourceIdentity identity;
    bool syntacticallyUsable = false;
};
```

### 17.3 Klasse

```cpp
class SourceModelIndex {
public:
    SourceModelIndexUpdate replace(
        const SourceIdentity& identity,
        const ParsedSourceHeader& strictHeader
    );

    SourceModelIndexUpdate remove(std::string_view uri);

    const SourceModelRecord* byUri(std::string_view uri) const noexcept;

    std::vector<std::string> definingUris(
        std::string_view model,
        std::string_view iliVersion
    ) const;

    std::vector<std::string> requiringUris(
        std::string_view model
    ) const;

    std::vector<std::string> allUris() const;

    bool checkInvariants(std::string* error = nullptr) const;
};
```

### 17.4 Update

`replace` muss atomar:

1. alten Record finden;
2. alte Reverseindizes entfernen;
3. neuen Record einfügen;
4. neue Reverseindizes ergänzen;
5. Delta zurückgeben.

```cpp
struct SourceModelIndexUpdate {
    std::vector<std::string> addedModels;
    std::vector<std::string> removedModels;
    std::vector<std::string> addedRequirements;
    std::vector<std::string> removedRequirements;
    bool iliVersionChanged = false;
    bool usabilityChanged = false;
};
```

### 17.5 Fehlerhafte Source

Auch ein syntaktisch fehlerhaftes Dokument kann teilweise Headerdaten liefern. Der Index muss konservativ sein.

Wenn Headerdaten unzuverlässig sind:

- Record als `syntacticallyUsable=false` markieren;
- betroffene Roots konservativ invalidieren;
- keine falsche Modellauflösung als gültig cachen.

### 17.6 Mehrdeutigkeit

Mehrere definierende URIs für denselben Modellnamen und dieselbe Sprachversion müssen deterministisch behandelt und diagnostiziert werden.

---

## 18. `SourceDependencyIndex`

### 18.1 Klasse

```cpp
class SourceDependencyIndex {
public:
    void replaceSourceDependencies(
        std::string uri,
        std::vector<std::string> requiredModels
    );

    void removeSource(std::string_view uri);

    std::vector<std::string> directRequiredModels(
        std::string_view uri
    ) const;

    std::vector<std::string> dependentSourceUrisForModel(
        std::string_view model
    ) const;

    std::vector<std::string> transitiveClosureForRoots(
        const std::vector<std::string>& roots,
        const SourceModelIndex& models
    ) const;

    std::vector<std::string> reverseAffectedRoots(
        const SourceChangeImpact& impact,
        const RootAnalysisCache& roots
    ) const;

    bool checkInvariants(std::string* error = nullptr) const;
};
```

### 18.2 Modell- gegen URI-Abhängigkeit

INTERLIS-Imports beziehen sich fachlich auf Modelle, nicht direkt auf URIs.

Daher müssen beide Ebenen sichtbar bleiben:

```text
Source URI
  -> benötigte Modellnamen
  -> aktuell aufgelöste definierende URI(s)
```

### 18.3 Zyklen

Closureberechnung muss:

- Zyklen erkennen;
- nicht endlos laufen;
- deterministische Reihenfolge liefern;
- Zyklusdiagnostik nicht durch Cachehits verlieren.

---

## 19. Source-Auswirkungsanalyse

### 19.1 Typ

```cpp
enum class SourceImpactKind {
    None,
    VersionMetadataOnly,
    ContentLocal,
    HeaderChanged,
    ModelDefinitionsChanged,
    RequirementsChanged,
    ResolutionCandidateAdded,
    ResolutionCandidateRemoved,
    ResolutionAmbiguityChanged,
    UnknownConservative
};

struct SourceChangeImpact {
    SourceImpactKind kind;
    std::string uri;
    SourceUpdateKind updateKind;
    std::vector<std::string> modelsBefore;
    std::vector<std::string> modelsAfter;
    std::vector<std::string> requirementsBefore;
    std::vector<std::string> requirementsAfter;
    std::vector<std::string> potentiallyAffectedModels;
};
```

### 19.2 `SourceImpactAnalyzer`

```cpp
class SourceImpactAnalyzer {
public:
    SourceChangeImpact analyze(
        const SourceUpdateResult& sourceUpdate,
        const SourceModelIndexUpdate& modelUpdate,
        const SourceModelIndex& currentModels,
        const RootAnalysisCache& rootCache
    ) const;
};
```

### 19.3 Regeln

#### `Unchanged`

- kein Parserinvalidieren;
- kein Rootinvalidieren.

#### `VersionOnly`

- kein Parserinvalidieren;
- kein fachliches Rootinvalidieren;
- sichtbare Dokumentversion bei Materialisierung aktualisieren.

#### `ContentChanged`

- Parserartefakte der URI invalidieren;
- Header neu strikt ermitteln;
- Rootauswirkung anhand Headerdelta bestimmen;
- mindestens alle Roots invalidieren, deren Closure die URI enthält.

#### `Added`

Ein Add darf nicht pauschal ignoriert werden.

Mindestens invalidieren:

- Rootcacheeinträge mit `missingModels`, wenn die neue Source eines davon definiert;
- Roots, die einen nun mehrdeutigen Modellnamen verwenden;
- Roots, deren Modellauflösung durch neue Sprachversionskandidaten beeinflusst werden kann;
- Roots, deren Requestoptionen die neue Source automatisch suchen können und deren Auflösung nicht eindeutig nachweislich unverändert bleibt.

Wenn die präzise Analyse unsicher ist, MUSS konservativ invalidiert werden.

#### `Removed`

- Roots mit URI in Closure;
- Roots, die eines der entfernten Modelle benötigen;
- Roots, deren Mehrdeutigkeit oder Kandidatenauswahl geändert wird.

#### `Reintroduced`

Wie `Added` plus Identitätswechsel derselben URI.

### 19.4 Kurzfristige Sicherheitsstufe

Falls ein vollständiger SourceModelIndex nicht in einem Schritt sicher integriert werden kann, MUSS als Zwischenzustand gelten:

```text
Added
  -> alle Rootcacheeinträge invalidieren
```

Dieser konservative Zustand muss durch Tests abgesichert und später im selben P5.1-Auftrag durch präzisere Invalidierung ersetzt werden, sofern kein echter Blocker vorliegt.

---

## 20. Compilerfingerprint

### 20.1 Keine Magic Strings in Rootkeys

Hardcodierte Fragmente wie:

```text
grammar=0.9.9
abi=1
builtin=INTERLIS-2.3
```

dürfen nicht in beliebigen Keyfunktionen verteilt sein.

### 20.2 Typ

```cpp
struct CompilerFingerprint {
    std::string compilerVersion;
    std::uint32_t abiVersion = 0;
    std::string grammarFingerprint;
    std::string builtinFingerprint;
    std::string semanticRulesFingerprint;

    std::string canonicalString() const;
};
```

### 20.3 Erzeugung

```cpp
CompilerFingerprint currentCompilerFingerprint();
```

### 20.4 Grammar-Fingerprint

Der Fingerprint MUSS sich ändern, wenn sich parserrelevante Grammatik oder Recoveryregeln ändern.

Zulässige Verfahren:

- Buildzeitgenerierter Hash der Grammar-Dateien;
- explizite zentrale Konstante mit Architecture Guard;
- Kombination aus generiertem Parserstand und Recoveryversion.

### 20.5 Builtin-Fingerprint

Der tatsächliche Inhalt des eingebauten INTERLIS-Modells muss einfließen, nicht nur die Zeichenkette `INTERLIS-2.3`.

---

## 21. Rootanalyse-Schlüssel

### 21.1 Typ

```cpp
struct RootAnalysisKey {
    std::vector<std::string> roots;
    NormalizedCompilerOptions options;
    std::vector<NormalizedExternalMetaAttribute> externalMetaAttributes;
    std::vector<SourceIdentityKey> closure;
    CompilerFingerprint compiler;

    std::string canonicalString() const;
};
```

### 21.2 Canonicalization

- Roots deterministisch normalisieren;
- Reihenfolgesemantik des Requests beachten;
- Verzeichnisse canonical darstellen, ohne bestehende sichtbare URI-Semantik zu brechen;
- Metaattribute stabil sortieren, falls ihre Reihenfolge fachlich irrelevant ist;
- Bool- und Enumwerte eindeutig serialisieren;
- Längenpräfixe oder strukturelle Serialisierung verwenden.

### 21.3 Keine Zeitwerte

Zeitstempel, aktuelle Uhrzeit und volatile Transcriptwerte gehören nicht in den Cachekey.

### 21.4 Missing Models

Ein Eintrag mit Missing Models muss zusätzlich so invalidiert werden können, dass ein später hinzugefügter Kandidat den Treffer verhindert.

---

## 22. `RootAnalysisCache`

### 22.1 Klasse

```cpp
class RootAnalysisCache {
public:
    explicit RootAnalysisCache(RootAnalysisCacheOptions options);

    std::shared_ptr<const RootAnalysisCacheEntry> findExact(
        const RootAnalysisKey& key,
        IncrementalOperationRecorder& recorder
    );

    void insert(
        RootAnalysisCacheEntry entry,
        IncrementalOperationRecorder& recorder
    );

    RootInvalidationResult invalidate(
        const SourceChangeImpact& impact,
        const SourceModelIndex& models,
        const SourceDependencyIndex& dependencies,
        IncrementalOperationRecorder& recorder
    );

    void clear(IncrementalOperationRecorder& recorder);

    RootAnalysisCacheSnapshot snapshot() const;

    bool checkInvariants(std::string* error = nullptr) const;
};
```

### 22.2 Entry

```cpp
struct RootAnalysisCacheEntry {
    RootAnalysisKey key;
    std::vector<std::string> closureUris;
    std::vector<std::string> resolvedModels;
    std::vector<std::string> missingModels;
    CompilationAnalysisResult canonicalResult;
    std::size_t retainedBytes = 0;
    std::uint64_t lastUsedTick = 0;
};
```

### 22.3 Zentraler Erase-Pfad

```cpp
iterator eraseEntry(
    iterator position,
    RootCacheRemovalReason reason,
    IncrementalOperationRecorder& recorder
);
```

Jede Löschung MUSS:

- `retainedBytes_` reduzieren;
- Removal Counter erhöhen;
- Trace ergänzen;
- Entryanzahl aktualisieren;
- Debug-Invarianten prüfen.

### 22.4 Ersetzung

Das Ersetzen eines identischen Keys MUSS alten Bytewert abziehen, bevor der neue addiert wird.

### 22.5 Invalidierungsergebnis

```cpp
struct RootInvalidationResult {
    std::vector<std::string> invalidatedKeyIds;
    std::vector<std::string> invalidatedRoots;
    std::size_t releasedBytes = 0;
};
```

### 22.6 Clear

`clear()` löscht Einträge, nicht Sources.

Nach `clear()` gilt:

```text
entryCount == 0
retainedBytes == 0
```

Statistiken dürfen wahlweise kumulativ bleiben. Die Semantik muss dokumentiert und getestet werden.

---

## 23. Speicherermittlung

### 23.1 Ziel

Die Schätzung muss konservativ genug sein, um Browser- und WASM-Heaps nicht systematisch zu überbelegen.

### 23.2 `IncrementalMemoryEstimator`

```cpp
class IncrementalMemoryEstimator {
public:
    std::size_t estimate(const ParsedSourceArtifact& artifact) const;
    std::size_t estimate(const SyntaxSnapshot& snapshot) const;
    std::size_t estimate(const EditorSnapshot& snapshot) const;
    std::size_t estimate(const CompilationResult& result) const;
    std::size_t estimate(const SemanticSnapshot& snapshot) const;
    std::size_t estimate(const CompilationAnalysisResult& result) const;
};
```

### 23.3 Stringschätzung

Mindestens:

```cpp
sizeof(std::string) + value.capacity() + 1
```

Die genaue Small-String-Implementation ist plattformabhängig. Doppelzählung interner SSO-Bytes ist tolerierbar, wenn die Schätzung konservativ bleibt.

### 23.4 Vector

Mindestens:

```cpp
sizeof(vector) + capacity * sizeof(value_type)
```

Für dynamische Kindobjekte muss zusätzlich rekursiv geschätzt werden.

### 23.5 Map/Set

Knotenbasierte Container müssen mit konservativem Node-Overhead geschätzt werden.

### 23.6 ANTLR

Mindestens berücksichtigen:

- Sourcebytes;
- normalisierte Bytes;
- Tokenanzahl;
- Tokentexte;
- Parse-Tree-Nodeanzahl;
- Parserdiagnosen;
- konservativer Overhead pro Token;
- konservativer Overhead pro Parse-Tree-Knoten;
- Parser-/Lexerbasisoverhead.

Konstanten müssen zentral benannt und dokumentiert werden:

```cpp
struct AntlrMemoryEstimationPolicy {
    std::size_t bytesPerToken;
    std::size_t bytesPerParseTreeNode;
    std::size_t fixedParserOverhead;
};
```

### 23.7 Rootresultat

Vollständig rekursiv schätzen:

- Compilation Result;
- Syntaxsnapshots;
- Semantic Snapshot;
- Symbols;
- References;
- Dependencies;
- Diagram nodes/edges/members;
- Documentation;
- Diagnostics;
- Logs;
- Transcript;
- Missing Models;
- Document Versions.

### 23.8 Validierung

Die Schätzung ist kein exakter Allocator-Profiler. Sie muss aber über Tests zeigen:

- größere Source => größere Schätzung;
- mehr Tokens => größere Schätzung;
- mehr Diagrammknoten => größere Schätzung;
- mehr Dokumentation => größere Schätzung;
- Eviction tritt mit kleinem Budget deterministisch ein;
- Bytecounter kehrt nach Clear auf null zurück.

### 23.9 WASM-Heap-Beobachtung

Die Web-/WASM-Tests SOLLEN ergänzend erfassen:

```js
module.HEAPU8.buffer.byteLength
```

oder eine gleichwertige, offiziell verfügbare Heapgröße.

Absolute Heap-Schrumpfung nach Eviction ist wegen Emscripten Memory Growth nicht zu erwarten. Geprüft wird:

- Retained-Cache-Schätzung sinkt;
- erneute Operationen verursachen kein unbeschränktes Wachstum;
- nach wiederholten Mutationszyklen stabilisiert sich das Wachstum.

---

## 24. Statistiken

### 24.1 Begriffe trennen

P5.1 MUSS mindestens unterscheiden:

```text
API Request
tatsächliche Parserausführung
Parserartefakt-Hit
Snapshotmaterialisierung
Compile Request
tatsächliche Compile Execution
Rootcache-Hit
semantischer Neuaufbau
Closure-Source-Wiederverwendung
Closure-Source-Neuparsing
Cacheinvalidierung
Eviction
Worker-Replay
Workspace-Full-Scan
```

### 24.2 Neuer Typ

Die bestehende API soll kompatibel erweitert werden:

```cpp
struct IncrementalStats {
    std::uint64_t sourceAdds = 0;
    std::uint64_t sourceRemoves = 0;
    std::uint64_t sourceNoOps = 0;
    std::uint64_t versionOnlyUpdates = 0;
    std::uint64_t contentChanges = 0;
    std::uint64_t sourceReintroductions = 0;
    std::uint64_t rejectedUpdates = 0;

    std::uint64_t parseRequests = 0;
    std::uint64_t editorSnapshotRequests = 0;
    std::uint64_t strictParserBuilds = 0;
    std::uint64_t tolerantParserBuilds = 0;
    std::uint64_t strictParserHits = 0;
    std::uint64_t tolerantParserHits = 0;
    std::uint64_t parserEvictions = 0;
    std::uint64_t parserEntries = 0;
    std::uint64_t parserRetainedBytes = 0;

    std::uint64_t syntaxMaterializations = 0;
    std::uint64_t editorMaterializations = 0;

    std::uint64_t compileRequests = 0;
    std::uint64_t compileExecutions = 0;
    std::uint64_t rootAnalysisHits = 0;
    std::uint64_t rootAnalysisMisses = 0;
    std::uint64_t rootAnalysisBuilds = 0;
    std::uint64_t rootAnalysisEvictions = 0;
    std::uint64_t rootEntries = 0;
    std::uint64_t rootRetainedBytes = 0;

    std::uint64_t invalidatedRootEntries = 0;
    std::uint64_t reusedClosureSources = 0;
    std::uint64_t reparsedClosureSources = 0;
    std::uint64_t cancelledPlans = 0;
};
```

### 24.3 Kompatibilität alter Felder

Bestehende Felder dürfen nicht ohne Major Release entfernt werden.

Mögliche Strategie:

- alte Felder weiter serialisieren;
- klare Mappingdefinition;
- neue präzisere Felder ergänzen;
- alte Felder als deprecated dokumentieren.

Beispiel:

```text
parserBuilds = strictParserBuilds + tolerantParserBuilds
parserHits   = strictParserHits + tolerantParserHits
parserBytes  = parserRetainedBytes
compilationInvocations = compileRequests
```

### 24.4 Countersemantik

`reusedClosureSources` muss um die tatsächliche Zahl wiederverwendeter Closure-Sources erhöht werden.

`reparsedClosureSources` muss um die tatsächliche Zahl neu gebauter strikter Parserartefakte im Zusammenhang mit der Rootausführung erhöht werden.

### 24.5 Reset

Es MUSS klar getrennt werden:

```cpp
void resetIncrementalStats();
void clearIncrementalCaches();
```

Falls die öffentliche API nur `clearIncrementalCaches()` besitzt, soll Reset additiv ergänzt werden.

Cache-Clear darf Statistiken nicht implizit und überraschend auf null setzen.

---

## 25. Operationstrace

### 25.1 Strukturierter Trace

Strings allein sind für Tests zu fragil.

```cpp
enum class IncrementalOperationKind {
    UpdateSource,
    RemoveSource,
    Parse,
    EditorSnapshot,
    Compile,
    CompileAndAnalyze,
    ClearCaches
};

enum class IncrementalPlanKind {
    NoOp,
    VersionOnly,
    ParsedArtifactHit,
    ParsedArtifactBuild,
    ExactRootCacheHit,
    RebuildWithParseReuse,
    FullRebuild,
    Rejected,
    Cancelled
};

struct IncrementalTrace {
    IncrementalOperationKind operation;
    IncrementalPlanKind plan;
    std::vector<std::string> roots;
    std::vector<std::string> closure;
    std::vector<std::string> strictParserHits;
    std::vector<std::string> strictParserBuilds;
    std::vector<std::string> tolerantParserHits;
    std::vector<std::string> tolerantParserBuilds;
    std::vector<std::string> invalidatedRoots;
    std::vector<std::string> reasons;
    std::size_t bytesRetained = 0;
    std::size_t bytesReleased = 0;
};
```

### 25.2 Sortierung

Alle URI- und Rootlisten müssen canonical sortiert werden, außer eine fachlich relevante Kompilationsreihenfolge ist ausdrücklich Teil des Felds.

### 25.3 Öffentliche Serialisierung

C-ABI/WASM dürfen weiterhin stringbasierte Werte liefern. Die Werte müssen aus Enums zentral erzeugt werden.

### 25.4 Recorder

```cpp
class IncrementalOperationRecorder {
public:
    explicit IncrementalOperationRecorder(
        IncrementalOperationKind operation
    );

    void recordParserHit(ParseMode, std::string uri);
    void recordParserBuild(ParseMode, std::string uri);
    void recordRootInvalidation(std::string root, InvalidationReason);
    void recordRetainedBytes(std::size_t);
    void recordReleasedBytes(std::size_t);

    IncrementalTrace finish(IncrementalPlanKind plan);
};
```

Jede öffentliche Sessionoperation soll genau einen Recorder besitzen.

---

## 26. `IncrementalPlanner`

### 26.1 Klasse

```cpp
class IncrementalPlanner {
public:
    CompilationPlan plan(
        const CompilationRequest& request,
        const SourceManager& sources,
        ParsedSourceCache& parsedSources,
        SourceModelIndex& models,
        SourceDependencyIndex& dependencies,
        RootAnalysisCache& analyses,
        IncrementalOperationRecorder& recorder
    );
};
```

### 26.2 Plan

```cpp
struct CompilationPlan {
    RootAnalysisKey key;
    std::vector<std::string> roots;
    std::vector<std::string> closure;
    std::vector<std::string> unresolvedModels;
    std::shared_ptr<const RootAnalysisCacheEntry> exactHit;
    bool requiresExecution = true;
};
```

### 26.3 Verantwortung

Der Planner:

- normalisiert Request;
- stellt benötigte strikte Header bereit;
- baut/aktualisiert Modelindex;
- ermittelt Closure;
- erzeugt Rootkey;
- sucht Exact Hit;
- bestimmt wiederverwendete und neu zu parsende Sources.

Der Executor führt die eigentliche Kompilation aus.

### 26.4 Keine UI-Freshness

Dirty-/Saved-/Last-Good-Semantik gehört nicht in den nativen Planner.

---

## 27. `CompilerSession::Impl`

### 27.1 Zielzustand

```cpp
struct CompilerSession::Impl {
    SourceManager sources;
    SourceParser parser;
    SyntaxSnapshotProjector syntaxProjector;
    EditorSnapshotProjector editorProjector;

    ParsedSourceCache parsedSources;
    SourceModelIndex modelIndex;
    SourceDependencyIndex dependencyIndex;
    RootAnalysisCache rootAnalyses;
    IncrementalPlanner planner;
    IncrementalMemoryEstimator memoryEstimator;
    IncrementalStatistics statistics;

    mutable std::mutex mutex;
    IncrementalTrace lastTrace;
};
```

### 27.2 `Compiler.cpp`

`Compiler.cpp` SOLL nach P5.1 primär enthalten:

- öffentliche `CompilerSession`-Methoden;
- Compileexecutor beziehungsweise Orchestrierung;
- fachliche Compilerpipeline;
- External Meta Attributes, falls noch nicht sinnvoll ausgelagert.

Es SOLL NICHT mehr lokale Implementierungen enthalten für:

- LRU;
- Cachebytebuchhaltung;
- Hashing;
- Sourceimpact;
- Rootkey-Stringbau;
- Memory Estimation;
- Parsermodusschalter.

### 27.3 Mutex

Die bestehende Sessionserialisierung darf erhalten bleiben.

Cachekomponenten müssen nicht selbst noch einmal dieselbe Sessionmutex besitzen, solange dies klar dokumentiert ist.

### 27.4 `sources()`

Der mutable `sources()`-Zugriff bleibt deprecated.

P5.1 MUSS Architecture Guards oder Tests ergänzen, damit produktiver interner Code nicht über diesen Bypass P5.1-Indizes und Caches inkonsistent macht.

---

## 28. Öffentliche Sessionmethoden

### 28.1 `updateSource`

Ablauf:

```text
lock
-> SourceManager.update
-> bei abgelehntem Update: Trace/Stats, return
-> bei Inhalts-/Add-/Remove-Wirkung:
     strictes Headerartefakt ermitteln, soweit Source existiert
     SourceModelIndex aktualisieren
     DependencyIndex aktualisieren
     SourceChangeImpact berechnen
     ParsedSourceCache invalidieren
     RootAnalysisCache invalidieren
-> Trace publizieren
-> Invarianten prüfen
-> return
```

### 28.2 `parse`

```text
lock
-> parseRequests++
-> striktes Artefakt getOrBuild
-> SyntaxSnapshot aus Template/Projector
-> sichtbare Dokumentversion einsetzen
-> Trace publizieren
```

### 28.3 `editorSnapshot`

```text
lock
-> editorSnapshotRequests++
-> tolerantes Artefakt getOrBuild
-> EditorSnapshot projizieren
-> sichtbare Dokumentversion einsetzen
-> Trace publizieren
```

### 28.4 `compileAndAnalyze`

```text
lock
-> compileRequests++
-> Planner.plan
-> Exact Hit:
     Ergebnis materialisieren
     aktuelle sichtbare Dokumentversionen einsetzen
     volatile Transcriptwerte aktualisieren
     kein compileExecutions++
-> Miss:
     compileExecutions++
     run-lokalen CompilerContext erzeugen
     strikte Artefakte verwenden
     CompilationResult erzeugen
     Syntax aus strikten Artefakten projizieren
     SemanticSnapshot ohne erneutes Parsing erzeugen
     canonical Cacheentry speichern
-> Trace publizieren
```

### 28.5 `compile`

`compile()` darf weiterhin ohne Semantic Snapshot arbeiten. Es SOLL dieselben strikten Parserartefakte verwenden.

Ob `compile()` einen eigenen Rootcache nutzt, ist nicht zwingend. Ein unnötig zweiter Resultatcache ist zu vermeiden.

### 28.6 `analyze`

`analyze()` darf intern `compileAndAnalyze()` delegieren und nur `semantic` zurückgeben.

---

## 29. Compilerpipeline

### 29.1 `CompilerContext`

Der Context bleibt run-lokal.

Er erhält einen Provider, der ausschließlich strikte Artefakte liefert:

```cpp
using StrictParsedSourceProvider =
    std::function<StrictParsedSourceArtifactPtr(const SourceBuffer&)>;
```

Der Typname soll eine versehentliche Übergabe eines Editorartefakts erschweren.

### 29.2 `IliFileCatalog`

Der Headerprovider muss strikt sein:

```cpp
using ParsedSourceHeaderProvider =
    std::function<ParsedSourceHeader(const SourceBuffer&)>;
```

Die Implementierung darf intern ein striktes Artefakt abrufen.

### 29.3 `compileFile`

`compileFile` MUSS:

- striktes Artefakt abrufen;
- dessen Parserdiagnosen in den run-lokalen Logger übertragen;
- bei Parserfehlern keinen Metamodellvisitor ausführen;
- bei gültigem Tree `buildMetaModel` ausführen;
- niemals `parseIli1`/`parseIli2` erneut aufrufen, wenn ein passendes striktes Artefakt vorhanden ist.

Fallbacks dürfen nur für ausdrücklich nicht registrierte externe Dateisystemsources existieren und müssen klar gemessen werden.

### 29.4 Eingebaute Source

Die Builtin-Source muss denselben klaren Ownershipvertrag besitzen.

### 29.5 Semantic Snapshot

`buildSemanticSnapshot` darf nicht erneut parsen.

---

## 30. Cachehit-Materialisierung

### 30.1 Canonical Result

Ein Rootcache speichert ein canonical Resultat ohne requestzeitabhängige Werte.

### 30.2 Dokumentversionen

Bei Hit werden aktuelle sichtbare Dokumentversionen eingesetzt.

### 30.3 Transcriptzeitstempel

Zeitstempel dürfen bei Materialisierung aktualisiert werden. Das canonical Resultat darf dadurch nicht mutiert werden.

### 30.4 Copy-on-materialize

Es ist zulässig, das Resultat für den API-Rückgabewert zu kopieren.

Bei sehr großen Resultaten KANN später ein effizienterer immutable Snapshotvertrag eingeführt werden. Dies ist kein Muss von P5.1.

### 30.5 Keine Cacheentry-Mutation

LRU-Tick darf mutiert werden. Das canonical fachliche Resultat darf nach Insert nicht mutiert werden.

---

## 31. C++-API

### 31.1 Bestehende Methoden

Unverändert funktionsfähig:

```cpp
putSource
updateSource
removeSource
parse
editorSnapshot
analyze
compileAndAnalyze
compile
format
incrementalStats
lastIncrementalTrace
clearIncrementalCaches
```

### 31.2 Additive Methoden

Empfohlen:

```cpp
void resetIncrementalStats();
IncrementalCacheSnapshot incrementalCacheSnapshot() const;
bool checkIncrementalInvariants(std::string* error = nullptr) const;
```

`checkIncrementalInvariants` kann test-only oder debug-only sein, sofern keine stabile öffentliche API gewünscht ist.

### 31.3 ABI-Stabilität

Keine vorhandene Struktur darf binär inkompatibel geändert werden, wenn sie bereits als öffentliche ABI betrachtet wird. Additive freie Funktionen oder Pimpl-gekapselte Methoden sind vorzuziehen.

---

## 32. C-ABI

### 32.1 Bestehende Funktionen

Erhalten:

```c
ilic_incremental_stats
ilic_clear_incremental_caches
```

### 32.2 Additive Funktionen

Empfohlen:

```c
uint32_t ilic_incremental_trace(uint32_t session);
int32_t ilic_reset_incremental_stats(uint32_t session);
uint32_t ilic_incremental_cache_snapshot(uint32_t session);
```

### 32.3 JSON-Schema

Neue Felder sind additiv.

64-Bit-Counter werden aktuell über JSON-Number übertragen. Der Agent MUSS prüfen, ob Werte theoretisch `Number.MAX_SAFE_INTEGER` überschreiten können.

Für langlebige Sessions SOLL eine sichere Strategie dokumentiert werden:

- Strings für 64-Bit-Werte; oder
- klarer Maximalvertrag; oder
- BigInt-kompatibles Schema in einer späteren Version.

P5.1 muss keine Breaking Änderung erzwingen.

### 32.4 Serializeraufteilung

`Capi.cpp` SOLL entlastet werden:

```text
source/abi/IncrementalJson.*
source/abi/SyntaxJson.*
source/abi/SemanticJson.*
source/abi/CompilationJson.*
source/abi/RequestJson.*
```

Die Aufteilung ist ein Soll. Fachliche Hardening-Aufgaben haben Vorrang.

### 32.5 Registry

Session- und Resultregistry bleiben threadsicher.

---

## 33. WASM-Wrapper

### 33.1 Bestehende API

Erhalten:

```ts
Compiler
CompilerSession
createCompiler
putSource
putWorkspace
removeSource
parse
editorSnapshot
analyze
compileAndAnalyze
compile
format
incrementalStats
clearIncrementalCaches
dispose
```

### 33.2 Capabilities

Additiv:

```ts
interface CompilerCapabilities {
  nativeEditorSnapshot: boolean;
  incrementalSession?: boolean;
  incrementalStats?: boolean;
  incrementalTrace?: boolean;
  incrementalCacheSnapshot?: boolean;
  strictEditorSeparation?: boolean;
}
```

### 33.3 Legacy Editor Projection

Die Legacyprojektion aus Syntaxdaten SOLL in eine eigene Datei verschoben werden:

```text
packages/compiler-wasm/legacy-editor-projection.js
```

Sie muss klar deprecated sein.

Sie darf nur explizit über Compatibility Option aktiviert werden.

### 33.4 Fehler bei Mischversionen

Neuer Wrapper + altes WASM:

- muss klar erkennen, welche Capability fehlt;
- darf nicht still Stats oder Strict-/Tolerant-Semantik vortäuschen;
- muss verständliche Fehler erzeugen.

### 33.5 Resultatvalidierung

Der Wrapper SOLL für neue Stats-/Trace-Schemas minimale strukturelle Validierung durchführen.

---

## 34. `interlis-language-tools`: Backend

### 34.1 `CompilerBackend`

Additiv erweitern:

```ts
interface CompilerBackend {
  readonly capabilities?: CompilerBackendCapabilities;

  incrementalStats?():
    | IncrementalStats
    | Promise<IncrementalStats>;

  incrementalTrace?():
    | IncrementalTrace
    | Promise<IncrementalTrace>;

  incrementalCacheSnapshot?():
    | IncrementalCacheSnapshot
    | Promise<IncrementalCacheSnapshot>;

  clearIncrementalCaches?(): void | Promise<void>;
  resetIncrementalStats?(): void | Promise<void>;
}
```

### 34.2 Kein fachlicher TypeScript-Cache

Der native Compiler bleibt die autoritative Instanz für:

- Content Identity;
- Parsermodi;
- Rootkey;
- Rootinvalidierung;
- Cachehits;
- Memory Budgets.

TypeScript darf:

- Requests deduplizieren;
- UI-Freshness halten;
- Worker-Recovery orchestrieren;
- sichtbare Snapshots pro Root halten.

### 34.3 `AnalysisCache`

Der bestehende Legacy-Helper muss entweder:

- entfernt werden, wenn die öffentliche Kompatibilität dies zulässt; oder
- in eine eigene Legacy-Datei verschoben und eindeutig deprecated werden.

Er darf nicht in Compilerentscheidungen zurückkehren.

---

## 35. `interlis-language-tools`: Worker

### 35.1 Protokoll

Additiv ergänzen:

```ts
| { id: number; method: "incrementalTrace" }
| { id: number; method: "incrementalCacheSnapshot" }
| { id: number; method: "resetIncrementalStats" }
```

### 35.2 Replay

Full Replay ist nur zulässig bei:

- echtem Worker-Crash;
- explizitem `restart()`;
- Worker-/WASM-Versionswechsel;
- nachgewiesen inkonsistentem Zustand.

Nicht bei:

- Repositorymodell hinzugefügt;
- Source geändert;
- VersionOnly;
- Cache-Clear;
- Statsabfrage.

### 35.3 Replay-Metrik

Hostseitig soll gemessen werden:

```ts
interface WorkerLifecycleStats {
  restarts: number;
  replayBatches: number;
  replayedSources: number;
  replayedBytes: number;
  fallbackExecutions: number;
}
```

Diese Metrik gehört nicht zwingend in native `IncrementalStats`.

### 35.4 Requestqueue

Die Workerqueue ist aktuell seriell. P5.1 MUSS sicherstellen:

- keine unbeschränkte Akkumulation veralteter Editorrequests;
- Compile-/Save-Requests gehen nicht verloren;
- Statsrequests verursachen keine fachlichen Seiteneffekte;
- ein Fehler eines Requests bricht die Queue nicht dauerhaft.

### 35.5 Separate Worker

Compile-Worker und Editor-Worker dürfen separate WASM-Sessions besitzen.

Damit gilt:

- Compile-Worker nutzt Strict-Compiler-Artefakte;
- Editor-Worker nutzt Tolerant-Editor-Artefakte;
- Fallback im Hauptprozess muss dieselbe Semantik einhalten.

---

## 36. `LanguageService`

### 36.1 Produktsemantik erhalten

Weiterhin:

```text
Tippen
  -> aktueller Editorbuffer
  -> konservative Liveanalyse

Speichern
  -> vollständige semantische Kompilation

Compilefehler
  -> Last-Good-Semantik bleibt für Diagramm erhalten
```

### 36.2 Completion während nachlaufender Analyse

Die nach P5 vorgenommenen Korrekturen, die Completion aus aktuellem Text bereitstellen, dürfen nicht zurückgebaut werden.

P5.1 MUSS Tests behalten/ergänzen für:

- Editoranalyse geplant;
- Editoranalyse noch nicht fertig;
- Completion trotzdem verfügbar;
- spätes altes Resultat überschreibt aktuellen Zustand nicht.

### 36.3 Diagnoselebenszyklus

Compilerdiagnosen, Live-Diagnosen und Saved-Lint-Diagnosen müssen weiterhin getrennt bleiben.

Strict-/Tolerant-Trennung darf nicht dazu führen, dass:

- tolerante Live-Diagnosen als vollständige Compilerdiagnosen erscheinen;
- strikte Compilerfehler beim Tippen vorzeitig Last-Good löschen;
- Parserdiagnosen doppelt publiziert werden.

### 36.4 Repository-Nachladen

Ablauf bleibt:

```text
compile
-> missing model
-> Repositorymodell laden
-> putSource in bestehende Session
-> erneut compileAndAnalyze
```

Kein normaler `restart()`.

### 36.5 Native Trace-Nutzung

Language Tools DÜRFEN native Traces in Tests und Debugansichten verwenden.

Sie dürfen nicht produktiv von Trace-Strings abhängen, um fachliche Entscheidungen zu treffen.

---

## 37. LSP und Monaco

### 37.1 LSP-Kompatibilität

Keine Breaking Änderungen an vorhandenen Notifications oder Requests.

### 37.2 Full Text Sync

Full Text Sync darf erhalten bleiben. P5.1 benötigt keine LSP-Incremental-Text-Edits.

### 37.3 Diagnosequellen

Die Quelle von Diagnosen muss erkennbar bleiben:

```text
compiler
live
repository
language-service
```

### 37.4 Navigation

Go to Definition, References, Rename, Hover, Outline und Completion müssen bei:

- gültigem gespeicherten Zustand;
- ungültigem Dirty-Zustand;
- Last-Good-Zustand;
- nachlaufendem Editorworker

weiter funktionieren wie vor P5.1.

### 37.5 Performancebudgets

Warm- und Cold-Budgets müssen getrennt bleiben. Coverage-Läufe dürfen keine flakigen Wall-Clock-Gates erzwingen.

---

## 38. `interlis-web-ide`: Workspace-Synchronisierung

### 38.1 Bestehender Synchronizer

`WorkspaceSourceSynchronizer` bleibt eine zentrale Komponente.

### 38.2 Neue API

Er soll ereignisbasierte Operationen unterstützen:

```ts
export class WorkspaceSourceSynchronizer {
  replaceAll(
    sources: readonly WorkspaceSourceUpdate[],
    reason: WorkspaceFullSyncReason,
  ): WorkspaceSyncResult;

  put(source: WorkspaceSourceUpdate): WorkspaceSyncResult;

  remove(uri: string): WorkspaceSyncResult;

  rename(
    previousUri: string,
    next: WorkspaceSourceUpdate,
  ): WorkspaceSyncResult;

  clear(): WorkspaceSyncResult;

  snapshot(): readonly WorkspaceSourceState[];
}
```

### 38.3 `sync`-Kompatibilität

Die vorhandene `sync(sources)`-Methode darf als Alias für `replaceAll` erhalten bleiben.

### 38.4 Direkte Dateioperationen

Bekannte Workbenchoperationen müssen direkt synchronisieren:

#### Neue Datei

```text
workspace.write
-> synchronizer.put
```

#### Save

```text
workspace.write
-> synchronizer.put mit aktuellem Inhalt/Version
-> markSaved
-> compileDocument
```

#### Delete

```text
workspace.delete
-> synchronizer.remove
```

#### Rename

```text
workspace.rename
-> synchronizer.rename
```

#### ZIP-Import

- wenn atomarer Gesamtimport: `replaceAll`;
- wenn Einzelereignisse verfügbar: direkte `put/remove`.

#### Git Checkout/Pull

Wenn der Umfang unbekannt ist, ist ein Full Refresh zulässig.

### 38.5 Full Scan

Ein vollständiger rekursiver Scan ist nur zulässig bei:

```text
Startup
Workspacewechsel
Reconnect Local Folder
ZIP-Gesamtimport
Git-Operation mit unbekanntem Delta
Recovery nach inkonsistentem Watcherzustand
explizitem Manual Refresh
```

### 38.6 Metriken

```ts
interface WorkspaceSynchronizationStats {
  fullScans: number;
  filesReadDuringFullScans: number;
  directPuts: number;
  directRemoves: number;
  directRenames: number;
  noOps: number;
}
```

### 38.7 Watcher Echo

Externe Dateisystemwatcher können ein lokales Save erneut melden. Identische Bytes und Versionen müssen als No-Op behandelt werden.

---

## 39. Web-IDE sichtbare Verträge

P5.1 MUSS erhalten:

- Problems-Ansicht;
- Output;
- Last-Good-Diagramm;
- Status „Showing the last valid diagram“;
- Diagrammviewport;
- Space-Drag-Panning;
- Outline;
- Completion;
- Recovery Buffer;
- Auto-Save;
- Read-only Repositorytabs;
- Offline/PWA;
- DOCX/SVG-Export.

### 39.1 Diagramm

Ein Strict-Compilefehler darf:

- das letzte gültige Diagramm nicht löschen;
- es als stale markieren;
- keine neue falsche Diagrammsemantik aus tolerantem Editorparse erzeugen.

### 39.2 Completion

Tolerante Editoranalyse darf Completion liefern, aber keine gespeicherte semantische Wahrheit überschreiben.

### 39.3 Problems

Live- und Compilerdiagnosen dürfen nicht doppelt erscheinen.

---

## 40. Native Testarchitektur

P5.1 MUSS mehrere fokussierte Testdateien statt eines einzigen monolithischen Tests erhalten.

Empfohlene Targets:

```text
IncrementalSourceUpdateTest.cpp
StrictTolerantParserTest.cpp
ParsedSourceCacheTest.cpp
SourceModelIndexTest.cpp
SourceDependencyIndexTest.cpp
RootAnalysisCacheTest.cpp
IncrementalMemoryEstimatorTest.cpp
IncrementalStatisticsTest.cpp
IncrementalEquivalenceTest.cpp
IncrementalConcurrencyTest.cpp
```

---

## 41. Strict-/Tolerant-Tests

### 41.1 Aufrufreihenfolgen

Für jede Fixture:

```text
A: compile direkt
B: editorSnapshot -> compile
C: parse -> compile
D: editorSnapshot -> parse -> compile
E: parse -> editorSnapshot -> compile
F: compile -> editorSnapshot -> compile
```

Das strikte Compilerresultat muss in A bis F canonical identisch sein.

### 41.2 Bare Model Header

Fixture:

```ili
INTERLIS 2.4;
MODEL Test =
```

Erwartung:

- `editorSnapshot` liefert verwertbaren Snapshot;
- `recovered=true`;
- `complete=false`;
- `compileAndAnalyze` schlägt strikt fehl;
- kein synthetisches `AT "" VERSION "1"` macht Compile erfolgreich;
- Aufrufreihenfolge ändert das Compilerresultat nicht.

### 41.3 Missing END

Fixture mit fehlendem `END`.

### 41.4 Falscher END-Name

Editordiagnose und Compilerdiagnose dürfen unterschiedlich aufbereitet sein, aber der Compiler muss strikt bleiben.

### 41.5 Ungültige Version

Darf keinen Internal Error auslösen.

### 41.6 Truncated Unicode

Ungültige oder abgeschnittene UTF-8-Sequenzen müssen kontrolliert behandelt werden.

---

## 42. Source-Impact-Tests

### 42.1 Unabhängiger Add

- Root A erfolgreich cachen;
- unabhängige Source B hinzufügen;
- erneuter Root A darf Hit bleiben, wenn präzise Analyse dies beweist;
- Cold-/Incremental-Resultat gleich.

### 42.2 Add erfüllt Missing Model

- Root importiert `Base`;
- erster Lauf meldet Missing Model;
- `Base.ili` hinzufügen;
- Rootcache muss invalidiert werden;
- zweiter Lauf muss Base auflösen;
- kein Workerrestart in Consumer.

### 42.3 Add erzeugt Duplikat

- Root verwendet Modell `Base`;
- erste `Base.ili` vorhanden;
- zweite Source definiert ebenfalls `Base`;
- vorheriger Rootcache darf nicht als gültig zurückgegeben werden;
- cold und incremental müssen gleiche Mehrdeutigkeitsdiagnostik liefern.

### 42.4 Remove Dependency

- Dependency entfernen;
- Root invalidieren;
- Missing Model korrekt melden.

### 42.5 Reintroduce

- entfernen;
- neu hinzufügen;
- Content Revision und Generation müssen steigen;
- alte Parserartefakte dürfen nicht als aktuell gelten.

### 42.6 Importset ändern

- Import hinzufügen;
- Import entfernen;
- Closure und Rootkey müssen sich ändern.

### 42.7 Modellname ändern

- `MODEL A` zu `MODEL B`;
- alte Reverseindizes entfernen;
- neue hinzufügen;
- betroffene Roots invalidieren.

---

## 43. Cachebuchhaltungs-Tests

### 43.1 Parsercache

Nach jedem:

```text
insert
replace
invalidateUri
LRU eviction
oversized not retained
clear
```

muss `checkInvariants()` wahr sein.

### 43.2 Rootcache

Dasselbe für:

```text
insert
replace
source invalidation
model invalidation
LRU eviction
clear
```

### 43.3 Bytewerte

Test mit kleinen Budgets:

- Entry A;
- Entry B;
- Hit A;
- Entry C;
- deterministischer Victim;
- Bytewert exakt Summe der Entry-Schätzungen.

### 43.4 External Lifetime

- Cacheentry abrufen;
- Cache clear;
- gehaltenes `shared_ptr` weiterhin verwendbar;
- kein ASan-/UBSan-Fund.

### 43.5 Wiederholungszyklus

100 oder mehr:

```text
put changed source
parse
compile
invalidate
```

Retained Bytes müssen innerhalb des konfigurierten Budgets bleiben.

---

## 44. Memory-Estimator-Tests

Fixtures mit zunehmender Komplexität:

```text
empty/minimal model
10 attributes
100 attributes
deep enum
many imports
large documentation
large diagram
many diagnostics
```

Erwartung:

- monotone oder plausibel steigende Schätzung;
- kein Nullwert für nichtleere Artefakte;
- keine Überläufe;
- `size_t`-sichere Addition.

Saturating Addition wird empfohlen:

```cpp
std::size_t saturatingAdd(
    std::size_t left,
    std::size_t right
) noexcept;
```

---

## 45. Statistiktests

### 45.1 VersionOnly

```text
initial put
editorSnapshot
same bytes new version
editorSnapshot
```

Erwartung:

- genau ein toleranter Parserbuild;
- mindestens ein toleranter Hit;
- kein strikter Build, wenn nie strikt angefragt;
- `versionOnlyUpdates == 1`.

### 45.2 Exact Root Hit

Closure mit drei Sources:

- erster Lauf: drei strikte Builds oder nachvollziehbare Zahl;
- zweiter Lauf: Exact Root Hit;
- `reusedClosureSources += 3`;
- `compileRequests += 2`;
- `compileExecutions += 1`.

### 45.3 Eine Dependency geändert

- nur geänderte Source erhält neuen strikten Build;
- unveränderte Closureartefakte Hits;
- Counter entsprechen tatsächlichen URI-Listen im Trace.

### 45.4 Clear

- Cacheentry- und Bytegauge auf null;
- kumulative Build-/Hitcounter bleiben;
- nach `resetIncrementalStats` Counter resetten, Gauges spiegeln aktuellen Cachezustand nach dokumentiertem Vertrag.

---

## 46. Cold-/Incremental-Äquivalenz

### 46.1 Harness

```cpp
class IncrementalEquivalenceHarness {
public:
    void setInitialSources(std::vector<TestSource>);
    void apply(std::vector<SourceMutation>);
    EquivalenceReport compare(
        const CompilationRequest& request
    );
};
```

### 46.2 Ablauf

1. Session A mit Initialsources;
2. optional mehrere Zwischenkompilationen;
3. Mutationen anwenden;
4. inkrementell kompilieren;
5. Session B frisch erstellen;
6. finalen Sourcezustand registrieren;
7. kalt kompilieren;
8. canonical vergleichen.

### 46.3 Canonicalisierung

Zulässig zu normalisieren:

- Completion-Timestamp im Transcript;
- explizit dokumentierte volatile Logzeitwerte.

Nicht zulässig:

- Diagnosereihenfolge ohne Grund;
- Modellreihenfolge;
- Symbol-IDs;
- Ranges;
- Missing Models;
- Dependencyedges;
- Diagrammknoten;
- Dokumentation;
- Erfolgsstatus.

### 46.4 Vergleichsfelder

Mindestens:

```text
CompilationResult.success
cancelled
errorCount
warningCount
missingModels
models
diagnostics
logs
transcript canonical

SyntaxSnapshot pro URI
SemanticSnapshot.roots
documentVersions
symbols
references
dependencies
diagram
documentation
diagnostics
logs
```

### 46.5 Sequenzen

Mindestens:

1. Kommentaränderung;
2. Whitespaceänderung;
3. Attribut hinzufügen;
4. Attributtyp ändern;
5. Attribut umbenennen;
6. Import hinzufügen;
7. Import entfernen;
8. Modell umbenennen;
9. Dependency hinzufügen;
10. Dependency entfernen;
11. Missing Model erfüllen;
12. Duplikatmodell hinzufügen;
13. Syntaxfehler einführen;
14. Syntaxfehler korrigieren;
15. Semantic Error einführen;
16. Semantic Error korrigieren;
17. Source entfernen;
18. Source reintroduzieren;
19. VersionOnly;
20. Cacheclear zwischen Mutationen;
21. Parsercache-Eviction;
22. Rootcache-Eviction;
23. EditorSnapshot vor Compile;
24. EditorSnapshot nach Compile;
25. zwei unabhängige Roots;
26. gemeinsame Dependency;
27. Zyklus;
28. External Meta Attribute ändern;
29. Compileroption ändern;
30. Model Directory ändern.

---

## 47. Externer Corpus

Der bestehende Conformance-Corpus MUSS zusätzlich als Mutationsquelle verwendet werden.

### 47.1 Auswahl

Deterministische Teilmenge:

- gültige Ili1-Modelle;
- ungültige Ili1-Modelle;
- gültige Ili2.3-Modelle;
- ungültige Ili2.3-Modelle;
- Ili2.4;
- Imports;
- Translation;
- Associations;
- Constraints;
- Surfaces;
- Units;
- Enumerations.

### 47.2 Mutationen

Nur deterministic, syntaxbewusste oder klar textuelle Mutationen:

- Kommentar am Ende;
- Whitespace;
- sichere Identifierumbenennung in minimalen Fixtures;
- Importzeile entfernen;
- EOF truncation;
- Restore original.

### 47.3 Report

Report pro Fall:

```json
{
  "testId": "...",
  "sequence": "...",
  "coldOutcome": "...",
  "incrementalOutcome": "...",
  "equivalent": true,
  "strictParserBuilds": 1,
  "strictParserHits": 4,
  "rootCacheHits": 1
}
```

---

## 48. C-ABI- und WASM-Tests

### 48.1 C-ABI

Testen:

- gültige Session;
- ungültige Session;
- zerstörte Session;
- Stats;
- Trace;
- Clear;
- Reset;
- Strict-/Tolerant-Aufrufreihenfolge;
- Added-Invalidierung;
- Bytegauge;
- Result Handle Lifetime.

### 48.2 WASM

Testen:

- Capabilities;
- API-Kompatibilität;
- `editorSnapshot` bleibt tolerant;
- `compileAndAnalyze` bleibt strikt;
- neue Statsfelder;
- Mixed Wrapper/WASM;
- Cacheclear;
- Heapwachstum unter Mutationsschleife;
- Native-/WASM-Äquivalenz.

### 48.3 Browser und Node

Das WASM-Paket muss in beiden Umgebungen getestet werden.

---

## 49. Tests in `interlis-language-tools`

### 49.1 Backend

- neue Capabilities;
- Stats-/Trace-Weiterleitung;
- Clear-/Reset-Weiterleitung;
- Fallback bei fehlender Capability;
- keine fachliche Emulation.

### 49.2 Worker

- normaler Put => kein Restart;
- Missing Model => kein Restart;
- Crash => genau ein Replaybatch;
- Replayquellen und Bytes korrekt;
- Statsrequest ohne Replay;
- Clear ohne Replay;
- Queue nach Fehler weiter funktionsfähig.

### 49.3 Language Service

- Completion während nachlaufender Editoranalyse;
- Strict Compiler Error überschreibt nicht tolerante UI falsch;
- Live-Diagnosen bleiben getrennt;
- Last-Good bleibt;
- Root-Isolation;
- Repositorymodell hinzugefügt;
- Duplikatmodell hinzugefügt;
- VersionOnly;
- stale Result wird verworfen.

### 49.4 LSP

- DidOpen;
- DidChange;
- DidSave;
- DidClose;
- Diagnostics;
- Completion;
- Definition;
- References;
- Rename;
- Semantic Notifications;
- Worker Recovery.

### 49.5 Monaco

- unvollständiger Modellkopf;
- Completion sichtbar;
- Outline brauchbar;
- Save löst strikte Fehler aus;
- Last-Good Navigation bleibt.

---

## 50. Tests in `interlis-web-ide`

### 50.1 Synchronizer Unit Tests

- `replaceAll`;
- `put`;
- `remove`;
- `rename`;
- `clear`;
- VersionOnly;
- Watcher Echo;
- duplicate URI in input;
- deterministic order;
- metrics.

### 50.2 Workbench Unit/Integration

- New File direkt put;
- Save direkt put;
- Delete direkt remove;
- Rename direkt rename;
- Manual Compile führt nicht unnötig Full Scan aus, wenn Zustand bekannt;
- Workspacewechsel führt Full Scan aus;
- ZIP-Import definiert klaren Full-Sync-Pfad;
- Git Pull mit unbekanntem Delta führt Full Scan aus.

### 50.3 E2E

- gültiges Modell;
- Fehler tippen;
- tolerante Completion bleibt;
- Save;
- strikter Compilefehler;
- Last-Good-Diagramm bleibt;
- Fehler korrigieren;
- neues Diagramm erscheint;
- kein unnötiger Workerrestart;
- kein unnötiger Full Scan.

### 50.4 Performance-/Arbeitszähler

Im Testmodus soll eine Debugschnittstelle verfügbar sein, die mindestens liefert:

```text
workspace full scans
files read
direct source puts
worker restarts
worker replayed sources
native strict parser builds
native tolerant parser builds
root cache hits
```

Diese Schnittstelle darf nur in Test-/Debugbuilds exponiert werden.

---

## 51. Architecture Guards

P5.1 MUSS Guards gegen folgende Fehler enthalten.

### 51.1 Parsermodus

Verboten:

- `EditorTokenStream` oder vergleichbare Recovery im Strict-Compilerpfad;
- `buildMetaModel` auf tolerantem Artefakt;
- Cachekey ohne `ParseMode`, wenn beide Modi denselben Cache nutzen.

### 51.2 Cache

Verboten:

- direkte `entries.erase` außerhalb zentraler Cachemethoden;
- globale Caches;
- `thread_local` Caches;
- unbeschränkte Cachecontainer;
- `sizeof(result)` als alleinige Rootspeicherschätzung;
- Copy-return großer Cachebundles in internen Lookupmethoden.

### 51.3 Sourceimpact

Verboten:

- `Added` ohne explizite Impactbehandlung;
- Rootinvalidierung nur durch Closure-URI, wenn Missing-Model- oder Modellkandidatenänderung möglich ist.

### 51.4 Consumer

Verboten:

- normaler Workerrestart nach Repository-Nachladung;
- Full Replay bei normalem Put;
- Web-IDE-Full-Scan nach jeder bekannten Einzeldateioperation;
- zweiter JavaScript-Parser;
- toleranter Editor-Snapshot als gespeicherte Semantic Snapshot-Wahrheit.

### 51.5 Canaries

Jeder Guard braucht einen negativen Canary, der nachweist, dass der Guard bei einer kontrollierten Verletzung fehlschlägt.

---

## 52. Dokumentation

Mindestens aktualisieren:

```text
docs/architecture/incremental-compilation.md
docs/language-tooling-snapshots.md
docs/wasm.md
docs/native-api.md
```

Neue ADRs empfohlen:

```text
ADR: Strict compiler parsing versus tolerant editor recovery
ADR: Source model index and root invalidation
ADR: Incremental cache memory accounting
ADR: Consumer ownership of freshness versus native cache correctness
```

Language Tools:

```text
docs/live-diagnostik-und-dirty-navigation.md
docs/testing.md
docs/local-development.md
```

Web-IDE:

- Workspace-Synchronisierungsvertrag;
- Full-Scan-Gründe;
- Last-Good-Diagramm;
- Worker-Recovery.

---

## 53. CI

### 53.1 `ilic-fork`

MUSS ausführen:

- Linux Release;
- macOS Release;
- Windows Release;
- Linux Debug;
- ASan/UBSan/LSan;
- TSan;
- Conformance;
- WASM;
- npm-Pakettests;
- Native-/WASM-Parität;
- Architecture Guards;
- P5.1 Equivalence Corpus.

### 53.2 `interlis-language-tools`

MUSS ausführen:

```bash
corepack pnpm install --frozen-lockfile
corepack pnpm build
corepack pnpm lint
corepack pnpm typecheck
corepack pnpm test
corepack pnpm test:snapshot
corepack pnpm pack:verify
```

An reale Scripts anpassen.

### 53.3 `interlis-web-ide`

MUSS ausführen:

```bash
corepack pnpm install --no-frozen-lockfile --force --update-checksums
corepack pnpm check
corepack pnpm e2e
corepack pnpm build
```

An reale Scripts anpassen.

### 53.4 Cross-Repo

Die Web-IDE muss gegen lokal erzeugte Pakete aus Language Tools und den lokal gebauten P5.1-WASM-Compiler getestet werden.

---

## 54. Implementierungsphasen

### P5.1.0 – Inventar und Baseline

- Gitstatus aller Repositories;
- Ausgangscommits;
- fremde Änderungen;
- Toolchain;
- Baseline-Builds;
- aktuelle Stats bei Referenzszenarien;
- aktuelle Full-Scan-/Worker-Replay-Zähler.

### P5.1.1 – Reproduktion der Risiken

Vor Refactoring Tests schreiben, die mindestens reproduzieren:

- EditorSnapshot vor Strict Compile;
- Added erfüllt Missing Model;
- Added erzeugt Duplikat;
- Rootbytes nach Invalidierung;
- falsche Closurecounter;
- Web-IDE Full Scan nach Einzeloperation.

### P5.1.2 – ParseMode und Strict-/Tolerant-Trennung

- `ParseMode`;
- strikter Parser;
- toleranter Parser;
- Artefakttypen;
- Moduskey;
- Aufrufreihenfolgentests.

### P5.1.3 – Parsercache extrahieren

- zentrale Klasse;
- `getOrBuild`;
- zentraler Erase;
- LRU;
- Invarianten;
- keine großen Bundlekopien.

### P5.1.4 – SourceModelIndex und Impact

- Record;
- Reverseindizes;
- Add/Remove/Reintroduce;
- Missing Model;
- Duplikat;
- präzise oder konservative Invalidierung.

### P5.1.5 – Dependencyindex und Planner

- Closure;
- Reverse-Auswirkung;
- Rootkey;
- Compilerfingerprint;
- Exact Hit.

### P5.1.6 – Rootcache extrahieren

- zentrale Klasse;
- korrekte Bytes;
- zentraler Erase;
- Invarianten;
- deterministic LRU.

### P5.1.7 – Deep Memory Estimation

- Parser;
- Syntax;
- Editor;
- Compilation;
- Semantic;
- WASM-Tests.

### P5.1.8 – Stats und Trace

- neue präzise Counter;
- Compatibility Mapping;
- Operation Recorder;
- C-ABI/WASM.

### P5.1.9 – Cold-/Incremental-Harness

- native Sequenzen;
- Corpus;
- Native/WASM-Parität;
- Reports.

### P5.1.10 – Language Tools

- Backend;
- Worker;
- Lifecycle Stats;
- Completion/Diagnostics/Last-Good;
- keine Restartregression.

### P5.1.11 – Web-IDE

- ereignisbasierter Synchronizer;
- Workbenchintegration;
- E2E;
- Full-Scan-Metriken.

### P5.1.12 – Vereinfachung und Dokumentation

- `Compiler.cpp` verkleinern;
- `SnapshotPipeline.cpp` aufteilen;
- Legacy-WASM-Projektion auslagern;
- optional Capi-Serializer aufteilen;
- ADRs;
- Architecture Guards.

### P5.1.13 – Cross-Repo-Abnahme

- alle Builds;
- alle Tests;
- Sanitizer;
- TSan;
- Conformance;
- Pakete;
- Browser;
- PWA;
- Abschlussbericht.

---

## 55. Verbotene Abkürzungen

Nicht zulässig:

- toleranten Tree weiter für Compile verwenden und nur einen Kommentar ergänzen;
- zwei Cachekeys ohne expliziten ParseMode verwenden;
- bei `Added` nur bestehende Closure-URIs prüfen;
- alle Roots dauerhaft pauschal invalidieren, obwohl die präzise Indeximplementierung ohne Blocker möglich ist;
- Bytecounter nur beim LRU-Pfad korrigieren;
- `sizeof` als Deep-Size verkaufen;
- Counter um pauschal `1` erhöhen, obwohl mehrere Sources betroffen sind;
- Cold-/Incremental nur über Success-Bool vergleichen;
- nur Wall-Clock-Benchmarks verwenden;
- Worker-Restart als bequemen Konsistenzmechanismus verwenden;
- Web-IDE weiterhin nach jeder bekannten Operation vollständig scannen;
- Legacy-Parserlogik zurück in JavaScript kopieren;
- Conformance-Baseline erweitern;
- Tests lockern;
- Sanitizer deaktivieren;
- neue globale Zustände einführen;
- fremde Änderungen überschreiben;
- nur `ilic-fork` ändern und Consumerarbeit als „nicht nötig“ behaupten, ohne Tests in den Consumer-Repositories;
- P5.1 als abgeschlossen bezeichnen, wenn ein Muss-Punkt offen ist.

---

## 56. Verifikationsmatrix

### 56.1 Native Release

```bash
cmake -S . -B build/p51-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON

cmake --build build/p51-release --parallel
ctest --test-dir build/p51-release --output-on-failure
```

### 56.2 Native Debug

```bash
cmake -S . -B build/p51-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON

cmake --build build/p51-debug --parallel
ctest --test-dir build/p51-debug --output-on-failure
```

### 56.3 Sanitizer

```bash
cmake -S . -B build/p51-sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON

cmake --build build/p51-sanitizers --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:strict_string_checks=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/p51-sanitizers --output-on-failure -j2
```

### 56.4 TSan

```bash
cmake -S . -B build/p51-tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_THREAD_SANITIZER=ON

cmake --build build/p51-tsan --parallel

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p51-tsan --output-on-failure
```

### 56.5 Wiederholung

```bash
ctest --test-dir build/p51-debug \
  --repeat until-fail:25 \
  -R "Incremental|Strict|Tolerant|ParsedSource|RootAnalysis|Dependency|Memory" \
  --output-on-failure
```

### 56.6 WASM

```bash
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
node --test test/npm/*.mjs
```

### 56.7 Language Tools

```bash
cd ../interlis-language-tools
corepack pnpm install --frozen-lockfile
corepack pnpm check
corepack pnpm --filter @ilic/language-service test
corepack pnpm pack:verify
```

### 56.8 Web-IDE

```bash
cd ../interlis-web-ide
corepack pnpm install --no-frozen-lockfile --force --update-checksums
corepack pnpm check
corepack pnpm e2e
corepack pnpm build
```

### 56.9 Gitstatus

```bash
git -C ../ilic-fork diff --check
git -C ../interlis-language-tools diff --check
git -C ../interlis-web-ide diff --check

git -C ../ilic-fork status --short
git -C ../interlis-language-tools status --short
git -C ../interlis-web-ide status --short
```

---

## 57. Definition of Done

P5.1 ist nur abgeschlossen, wenn alle Aussagen wahr sind.

### 57.1 Parser

- Strict und Tolerant sind explizite Modi.
- Strict Compile verwendet keine synthetischen Editor-Recovery-Tokens.
- EditorSnapshot bleibt tolerant.
- Aufrufreihenfolge verändert Compilerresultate nicht.
- ParseMode ist im Cachekey.
- `buildMetaModel` ist nur auf striktem Artefakt möglich.

### 57.2 Sourceimpact

- Added wird korrekt behandelt.
- Missing Model wird nach Add aufgelöst.
- Duplikatmodell invalidiert alte Resultate.
- Remove und Reintroduce sind korrekt.
- SourceModelIndex ist konsistent.
- Dependencyindex ist konsistent.
- Unsichere Fälle invalidieren konservativ.

### 57.3 Caches

- Parsercache ist extrahiert.
- Rootcache ist extrahiert.
- Alle Löschungen laufen zentral.
- Byteinvarianten sind grün.
- LRU ist deterministisch.
- Oversized Entries sind definiert.
- Eviction invalidiert keine externen Snapshots.
- Clear lässt Sources bestehen.

### 57.4 Speicher

- Deep Estimator existiert.
- ANTLR wird konservativ berücksichtigt.
- Semantic/Diagram/Documentation werden berücksichtigt.
- WASM-Mutationsschleifen zeigen begrenztes Retained-Cache-Wachstum.

### 57.5 Stats/Trace

- API Requests und Executions sind getrennt.
- Strict-/Tolerant-Builds sind getrennt.
- Closurecounter zählen tatsächliche Sources.
- Gauges stimmen mit Cachezustand überein.
- Trace ist strukturiert und deterministisch.
- C-ABI/WASM liefern kompatible Daten.

### 57.6 Korrektheit

- Cold-/Incremental-Harness ist vorhanden.
- alle verbindlichen Mutationssequenzen sind grün;
- Conformance zeigt keine neue Abweichung;
- Native-/WASM-Parität ist grün;
- keine neuen Internal Errors.

### 57.7 Language Tools

- kein normaler Restart nach Repository-Nachladen;
- Worker-Replay nur bei Recovery;
- Completion während nachlaufender Analyse bleibt;
- Diagnostiklebenszyklus bleibt;
- Last-Good bleibt;
- LSP/Monaco sind grün.

### 57.8 Web-IDE

- bekannte Einzeldateioperationen synchronisieren direkt;
- Full Scans sind auf definierte Gründe beschränkt;
- Full-Scan-Zähler sind testbar;
- Last-Good-Diagramm bleibt;
- Problems/Output bleiben;
- PWA/Offline bleiben;
- E2E ist grün.

### 57.9 Qualität

- Release grün;
- Debug grün;
- ASan/UBSan/LSan grün;
- TSan grün;
- Architecture Guards grün;
- negative Canaries grün;
- Dokumentation aktualisiert;
- keine fremden Änderungen beschädigt.

---

## 58. Verhalten bei Blockern

Ein echter Blocker ist beispielsweise:

- fehlende Plattformtoolchain;
- nicht installierbare Browserengine;
- externer Paketregistry-Ausfall;
- nicht zugängliche externe Testsuite.

Kein echter Blocker:

- hoher Implementierungsaufwand;
- viele fehlschlagende Tests;
- notwendiges Refactoring;
- lange Testlaufzeit;
- komplizierte Ownership;
- bestehende Architektur passt nicht zur Spezifikation.

Bei einem echten Blocker:

1. alle unabhängigen Teile implementieren;
2. Repository konsistent hinterlassen;
3. exakten Befehl dokumentieren;
4. Exitcode nennen;
5. betroffene Muss-Anforderung nennen;
6. nächsten technischen Schritt nennen;
7. P5.1 nicht als vollständig abgeschlossen bezeichnen.

---

## 59. Abschlussbericht

Der Agent MUSS am Ende einen Bericht mit exakt diesen Abschnitten liefern:

1. **Ausgangszustand**
2. **Repositorypfade**
3. **Ausgangscommits**
4. **Fremde Änderungen**
5. **Baseline**
6. **Reproduzierte P5-Risiken**
7. **Strict-/Tolerant-Zielarchitektur**
8. **ParseMode**
9. **StrictSourceParser**
10. **TolerantEditorParser**
11. **Parserartefakt-Ownership**
12. **Snapshotprojektionen**
13. **ParsedSourceCache**
14. **SourceManager**
15. **SourceModelIndex**
16. **SourceDependencyIndex**
17. **SourceImpactAnalyzer**
18. **CompilerFingerprint**
19. **RootAnalysisKey**
20. **IncrementalPlanner**
21. **RootAnalysisCache**
22. **Bytebuchhaltung**
23. **Memory Estimator**
24. **IncrementalStats**
25. **IncrementalTrace**
26. **CompilerSession-Vereinfachung**
27. **Compilerpipeline**
28. **C++-API**
29. **C-ABI**
30. **WASM**
31. **Language-Tools-Backend**
32. **Compiler-Worker**
33. **Editor-Worker**
34. **LanguageService**
35. **LSP**
36. **Monaco**
37. **WorkspaceSourceSynchronizer**
38. **Web-IDE**
39. **Last-Good/Diagramm**
40. **Neue native Tests**
41. **Cold-/Incremental-Äquivalenz**
42. **Corpus-Tests**
43. **C-ABI-/WASM-Tests**
44. **Language-Tools-Tests**
45. **Web-IDE-Tests**
46. **Architecture Guards**
47. **Release**
48. **Debug**
49. **ASan/UBSan/LSan**
50. **TSan**
51. **Conformance**
52. **WASM/npm**
53. **Package-Verifikation**
54. **Browser/PWA**
55. **Arbeitszähler vorher/nachher**
56. **Speicherverhalten vorher/nachher**
57. **Ausgeführte Befehle**
58. **Abweichungen**
59. **Blocker**
60. **Verbleibende Risiken**
61. **Abschließender Gitstatus**

Für jeden Testlauf:

- Repository;
- exakter Befehl;
- Exitcode;
- Testanzahl;
- Fehleranzahl;
- Buildtyp;
- Umgebung;
- Reportpfad.

Keine erfundenen oder geschätzten Zahlen.

---

## 60. Explizite Erfolgsaussage

P5.1 darf nur dann als abgeschlossen bezeichnet werden, wenn folgende Aussage durch tatsächlich ausgeführte Tests belegt ist:

> Der INTERLIS-Compiler trennt striktes Compilerparsing und tolerante Editor-Recovery explizit und verhindert, dass synthetische Editortokens die vollständige Kompilation beeinflussen. Parser- und Rootcache sind sessionlokal, lebensdauersicher, zentral verwaltet und besitzen konsistente konservative Speicherbudgets. Neu hinzugefügte, entfernte oder veränderte Sources invalidieren alle und nur die nachweislich oder konservativ möglicherweise betroffenen Rootanalysen, einschließlich Missing-Model- und Mehrdeutigkeitsszenarien. Statistiken und Traces zählen tatsächliche Arbeit. Kalte und inkrementelle Resultate sind über deterministische Mutationssequenzen fachlich äquivalent. C-ABI, WASM, Language Tools, Node-LSP, Monaco und Web-IDE behalten kompatible öffentliche Verträge. Normale Sourceupdates und Repository-Nachladungen verursachen keinen Worker-Neustart oder Full Replay. Die Web-IDE verarbeitet bekannte Einzeldateioperationen direkt und verwendet Full Scans nur für definierte Recovery- und Workspacegrenzen. Last-Good-Diagramm, Completion, Diagnostik, Problems, Output, PWA und Offline-Verhalten bleiben erhalten.

---

# Anhang A – Empfohlene Header

## A.1 `ParseMode.h`

```cpp
#pragma once

#include <cstdint>

namespace ilic::detail {

enum class ParseMode : std::uint8_t {
    StrictCompiler,
    TolerantEditor
};

const char* toString(ParseMode mode) noexcept;

}
```

## A.2 `ParsedSourceCache.h`

```cpp
#pragma once

#include "ParseMode.h"
#include "ParsedSourceArtifact.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace ilic::detail {

class SourceParser;
class IncrementalOperationRecorder;

struct ParsedSourceKey;
struct ParsedSourceCacheEntry;
struct ParsedSourceCacheSnapshot;

class ParsedSourceCache {
public:
    explicit ParsedSourceCache(ParsedSourceCacheOptions options);

    std::shared_ptr<const ParsedSourceCacheEntry> getOrBuild(
        const SourceBuffer& source,
        const SourceIdentity& identity,
        ParseMode mode,
        const SourceParser& parser,
        IncrementalOperationRecorder& recorder
    );

    void invalidateUri(
        std::string_view uri,
        InvalidationReason reason,
        IncrementalOperationRecorder& recorder
    );

    void clear(IncrementalOperationRecorder& recorder);

    ParsedSourceCacheSnapshot snapshot() const;
    bool checkInvariants(std::string* error = nullptr) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
```

## A.3 `RootAnalysisCache.h`

```cpp
#pragma once

#include <memory>
#include <string>

namespace ilic::detail {

class IncrementalOperationRecorder;
class SourceModelIndex;
class SourceDependencyIndex;

class RootAnalysisCache {
public:
    explicit RootAnalysisCache(RootAnalysisCacheOptions options);

    std::shared_ptr<const RootAnalysisCacheEntry> findExact(
        const RootAnalysisKey& key,
        IncrementalOperationRecorder& recorder
    );

    void insert(
        RootAnalysisCacheEntry entry,
        IncrementalOperationRecorder& recorder
    );

    RootInvalidationResult invalidate(
        const SourceChangeImpact& impact,
        const SourceModelIndex& models,
        const SourceDependencyIndex& dependencies,
        IncrementalOperationRecorder& recorder
    );

    void clear(IncrementalOperationRecorder& recorder);

    RootAnalysisCacheSnapshot snapshot() const;
    bool checkInvariants(std::string* error = nullptr) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
```

---

# Anhang B – Invalidierungsmatrix

| Sourceoperation | Parser Strict | Parser Editor | Modelindex | Rootcache |
|---|---:|---:|---:|---:|
| Unchanged | behalten | behalten | unverändert | behalten |
| VersionOnly | behalten | behalten | Version aktualisieren | behalten, bei Materialisierung Version ersetzen |
| ContentChanged, Header gleich | invalidieren | invalidieren | Identität aktualisieren | Roots mit URI in Closure invalidieren |
| ContentChanged, Importset geändert | invalidieren | invalidieren | ersetzen | Closure-/Reverse-Roots invalidieren |
| ContentChanged, Modellname geändert | invalidieren | invalidieren | Reverseindizes ersetzen | alte und neue Modellnutzer invalidieren |
| Added unabhängig | neu bei Bedarf | neu bei Bedarf | hinzufügen | behalten, wenn Unabhängigkeit bewiesen |
| Added erfüllt Missing Model | neu bei Bedarf | neu bei Bedarf | hinzufügen | Missing-Model-Roots invalidieren |
| Added erzeugt Duplikat | neu bei Bedarf | neu bei Bedarf | hinzufügen | betroffene Modellnutzer invalidieren |
| Removed unabhängig | entfernen | entfernen | entfernen | behalten, wenn Unabhängigkeit bewiesen |
| Removed Dependency | entfernen | entfernen | entfernen | Nutzer invalidieren |
| Reintroduced | neu | neu | ersetzen | wie Added plus URI-Identitätswechsel |
| Compileroption geändert | behalten | behalten | behalten | anderer Key, kein falscher Hit |
| External Meta Attribute geändert | behalten | behalten | behalten | anderer Key |
| Grammarfingerprint geändert | Miss | Miss | neu aus Strict Header | Rootkey Miss |

---

# Anhang C – Minimale Cross-Repo-Abnahmesequenz

## C.1 Sources

`Base.ili`:

```ili
INTERLIS 2.4;

MODEL Base AT "https://example.invalid" VERSION "1" =
  DOMAIN
    Identifier = TEXT*20;
END Base.
```

`Roads.ili`:

```ili
INTERLIS 2.4;

MODEL Roads AT "https://example.invalid" VERSION "1" =
  IMPORTS Base;

  TOPIC Network =
    CLASS Road =
      Id : Base.Identifier;
    END Road;
  END Network;
END Roads.
```

`Buildings.ili`:

```ili
INTERLIS 2.4;

MODEL Buildings AT "https://example.invalid" VERSION "1" =
  IMPORTS Base;

  TOPIC Objects =
    CLASS Building =
      Id : Base.Identifier;
    END Building;
  END Objects;
END Buildings.
```

## C.2 Schritte

1. alle Sources registrieren;
2. Roads kompilieren;
3. Buildings kompilieren;
4. beide erneut kompilieren;
5. Roads-EditorSnapshot anfordern;
6. Roads unvollständig machen;
7. EditorSnapshot anfordern;
8. Roads strikt kompilieren;
9. Fehler korrigieren;
10. Roads kompilieren;
11. Base ändern;
12. Roads und Buildings kompilieren;
13. unabhängige Source hinzufügen;
14. Roads kompilieren;
15. Duplikat `Base` hinzufügen;
16. Roads kompilieren;
17. Duplikat entfernen;
18. Roads kompilieren;
19. Cache clear;
20. Roads kompilieren;
21. Worker crash simulieren;
22. Replay;
23. Roads kompilieren;
24. Cold-/Incremental vergleichen.

## C.3 Erwartung

- toleranter Snapshot bleibt brauchbar;
- strikter Compile akzeptiert unvollständige Source nie;
- Baseänderung invalidiert beide Roots;
- unabhängige Source invalidiert keinen Root, wenn bewiesen;
- Duplikat invalidiert;
- Cacheclear zerstört Sources nicht;
- Workercrash replayt genau einmal;
- finale Resultate cold/incremental identisch.

---

# Anhang D – Reviewcheckliste

## Parsing

- [ ] Ist `ParseMode` in Typen sichtbar?
- [ ] Kann ein tolerantes Artefakt `buildMetaModel` aufrufen?
- [ ] Ist Editor-Recovery in eigener Klasse/Datei?
- [ ] Enthält der Cachekey den Modus?
- [ ] Sind Aufrufreihenfolgentests vorhanden?

## Sourceimpact

- [ ] Wird `Added` behandelt?
- [ ] Werden Missing Models berücksichtigt?
- [ ] Werden Duplikate berücksichtigt?
- [ ] Werden alte Reverseindizes entfernt?
- [ ] Gibt es konservativen Fallback?

## Cache

- [ ] Gibt es nur einen Erase-Pfad?
- [ ] Stimmen Bytes nach Invalidierung?
- [ ] Stimmen Bytes nach Eviction?
- [ ] Stimmen Bytes nach Clear?
- [ ] Bleiben externe Snapshots gültig?

## Memory

- [ ] Werden Strings rekursiv geschätzt?
- [ ] Werden ANTLR-Tokens/Knoten geschätzt?
- [ ] Werden Diagramm und Dokumentation geschätzt?
- [ ] Gibt es Overflow-Schutz?
- [ ] Gibt es WASM-Schleifentest?

## Stats

- [ ] Request und Execution getrennt?
- [ ] Strict und Tolerant getrennt?
- [ ] Closurecounter exakt?
- [ ] Gauges aktuell?
- [ ] Resetvertrag klar?

## Consumers

- [ ] Kein Restart nach Missing Model?
- [ ] Replay nur bei Recovery?
- [ ] Completion bleibt bei nachlaufender Analyse?
- [ ] Last-Good bleibt?
- [ ] Einzeldateioperation ohne Full Scan?
- [ ] PWA/Offline grün?

---

# Anhang E – Agentenarbeitsregeln

Der Agent MUSS zu Beginn ausführen:

```bash
pwd

git status --short
git branch --show-current
git rev-parse HEAD

git -C ../interlis-language-tools status --short
git -C ../interlis-language-tools branch --show-current
git -C ../interlis-language-tools rev-parse HEAD

git -C ../interlis-web-ide status --short
git -C ../interlis-web-ide branch --show-current
git -C ../interlis-web-ide rev-parse HEAD
```

Er MUSS:

- fremde Änderungen bewahren;
- keine Hard Resets verwenden;
- keine Tests behaupten, die nicht liefen;
- nicht nach jeder normalen Entscheidung rückfragen;
- in kleinen buildbaren Schritten arbeiten;
- Fehler klassifizieren und beheben;
- alle drei Repositories tatsächlich testen.

Er DARF NICHT ohne separate ausdrückliche Anweisung:

- committen;
- pushen;
- Pull Requests öffnen;
- Branches löschen;
- fremde Änderungen revertieren.
