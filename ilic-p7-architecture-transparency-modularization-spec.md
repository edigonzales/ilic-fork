# P7 – Architecture Transparency and Sustainable Modularization

## Verbindliche Spezifikation für einen autonomen LLM-Coding-Agenten

**Projektverbund**

```text
../ilic-fork
../interlis-language-tools
../interlis-web-ide
```

**Status:** normative Implementierungsspezifikation  
**Phase:** P7  
**Schwerpunkt:** Verständlichkeit, Modulgrenzen, Ownership, Wartbarkeit und verhaltensbewahrendes Refactoring  
**Explizit nicht Bestandteil:** neue INTERLIS-Funktionalität, geänderte Compilerentscheidungen oder ein Framework-Neubau

---

## 1. Zweck dieser Phase

P7 macht die über P0 bis P6 entstandene Codebasis transparent, nachhaltig und für neue Entwicklerinnen und Entwickler nachvollziehbar.

Die vorangegangenen Phasen haben fachliche Robustheit, Sessionisolation, einen autoritativen Parser, Repositorymodularisierung, inkrementelle Kompilation, deren Hardening und einen strukturierten Diagnosevertrag geschaffen. Dadurch sind jedoch mehrere Orchestrierungsdateien und Fassaden stark gewachsen.

P7 soll diese Architektur nicht erneut erfinden. P7 soll die bereits vorhandenen Verantwortlichkeiten sichtbar machen und in klar benannte Komponenten überführen.

Die Leitfrage lautet:

> Kann eine fachkundige Person eine Änderung an Parsing, Kompilation, ABI, Language Service oder Web-IDE durchführen, ohne zuerst eine sehr grosse Datei mit vielen unabhängigen Zuständigkeiten vollständig verstehen zu müssen?

P7 ist erfolgreich, wenn:

- öffentliche Fassaden klein und stabil bleiben;
- jede Komponente einen klaren Lebenszyklus und eine klar benannte Verantwortung besitzt;
- Abhängigkeiten gerichtet und überprüfbar sind;
- temporärer Requestzustand nicht mit langfristigem Sessionzustand vermischt ist;
- reine Projektion nicht mit fachlicher Entscheidung vermischt ist;
- Workertransport nicht mit Compilersemantik vermischt ist;
- DOM-Orchestrierung nicht mit Workspace-, Compiler- und Diagrammlogik vermischt ist;
- Tests Komponenten direkt und nicht nur über God-Classes ansprechen können;
- Verhalten, ABI, Conformance, Diagnosen und Performancebudgets unverändert bleiben.

---

## 2. Informative Referenzstände

Zum Zeitpunkt der Erstellung dieser Spezifikation waren folgende öffentliche Stände sichtbar:

```text
ilic-fork:
c9cb9f88a1699eeca0d70cb18c69528b0c8d45f2

interlis-language-tools:
c3c42d52bb445ddd7e1330e5fea2b562b2502b52

interlis-web-ide:
dcd11c024c2df045aa04609ba4426390ff43b243
```

Diese Commits sind nur informative Referenzen.

Der Coding-Agent MUSS zu Beginn die tatsächlichen lokalen Commits, Branches, Dateien und bereits vorhandenen Refactorings ermitteln. Der lokale Arbeitsstand ist die Quelle der Wahrheit.

P7 MUSS auf dem tatsächlich vorhandenen P5.1- und P6-Zustand aufbauen. Bereits extrahierte Komponenten dürfen nicht dupliziert oder durch parallele Neuentwürfe ersetzt werden.

---

## 3. Normative Schlüsselwörter

Die Begriffe **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT** und **DARF** sind normativ.

- **MUSS** ist eine zwingende Abnahmebedingung.
- **DARF NICHT** ist ein zwingendes Verbot.
- **SOLL** darf nur mit dokumentierter technischer Begründung abweichend umgesetzt werden.
- **DARF** bezeichnet eine ausdrücklich erlaubte Variante.
- Eine offene MUSS-Anforderung verhindert die Bezeichnung von P7 als abgeschlossen.

---

## 4. Unverhandelbarer Behavior Lock

P7 ist ein verhaltensbewahrendes Refactoring.

Nach P7 müssen für unveränderte Eingaben unverändert bleiben:

```text
Accept/Reject
Compilererfolg
Fehler- und Warnungszahl
Missing Models
kompilierte Modelle
Modellreihenfolge, soweit öffentlich festgelegt
Diagnosecodes
Diagnose-Severity
Diagnose-Ranges
Related Information
Diagnose-Reihenfolge
Transkript, abzüglich bereits erlaubter Zeitstempel
Logs
SyntaxSnapshot
EditorSnapshot
SemanticSnapshot
Diagramm
Dokumentation
Repositoryauflösung
Incremental Stats
Incremental Trace
Cache Snapshot
Worker-Replay-Semantik
LSP-Verhalten
Monaco-Verhalten
Web-IDE-Verhalten
```

P7 darf keine bestehende Conformance-Baseline ändern.

P7 darf keine bekannte Conformanceabweichung korrigieren oder neu erzeugen.

P7 darf keine P6-Diagnoseformulierung ändern, ausser eine rein mechanische Verschiebung erfordert eine unvermeidbare Anpassung. Jede solche Anpassung benötigt einen expliziten Snapshot- und Decision-Lock-Nachweis.

---

## 5. API-, ABI- und Package-Lock

### 5.1 C++ API

Alle bestehenden öffentlichen Header und Signaturen bleiben source-kompatibel.

Insbesondere bleiben erhalten:

```cpp
ilic::CompilerSession
ilic::Compiler
ilic::CompilationRequest
ilic::CompilationResult
ilic::CompilationAnalysisResult
ilic::SyntaxSnapshot
ilic::EditorSnapshot
ilic::SemanticSnapshot
ilic::Diagnostic
ilic::IncrementalStats
ilic::IncrementalTrace
ilic::IncrementalCacheSnapshot
```

Neue öffentliche Typen sind nur zulässig, wenn sie für eine bestehende öffentliche Funktion zwingend benötigt werden. Reine Refactoringkomponenten bleiben intern.

### 5.2 C-ABI

Alle bestehenden exportierten Symbole, Statuscodes, Handles und JSON-Felder bleiben kompatibel.

Bestehende Exportnamen dürfen nicht entfernt, umbenannt oder in ihrer Semantik verändert werden.

### 5.3 npm

Bestehende Package-Namen und Exportpfade bleiben erhalten:

```text
@ilic/compiler-wasm
@ilic/language-service
@ilic/language-server
@ilic/monaco-adapter
@ilic/diagram
@ilic/docx
@ilic/repository-core
@ilic/tools
```

Interne Module dürfen über `exports` verborgen bleiben. P7 soll keine unnötig neue öffentliche Oberfläche schaffen.

### 5.4 LSP und Web-IDE

Bestehende LSP-Requests, Notifications, Monaco-Verträge, Workspace-URIs, Problems-Verhalten und Web-IDE-Bedienabläufe bleiben erhalten.

---

## 6. Nichtziele

P7 implementiert ausdrücklich nicht:

- neue INTERLIS-Sprachregeln;
- neue Parserregeln;
- neue Diagnosearten;
- neue Repositoryauflösung;
- neue Inkrementalitätsalgorithmen;
- neue Cachepolitiken;
- Compilerdaemon;
- persistente Compiler-Caches;
- neue Quick Fixes;
- neue Diagrammfunktionen;
- neue Web-IDE-Funktionen;
- Wechsel zu React, Vue, Svelte oder einem anderen UI-Framework;
- Wechsel zu einer neuen State-Management-Bibliothek;
- Wechsel des Workerprotokolls auf eine externe RPC-Bibliothek;
- vollständige Neuschreibung;
- rein kosmetische Dateizerstückelung ohne echte Verantwortungsgrenzen;
- Umbenennung sämtlicher bestehender Begriffe nur zur Stilvereinheitlichung;
- erzwungene Header-only-Architektur;
- C++-Module;
- Migration auf eine andere C++-Standardversion;
- automatisches Dependency Injection Framework.

---

## 7. Architekturprinzipien

### 7.1 Fassade und Komponenten

Öffentliche Fassaden bleiben klein und delegieren.

```text
Public Facade
    |
    +--> Lifecycle Component
    +--> State Owner
    +--> Domain Service
    +--> Projection/Serialization
```

Eine Fassade darf:

- Parameter prüfen;
- Locks setzen;
- Komponenten aufrufen;
- Ergebnisse zurückgeben;
- Lebenszyklus koordinieren.

Eine Fassade soll nicht:

- fachliche Traversierungen implementieren;
- Cachekeys bauen;
- JSON vollständig serialisieren;
- Worker-Replay durchführen;
- DOM-Bäume rendern;
- mehrere unabhängige Stores direkt verwalten.

### 7.2 Ein Eigentümer pro Zustand

Für jeden veränderlichen Zustand muss genau ein Eigentümer dokumentiert sein.

Beispiele:

```text
SourceManager                owns registered compiler sources
ParsedSourceCache            owns cache entries
RootAnalysisCache            owns root result entries
CompilationRun               owns request-local compile state
SourceRegistry               owns language-service source layers
SemanticSnapshotStore        owns semantic snapshots
WorkerSourceMirror           owns worker replay source mirror
TabController                owns open editor tabs
DiagramPanelController       owns diagram UI state
```

Andere Komponenten erhalten Ports, Views oder kontrollierte Methoden, aber keinen unkontrollierten direkten Schreibzugriff.

### 7.3 Langfristiger und temporärer Zustand

Sessionzustand und Requestzustand müssen getrennt sein.

```text
long-lived:
    sources
    caches
    indices
    worker mirror
    open documents
    UI state

per request:
    CompilerContext
    CompilationRun
    transcript cursor
    dependency scheduling
    pending RPC
    one diagram layout request
```

Temporärer Zustand darf nach Requestende nicht versehentlich in langlebigen Komponenten hängen bleiben.

### 7.4 Projektion ist keine Fachlogik

Folgende Bereiche sind Projektion:

- Snapshot-Erzeugung aus Parserartefakten;
- JSON-Serialisierung;
- LSP-Mapping;
- Monaco-Mapping;
- Problems-Projektion;
- CLI-Rendering.

Sie dürfen keine neue Compilerentscheidung treffen.

### 7.5 Gerichtete Abhängigkeiten

Abhängigkeiten müssen von Fassade zu internen Komponenten und von Orchestrierung zu Domänenports zeigen.

Nicht zulässig:

```text
cache -> CompilerSession
serializer -> C-ABI facade
worker protocol -> LanguageService implementation
problem renderer -> WorkspaceManager internals
diagram controller -> tab map internals
```

### 7.6 Keine abstrakten Interfaces ohne Bedarf

P7 soll nicht für jede Klasse ein Interface erzeugen.

Ein Interface oder Port ist sinnvoll, wenn mindestens eine Bedingung gilt:

- zwei reale Implementierungen;
- Test-Doppel nötig;
- Paket- oder Threadgrenze;
- Plattformgrenze;
- Ownership muss bewusst verborgen werden.

---

## 8. Verbindliche Ausgangsanalyse

Vor der ersten produktiven Änderung muss der Coding-Agent einen Architekturreport erzeugen.

Mindestens:

```text
build/p7-baseline/file-metrics.json
build/p7-baseline/dependency-graph.json
build/p7-baseline/public-api.json
build/p7-baseline/capi-symbols.txt
build/p7-baseline/npm-exports.json
build/p7-baseline/behavior-lock.json
```

### 8.1 Dateimetriken

Für alle produktiven Dateien erfassen:

- Zeilen;
- nicht leere Zeilen;
- Include-/Importzahl;
- Klassen;
- Methoden;
- Funktionen;
- längste Funktion;
- private Felder;
- direkte Abhängigkeiten;
- Testdateien;
- zyklische Abhängigkeiten.

### 8.2 Hotspots

Mindestens untersuchen:

```text
ilic-fork/source/core/Compiler.cpp
ilic-fork/source/core/SnapshotPipeline.cpp
ilic-fork/source/abi/Capi.cpp
ilic-fork/packages/compiler-wasm/index.js

interlis-language-tools/packages/language-service/src/service.ts
interlis-language-tools/packages/language-service/src/compiler-worker.ts

interlis-web-ide/src/workbench/workbench.ts
```

Falls diese Dateien bereits aufgeteilt wurden, die aktuellen Nachfolger inventarisieren.

### 8.3 Zustandsinventar

Für jede Hotspotklasse oder -datei erfassen:

- langlebigen Zustand;
- Requestzustand;
- Locks;
- Timer;
- Listener;
- Caches;
- Maps und Sets;
- Ownership;
- Dispose-Pfad;
- Fehlerpfad;
- Testzugang.

### 8.4 Call Graph

Mindestens folgende Pfade als Diagramm dokumentieren:

```text
putSource -> invalidation -> parser cache -> root cache
compileAndAnalyze -> compileRun -> semantic snapshot
editorSnapshot -> tolerant parser -> projection
C API call -> request decode -> session -> result encode
WASM call -> memory copy -> C API -> JSON -> JS result
open/change/save -> LanguageService -> workers -> diagnostics
workspace event -> synchronizer -> LanguageService
compile event -> Problems/Output/Diagram
```

---

## 9. Zielarchitektur `ilic-fork`

Empfohlene interne Struktur:

```text
source/core/
    Compiler.cpp
    CompilerSessionState.h
    CompilerSessionState.cpp
    SourceUpdateCoordinator.h
    SourceUpdateCoordinator.cpp
    SnapshotService.h
    SnapshotService.cpp
    CompilationOrchestrator.h
    CompilationOrchestrator.cpp
    CompilationRun.h
    CompilationRun.cpp
    CompilationTranscript.h
    CompilationTranscript.cpp
    ExternalMetaAttributeApplier.h
    ExternalMetaAttributeApplier.cpp
    ModelCompilationScheduler.h
    ModelCompilationScheduler.cpp
    SourceMaterializer.h
    SourceMaterializer.cpp

source/core/snapshot/
    ParseMode.cpp
    LanguageDetector.h
    LanguageDetector.cpp
    ParsedSourceArtifact.cpp
    StrictSourceParser.h
    StrictSourceParser.cpp
    TolerantEditorParser.h
    TolerantEditorParser.cpp
    EditorRecoveryTokenStream.h
    EditorRecoveryTokenStream.cpp
    SyntaxSnapshotProjector.h
    SyntaxSnapshotProjector.cpp
    EditorSnapshotProjector.h
    EditorSnapshotProjector.cpp
    Ili1EditorProjector.h
    Ili1EditorProjector.cpp
    Ili2EditorProjector.h
    Ili2EditorProjector.cpp
    SnapshotDiagnosticOrderer.h
    SnapshotDiagnosticOrderer.cpp

source/core/incremental/
    existing P5.1 components

source/abi/
    Capi.cpp
    SessionRegistry.h
    SessionRegistry.cpp
    ResultRegistry.h
    ResultRegistry.cpp
    RequestDecoder.h
    RequestDecoder.cpp
    DiagnosticJson.h
    DiagnosticJson.cpp
    CompilationJson.h
    CompilationJson.cpp
    SyntaxJson.h
    SyntaxJson.cpp
    EditorJson.h
    EditorJson.cpp
    SemanticJson.h
    SemanticJson.cpp
    IncrementalJson.h
    IncrementalJson.cpp
    FormattingJson.h
    FormattingJson.cpp
```

Die genaue Dateibenennung darf an den bestehenden Stil angepasst werden. Die Verantwortungsgrenzen sind normativ.

---

## 10. `CompilerSessionState`

### 10.1 Verantwortung

`CompilerSessionState` besitzt den gesamten langlebigen nativen Sessionzustand.

Empfohlene Struktur:

```cpp
namespace ilic::detail {

class CompilerSessionState final {
public:
    explicit CompilerSessionState(IncrementalCacheOptions options);

    SourceManager& sources() noexcept;
    const SourceManager& sources() const noexcept;

    ParsedSourceCache& parsedSources() noexcept;
    RootAnalysisCache& rootAnalyses() noexcept;
    SourceModelIndex& modelIndex() noexcept;
    SourceDependencyIndex& dependencyIndex() noexcept;

    IncrementalStatistics& incrementalStatistics() noexcept;
    IncrementalTraceRecorder& incrementalTrace() noexcept;

    std::mutex& mutex() noexcept;

private:
    SourceManager sources_;
    ParsedSourceCache parsedSources_;
    RootAnalysisCache rootAnalyses_;
    SourceModelIndex modelIndex_;
    SourceDependencyIndex dependencyIndex_;
    IncrementalStatistics statistics_;
    IncrementalTraceRecorder trace_;
    std::mutex mutex_;
};

}
```

Die realen Statistiktypen sind anzupassen.

### 10.2 Regeln

- Keine Fachlogik in Gettern.
- Keine `CompilerSession`-Rückreferenz.
- Keine globalen oder `thread_local` Zustände.
- Keine öffentliche Exposition im installierten C++-API.
- Konstruktor initialisiert alle Budgets deterministisch.
- Zerstörungsreihenfolge ist dokumentiert.
- Mutex schützt die Session, nicht globale Ressourcen.
- Komponenten dürfen nicht ihre eigenen parallelen Sessionmutexes einführen, sofern nicht technisch erforderlich.

---

## 11. `SourceUpdateCoordinator`

### 11.1 Verantwortung

Kapselt `updateSource` und `removeSource` einschliesslich:

- SourceManager-Update;
- Parsercacheinvalidierung;
- SourceModelIndex;
- SourceDependencyIndex;
- SourceImpactAnalyzer;
- Rootinvalidierung;
- Statistik;
- Trace;
- Bytes released.

### 11.2 API

```cpp
class SourceUpdateCoordinator final {
public:
    explicit SourceUpdateCoordinator(CompilerSessionState& state);

    SourceUpdateResult update(
        std::string uri,
        std::string utf8,
        std::uint64_t documentVersion
    );

    bool remove(std::string_view uri);

private:
    void updateIndexes(
        const SourceUpdateResult& update,
        const SourceBuffer& source
    );

    RootInvalidationResult invalidateAffectedRoots(
        const SourceUpdateResult& update,
        const SourceModelRecord* before,
        const SourceModelRecord* after
    );

    CompilerSessionState& state_;
};
```

### 11.3 Invarianten

- Genau ein Source-Update pro Aufruf.
- Indexupdate und Invalidierungsentscheidung verwenden denselben Before-/After-Zustand.
- Statistik wird genau einmal aktualisiert.
- Trace wird vor dem Aufruf zurückgesetzt.
- Keine Parser- oder Root-Map wird direkt ausserhalb ihrer API verändert.
- Entfernen und Update teilen Hilfslogik, wo fachlich identisch.
- Verhalten gegenüber `Added`, `VersionOnly`, `ContentChanged`, `Reintroduced`, `Removed` und `Rejected` bleibt unverändert.

---

## 12. `SnapshotService`

### 12.1 Verantwortung

Kapselt:

- striktes `parse(uri)`;
- tolerantes `editorSnapshot(uri)`;
- Zugriff auf `ParsedSourceCache`;
- Materialisierungszähler;
- Dokumentversionsprojektion;
- Missing-Source-Fallback.

### 12.2 API

```cpp
class SnapshotService final {
public:
    explicit SnapshotService(CompilerSessionState& state);

    SyntaxSnapshot syntax(std::string_view uri);
    EditorSnapshot editor(std::string_view uri);

    ParsedSourceArtifactPtr strictArtifact(
        const SourceBuffer& source
    );

    std::shared_ptr<const ParsedSourceCacheEntry> parsed(
        const SourceBuffer& source,
        ParseMode mode
    );

private:
    CompilerSessionState& state_;
    ParsedSourceArtifactFactory factory_;
};
```

### 12.3 Regeln

- Ein zentraler `parsed()`-Pfad.
- `StrictCompiler` und `TolerantEditor` bleiben getrennt.
- Statistik und Trace werden im zentralen Pfad erzeugt.
- Keine vollständigen SnapshotBundle-Kopien, wenn nur ein Artefakt benötigt wird.
- Fallbacks verwenden dieselben Parserkomponenten und keine parallele Pipeline.
- `CompilerSession::parse()` und `editorSnapshot()` setzen nur Lock, Traceoperation und Delegation.

---

## 13. `CompilationOrchestrator`

### 13.1 Verantwortung

Kapselt die langfristige Sessionintegration eines Compile-Requests:

- Requestkey;
- Rootcachelookup;
- Exact-Hit-Projektion;
- Erzeugung eines `CompilationRun`;
- Syntax- und Semantic-Projektion;
- Rootcacheinsert;
- Statistiken;
- Trace.

### 13.2 API

```cpp
class CompilationOrchestrator final {
public:
    CompilationOrchestrator(
        CompilerSessionState& state,
        SnapshotService& snapshots
    );

    CompilationAnalysisResult compileAndAnalyze(
        const CompilationRequest& request
    );

    CompilationResult compile(
        const CompilationRequest& request
    );

private:
    std::optional<CompilationAnalysisResult> exactCacheHit(
        const CompilationRequest& request,
        const RootRequestKey& key
    );

    RootAnalysisCacheEntry makeCacheEntry(
        const RootRequestKey& key,
        const CompilationRunResult& run,
        const CompilationAnalysisResult& result
    );

    void updateVisibleDocumentVersions(
        CompilationAnalysisResult& result
    ) const;

    CompilerSessionState& state_;
    SnapshotService& snapshots_;
    RootRequestKeyBuilder keyBuilder_;
};
```

### 13.3 Regeln

- Kein Parserdetailswissen.
- Kein direkter ANTLR-Zugriff.
- Kein CLI-Textformatierungswissen.
- Keine externe Metaattributtraversierung.
- Kein Dateisystemladen ausser über `CompilationRun`/`SourceMaterializer`.
- Exact Cache Hit führt keine Compilerarbeit aus.
- Verhalten der bestehenden Stats und Traces bleibt identisch.
- `compile()` und `compileAndAnalyze()` teilen den fachlich gemeinsamen Run-Pfad, ohne Cacheverhalten zu verfälschen.

---

## 14. `CompilationRun`

### 14.1 Verantwortung

`CompilationRun` ist ein kurzlebiges Objekt pro tatsächlicher Compiler-Ausführung.

Es besitzt:

- `CompilerContext`;
- `CompilationRequest`-View;
- geladenen Dateikatalog;
- Request-lokale Sets und Vektoren;
- Transcript;
- Diagnostic-/Log-Cursor;
- kompilierte Dateien und Modelle;
- Ergebnisaufbau.

### 14.2 API

```cpp
struct CompilationRunResult {
    CompilationResult compilation;
    std::vector<std::string> sourceUris;
    const metamodel::MetaModelStore* modelStore = nullptr;
};

class CompilationRun final {
public:
    CompilationRun(
        const SourceManager& sources,
        const CompilationRequest& request,
        ParsedArtifactProvider artifacts
    );

    CompilationRunResult execute();

private:
    void loadRoots();
    void loadImportedModels();
    void recordInputFiles();
    void compileBuiltinModel();
    void compileModels();
    void applyExternalMetaAttributes();
    void runSemanticChecks();
    void collectCompiledModels();

    CompilationResult finish();

    CompilerContext context_;
    const CompilationRequest& request_;
    CompilationTranscript transcript_;
    ModelCompilationScheduler scheduler_;
    std::vector<std::string> sourceUris_;
};
```

### 14.3 Lebensdauer

- `CompilationRun` darf nicht in Sessioncaches gespeichert werden.
- Ein Cacheeintrag erhält nur vollständig besessene Resultatobjekte.
- Keine Pointer in veröffentlichten Resultaten dürfen auf `CompilationRun` zeigen.
- `MetaModelStore` darf nur während SemanticSnapshot-Erzeugung verwendet werden.
- Cancellation und Exceptions müssen durch einen einzigen Abschlussweg laufen.

---

## 15. `CompilationTranscript`

### 15.1 Verantwortung

Kapselt:

- Transcriptzeilen;
- Diagnostic-/Log-Cursor;
- Startheader;
- Inputdateien;
- Compilefortschritt;
- Abschlusszeile;
- Zeitstempelprojektion.

### 15.2 API

```cpp
class CompilationTranscript final {
public:
    explicit CompilationTranscript(
        std::string compilerVersion,
        CompilerClock& clock
    );

    void info(std::string value);
    void warning(std::string value);
    void error(std::string value);
    void blank();

    void appendLoggerEvents(const util::Logger& logger);
    void inputFile(const util::IliFile& file);
    void compilationCompleted(int errors, int warnings);

    std::vector<std::string> release();

private:
    std::vector<std::string> lines_;
    std::size_t diagnosticCursor_ = 0;
    std::size_t logCursor_ = 0;
    CompilerClock& clock_;
};
```

### 15.3 Clock

Direkte Systemzeit soll aus Compilerorchestrierung verschwinden.

```cpp
class CompilerClock {
public:
    virtual ~CompilerClock() = default;
    virtual std::string localTimestamp() const = 0;
};
```

Produktiv darf eine kleine konkrete Systemclock verwendet werden. Tests erhalten eine feste Clock.

Die öffentliche Transcriptsemantik bleibt unverändert.

---

## 16. `ExternalMetaAttributeApplier`

### 16.1 Verantwortung

Kapselt ausschliesslich das Auffinden von Metaelementen und Anwenden bereits unterstützter externer Metaattribute.

### 16.2 API

```cpp
class ExternalMetaAttributeApplier final {
public:
    void apply(
        const std::vector<ExternalMetaAttribute>& attributes,
        metamodel::MetaModelStore& store,
        util::Logger& logger
    ) const;

private:
    metamodel::MetaElement* findTarget(
        metamodel::Package& package,
        std::string_view path,
        std::string_view prefix
    ) const;
};
```

### 16.3 Regeln

- Keine neuen Metaattribute.
- Keine geänderte Fehlermeldung ohne P6-Snapshotnachweis.
- Keine Abhängigkeit von CompilerSession oder C-ABI.
- Traversierung ist direkt testbar.
- Rekursion besitzt eine klare Zuständigkeit und Nullbehandlung.

---

## 17. `ModelCompilationScheduler`

### 17.1 Verantwortung

Kapselt die bestehende Kompilierreihenfolge und das Erkennen nicht auflösbarer Dependencyordnungen.

### 17.2 API

```cpp
class ModelCompilationScheduler final {
public:
    explicit ModelCompilationScheduler(
        std::span<util::IliFile* const> files
    );

    std::vector<util::IliFile*> ordered(
        const std::set<std::string>& initiallyAvailableModels
    ) const;

    bool canCompile(
        const util::IliFile& file,
        const std::set<std::string>& availableModels
    ) const;
};
```

Die Implementierung darf den bestehenden iterativen Algorithmus zunächst kapseln. Eine algorithmische Änderung ist kein P7-Ziel.

### 17.3 Behavior Lock

- gleiche Reihenfolge;
- gleicher Zyklus-/Dependencyfehler;
- gleiche Behandlung lokaler Modelle;
- gleiche Behandlung von `INTERLIS`;
- gleiche Transcriptreihenfolge.

---

## 18. Snapshotmodularisierung

`SnapshotPipeline.cpp` darf nicht nur mechanisch in willkürliche Dateien geteilt werden. Die Trennung folgt Datenfluss und Lebensdauer.

### 18.1 `LanguageDetector`

```cpp
enum class DetectedLanguage {
    Unknown,
    Ili1,
    Ili2
};

class LanguageDetector final {
public:
    DetectedLanguage detect(
        std::string_view utf8
    ) const noexcept;
};
```

Regeln:

- keine Parsersemantik;
- nur vorhandenes Erkennungsverhalten;
- Kommentare und Whitespace unverändert behandeln;
- fokussierte Tests.

### 18.2 `ParsedSourceArtifactFactory`

```cpp
class ParsedSourceArtifactFactory final {
public:
    ParsedSourceArtifactPtr build(
        const SourceBuffer& source,
        ParseMode mode
    ) const;
};
```

Sie dispatcht nur zu `StrictSourceParser` oder `TolerantEditorParser`.

### 18.3 `StrictSourceParser`

```cpp
class StrictSourceParser final {
public:
    ParsedSourceArtifactPtr parse(
        const SourceBuffer& source,
        DetectedLanguage language
    ) const;
};
```

- verwendet normalen `CommonTokenStream`;
- erzeugt striktes Artefakt;
- unterstützt MetaModelBuild;
- keine Editor-Recovery.

### 18.4 `TolerantEditorParser`

```cpp
class TolerantEditorParser final {
public:
    ParsedSourceArtifactPtr parse(
        const SourceBuffer& source,
        DetectedLanguage language
    ) const;
};
```

- verwendet gezielte Editor-Recovery;
- unterstützt kein MetaModelBuild;
- kennzeichnet Recovery;
- keine Compilersemantik.

### 18.5 `EditorRecoveryTokenStream`

Die bestehende Token-Recovery erhält eine eigene Datei und Tests.

```cpp
class EditorRecoveryTokenStream final
    : public antlr4::CommonTokenStream {
public:
    explicit EditorRecoveryTokenStream(
        antlr4::TokenSource* source
    );

    EditorRecoveryResult recover();

    bool recovered() const noexcept;
};
```

`EditorRecoveryResult` kann Zähler oder Recoveryarten enthalten, darf aber nicht öffentlich sein.

### 18.6 `SyntaxSnapshotProjector`

```cpp
class SyntaxSnapshotProjector final {
public:
    SyntaxSnapshot project(
        const ParsedSourceArtifact& artifact
    ) const;
};
```

Verantwortet:

- Tokens;
- Nodes;
- Kontexte;
- Imports;
- parserseitige Diagnosen;
- deterministische Ordnung.

### 18.7 `EditorSnapshotProjector`

```cpp
class EditorSnapshotProjector final {
public:
    EditorSnapshot project(
        const ParsedSourceArtifact& artifact
    ) const;
};
```

Delegiert sprachspezifisch:

```cpp
class Ili1EditorProjector;
class Ili2EditorProjector;
```

### 18.8 Verbotene Kopplung

- Projector kennt keinen Cache.
- Parser kennt keinen CompilerSession.
- Recoverytokenstream kennt keinen EditorSnapshot.
- Ili1-Projector enthält keine Ili2-Dynamic-Casts.
- Diagnoseordnung ist zentral.
- Keine öffentliche API exponiert ANTLR-Typen.

---

## 19. C-ABI-Modularisierung

### 19.1 `HandleRegistry<T>`

Eine generische interne Registry ist zulässig:

```cpp
template<class T>
class HandleRegistry final {
public:
    std::uint32_t insert(std::shared_ptr<T> value);
    std::shared_ptr<T> get(std::uint32_t handle) const;
    bool erase(std::uint32_t handle);
    void clear();

private:
    mutable std::mutex mutex_;
    std::map<std::uint32_t, std::shared_ptr<T>> values_;
    std::uint32_t next_ = 1;
};
```

Für Resultstrings kann ein spezialisierter `ResultRegistry` sinnvoller sein.

### 19.2 `SessionRegistry`

```cpp
class SessionRegistry final {
public:
    std::uint32_t create();
    std::shared_ptr<CompilerSession> get(
        std::uint32_t handle
    ) const;
    bool destroy(std::uint32_t handle);
};
```

### 19.3 `ResultRegistry`

```cpp
class ResultRegistry final {
public:
    std::uint32_t store(json::Value value);
    ResultView view(std::uint32_t handle) const;
    bool destroy(std::uint32_t handle);
};
```

Die Speicher- und Threadsemantik der bestehenden C-ABI bleibt unverändert.

### 19.4 `RequestDecoder`

```cpp
class RequestDecoder final {
public:
    CompilationRequest compilation(
        const json::Value& input
    ) const;

    FormatRequest formatting(
        const json::Value& input
    ) const;
};
```

- keine Resultatserialisierung;
- keine Sessionregistry;
- kontrollierte Requestfehler;
- bestehende Fehlermeldungen und Codes bewahren.

### 19.5 JSON-Projectors

Jeder Resultattyp erhält einen eindeutigen Projector:

```cpp
json::Value diagnosticToJson(const Diagnostic&);
json::Value compilationToJson(const CompilationResult&);
json::Value syntaxToJson(const SyntaxSnapshot&);
json::Value editorToJson(const EditorSnapshot&);
json::Value semanticToJson(const SemanticSnapshot&);
json::Value incrementalStatsToJson(const IncrementalStats&);
json::Value incrementalTraceToJson(const IncrementalTrace&);
json::Value cacheSnapshotToJson(const IncrementalCacheSnapshot&);
json::Value formattingToJson(const FormatResult&);
```

### 19.6 `Capi.cpp`

Nach P7 enthält `Capi.cpp` primär:

- `extern "C"`-Funktionen;
- Parametergrenzen;
- Registrylookup;
- Delegation;
- zentralen Exceptionguard.

Empfohlener Guard:

```cpp
template<class Operation>
std::uint32_t capiJsonOperation(
    const char* kind,
    Operation&& operation
) noexcept;
```

Die konkrete Form muss C-ABI-kompatibel sein.

---

## 20. WASM-JavaScript-Modularisierung

Empfohlene Struktur:

```text
packages/compiler-wasm/
    index.js
    compiler.js
    compiler-session.js
    wasm-memory.js
    wasm-results.js
    capabilities.js
    legacy-editor-projection.js
    index.d.ts
```

### 20.1 `wasm-memory.js`

```js
export function copyIntoWasm(module, value) {}
export function withWasmBytes(module, value, callback) {}
export function utf8ByteLength(value) {}
```

Verantwortung:

- Allocation;
- Copy;
- Free;
- keine Compilersemantik.

### 20.2 `wasm-results.js`

```js
export function readResultJson(module, handle) {}
export function callJson(module, operation, session, request) {}
```

Verantwortung:

- Resultathandle;
- JSON;
- sichere Freigabe;
- keine Capabilityentscheidung.

### 20.3 `capabilities.js`

```js
export function detectCompilerCapabilities(module) {}
```

Ein einziger Ort für Capability Detection.

### 20.4 `CompilerSession`

`compiler-session.js` enthält nur Sessionoperationen.

Bestehende Methoden bleiben unverändert.

### 20.5 Legacy-Editorprojektion

Die Legacyprojektion muss aus `index.js` entfernt und nach:

```text
legacy-editor-projection.js
```

verschoben werden.

Sie bleibt nur dann produktiv erreichbar, wenn die bestehende Kompatibilitätsoption weiterhin benötigt wird.

Regeln:

- klar `@deprecated` dokumentieren;
- Capabilityfehler bleibt verständlich;
- keine neue Consumerverwendung;
- Guard verhindert Imports ausserhalb des Wrappers und der Legacytests;
- Entfernung erst in einer separaten Major-Version.

### 20.6 `index.js`

Nach P7 ist `index.js` eine kleine Exportdatei.

---

## 21. CMake- und Include-Grenzen

### 21.1 Targets

P7 soll nicht für jede kleine Klasse ein neues Librarytarget erzeugen.

Mindestens bleiben klar:

```text
ilic-core
ilic-json
ilic-capi
ilic-repository-core
ilic-repository
```

Interne Dateien können im bestehenden Target liegen.

### 21.2 Include-Regeln

- Öffentliche Header inkludieren keine internen Header.
- Interne Snapshotkomponenten inkludieren keine ABI-Dateien.
- ABI-Projectors inkludieren öffentliche Resultattypen und `Json.h`, nicht Compilerinternas.
- Incremental-Komponenten kennen keine C-ABI.
- Repository-Core kennt keine CompilerSession.
- `Compiler.cpp` darf keine generierten Parserheader mehr direkt inkludieren.
- `Capi.cpp` darf keine semantischen Traversierungsdetails enthalten.

### 21.3 Forward Declarations

Forward Declarations sind dort zu verwenden, wo sie Ownership und Compilegrenzen klarer machen. Sie dürfen nicht zu unvollständigen Typtricks führen.

---

## 22. Zielarchitektur `interlis-language-tools`

Empfohlene interne Struktur:

```text
packages/language-service/src/
    service.ts
    source/
        source-registry.ts
        effective-source-resolver.ts
    syntax/
        syntax-snapshot-store.ts
    editor/
        editor-analysis-controller.ts
        editor-snapshot-store.ts
        live-diagnostic-controller.ts
    compilation/
        compilation-scheduler.ts
        compilation-run-coordinator.ts
        compilation-request.ts
    semantic/
        semantic-snapshot-store.ts
        semantic-freshness.ts
    diagnostics/
        diagnostic-store.ts
        diagnostic-merger.ts
        diagnostic-version-gate.ts
    repository/
        repository-model-controller.ts
        repository-catalog-controller.ts
    events/
        language-service-event-hub.ts
    workers/
        worker-protocol.ts
        worker-rpc-client.ts
        worker-source-mirror.ts
        worker-lifecycle-tracker.ts
        compiler-worker-backend.ts
        editor-worker-backend.ts
        compiler-worker-host.ts
```

Bestehende Dateien wie `features.ts`, `completion.ts` und `live-analysis.ts` müssen nicht zwangsläufig weiter geteilt werden, sofern ihre Verantwortung bereits klar ist.

---

## 23. `LanguageService` als Fassade

### 23.1 Öffentliche API

Alle bestehenden öffentlichen Methoden und Properties bleiben erhalten.

`LanguageService` delegiert intern.

### 23.2 Erlaubte Verantwortung

- Komponenten konstruieren;
- Lifecycle koordinieren;
- öffentliche Methoden an Komponenten delegieren;
- wenige Cross-Component-Workflows explizit orchestrieren;
- Dispose auslösen.

### 23.3 Verbotene Verantwortung

Nach P7 soll `LanguageService` nicht direkt besitzen:

- alle Source-Layer-Maps;
- alle Diagnostic-Maps;
- alle Semantic-Maps;
- Live-Timer;
- Pending-Compilation-Queue;
- Repositorykatalog;
- Reverse Dependencies;
- sämtliche Eventlistener;
- sämtliche Generationen und Run-IDs.

### 23.4 Zielgrösse

`service.ts` soll auf eine nachvollziehbare Fassade und wenige öffentliche Featuredelegationen reduziert werden.

Richtwert:

```text
<= 500 nicht leere Zeilen
```

Abweichungen müssen im Abschlussbericht begründet werden.

---

## 24. `SourceRegistry`

### 24.1 Verantwortung

Besitzt:

- offene Dokumente;
- Workspacequellen;
- Repositoryquellen;
- effektive Quellen;
- Read-only-URIs;
- entfernte URIs;
- Sourceversionen;
- Sourcegeneration.

### 24.2 API

```ts
export class SourceRegistry {
  openDocument(
    uri: string,
    text: string,
    version: number,
  ): SourceChange;

  changeDocument(
    uri: string,
    text: string,
    version: number,
  ): SourceChange;

  markSaved(uri: string): SourceChange | null;
  closeDocument(uri: string): SourceChange | null;

  replaceWorkspaceSources(
    sources: readonly WorkspaceSource[],
  ): readonly SourceChange[];

  putWorkspaceSource(
    uri: string,
    text: string,
    version?: number,
  ): SourceChange;

  removeWorkspaceSource(uri: string): SourceChange | null;

  putRepositorySource(
    model: ResolvedRepositoryModel,
  ): SourceChange;

  clearRepositorySources(): readonly SourceChange[];

  effective(uri: string): EffectiveSource | undefined;
  document(uri: string): OpenDocument | undefined;
  documents(): readonly OpenDocument[];
  isReadOnly(uri: string): boolean;
  generation(): number;
}
```

### 24.3 `SourceChange`

```ts
export interface SourceChange {
  readonly uri: string;
  readonly kind:
    | "added"
    | "updated"
    | "version-only"
    | "removed"
    | "shadowed"
    | "revealed"
    | "unchanged";
  readonly previous?: EffectiveSource;
  readonly current?: EffectiveSource;
  readonly generation: number;
}
```

Die Begriffe sind intern und dürfen dem Projektstil angepasst werden.

### 24.4 Regeln

- Priorität offenes Dokument > Workspace > Repository bleibt unverändert.
- Read-only wird zentral bestimmt.
- Registry ruft keine Compileroperation selbst auf.
- Registry publiziert SourceChanges.
- Language-Service-Orchestrierung synchronisiert diese Änderungen zu Backends.

---

## 25. `SyntaxSnapshotStore`

```ts
export class SyntaxSnapshotStore {
  get(
    uri: string,
    sourceVersion: number,
  ): VersionedResult<SyntaxSnapshot> | null;

  put(
    uri: string,
    value: VersionedResult<SyntaxSnapshot>,
  ): void;

  invalidate(uri: string): void;
  clear(): void;
}
```

Die Storeklasse entscheidet nicht selbst, wann geparst wird.

---

## 26. `EditorAnalysisController`

### 26.1 Verantwortung

Besitzt:

- Live-Timer;
- Request-IDs;
- Status;
- On-demand-Requests;
- EditorSnapshotStore;
- Live-Diagnostics und Live-Fixes, soweit nicht bereits P6-separiert;
- Timeout;
- EditorWorkerbackend.

### 26.2 API

```ts
export class EditorAnalysisController {
  schedule(
    uri: string,
    text: string,
    version: number,
  ): void;

  analyzeNow(
    uri: string,
    text: string,
    version: number,
  ): Promise<EditorSnapshot | null>;

  snapshot(
    uri: string,
    version: number,
  ): VersionedResult<EditorSnapshot> | null;

  status(uri: string): LiveAnalysisStatus;
  cancel(uri: string): void;
  remove(uri: string): void;
  configure(mode: "off" | "conservative"): void;
  dispose(): void;
}
```

### 26.3 Regeln

- Timer werden zentral freigegeben.
- Überholte Resultate werden verworfen.
- Completion kann bestehenden last-current EditorSnapshot verwenden, soweit aktueller Vertrag dies erlaubt.
- Keine SemanticSnapshot-Speicherung.
- Keine Repositoryauflösung.

---

## 27. `SemanticSnapshotStore`

### 27.1 Verantwortung

Besitzt:

- current per root;
- last good per root;
- saved per root;
- last selected root;
- Root-Freshness;
- Reverse Dependencies, sofern diese fachlich semantischer Snapshotzustand sind;
- Sticky Outlines, falls diese nicht in eine eigene UI-nahe Komponente gehören.

### 27.2 API

```ts
export class SemanticSnapshotStore {
  accept(
    rootUri: string,
    result: VersionedResult<SemanticSnapshot>,
    options: {
      readonly saved: boolean;
      readonly successful: boolean;
    },
  ): SemanticAcceptance;

  current(rootUri: string): VersionedResult<SemanticSnapshot> | null;
  lastGood(rootUri: string): VersionedResult<SemanticSnapshot> | null;
  saved(rootUri: string): VersionedResult<SemanticSnapshot> | null;

  forDocument(uri: string): VersionedResult<SemanticSnapshot> | null;
  completionForDocument(
    uri: string,
  ): VersionedResult<SemanticSnapshot> | null;

  invalidateBySource(uri: string): readonly string[];
  clear(): void;
}
```

### 27.3 Regeln

- Last-Good-Verhalten bleibt identisch.
- Saved und Current werden nicht vermischt.
- Store startet keine Kompilation.
- Store publiziert keine LSP-Events direkt.

---

## 28. `CompilationScheduler`

### 28.1 Verantwortung

Besitzt:

- Pending-Compilation-Queue;
- Active-Flag;
- Run-IDs;
- Compilation-Epoch;
- aktuelle Request-ID pro Root;
- Triggerpriorität;
- Coalescing;
- Promiseauflösung.

### 28.2 API

```ts
export class CompilationScheduler {
  enqueue(request: ScheduledCompilation): Promise<CompilationEvent>;
  cancelRoot(rootUri: string): void;
  invalidateAll(): void;
  dispose(): void;

  private pump(): Promise<void>;
  private coalesce(
    pending: readonly PendingCompilation[],
  ): readonly PendingCompilation[];
}
```

### 28.3 `ScheduledCompilation`

```ts
export interface ScheduledCompilation {
  readonly rootUri: string;
  readonly trigger: CompilationTrigger;
  readonly requestedDocumentVersion: number;
  readonly requestedSourceVersion?: number;
  readonly compilationEpoch: number;
}
```

### 28.4 Regeln

- `manual` und `save` dürfen nicht verloren gehen.
- überholte Background-Requests dürfen zusammengefasst werden.
- Scheduler kennt keine Repositorydetails.
- Tatsächliche Ausführung erfolgt über `CompilationRunCoordinator`.
- Jede Pending-Promise wird genau einmal resolved oder rejected.
- Dispose verwirft keine Promise still.

---

## 29. `CompilationRunCoordinator`

### 29.1 Verantwortung

Kapselt den fachlichen Hostworkflow:

```text
compile
-> Missing Models erkennen
-> Repositorymodelle laden
-> SourceRegistry aktualisieren
-> gleiche Session weiterverwenden
-> erneut kompilieren
-> Ergebnis akzeptieren oder als stale verwerfen
```

### 29.2 API

```ts
export class CompilationRunCoordinator {
  constructor(
    compiler: CompilerBackend,
    sources: SourceRegistry,
    repository: RepositoryModelController,
    semantics: SemanticSnapshotStore,
    diagnostics: DiagnosticCoordinator,
    events: LanguageServiceEventHub,
  );

  run(
    request: ScheduledCompilation,
  ): Promise<CompilationEvent>;

  dispose(): void;
}
```

### 29.3 Regeln

- Kein Worker-Restart bei normalem Missing-Model-Nachladen.
- Kein Full Source Replay.
- Compile-Outcome unverändert.
- Generation, Run-ID und Dokumentversion werden vor Akzeptanz geprüft.
- Outputevent und SemanticEvent bleiben kompatibel.

---

## 30. `RepositoryModelController`

Besitzt:

- ModelRepository;
- Katalog;
- Katalog-Promise;
- Repositorysources;
- Repositorysourceversionen;
- Read-only-Metadaten;
- Dispose der Repositoryinstanz.

API sinngemäss:

```ts
export class RepositoryModelController {
  setRepository(repository?: ModelRepository): Promise<RepositoryChange>;
  listModels(): Promise<readonly ModelCatalogEntry[]>;
  resolveMissing(
    names: readonly string[],
    language: RepositorySchemaLanguage,
  ): Promise<readonly ResolvedRepositoryModel[]>;
  document(uri: string): ResolvedRepositoryModel | undefined;
  dispose(): Promise<void>;
}
```

Keine Compilerworkersteuerung.

---

## 31. `LanguageServiceEventHub`

```ts
export class LanguageServiceEventHub {
  onAnalysis(listener: AnalysisListener): Disposable;
  onCompilation(listener: CompilationListener): Disposable;
  onDiagnostics(listener: DiagnosticsListener): Disposable;

  emitAnalysis(event: AnalysisEvent): void;
  emitCompilation(event: CompilationEvent): void;
  emitDiagnostics(event: DiagnosticsChangedEvent): void;

  clear(): void;
}
```

Regeln:

- Listenerfehler werden kontrolliert behandelt.
- Ein Listener darf die interne Setiteration nicht beschädigen.
- Dispose ist idempotent.
- Keine Fachzustände.

---

## 32. Workerarchitektur

`compiler-worker.ts` enthält derzeit mehrere parallele Verantwortungen. P7 trennt sie.

### 32.1 `worker-protocol.ts`

Enthält ausschliesslich:

- Requesttypen;
- Responsetypen;
- Commandunion;
- Runtimeguards, falls nötig.

### 32.2 `WorkerRpcClient`

```ts
export class WorkerRpcClient {
  constructor(factory: CompilerWorkerFactory);

  request<T>(command: CompilerWorkerCommand): Promise<T>;
  notify(command: CompilerWorkerNotification): void;

  onFailure(listener: (error: unknown) => void): Disposable;
  restart(): void;
  terminate(): void;
  pendingCount(): number;
}
```

Verantwortung:

- IDs;
- Pending Map;
- Port;
- Responses;
- Errorpropagation;
- kein Source Mirror;
- keine Fallbackentscheidung.

### 32.3 `WorkerSourceMirror`

```ts
export class WorkerSourceMirror {
  put(
    uri: string,
    source: string | Uint8Array,
    version: number,
  ): void;

  remove(uri: string): void;
  replayTo(client: WorkerRpcClient): WorkerReplayResult;
  clear(): void;
  snapshot(): readonly MirroredSource[];
}
```

### 32.4 `WorkerLifecycleTracker`

```ts
export class WorkerLifecycleTracker {
  started(): void;
  restarted(reason: WorkerRestartReason): void;
  replayed(batch: WorkerReplayResult): void;
  fallbackExecuted(): void;
  queueSize(value: number): void;
  snapshot(): WorkerLifecycleStats;
}
```

### 32.5 `CompilerWorkerBackend`

Kombiniert:

- RPC;
- Mirror;
- Lifecycle;
- Local Fallback;
- CompilerBackend-Fassade.

### 32.6 `EditorWorkerBackend`

Verwendet dieselben RPC-/Mirror-/Lifecycle-Komponenten, aber einen eigenen fachlichen Adapter.

### 32.7 `CompilerWorkerHost`

```ts
export async function runCompilerWorker(
  endpoint: WorkerEndpoint,
): Promise<void>;
```

Der Host delegiert pro Command an eine kleine Dispatchklasse oder Map.

### 32.8 Regeln

- Keine duplizierten Attach-/Detach-/Replay-Funktionen.
- Compiler- und Editorworker dürfen unterschiedliche Recoverypolitik besitzen, diese muss aber explizit konfiguriert sein.
- Normaler Sourceupdate löst keinen Restart aus.
- Fallback zählt genau einmal.
- Pending Requests werden genau einmal abgewiesen.
- Queue-Metrik ist korrekt.
- Protokoll bleibt kompatibel.

---

## 33. Zielarchitektur `interlis-web-ide`

Empfohlene Struktur:

```text
src/workbench/
    workbench.ts
    workbench-context.ts
    command-registry.ts
    view/
        workbench-view.ts
        workbench-elements.ts
    controllers/
        tab-controller.ts
        editor-controller.ts
        workspace-controller.ts
        save-controller.ts
        recovery-controller.ts
        compilation-controller.ts
        problems-controller.ts
        outline-controller.ts
        diagram-panel-controller.ts
        layout-controller.ts
        import-controller.ts
        source-control-controller.ts
    state/
        workbench-state.ts
        workbench-settings.ts
```

Bereits vorhandene Module sind zu verwenden und nicht parallel neu zu erstellen.

---

## 34. `WebIdeWorkbench` als Fassade

### 34.1 Öffentliche Rolle

`WebIdeWorkbench` bleibt der öffentliche Einstieg.

Es darf:

- Context und Controller konstruieren;
- Initialisierung koordinieren;
- öffentliche Kommandos delegieren;
- Controller entsorgen;
- wenige Cross-Controller-Workflows explizit koordinieren.

Es soll nicht mehr direkt besitzen:

- sämtliche Tabs;
- Recoverytimer;
- Autosavetimer;
- Diagrammzustand;
- Outline-DOM;
- Workspaceimportlogik;
- Source-Control-Rendering;
- Problems-Rendering;
- alle DOM-Listener.

### 34.2 Zielgrösse

Richtwert:

```text
<= 650 nicht leere Zeilen
```

Eine begründete Abweichung ist zulässig, wenn die Fassade klar strukturiert bleibt.

---

## 35. `WorkbenchContext`

Eine kleine gemeinsame Contextstruktur ist zulässig:

```ts
export interface WorkbenchContext {
  readonly host: HTMLElement;
  readonly manager: WorkspaceManager;
  readonly languageService: LanguageService;
  readonly languageAdapter: MonacoLanguageAdapter;
  readonly sourceSynchronizer: WorkspaceSourceSynchronizer;
  readonly view: WorkbenchView;
}
```

Keine mutable Service-Locator-Klasse.

Controller erhalten nur die benötigten Ports, nicht automatisch den gesamten Context.

---

## 36. `TabController`

Besitzt:

- Tab Map;
- active path;
- primären und sekundären Editorbezug, soweit tabbezogen;
- Model Lifecycle;
- Language Adapter Disposables;
- Read-only;
- Dirty-Status.

API:

```ts
export class TabController {
  open(path: string, options?: OpenTabOptions): Promise<OpenTab>;
  close(path: string): Promise<void>;
  activate(path: string): void;
  split(path?: string): void;
  tabs(): readonly OpenTabView[];
  active(): OpenTabView | null;
  byPath(path: string): OpenTabView | null;
  dispose(): void;
}
```

Timer gehören nicht in `OpenTab`, wenn sie Save/Recovery betreffen; sie sollen in entsprechenden Controllern liegen.

---

## 37. `WorkspaceController`

Besitzt:

- aktives `WorkspaceFileSystem`;
- Workspacewechsel;
- Workspaceerstellung/-umbenennung/-löschung;
- Initialinhalt;
- Sidebar-Refreshsignal;
- SourceSynchronizer-Full-Refresh;
- Workspacegeneration.

API:

```ts
export class WorkspaceController {
  initialize(): Promise<void>;
  switchTo(id: string): Promise<void>;
  create(name: string): Promise<WorkspaceDescriptor>;
  rename(id: string, name: string): Promise<void>;
  remove(id: string): Promise<void>;

  fileSystem(): WorkspaceFileSystem;
  descriptor(): WorkspaceDescriptor;
  generation(): number;

  fullSourceRefresh(
    reason: WorkspaceFullSyncReason,
  ): Promise<WorkspaceSyncResult>;

  dispose(): void;
}
```

Bekannte einzelne Dateioperationen werden nicht als Full Refresh ausgeführt.

---

## 38. `SaveController`

Besitzt:

- Auto-Save-Timer;
- Save-Lifecycle;
- Mark Saved;
- direkte SourceSynchronizer-Puts;
- optional Compile-after-save.

API:

```ts
export class SaveController {
  scheduleAutoSave(tab: OpenTabView): void;
  cancelAutoSave(path: string): void;
  save(
    tab: OpenTabView,
    options: { readonly compile: boolean },
  ): Promise<void>;
  saveAll(options?: SaveAllOptions): Promise<void>;
  dispose(): void;
}
```

Regeln:

- Timer pro Pfad zentral.
- Save schreibt genau einmal.
- SourceSynchronizer erhält aktuellen Text und Version.
- Compiletrigger bleibt unverändert.
- Read-only kann nicht gespeichert werden.
- Fehler werden kontrolliert an Workbench Error Sink gemeldet.

---

## 39. `RecoveryController`

Besitzt:

- BufferRecoveryStore;
- Recoverytimer;
- Recoveryrestore;
- Cleanup nach Save/Close;
- Workspacewechsel.

```ts
export class RecoveryController {
  attach(workspace: WorkspaceFileSystem): void;
  schedule(tab: OpenTabView): void;
  restore(): Promise<readonly RecoveredBuffer[]>;
  clear(path: string): Promise<void>;
  dispose(): void;
}
```

Save- und Recoverytimer dürfen nicht in derselben Map vermischt werden.

---

## 40. `CompilationController`

Besitzt:

- manuellen Compile;
- Save-Compile;
- CompilationEvent-Verarbeitung;
- Output;
- Status;
- Problemsweitergabe;
- Diagrammbenachrichtigung;
- Generation.

```ts
export class CompilationController {
  compileActive(
    trigger: CompilationTrigger,
  ): Promise<void>;

  handleCompilation(event: CompilationEvent): void;
  status(): CompilationUiState;
  dispose(): void;
}
```

Keine direkte Diagrammlayoutimplementierung.

---

## 41. `ProblemsController`

Setzt P6 fort und besitzt:

- ProblemStore;
- Rendering;
- Zähler;
- Navigation;
- Related Information;
- Versionsfilter.

```ts
export class ProblemsController {
  setDiagnostics(event: DiagnosticsChangedEvent): void;
  clearUri(uri: string): void;
  clearAll(): void;
  render(): void;
  navigate(problemId: string): Promise<void>;
  navigateRelated(
    problemId: string,
    relatedIndex: number,
  ): Promise<void>;
  dispose(): void;
}
```

Keine Rückinterpretation von Meldungstext.

---

## 42. `OutlineController`

Besitzt:

- Outlinegeneration;
- Debounce;
- Collapsed Set;
- DOM-Reihen;
- Navigation;
- Sticky Outline-Vertrag.

```ts
export class OutlineController {
  schedule(uri: string): void;
  refresh(uri: string): Promise<void>;
  toggle(symbolId: string): void;
  navigate(symbolId: string): void;
  clear(): void;
  dispose(): void;
}
```

---

## 43. `DiagramPanelController`

Besitzt:

- `DiagramController`;
- Diagrammsettings;
- Layout;
- Viewport;
- SVG;
- Zoom;
- Pan;
- Interaktions-AbortController;
- Last-Good-Darstellung;
- Export.

```ts
export class DiagramPanelController {
  show(options?: { readonly relayout?: boolean }): Promise<void>;
  hide(): void;
  refresh(): Promise<void>;
  exportSvg(): Promise<void>;
  exportDocx(): Promise<void>;

  handleSemanticSnapshot(event: AnalysisEvent): void;
  handleCompilation(event: CompilationEvent): void;

  zoomIn(): void;
  zoomOut(): void;
  resetViewport(): void;
  dispose(): void;
}
```

Keine Tab-Map-Manipulation. Navigation erfolgt über einen Port.

---

## 44. `CommandRegistry`

```ts
export interface WorkbenchCommand {
  readonly id: string;
  readonly label: string;
  readonly run: () => void | Promise<void>;
  readonly enabled?: () => boolean;
}

export class CommandRegistry {
  register(command: WorkbenchCommand): Disposable;
  execute(id: string): Promise<void>;
  list(): readonly WorkbenchCommand[];
  dispose(): void;
}
```

Die bestehende Command-Palette bleibt unverändert.

---

## 45. `WorkbenchView`

Die View kapselt DOM-Abfragen und Renderinghilfen.

```ts
export class WorkbenchView {
  constructor(host: HTMLElement);

  elements(): WorkbenchElements;
  renderTabs(tabs: readonly OpenTabView[]): void;
  renderStatus(state: CompilationUiState): void;
  renderOutput(text: string): void;
  renderProblems(items: readonly ProblemItem[]): void;
  renderOutline(items: readonly OutlineItem[]): void;
  renderWorkspace(state: WorkspaceUiState): void;

  dispose(): void;
}
```

Regeln:

- Controller dürfen nicht wiederholt `querySelector` über den gesamten Host ausführen.
- DOM-Elemente werden einmal validiert.
- Fehlende Pflichtknoten erzeugen kontrollierten Initialisierungsfehler.
- View enthält keine Compilersemantik.
- Keine neue UI-Framework-Abhängigkeit.

---

## 46. Controllerkommunikation

Controller sollen über kleine Ports und Events kommunizieren.

Beispiel:

```ts
export interface EditorNavigationPort {
  openUri(uri: string): Promise<void>;
  reveal(uri: string, range: EditorRange): Promise<void>;
}

export interface OutputPort {
  setOutput(text: string): void;
  appendOutput(text: string): void;
}

export interface DiagramNavigationPort {
  revealSource(uri: string, range: EditorRange): Promise<void>;
}
```

Nicht zulässig:

- Controller greifen auf private Felder anderer Controller zu.
- zyklische Konstruktorabhängigkeiten.
- globaler Eventbus ohne typisierten Vertrag.
- DOM CustomEvents als interner Allzweckmechanismus.

---

## 47. Lifecycle und Dispose

Jede langlebige Komponente mit mindestens einem der folgenden Ressourcen benötigt `dispose()`:

- Timer;
- Worker;
- Listener;
- AbortController;
- Monaco Model;
- FileSystem Watcher;
- Promisequeue;
- Repository;
- DOM Listener.

### 47.1 Idempotenz

`dispose()` MUSS mehrfach aufrufbar sein.

### 47.2 Reihenfolge

Dokumentiere die Reihenfolge:

```text
stop accepting work
cancel timers
reject/cancel pending requests
detach listeners
dispose child components
release models/resources
clear stores
```

### 47.3 Tests

Für jede relevante Komponente:

- Dispose vor Init;
- Dispose nach Init;
- doppeltes Dispose;
- Pending Request;
- Timer;
- Workerfehler;
- Workspacewechsel;
- kein Event nach Dispose.

---

## 48. Error Boundaries

Refactoring darf keine Fehler verschlucken.

Definiere kleine Error Sinks:

```ts
export interface ErrorSink {
  report(error: unknown, context: string): void;
}
```

C++-seitig bleibt der bestehende kontrollierte Errorpfad.

Regeln:

- Keine leeren `catch`.
- Fallback wird protokolliert.
- Fehlerkontext nennt Komponente und Operation.
- Benutzerdiagnosen und Infrastrukturfehler bleiben getrennt.
- Workerfehler dürfen Compilerdiagnosen nicht simulieren.
- Web-IDE darf Last-Good erhalten, aber Fehler sichtbar machen.

---

## 49. Architekturmetriken und Ratchets

Metriken sind Hilfsmittel, kein Selbstzweck.

### 49.1 Zielwerte

Richtwerte nach P7:

| Datei/Fassade | Ziel |
|---|---:|
| `source/core/Compiler.cpp` | höchstens 300 nicht leere Zeilen |
| `source/core/SnapshotPipeline.cpp` | entfernt oder höchstens 180 Zeilen als Fassade |
| `source/abi/Capi.cpp` | höchstens 300 nicht leere Zeilen |
| `packages/compiler-wasm/index.js` | höchstens 100 nicht leere Zeilen |
| `language-service/src/service.ts` | höchstens 500 nicht leere Zeilen |
| bisheriges `compiler-worker.ts` | entfernt oder höchstens 200 Zeilen als Export-/Hostdatei |
| `workbench/workbench.ts` | höchstens 650 nicht leere Zeilen |

Abweichungen müssen fachlich begründet werden.

### 49.2 Funktionsgrösse

Neue oder extrahierte Funktionen sollen im Regelfall höchstens 80 nicht leere Zeilen besitzen.

Komplexe Parservisitoren dürfen abweichen, müssen aber eine einzige klar benannte Traversierungsverantwortung besitzen.

### 49.3 Klassenfelder

Eine neu extrahierte Klasse mit mehr als 15 veränderlichen Feldern ist ein Reviewwarnsignal.

### 49.4 Dependency Cycles

Zwischen den neuen Modulen sind keine zyklischen Imports beziehungsweise Includes zulässig.

### 49.5 Ratchet

CI vergleicht Architekturmetriken mit einer eingecheckten Baseline.

Erlaubt:

- kleinere oder gleiche Hotspotmetriken;
- neue klar begründete Komponente.

Nicht erlaubt:

- neue God-Class;
- erneutes Anwachsen der Fassaden über Budget ohne explizite dokumentierte Ausnahme;
- neuer Dependency Cycle.

---

## 50. Architektur-Guards

Mindestens folgende Guards implementieren:

1. `Compiler.cpp` inkludiert keine generierten Parserheader.
2. `Capi.cpp` enthält keine grossen JSON-Projectorimplementierungen.
3. Snapshot-Projectors inkludieren keine ABI-Dateien.
4. ABI-Module inkludieren keine ANTLR-Header.
5. Incremental-Module inkludieren keine C-ABI-Header.
6. Repository-Core inkludiert keine CompilerSession.
7. `index.js` enthält keine Legacyprojektion.
8. Language-Service-Komponenten besitzen keine zyklischen Imports.
9. Worker-RPC enthält keine Source-Replay-Policy.
10. Worker-Source-Mirror enthält keine RPC-Pending-Map.
11. `LanguageService` besitzt keine Pending-Compilation-Array-Implementierung.
12. `WebIdeWorkbench` besitzt keine Auto-Save- oder Recoverytimer.
13. Diagrammcontroller greift nicht auf Tab-Map-Interna zu.
14. Problems-Controller parst keine Meldungstexte.
15. Keine neue globale mutable Registry ausser kontrollierter C-ABI-Handle-Registry.
16. Keine Service-Locator-Klasse.
17. Keine direkte DOM-Abfrage aus Domänencontrollern ausser über View/Elements.
18. Keine Veränderung öffentlicher Package-Exports ohne Snapshotupdate und Begründung.
19. Keine Conformance-Baselineänderung.
20. Keine Behavior-Lock-Änderung.

Jeder Guard benötigt einen negativen Canary.

---

## 51. Tests auf Komponentenebene

### 51.1 Native

Mindestens:

```text
CompilerSessionStateTest
SourceUpdateCoordinatorTest
SnapshotServiceTest
CompilationOrchestratorTest
CompilationRunTest
CompilationTranscriptTest
ExternalMetaAttributeApplierTest
ModelCompilationSchedulerTest
LanguageDetectorTest
StrictSourceParserTest
TolerantEditorParserTest
SyntaxSnapshotProjectorTest
EditorSnapshotProjectorTest
SessionRegistryTest
ResultRegistryTest
RequestDecoderTest
ResultJsonProjectorTest
```

### 51.2 Language Tools

Mindestens:

```text
SourceRegistryTest
SyntaxSnapshotStoreTest
EditorAnalysisControllerTest
SemanticSnapshotStoreTest
CompilationSchedulerTest
CompilationRunCoordinatorTest
RepositoryModelControllerTest
LanguageServiceEventHubTest
WorkerRpcClientTest
WorkerSourceMirrorTest
WorkerLifecycleTrackerTest
CompilerWorkerBackendTest
EditorWorkerBackendTest
```

### 51.3 Web-IDE

Mindestens:

```text
TabControllerTest
WorkspaceControllerTest
SaveControllerTest
RecoveryControllerTest
CompilationControllerTest
ProblemsControllerTest
OutlineControllerTest
DiagramPanelControllerTest
CommandRegistryTest
WorkbenchViewTest
WorkbenchLifecycleTest
```

---

## 52. Behavior-Lock-Testmatrix

Für jede Refactoringphase mindestens:

- native Resultat-Snapshots;
- C-ABI JSON;
- WASM-Parität;
- Conformance;
- Diagnostics Decision Lock;
- Incremental Stats und Trace;
- Language-Service-Events;
- LSP;
- Monaco;
- Web-IDE E2E.

### 52.1 Native Kernfälle

- gültiges INTERLIS 1;
- gültiges INTERLIS 2.3;
- gültiges INTERLIS 2.4;
- Syntaxfehler;
- Missing Model;
- mehrere Roots;
- externe Metaattribute;
- `warningsAsErrors`;
- Cache Hit;
- Cache Miss;
- VersionOnly;
- ContentChanged;
- Repositorysource;
- Cancellation;
- bekannte Internal-Error-Baselinefälle.

### 52.2 Workerfälle

- normaler Put;
- normaler Remove;
- Compile;
- Editor Snapshot;
- Incremental Stats;
- Trace;
- Cache Snapshot;
- Reset;
- Worker Crash;
- Replay;
- Local Fallback;
- Dispose mit Pending Request.

### 52.3 Web-IDE-Fälle

- Startup;
- Open;
- Edit;
- Save;
- Auto-Save;
- Manual Compile;
- Delete;
- Rename;
- File Import;
- ZIP Import;
- Workspacewechsel;
- Recovery;
- Problems;
- Outline;
- Diagramm;
- Last Good;
- PWA Reload;
- Worker Fallback.

---

## 53. Performance- und Speicherlock

P7 ist kein Performanceprojekt, darf aber keine deutliche Regression verursachen.

### 53.1 Arbeitszähler

Unverändert bleiben für gleiche Sequenzen:

- Parser Builds/Hits;
- Root Builds/Hits;
- Compile Requests/Executions;
- Worker Restarts;
- Replay Batches;
- Replayed Sources;
- Full Workspace Scans;
- Direct Source Puts/Removes/Renames.

### 53.2 Laufzeit

Erfasse Medianwerte vor/nach P7 für:

- Cold Compile;
- Root Cache Hit;
- Editor Snapshot;
- Worker Compile;
- Web-IDE Save+Compile;
- Diagrammrefresh.

Keine engen absoluten CI-Gates. Eine deutliche Regression von mehr als etwa 15 Prozent muss untersucht und dokumentiert werden.

### 53.3 Bundlegrösse

Prüfe:

- WASM JS Wrapper;
- Language-Tools-Packages;
- Web-IDE Production Bundle;
- PWA Assets.

Refactoring darf keine grosse neue Runtime-Abhängigkeit einführen.

---

## 54. Dokumentation und ADRs

Mindestens erstellen oder aktualisieren:

```text
ilic-fork/docs/architecture/compiler-session.md
ilic-fork/docs/architecture/compilation-run.md
ilic-fork/docs/architecture/snapshot-pipeline.md
ilic-fork/docs/architecture/c-api.md
ilic-fork/docs/architecture/module-dependencies.md

interlis-language-tools/docs/architecture/language-service.md
interlis-language-tools/docs/architecture/workers.md
interlis-language-tools/docs/architecture/module-dependencies.md

interlis-web-ide/docs/architecture/workbench.md
interlis-web-ide/docs/architecture/controllers.md
interlis-web-ide/docs/architecture/module-dependencies.md
```

### 54.1 C4-/Mermaid-Diagramme

Dokumentiere:

- Container-/Packageübersicht;
- Komponenten;
- Sourceupdate;
- Compile;
- Workerrecovery;
- Web-IDE Save;
- Diagnosefluss;
- Dispose.

### 54.2 „Where do I change …?“

Jedes Projekt erhält eine kurze Tabelle:

| Änderung | Zuständige Komponente |
|---|---|
| Sourceupdate | `SourceUpdateCoordinator` |
| Compilerlauf | `CompilationRun` |
| Snapshotprojektion | `SyntaxSnapshotProjector` / `EditorSnapshotProjector` |
| JSON-Vertrag | `source/abi/*Json` |
| Worker-Replay | `WorkerSourceMirror` |
| Compilequeue | `CompilationScheduler` |
| Web-IDE Save | `SaveController` |
| Diagramm | `DiagramPanelController` |

### 54.3 Ownershiptabelle

Für alle langfristigen Stores und Controller dokumentieren:

- Eigentümer;
- Lebensdauer;
- Thread/Worker;
- Schreibzugriffe;
- Dispose.

---

## 55. Migrationsstrategie

P7 muss in kleinen grünen Schritten umgesetzt werden.

### P7.0 – Inventar und Baseline

- Gitstatus;
- Commits;
- Toolchain;
- Dateimetriken;
- Dependencygraph;
- API-/ABI-/Package-Snapshots;
- Behavior Lock;
- vollständige Tests.

### P7.1 – Native Utilities aus `Compiler.cpp`

- Clock;
- Transcript;
- ExternalMetaAttributeApplier;
- SourceMaterializer;
- Tests.

### P7.2 – Native Compile Run

- ModelCompilationScheduler;
- CompilationRun;
- CompileRun-Verhalten;
- Snapshots;
- Conformance.

### P7.3 – Native Session Services

- CompilerSessionState;
- SourceUpdateCoordinator;
- SnapshotService;
- CompilationOrchestrator;
- Compiler.cpp als Fassade.

### P7.4 – Snapshotpipeline

- LanguageDetector;
- RecoveryTokenStream;
- Parserfactory;
- Projectors;
- Sprachprojectors;
- SnapshotPipeline-Fassade entfernen oder verkleinern.

### P7.5 – C-ABI

- Registries;
- RequestDecoder;
- JSON-Projectors;
- dünne Capi-Fassade;
- ABI-Snapshots.

### P7.6 – WASM Wrapper

- Memory;
- Resultate;
- Capabilities;
- Session;
- Legacyprojektion;
- Exports.

### P7.7 – Language-Service Source und Stores

- SourceRegistry;
- SyntaxStore;
- SemanticStore;
- Diagnosticstoreintegration;
- Behavior Lock.

### P7.8 – Language-Service Controller

- EditorAnalysisController;
- RepositoryModelController;
- EventHub;
- Completion/Navigationdelegation.

### P7.9 – Compilation Scheduler

- Queue;
- Run Coordinator;
- Missing Models;
- Events;
- Save-driven Lock.

### P7.10 – Worker

- Protocol;
- RPC;
- Mirror;
- Lifecycle;
- Backends;
- Host.

### P7.11 – Web-IDE Basis

- View;
- CommandRegistry;
- TabController;
- WorkspaceController.

### P7.12 – Web-IDE Lifecycle

- Save;
- Recovery;
- Import;
- Source Sync;
- Tests.

### P7.13 – Web-IDE Panels

- Compilation;
- Problems;
- Outline;
- Diagramm;
- Source Control.

### P7.14 – Guards, Dokumentation und Abnahme

- Architekturmetriken;
- Guards/Canaries;
- vollständige Cross-Repo-Pipeline;
- Final Report.

Nach jeder Phase:

1. fokussierte Tests;
2. Behavior Lock;
3. `diff --check`;
4. keine unkontrollierten API-Änderungen.

---

## 56. Verbotene Abkürzungen

Nicht zulässig:

- Dateien nur nach Zeilenzahl zu zerschneiden.
- Klassen mit `Manager`, `Helper` oder `Utils` ohne klare Verantwortung zu erzeugen.
- jede Klasse hinter ein Interface zu stellen.
- eine globale Service Registry einzuführen.
- bestehende Logik zu kopieren und später „aufräumen“ zu wollen.
- parallele alte und neue Implementierungen dauerhaft aktiv zu lassen.
- Behavior Locks zu lockern.
- Snapshots neu zu generieren, um Drift zu kaschieren.
- Conformance-Baseline zu ändern.
- öffentliche APIs umzubenennen.
- Workerprotokoll ohne Kompatibilität zu ändern.
- neuen UI-Framework einzuführen.
- neue Runtime-Abhängigkeit nur für Eventbus oder DI.
- `any` als Architekturbrücke in TypeScript zu verwenden.
- `void*` oder untypisierte Maps als Architekturbrücke in C++ zu verwenden.
- gegenseitige Friend-Klassenketten.
- Controllerzugriff auf private Interna anderer Controller.
- versteckte globale Timer.
- leere Catch-Blöcke.
- Dispose zu vergessen.
- Legacyprojektion aus Kompatibilitätsgründen in `index.js` zu belassen.
- Testabdeckung nur über End-to-End-Tests zu liefern.
- P7 als abgeschlossen zu bezeichnen, solange God-Class-Verantwortungen nur verschoben, aber nicht geklärt sind.

---

## 57. Baseline- und Verifikationsbefehle

Der Agent muss reale Skripte und Targetnamen verwenden. Folgende Befehle sind Richtwerte.

### 57.1 Native Release

```bash
cmake -S . -B build/p7-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON

cmake --build build/p7-release --parallel
ctest --test-dir build/p7-release --output-on-failure
```

### 57.2 Native Debug

```bash
cmake -S . -B build/p7-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON

cmake --build build/p7-debug --parallel
ctest --test-dir build/p7-debug --output-on-failure
```

### 57.3 Repository deaktiviert

```bash
cmake -S . -B build/p7-no-repository -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF

cmake --build build/p7-no-repository --parallel
ctest --test-dir build/p7-no-repository --output-on-failure
```

### 57.4 Sanitizer

```bash
cmake -S . -B build/p7-sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON

cmake --build build/p7-sanitizers --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/p7-sanitizers --output-on-failure -j2
```

### 57.5 TSan

```bash
cmake -S . -B build/p7-tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_THREAD_SANITIZER=ON

cmake --build build/p7-tsan --parallel

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p7-tsan \
  -R "Compiler|Incremental|Diagnostic|Capi|Snapshot" \
  --output-on-failure
```

### 57.6 WASM und npm

```bash
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
node --test test/npm/*.mjs
```

### 57.7 Language Tools

```bash
cd ../interlis-language-tools

corepack pnpm install --frozen-lockfile
corepack pnpm build
corepack pnpm lint
corepack pnpm typecheck
corepack pnpm test
corepack pnpm test:snapshot
corepack pnpm pack:verify
```

### 57.8 Web-IDE

```bash
cd ../interlis-web-ide

corepack pnpm install \
  --no-frozen-lockfile \
  --force \
  --update-checksums

corepack pnpm check
corepack pnpm e2e
corepack pnpm build
```

### 57.9 Abschluss

```bash
git -C ../ilic-fork diff --check
git -C ../interlis-language-tools diff --check
git -C ../interlis-web-ide diff --check

git -C ../ilic-fork status --short
git -C ../interlis-language-tools status --short
git -C ../interlis-web-ide status --short
```

---

## 58. Umgang mit Fehlern

Der Agent stoppt nicht beim ersten Fehler.

### 58.1 Behavior Drift

1. kleinsten reproduzierbaren Fall bestimmen;
2. Baseline und neuen Output sichern;
3. fachliche und reine Strukturänderung trennen;
4. Call Graph vergleichen;
5. Ownership-/Lebensdauerfehler prüfen;
6. Projektion und Reihenfolge prüfen;
7. Drift korrigieren;
8. fokussierte Tests;
9. vollständiger Behavior Lock;
10. Conformance.

### 58.2 Dependency Cycle

1. Cycle graphisch darstellen;
2. falsche Verantwortungsrichtung identifizieren;
3. gemeinsamen Datentyp in neutrales Modul verschieben;
4. keinen Service Locator einführen;
5. Import Guard ausführen.

### 58.3 Dispose-/Lifecyclefehler

1. Ressource bestimmen;
2. Eigentümer bestimmen;
3. Init-/Dispose-Reihenfolge dokumentieren;
4. Pending Requests und Timer prüfen;
5. idempotenten Test ergänzen;
6. ASan/TSan ausführen.

### 58.4 Workerfehler

1. RPC, Mirror, Lifecycle und Fallback getrennt prüfen;
2. Pending Map prüfen;
3. Replay zählen;
4. Source Mirror prüfen;
5. Doppelauflösung von Promises verhindern;
6. fokussierte Worker-Tests;
7. Language-Service-Integration;
8. Web-IDE-Fallback.

### 58.5 Web-IDE-Fehler

1. zuständigen Controller bestimmen;
2. State Owner prüfen;
3. Generation prüfen;
4. Listener und Timer prüfen;
5. Viewprojektion prüfen;
6. Last Good und Problems prüfen;
7. fokussierter E2E-Test;
8. vollständige relevante E2E-Suite.

---

## 59. Echte Blocker

Wenn ein externer Blocker vollständige Umsetzung verhindert:

1. alle unabhängigen Teile implementieren;
2. Repositories konsistent hinterlassen;
3. keine parallele halbfertige Produktionsimplementierung aktiv lassen;
4. offenen Punkt einzeln nennen;
5. Repository, Datei, Klasse und Methode nennen;
6. technischen Grund nennen;
7. fehlgeschlagenen Befehl nennen;
8. nächsten Schritt nennen;
9. P7 nicht als abgeschlossen bezeichnen.

Eine grosse Refactoringmenge oder lange Testlaufzeit ist kein externer Blocker.

---

## 60. Definition of Done

P7 ist nur abgeschlossen, wenn alle folgenden Aussagen belegt sind:

- `CompilerSession` ist eine kleine öffentliche Fassade.
- Langlebiger nativer Sessionzustand besitzt einen klaren Eigentümer.
- Sourceupdate ist aus `Compiler.cpp` extrahiert.
- Snapshotzugriff ist aus `Compiler.cpp` extrahiert.
- Compileorchestrierung ist aus `Compiler.cpp` extrahiert.
- Request-lokaler Compilezustand lebt in `CompilationRun`.
- Transcript besitzt eine eigene Komponente und testbare Clock.
- externe Metaattribute besitzen eine eigene Komponente.
- Modellkompilierreihenfolge besitzt eine eigene Komponente.
- Strict-/Tolerant-Parsergrenze bleibt erhalten.
- Snapshotparser und Projectors sind getrennt.
- sprachspezifische Editorprojektion ist getrennt.
- `Capi.cpp` ist eine dünne C-Fassade.
- Registries, Requestdecoder und JSON-Projectors sind getrennt.
- WASM-Memory, Results, Capabilities und Session sind getrennt.
- Legacy-Editorprojektion liegt nicht mehr in `index.js`.
- öffentliche C++-API ist kompatibel.
- C-ABI ist kompatibel.
- npm-Exports sind kompatibel.
- Conformance ist unverändert.
- Diagnose-Decision-Lock ist unverändert.
- Inkrementalitätszähler sind unverändert.
- `LanguageService` ist eine Fassade.
- Source-Layer besitzen einen `SourceRegistry`-Eigentümer.
- Editoranalyse besitzt einen Controller.
- Semantic Snapshots besitzen einen Store.
- Compilequeue besitzt einen Scheduler.
- Missing-Model-Workflow besitzt einen Run Coordinator.
- Repositoryzustand besitzt einen Controller.
- Worker-RPC, Source Mirror und Lifecycle sind getrennt.
- Workerprotokoll ist kompatibel.
- `WebIdeWorkbench` ist eine Fassade.
- Tabs besitzen einen Controller.
- Workspace-Lifecycle besitzt einen Controller.
- Save und Recovery besitzen getrennte Controller.
- Compile-UI besitzt einen Controller.
- Problems, Outline und Diagramm besitzen getrennte Controller.
- DOM-Zugriff ist über eine View konzentriert.
- alle Timer und Listener besitzen einen dokumentierten Dispose-Pfad.
- keine neuen Dependency Cycles.
- Architekturmetriken sind verbessert oder begründet.
- Guards und negative Canaries sind grün.
- Release ist grün.
- Debug ist grün.
- Repository-disabled ist grün.
- ASan/UBSan/LSan sind grün oder echter Plattformblocker dokumentiert.
- TSan ist grün oder echter Plattformblocker dokumentiert.
- WASM/npm ist grün.
- Language Tools sind grün.
- Package-Verifikation ist grün.
- Web-IDE-Check und E2E sind grün.
- PWA-/Browserbuild ist grün.
- Dokumentation und Diagramme sind aktualisiert.
- keine fremden Änderungen wurden beschädigt.

---

## 61. Abschlussbericht

Der Coding-Agent liefert einen Abschlussbericht mit exakt folgenden Abschnitten:

1. **Ausgangszustand**
2. **Repositorypfade**
3. **Ausgangs-Commits**
4. **Fremde Änderungen**
5. **Toolchain**
6. **Dateimetrik-Baseline**
7. **Dependency-Graph-Baseline**
8. **Public-API-Baseline**
9. **C-ABI-Baseline**
10. **npm-Export-Baseline**
11. **Behavior-Lock-Baseline**
12. **Zielarchitektur**
13. **CompilerSessionState**
14. **SourceUpdateCoordinator**
15. **SnapshotService**
16. **CompilationOrchestrator**
17. **CompilationRun**
18. **CompilationTranscript**
19. **ExternalMetaAttributeApplier**
20. **ModelCompilationScheduler**
21. **LanguageDetector**
22. **StrictSourceParser**
23. **TolerantEditorParser**
24. **EditorRecoveryTokenStream**
25. **SyntaxSnapshotProjector**
26. **EditorSnapshotProjector**
27. **Ili1-/Ili2-Projektion**
28. **SessionRegistry**
29. **ResultRegistry**
30. **RequestDecoder**
31. **C-ABI-JSON-Projectors**
32. **Capi-Fassade**
33. **WASM-Memory**
34. **WASM-Resultate**
35. **WASM-Capabilities**
36. **WASM-CompilerSession**
37. **Legacy-Editorprojektion**
38. **SourceRegistry**
39. **SyntaxSnapshotStore**
40. **EditorAnalysisController**
41. **SemanticSnapshotStore**
42. **CompilationScheduler**
43. **CompilationRunCoordinator**
44. **RepositoryModelController**
45. **LanguageServiceEventHub**
46. **LanguageService-Fassade**
47. **WorkerRpcClient**
48. **WorkerSourceMirror**
49. **WorkerLifecycleTracker**
50. **CompilerWorkerBackend**
51. **EditorWorkerBackend**
52. **WorkerHost**
53. **WorkbenchView**
54. **CommandRegistry**
55. **TabController**
56. **WorkspaceController**
57. **SaveController**
58. **RecoveryController**
59. **CompilationController**
60. **ProblemsController**
61. **OutlineController**
62. **DiagramPanelController**
63. **WebIdeWorkbench-Fassade**
64. **Lifecycle und Dispose**
65. **Error Boundaries**
66. **Architecture Guards**
67. **Architekturmetriken vorher/nachher**
68. **Dependency Cycles**
69. **Neue und geänderte Dateien**
70. **Neue Tests**
71. **Behavior-Lock-Ergebnis**
72. **Conformance-Ergebnis**
73. **Diagnose-Lock-Ergebnis**
74. **Incremental-Observability-Ergebnis**
75. **Release-Ergebnis**
76. **Debug-Ergebnis**
77. **Repository-disabled-Ergebnis**
78. **ASan-/UBSan-/LSan-Ergebnis**
79. **TSan-Ergebnis**
80. **WASM-/npm-Ergebnis**
81. **Language-Tools-Ergebnis**
82. **Package-Verifikation**
83. **Web-IDE-Ergebnis**
84. **Browser-/PWA-Ergebnis**
85. **Performance- und Speichervergleich**
86. **Dokumentation**
87. **Ausgeführte Befehle**
88. **Abweichungen von der Spezifikation**
89. **Externe Blocker**
90. **Verbleibende Risiken**
91. **Abschliessender Gitstatus aller drei Repositories**

Für jeden Build- und Testlauf nennen:

- Repository;
- exakter Befehl;
- Exitcode;
- Buildtyp;
- Testanzahl;
- Fehleranzahl;
- Umgebung;
- Reportpfad;
- Conformance-Suite-Commit;
- Baseline-Datei;
- verwendete npm-Tarballs;
- Browserprojekt;
- relevante Architektur- und Performancewerte.

Keine Testzahl schätzen.

Keinen Lauf behaupten, der nicht ausgeführt wurde.

---

## 62. Explizite Erfolgsaussage

P7 darf nur als abgeschlossen bezeichnet werden, wenn folgende Aussage wahr und belegt ist:

> Die drei Projekte besitzen nach P7 klare, gerichtete und dokumentierte Modulgrenzen. Öffentliche Fassaden koordinieren nur noch und überlassen Source-Lifecycle, Parsing, Kompilierung, C-ABI-Serialisierung, Workertransport, Language-Service-Zustand und Web-IDE-Controller jeweils eindeutig benannten Komponenten mit klarer Ownership und idempotentem Dispose. Die bisherigen Compilerentscheidungen, Diagnosen, Conformance-Ergebnisse, C++- und C-ABI-Verträge, npm-Exports, Inkrementalitätszähler, Save-driven Semantik, LSP-/Monaco-Verhalten, Problems und Last-Good-Diagramme bleiben unverändert. Die Codebasis ist kleiner pro Verantwortung, direkt testbar und durch Architecture Guards gegen erneute God-Class-Bildung geschützt.

---

## Anhang A – Verbindlicher Start

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

Danach:

1. diese Spezifikation vollständig lesen;
2. aktuelle Dateigrenzen und bereits vorhandene Refactorings inventarisieren;
3. Baseline und Behavior Lock ausführen;
4. Architekturgraph erzeugen;
5. P7 in der angegebenen Reihenfolge implementieren;
6. nach jeder Phase fokussierte Tests und Behavior Lock ausführen;
7. erst nach vollständiger Cross-Repo-Abnahme abschliessen.

---

## Anhang B – Reviewcheckliste für eine extrahierte Komponente

- [ ] genau eine klar benannte Verantwortung;
- [ ] eindeutiger Eigentümer;
- [ ] dokumentierte Lebensdauer;
- [ ] keine Rückreferenz zur Fassade;
- [ ] keine versteckte globale Abhängigkeit;
- [ ] minimale Konstruktorabhängigkeiten;
- [ ] keine unnötige Interface-Abstraktion;
- [ ] idempotentes Dispose, falls nötig;
- [ ] direkte Unit-Tests;
- [ ] Behavior Lock grün;
- [ ] keine neue API;
- [ ] keine neue Dependency Cycle;
- [ ] Architekturdiagramm aktualisiert.

---

## Anhang C – Reviewcheckliste für Fassaden

- [ ] öffentliche API unverändert;
- [ ] delegiert statt implementiert;
- [ ] keine grossen Maps oder Timer;
- [ ] kein JSON-Rendering;
- [ ] kein Parserdetail;
- [ ] keine Worker-Replay-Implementierung;
- [ ] keine DOM-Allzwecklogik;
- [ ] Fehlerpfad sichtbar;
- [ ] Lifecycle klar;
- [ ] Dateigrössenbudget eingehalten oder begründet.

---

## Anhang D – Reviewcheckliste für Ownership

- [ ] Zustand genau einem Eigentümer zugeordnet;
- [ ] Schreibzugriffe kontrolliert;
- [ ] Views oder Snapshots für Leser;
- [ ] Thread-/Workergrenze dokumentiert;
- [ ] Requestzustand nicht langfristig gespeichert;
- [ ] Cacheeintrag besitzt Resultat vollständig;
- [ ] Listener und Timer beim Dispose entfernt;
- [ ] keine Dangling Pointer beziehungsweise stale Promises.

---

## Anhang E – Reviewcheckliste für Cross-Repo-Kompatibilität

- [ ] C++ Public API Snapshot unverändert;
- [ ] C-ABI Symbole unverändert;
- [ ] JSON canonical gleich;
- [ ] WASM TypeScript gleich;
- [ ] npm Exports gleich;
- [ ] Workerprotokoll gleich;
- [ ] LanguageService Public API gleich;
- [ ] LSP gleich;
- [ ] Monaco gleich;
- [ ] Web-IDE E2E gleich;
- [ ] Conformance gleich;
- [ ] Diagnosen gleich;
- [ ] Incremental Stats/Trace gleich;
- [ ] Bundlegrössen geprüft.
