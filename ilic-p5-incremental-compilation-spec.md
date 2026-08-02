# P5-Spezifikation: Session-lokale inkrementelle Analyse über Compiler, Language Tools und Web-IDE

**Revision:** 1  
**Status:** normative Implementierungsspezifikation für einen LLM-Coding-Agenten  
**Primäres Repository:** `../ilic-fork`  
**Verbindlich einzubeziehende Consumer-Repositories:** `../interlis-language-tools`, `../interlis-web-ide`

---

## 1. Auftrag

Implementiere P5 als **sichere, deterministische und messbar wirksame Inkrementalitätsstufe** für die INTERLIS-Compiler- und Editor-Toolchain.

P5 soll nicht lediglich einen weiteren JavaScript-Cache hinzufügen. Der native `CompilerSession`-Zustand muss zur autoritativen Quelle für folgende Entscheidungen werden:

- ob eine Source inhaltlich verändert wurde;
- welche Parserartefakte noch gültig sind;
- welche Syntax- und Editor-Snapshots wiederverwendet werden können;
- welche Modelle und Root-Kompilationen von einer Sourceänderung betroffen sind;
- welche vollständigen Analyseartefakte exakt wiederverwendet werden können;
- welche Root-Analysen neu ausgeführt werden müssen;
- welche Abhängigkeiten unverändert bleiben;
- welche Resultate wegen Cancellation oder einer neueren Sourcegeneration nicht mehr publiziert werden dürfen.

P5 umfasst zwingend Arbeiten in allen drei Geschwister-Repositories:

```text
ilic-fork/
interlis-language-tools/
interlis-web-ide/
```

Die Umsetzung muss die bestehenden sichtbaren Produktverträge erhalten:

- semantische Vollanalyse bleibt standardmässig save-/compile-getrieben;
- Tippen verwendet weiterhin den toleranten Editor-Snapshot;
- Output, Problems, Diagramm und DOCX bleiben an akzeptierte semantische Resultate gebunden;
- `lastGood`- und `stale`-Verhalten bleiben erhalten;
- Node-LSP und Monaco/Web-IDE benötigen keine fachliche Migration;
- bestehende High-Level-Methoden bleiben verwendbar;
- das Worker-Protokoll bleibt rückwärtskompatibel;
- P1-Conformance, P2-Isolation, P3-Single-Parser und P4-Repository-Verträge dürfen nicht geschwächt werden.

P5 gilt nicht als abgeschlossen, wenn lediglich neue Klassen angelegt, Messwerte simuliert oder Tests mit einem weiterhin vollständigen Neuaufbau grün gestellt wurden. Die Wiederverwendung muss durch deterministische Zähler und negative Kontrolltests nachgewiesen werden.

---

## 2. Normative Sprache

Die Begriffe **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT**, **DARF** und **KANN** sind normativ.

- **MUSS / DARF NICHT:** zwingende Abnahmebedingung.
- **SOLL / SOLL NICHT:** darf nur mit dokumentierter technischer Begründung abweichen.
- **DARF / KANN:** zulässige Gestaltungsfreiheit.

Codebeispiele sind strukturell normativ. Namen dürfen an den vorhandenen Stil angepasst werden, wenn Verantwortlichkeiten, Lebensdauer, Thread-Safety und API-Verträge identisch bleiben.

---

## 3. Verbindliche Ausgangsbasis

Der Agent MUSS zu Beginn den tatsächlichen Stand aller drei Repositories feststellen. Die folgenden bei Erstellung dieser Spezifikation beobachteten Commits dienen nur als Orientierung und dürfen nicht blind als aktueller Stand angenommen werden:

```text
ilic-fork
4b2696b84df31070c461782578aa7bc02c072db7
perf: optimize native editor snapshot path

interlis-language-tools
1ffa3327016d288d1af018b1fd4e49ca92bd2ad8
fix(ci): retry npm snapshot visibility checks

interlis-web-ide
2564f1084ca972fea35d447eeb866033f084cebe
fix: hide Monaco editor scrollbars
```

Vor jeder Änderung MUSS in jedem Repository ausgeführt werden:

```bash
pwd
git status --short
git branch --show-current
git rev-parse HEAD
```

Der Agent MUSS für jedes Repository dokumentieren:

- absoluten Pfad;
- Branch;
- Ausgangs-Commit;
- uncommittete Änderungen;
- untracked Dateien;
- vorhandene Spezifikationsdateien;
- verwendete Package-Manager-Version;
- relevante lokale Overrides;
- ob P4 vollständig integriert ist.

Fremde Änderungen sind unverändert zu erhalten. Insbesondere verboten:

```bash
git reset --hard
git clean -fd
git checkout -- .
git restore .
```

Der Agent DARF keine Commits erzeugen oder pushen, sofern dies nicht separat verlangt wurde.

---

## 4. Voraussetzungen P0 bis P4

### 4.1 P0 – Testhärtung

P5 MUSS erhalten:

- echte Testassertions auch in Release;
- Release-Canary;
- Debug-Tests;
- ASan/UBSan/LSan;
- TSan, soweit vorhanden;
- Dokumentationstests;
- `git diff --check`;
- keine dauerhaft ignorierten kritischen CI-Fehler.

### 4.2 P1 – externe Compiler-Conformance

Die externe Suite ist weiterhin die fachliche Referenz für Compilerakzeptanz und Diagnostik. Der Agent MUSS den aktuell in `.github/workflows/ci.yml` gepinnten Suite-Commit, das Manifest, die Baseline und den exakten Runner-Befehl verwenden.

Die Baseline DARF NICHT erweitert werden, um Inkrementalitätsfehler zu verdecken.

P5 MUSS zusätzlich **Differenzläufe zwischen Cold und Incremental** ausführen:

```text
Cold Session
  Sources registrieren
  Root kompilieren

Incremental Session
  gleiche Ausgangssources registrieren
  Root kompilieren
  Mutation anwenden
  Root erneut kompilieren

Fresh Session after mutation
  mutierte Sources registrieren
  Root kompilieren
```

Das zweite Resultat der Incremental Session MUSS fachlich und strukturell dem Fresh-Session-Resultat entsprechen.

### 4.3 P2 – isolierter Compilerzustand

P5 MUSS auf P2 aufbauen:

- jede `CompilerSession` besitzt ihren eigenen Zustand;
- keine mutable globale Source-, Parser-, Metamodell- oder Cache-Registry;
- keine globale Compiler-Mutex;
- kein `thread_local` Aktivkontext;
- eindeutige Ownership;
- lebensdauersichere Snapshots;
- unabhängige parallele Sessions.

Inkrementelle Caches sind **sessionlokal**. Ein optionaler globaler, immutable, content-addressierter Cache wäre ein separates zukünftiges Projekt und ist in P5 verboten.

### 4.4 P3 – ein autoritativer Parser

P5 MUSS den nativen C++-/ANTLR-Pfad als einzige Sprachinterpretation verwenden.

Syntax-, Editor-, Header-, Import- und Kompilationsinformationen dürfen nicht durch unabhängige Parserpfade erzeugt werden.

P5 MUSS insbesondere verhindern, dass Inkrementalität zu folgenden Doppelpfaden führt:

```text
Parser für Editor-Snapshot
Parser für Header/Imports
Parser für Vollkompilation
Parser für SemanticSnapshot
```

Ziel ist ein einmal erzeugtes, immutable beziehungsweise kontrolliert wiederverwendbares Parserartefakt pro Source-Inhaltsrevision.

### 4.5 P4 – Repository-Modularisierung und Source-Identität

P5 setzt den P4-Zielzustand voraus:

- `uri + source` sind autoritativ;
- Repository-Source kann ohne obligatorischen `localPath` in die Session aufgenommen werden;
- Repositoryauflösung und Compiler sind getrennt;
- `@ilic/tools` bleibt kompatible Fassade;
- Repositorymodelle erhalten stabile URIs;
- Cache- und Transportdetails bestimmen nicht die Compileridentität.

Eine neu aufgelöste Repository-Source MUSS in eine bestehende CompilerSession aufgenommen werden können, ohne Session-Neustart und ohne Replay aller bereits registrierten Sources.

Ist P4 im tatsächlichen Branch noch nicht integriert, MUSS der Agent dies als Voraussetzung dokumentieren. P5 darf P4 nicht heimlich in einer inkompatiblen Nebenimplementierung nachbauen.

---

## 5. Aktueller Problemzustand

### 5.1 Native CompilerSession

Der aktuelle native Ablauf erzeugt bei jedem `compileAndAnalyze()` einen neuen `CompilerContext` und führt eine vollständige Kompilation aus.

Sinngemäss:

```cpp
CompilationAnalysisResult CompilerSession::compileAndAnalyzeUnlocked(
    const CompilationRequest& request)
{
    detail::CompilerContext context(impl_->sources, request.options);
    CompilationAnalysisResult result;
    result.compilation = compileRun(context, request, compilationSourceUris);
    result.semantic = buildSemanticSnapshot(
        impl_->sources,
        request,
        result.compilation,
        compilationSourceUris,
        &result.syntax,
        &context.models()
    );
    return result;
}
```

Dabei können dieselben Sources mehrfach verarbeitet werden:

1. Header-/Import-Erkennung im `IliFileCatalog`;
2. vollständiges ANTLR-Parsing für `Ili1Input` beziehungsweise `Ili2Input`;
3. erneutes Syntaxparsing in `buildSemanticSnapshot()`;
4. separates Snapshotparsing über `SnapshotPipeline`.

### 5.2 SourceManager

`SourceManager::put()` ersetzt heute eine Source ohne Klassifikation:

```cpp
void put(std::string uri, std::string utf8, std::uint64_t version);
```

Es ist nicht explizit unterscheidbar zwischen:

- neuer Source;
- identischem Inhalt und identischer Version;
- identischem Inhalt mit neuer Dokumentversion;
- verändertem Inhalt;
- Wiedereinfügen nach Löschung;
- veralteter Versionsnummer.

### 5.3 Language Service

Der Language Service besitzt bereits wichtige Zustände:

- effektive Sources;
- serviceeigene Source-Revisions;
- Syntax-Snapshots;
- Editor-Snapshots;
- root-spezifische SemanticSnapshots;
- `current`, `lastGood`, `saved`;
- Versionsvektoren;
- Reverse Dependencies;
- Compilation Queue;
- `generation` und `compilationEpoch`.

Diese Zustände sind heute hauptsächlich für Freshness und UI-Korrektheit zuständig. Die eigentliche Compilerarbeit wird bei jedem akzeptierten Compile-Aufruf vollständig neu ausgeführt.

Problematisch ist insbesondere der Repository-Nachladepfad: Nach dem Registrieren neu aufgelöster Modelle wird die CompilerSession aktuell neu gestartet und sämtliche Sources werden wiederholt eingespielt.

### 5.4 Worker

Die Toolchain verwendet getrennte Worker für:

- Vollkompilation;
- Editor-Snapshots.

Zusätzlich existiert ein lokales Backend als synchroner Parser-/Formatterpfad und Fallback.

Jeder Worker hält eine eigene Session. Bei Worker-Neustart werden alle Sources aus JavaScript-Maps replayed. Ein solcher Replay ist für Crash-Recovery zulässig, aber für normale Sourceupdates und Repositoryauflösung zu teuer.

### 5.5 Web-IDE

Die Web-IDE:

- synchronisiert Workspace-Sources;
- markiert sichtbare semantische Projektionen beim Tippen als `outdated`;
- kompiliert bei Startup, Save, manuellem Compile und Diagramm-/Dependency-Triggern;
- verwendet Editor-Snapshots für Outline und Completion;
- verwendet gespeicherte semantische Snapshots für Diagramm und DOCX;
- scannt bei bestimmten Operationen den Workspace erneut.

P5 darf diese sichtbare Semantik nicht verändern, soll aber redundante Sourceupdates, Parserläufe, Workerrestarts und vollständige Root-Neuanalysen eliminieren.

---

## 6. Ziele

P5 MUSS mindestens folgende Ziele erreichen.

### 6.1 Dateiweise Parser-Inkrementalität

Für jede Kombination aus:

```text
URI
+ Source-Inhaltsrevision
+ Parser-/Grammar-Version
```

MUSS höchstens ein autoritatives Parserartefakt pro Session erzeugt werden, solange es nicht eviziert wurde.

Dieses Artefakt MUSS für folgende Projektionen wiederverwendbar sein:

- SyntaxSnapshot;
- EditorSnapshot;
- Header-/Modellnamen;
- Imports und Required Models;
- Kompilationsvisitor;
- Dependency-Ermittlung für SemanticSnapshot.

### 6.2 Versionsänderung ohne Inhaltsänderung

Wird dieselbe Source mit höherer Dokumentversion erneut registriert, DARF kein erneutes Lexing oder Parsing stattfinden.

Snapshots müssen dennoch die neue `documentVersion` ausgeben.

### 6.3 Root-granulare semantische Inkrementalität

Ein vollständiges semantisches Resultat DARF exakt wiederverwendet werden, wenn:

- Rootset identisch ist;
- normalisierte Compileroptionen identisch sind;
- External Meta Attributes identisch sind;
- die transitive Sourceclosure inhaltlich identisch ist;
- Builtin-/Compiler-/Grammar-Fingerprints identisch sind;
- keine relevante Cancellation oder Invalidation erfolgt ist.

Ändert sich eine Source, dürfen nur Rootresultate invalidiert werden, deren bekannte Closure die Source enthält oder deren bislang unaufgelöste Abhängigkeit durch die Änderung betroffen sein kann.

### 6.4 Wiederverwendung unveränderter Dependencies

Bei der Neuanalyse eines betroffenen Roots MUSS der Compiler Parserartefakte unveränderter Dependency-Sources wiederverwenden.

P5 verlangt **nicht**, dass mutable Metamodellobjekte einer vorherigen Kompilation blind wiederverwendet werden. Der `MetaModelStore` darf für einen betroffenen Root neu aufgebaut werden, solange unveränderte Sources nicht erneut gelexed oder geparsed werden.

### 6.5 Keine normalen Session-Restarts

Folgende Ereignisse dürfen keinen Compiler-Neustart auslösen:

- normale Dokumentänderung;
- Save;
- Sourceaddition;
- Sourcelöschung;
- Auflösung eines fehlenden Repositorymodells;
- Änderung einer Repositorykonfiguration, sofern die Sourceänderungen explizit anwendbar sind;
- erneute Kompilation desselben Roots.

Restart/Replay ist nur für echte Recovery zulässig:

- Workercrash;
- explizite Cancellation durch Workertermination;
- ABI-/WASM-Reinitialisierung;
- fataler Sessionzustand.

### 6.6 Messbarkeit

P5 MUSS deterministische Zähler bereitstellen, mit denen Tests nachweisen können:

- wie viele Sourceupdates No-ops waren;
- wie viele nur die Dokumentversion änderten;
- wie viele Parserartefakte gebaut wurden;
- wie viele Parsercache-Hits auftraten;
- wie viele Rootanalyse-Hits und -Misses auftraten;
- welche Roots invalidiert wurden;
- ob ein Workerreplay stattfand;
- wie viele Sources replayed wurden.

Reine Wall-Clock-Benchmarks genügen nicht als Abnahmebeleg.

---

## 7. Nichtziele

P5 umfasst ausdrücklich nicht:

- partielles ANTLR-Reparsing innerhalb einer einzelnen Datei;
- Token-Splicing oder Parse-Tree-Patching anhand von LSP-Textdeltas;
- persistente Compiler-Caches über Prozess- oder Browserneustarts;
- einen Compiler-Daemon;
- einen verteilten Cache;
- eine globale content-addressierte Parse-Registry;
- unsichere Wiederverwendung mutabler `MetaModelStore`-Pointer;
- eine Änderung von save-getriebener Semantik zu dauernder Vollkompilation beim Tippen;
- eine zwingende Umstellung des LSP auf `TextDocumentSyncKind.Incremental`;
- eine neue Benutzeroberfläche;
- Änderungen an der INTERLIS-Sprache;
- Änderungen an Compilerakzeptanz oder Diagnosebedeutung;
- eine verpflichtende Migration von Node-LSP oder Web-IDE;
- einen On-Disk-Buildcache.

Textdeltas, feingranulare semantische Export-Fingerprints und persistente Caches können spätere Phasen bilden. P5 muss dafür saubere Erweiterungspunkte schaffen, darf diese Themen aber nicht halb implementieren.

---

## 8. Verbindliche Invarianten

### 8.1 Korrektheit vor Hit-Rate

Bei Unsicherheit MUSS konservativ invalidiert werden.

Ein zusätzlicher Cache-Miss ist zulässig. Ein fälschlich wiederverwendetes veraltetes Resultat ist ein P5-Blocker.

### 8.2 Cold/Incremental-Äquivalenz

Für jede Mutation MUSS gelten:

```text
incrementalResultAfterMutation == freshSessionResultAfterMutation
```

Verglichen werden mindestens:

- Erfolg;
- Modelle;
- Fehler-/Warnungszahlen;
- Missing Models;
- Diagnosen;
- Syntax-Snapshots;
- SemanticSnapshot;
- Symbole;
- Referenzen;
- Dependencies;
- Diagramm;
- Dokumentation;
- relevante Logs und Transcriptzeilen, abzüglich explizit erlaubter Zeitwerte.

### 8.3 Snapshot-Lebensdauer

Öffentliche Snapshots müssen unabhängig von Cacheeviction und Sessionänderungen gültig bleiben.

Kein Snapshot darf Pointer oder Views in folgende Objekte exponieren:

- ANTLRInputStream;
- Lexer;
- TokenStream;
- Parser;
- Parse Tree;
- `ParsedSourceArtifact`;
- `CompilerContext`;
- `MetaModelStore`;
- Cacheeinträge.

### 8.4 Sessionisolation

Zwei Sessions mit identischen Sources dürfen intern unabhängig cachen. Mutation oder Eviction in Session A darf Session B nicht beeinflussen.

### 8.5 Determinismus

Identische Sourcehistorien und Requests müssen identische:

- Cacheentscheidungen;
- Invalidationsmengen;
- Compilation Plans;
- Snapshots;
- Diagnoseordnungen;
- Metrikzähler

liefern.

### 8.6 Keine sichtbare Semantikänderung

P5 darf nicht dazu führen, dass:

- Output bei Tippen neu erzeugt wird;
- Diagramme ungefragt auf Dirty-Code umschalten;
- `lastGood` verschwindet;
- Problems vor einem akzeptierten Lauf aus einem falschen Root ersetzt werden;
- ältere Workerantworten neuere Zustände überschreiben;
- Repositorytabs editierbar werden;
- Save- und Manual-Trigger ihre Bedeutung ändern.

---

## 9. Zielarchitektur

```text
                         CompilerSession
                               |
                               v
                    IncrementalCompilerWorkspace
                               |
        +----------------------+----------------------+
        |                      |                      |
        v                      v                      v
 TrackedSourceStore     ParsedSourceCache      SourceDependencyIndex
        |                      |                      |
        |                      v                      |
        |             ParsedSourceArtifact            |
        |              /       |       \              |
        |             /        |        \             |
        |    SyntaxSnapshot  EditorSnapshot  Header/Imports
        |                      |                      |
        +----------------------+----------------------+
                               |
                               v
                      CompilationPlanner
                               |
                     +---------+---------+
                     |                   |
                     v                   v
              RootAnalysisCache   IncrementalCompileExecutor
                                         |
                                         v
                                  run-local CompilerContext
                                         |
                                  immutable final snapshots
```

Cross-Repository:

```text
ilic-fork
  authoritative source revisions, parser artifacts,
  dependency graph, root cache, metrics, C ABI/WASM
          |
          v
interlis-language-tools
  lifecycle, scheduling, freshness, workers, LSP/Monaco adapters
          |
          v
interlis-web-ide
  workspace source diffing, PWA workers, visible stale/last-good behavior
```

---

## 10. Terminologie

### 10.1 Dokumentversion

Die vom Consumer gelieferte Version einer Source. Sie dient zur Zuordnung von Editor- und LSP-Resultaten.

### 10.2 Inhaltsrevision

Eine sessionlokale, monoton steigende interne Revision, die sich nur ändert, wenn sich die Sourcebytes ändern oder eine URI gelöscht und semantisch neu eingeführt wird.

### 10.3 Sourcegeneration

Monotone Sessiongeneration für Add/Remove/Content-Change-Ereignisse. Sie dient zur Verwerfung überholter Pläne und Resultate.

### 10.4 Parserartefakt

Ein sessioneigenes, kontrolliert immutable Resultat aus genau einem Lexing-/Parsinglauf einer Source-Inhaltsrevision.

### 10.5 Headerartefakt

Aus dem Parserartefakt abgeleitete Modellnamen, INTERLIS-Version, Imports, Required Models, Translation-Beziehungen und relevante Headerdiagnosen.

### 10.6 Sourceclosure

Die transitive Menge aller Source-URIs, die für einen Rootrequest benötigt werden, einschliesslich Root, Imports und Builtins.

### 10.7 Rootanalyse

Kompilations- und SemanticSnapshot-Resultat für ein normalisiertes Rootset und normalisierte Optionen.

### 10.8 Exact Hit

Wiederverwendung eines Rootanalyseartefakts, dessen vollständiger fachlicher Schlüssel identisch ist.

### 10.9 Rebuild mit Parse-Reuse

Neuaufbau des `CompilerContext` und Metamodells bei Wiederverwendung unveränderter Parserartefakte.

---

## 11. Source-Identität und Update-Semantik

### 11.1 Neue Datentypen

Implementiere mindestens äquivalente Typen:

```cpp
enum class SourceUpdateKind {
    Added,
    Unchanged,
    VersionOnly,
    ContentChanged,
    Reintroduced,
    Removed,
    Rejected
};

struct SourceIdentity {
    std::string uri;
    std::uint64_t documentVersion = 0;
    std::uint64_t contentRevision = 0;
    std::uint64_t sourceGeneration = 0;
    std::string contentHash;
};

struct SourceUpdateResult {
    SourceUpdateKind kind = SourceUpdateKind::Rejected;
    SourceIdentity previous;
    SourceIdentity current;
    bool accepted = false;
    bool parserInvalidated = false;
    bool analysesInvalidated = false;
};
```

### 11.2 `TrackedSourceStore`

Implementiere eine Klasse oder erweitere `SourceManager` kontrolliert:

```cpp
class TrackedSourceStore final {
public:
    SourceUpdateResult update(
        std::string uri,
        std::string utf8,
        std::uint64_t documentVersion
    );

    SourceUpdateResult remove(const std::string& uri);

    const SourceBuffer* get(const std::string& uri) const noexcept;
    const SourceIdentity* identity(const std::string& uri) const noexcept;
    std::vector<std::string> uris() const;

    std::uint64_t generation() const noexcept;
};
```

Bestehende `SourceManager::put()`-Aufrufer dürfen über einen kompatiblen Wrapper weiterarbeiten.

### 11.3 Klassifikationsregeln

#### Added

URI existierte in der aktuellen Sessionhistorie noch nicht oder ist nicht aktiv.

- neue Inhaltsrevision;
- neue Sourcegeneration;
- Parsercache-Miss möglich;
- unresolved dependency watchers prüfen.

#### Unchanged

URI, Bytes und Dokumentversion identisch.

- keine neue Generation;
- keine Invalidierung;
- keine Parserarbeit;
- keine Workeranalyse.

#### VersionOnly

Bytes identisch, Dokumentversion höher.

- Dokumentversion aktualisieren;
- Inhaltsrevision unverändert;
- keine Parserinvalidierung;
- keine fachliche Rootinvalidierung;
- vorhandene Results dürfen mit aktualisiertem Versionsvektor materialisiert werden;
- alte asynchrone Resultate mit alter Dokumentversion dürfen nicht publiziert werden.

#### ContentChanged

Bytes unterscheiden sich.

- neue Inhaltsrevision;
- neue Sourcegeneration;
- Parserartefakt der alten Revision bleibt nur für bereits ausgegebene Snapshots lebensdauersicher, ist aber nicht mehr aktiv;
- Dependencyindex für die URI neu bestimmen;
- betroffene Rootanalysen invalidieren.

#### Reintroduced

URI wurde entfernt und wird wieder hinzugefügt.

- neue Sourcegeneration;
- aktive Identität ist neu;
- ein content-addressierter sessionlokaler Parse-Eintrag gleicher Bytes darf technisch wiederverwendet werden;
- Rootanalysen müssen dennoch neu geplant werden.

#### Removed

- aktive Source entfernen;
- Sourcegeneration erhöhen;
- Parserartefakt darf evizierbar werden;
- Roots mit URI in Closure invalidieren;
- Roots, die von exportierten Modellen dieser URI abhängen, invalidieren;
- unresolved dependencies aktualisieren.

#### Rejected

Mindestens folgende Updates müssen kontrolliert abgelehnt oder explizit normalisiert werden:

- leere URI;
- NUL in URI;
- Dokumentversion kleiner als die aktuelle, wenn der öffentliche Vertrag monotone Versionen verlangt;
- Versionoverflow;
- Sourcegrösse über konfiguriertem Limit.

### 11.4 Hashing

Der Inhalts-Hash dient als Cacheidentität, nicht als Sicherheitsanker.

Anforderungen:

- Hash über exakte UTF-8-Bytes;
- deterministisch Native/WASM;
- kein `std::hash`;
- keine localeabhängige Normalisierung;
- Algorithmus dokumentieren;
- Kollisionsbehandlung darf Korrektheit nicht allein vom Hash abhängig machen: bei gleichem Hash und abweichender Länge oder bei Debug-/Testmodus Bytes vergleichen.

Empfohlen ist ein schneller stabiler Hash mit zusätzlicher Byte-Länge. Kryptografische Stärke ist nicht erforderlich.

---

## 12. Parserartefakt

### 12.1 Verantwortung

`ParsedSourceArtifact` repräsentiert genau eine Source-Inhaltsrevision und MUSS alle Objekte besitzen, die zur Wiederverwendung des Parse Trees erforderlich sind.

Beispiel:

```cpp
class ParsedSourceArtifact final {
public:
    const SourceIdentity& identity() const noexcept;
    IliVersion iliVersion() const noexcept;
    const HeaderSnapshot& header() const noexcept;
    const std::vector<Diagnostic>& parserDiagnostics() const noexcept;

    SyntaxSnapshot materializeSyntax(std::uint64_t documentVersion) const;
    EditorSnapshot materializeEditor(std::uint64_t documentVersion) const;

    void buildMetaModel(
        metamodel::MetaModelBuilder& builder,
        util::Logger& logger
    ) const;

    std::size_t retainedBytes() const noexcept;
};
```

### 12.2 Ownership

Das Artefakt MUSS mindestens besitzen oder stabil referenzieren:

- immutable Sourcebytes;
- `ANTLRInputStream` oder äquivalent;
- Lexer;
- `CommonTokenStream`;
- Parser;
- Root Context;
- Parserdiagnosen;
- Sprachdetektion;
- HeaderSnapshot;
- SyntaxSnapshot-Template;
- EditorSnapshot-Template.

Raw Parserpointer dürfen nicht aus der Klasse herausgegeben werden.

### 12.3 Sprachspezifische Zustände

Zulässig ist beispielsweise:

```cpp
struct Ili1ParsedState {
    std::unique_ptr<antlr4::ANTLRInputStream> input;
    std::unique_ptr<lexer::Ili1Lexer> lexer;
    std::unique_ptr<antlr4::CommonTokenStream> tokens;
    std::unique_ptr<parser::Ili1Parser> parser;
    parser::Ili1Parser::IliFileContext* root = nullptr;
};

struct Ili2ParsedState {
    std::unique_ptr<antlr4::ANTLRInputStream> input;
    std::unique_ptr<lexer::Ili2Lexer> lexer;
    std::unique_ptr<antlr4::CommonTokenStream> tokens;
    std::unique_ptr<parser::Ili2Parser> parser;
    parser::Ili2Parser::Interlis2DefContext* root = nullptr;
};
```

Das Artefakt darf diese Zustände über `std::variant` kapseln.

### 12.4 Immutability und Wiederbesuch

Nach Konstruktion darf das Artefakt fachlich nicht verändert werden.

Falls bestehende Visitor den Parse Tree verändern, MUSS dies zuerst korrigiert oder durch ein immutable normalisiertes Zwischenmodell ersetzt werden.

Der Agent MUSS Tests schreiben, die dasselbe Artefakt mehrfach für:

- Syntax;
- Editor;
- Header;
- Metamodellaufbau

verwenden und identische Resultate liefern.

### 12.5 Alternative: normalisiertes Parse-Dokument

Falls dauerhaftes Halten des ANTLR-Parsers nachweislich unsicher oder unverhältnismässig ist, DARF ein immutable normalisiertes Parse-Dokument eingeführt werden.

Bedingungen:

- es wird aus genau einem ANTLR-Lauf erzeugt;
- es enthält genügend Struktur für Compilervisitor und Snapshots;
- es ist kein zweiter Textparser;
- Native/WASM-Parität bleibt exakt;
- Performance- und Speicherentscheidung wird dokumentiert.

Ein erneutes ANTLR-Parsing für den Kompilationsvisitor ist keine zulässige Alternative.

---

## 13. `ParsedSourceCache`

### 13.1 Schnittstelle

```cpp
struct ParsedSourceLookup {
    std::shared_ptr<const ParsedSourceArtifact> artifact;
    bool hit = false;
    bool built = false;
};

class ParsedSourceCache final {
public:
    explicit ParsedSourceCache(ParsedSourceCacheOptions options = {});

    ParsedSourceLookup getOrBuild(
        const SourceBuffer& source,
        const SourceIdentity& identity
    );

    void deactivate(const std::string& uri, std::uint64_t contentRevision);
    void clear();
    void trim();

    ParsedSourceCacheStats stats() const noexcept;
};
```

### 13.2 Schlüssel

Mindestens:

```text
URI
content hash
byte length
content revision or exact byte identity
parser grammar fingerprint
compiler parser ABI fingerprint
```

Die Dokumentversion gehört nicht in den Parsercacheschlüssel.

### 13.3 Speicherbudget

Der Cache MUSS begrenzt sein.

Optionen mindestens:

```cpp
struct ParsedSourceCacheOptions {
    std::size_t maxEntries = 512;
    std::size_t maxRetainedBytes = 256 * 1024 * 1024;
};
```

Werte dürfen anhand realer Messungen angepasst werden.

Eviction:

- deterministische LRU- oder Clock-Policy;
- aktive Artefakte dürfen eviziert werden, wenn keine laufende Operation sie benötigt;
- `shared_ptr`-Referenzen laufender Operationen sichern Lebensdauer;
- keine Eviction unter einem unkontrollierten Callback;
- keine unbeschränkte Historie pro URI;
- Metriken für Evictions.

### 13.4 Negative Cacheeinträge

Auch syntaktisch fehlerhafte Sources dürfen ein Parserartefakt besitzen.

Ein Parsefehler darf nicht dazu führen, dass bei jedem Editorrequest erneut geparsed wird.

Negative Einträge sind nur für exakt dieselbe Inhaltsrevision gültig.

---

## 14. Snapshot-Wiederverwendung

### 14.1 Templates

Parserartefakte sollen versionsunabhängige Snapshot-Templates halten.

```cpp
struct SyntaxSnapshotTemplate {
    bool success;
    IliVersion iliVersion;
    std::vector<SyntaxToken> tokens;
    std::vector<SyntaxNode> nodes;
    std::vector<SyntaxContext> contexts;
    std::vector<std::string> imports;
    std::vector<SyntaxImportReference> importReferences;
    std::vector<Diagnostic> diagnostics;
};
```

Bei Materialisierung werden mindestens gesetzt:

- URI;
- aktuelle Dokumentversion;
- Compiler-/ABI-/Schema-Metadaten.

Entsprechendes gilt für `EditorSnapshotTemplate`.

### 14.2 `IncrementalSnapshotService`

```cpp
class IncrementalSnapshotService final {
public:
    IncrementalSnapshotService(
        TrackedSourceStore& sources,
        ParsedSourceCache& parsed
    );

    SyntaxSnapshot syntax(const std::string& uri);
    EditorSnapshot editor(const std::string& uri);
    SnapshotBundle both(const std::string& uri);
};
```

Invarianten:

- `syntax()` und `editor()` für dieselbe Inhaltsrevision verwenden dasselbe Parserartefakt;
- aufeinanderfolgende Aufrufe bauen nicht erneut;
- `both()` materialisiert beide Projektionen ohne zweiten Parse;
- unbekannte URI liefert kontrollierte Diagnose;
- VersionOnly-Update materialisiert neue Version ohne Parserbuild.

### 14.3 Bestehende API

Folgende Methoden bleiben:

```cpp
SyntaxSnapshot CompilerSession::parse(const std::string& uri);
EditorSnapshot CompilerSession::editorSnapshot(const std::string& uri);
```

Sie delegieren an `IncrementalSnapshotService`.

---

## 15. Header- und Modellindex

### 15.1 `HeaderSnapshot`

```cpp
struct HeaderSnapshot {
    std::string uri;
    IliVersion iliVersion = IliVersion::Unknown;
    std::vector<std::string> models;
    std::vector<std::string> imports;
    std::vector<std::string> requiredModels;
    std::vector<std::string> translationModels;
    std::vector<Diagnostic> diagnostics;
};
```

Der Header MUSS aus dem Parserartefakt abgeleitet werden.

### 15.2 `SourceModelIndex`

```cpp
class SourceModelIndex final {
public:
    void update(
        const SourceIdentity& identity,
        const HeaderSnapshot& header
    );

    void remove(const std::string& uri);

    std::vector<std::string> urisForModel(
        std::string_view model,
        IliVersion version
    ) const;

    std::vector<std::string> modelsForUri(
        const std::string& uri
    ) const;
};
```

Anforderungen:

- mehrere Modelle pro Datei;
- duplicate model names werden nicht zufällig überschrieben;
- deterministische Kandidatenreihenfolge;
- Versionfilter;
- Repository-, Workspace- und Session-Sources werden gleich behandelt;
- Builtin `INTERLIS` explizit;
- Update einer Source entfernt alte Modellnamen atomar und fügt neue hinzu.


---

## 16. Abhängigkeitsindex

### 16.1 Zweck

Der Dependencyindex ist die autoritative sessionlokale Grundlage für gezielte Rootinvalidierung.

Er darf nicht nur aus dem letzten erfolgreichen SemanticSnapshot bestehen, weil auch fehlerhafte, unvollständige oder neu hinzugefügte Sources Imports enthalten können.

### 16.2 Datenmodell

```cpp
struct SourceDependency {
    std::string sourceUri;
    std::string importedModel;
    std::string targetUri;
    bool resolved = false;
    SourceRange range;
};

struct RootClosure {
    std::vector<std::string> orderedUris;
    std::vector<std::string> unresolvedModels;
    std::vector<Diagnostic> diagnostics;
    bool complete = false;
};
```

### 16.3 `SourceDependencyIndex`

```cpp
class SourceDependencyIndex final {
public:
    void updateSource(
        const SourceIdentity& identity,
        const HeaderSnapshot& header,
        const SourceModelIndex& models
    );

    void removeSource(const std::string& uri);

    RootClosure closure(
        const std::vector<std::string>& roots,
        const CompilerOptions& options
    ) const;

    std::vector<std::string> directDependencies(
        const std::string& uri
    ) const;

    std::vector<std::string> reverseDependants(
        const std::string& uri
    ) const;

    std::vector<std::string> rootsAffectedBy(
        const SourceChangeImpact& impact
    ) const;
};
```

### 16.4 Vorwärts- und Rückwärtskanten

Der Index MUSS beide Richtungen halten:

```text
source URI -> importiertes Modell -> target URI

target URI -> abhängige source URIs
```

Zusätzlich MUSS er unaufgelöste Modellnamen halten:

```text
missing model name -> source URIs, die darauf warten
```

Wird eine neue Source hinzugefügt, deren Header einen zuvor fehlenden Modellnamen exportiert, müssen wartende Roots invalidiert werden.

### 16.5 Closure-Reihenfolge

Die Closure muss eine deterministische Reihenfolge liefern:

- Dependencies vor Dependants;
- Rootreihenfolge erhalten;
- gleiche Datei nur einmal;
- Builtin `INTERLIS` an definierter Stelle;
- Zyklen diagnostizieren;
- keine Hash-Iterationsreihenfolge exponieren.

### 16.6 Fehlerhafte Sources

Eine fehlerhafte Source kann trotzdem verwertbare Imports besitzen.

Der Index SOLL tolerante Header-/Importdaten verwenden. Bei Unsicherheit:

- bekannte Kanten behalten, wenn sie eindeutig sind;
- potentiell betroffene Roots konservativ invalidieren;
- keine veraltete Zielauflösung als sicher behandeln.

### 16.7 Mehrere Kandidaten für ein Modell

Existieren mehrere Sources für denselben Modellnamen:

- bestehende Compilerpriorität erhalten;
- keine zufällige Auswahl;
- Konflikt diagnostizieren, falls bestehender Compilervertrag dies verlangt;
- Auswahlentscheidung in den Rootanalyse-Schlüssel aufnehmen;
- Änderung der Kandidatenmenge invalidiert betroffene Roots.

---

## 17. Änderungsauswirkung

### 17.1 `SourceChangeImpact`

```cpp
struct SourceChangeImpact {
    SourceUpdateKind kind;
    std::string uri;
    std::vector<std::string> previousModels;
    std::vector<std::string> currentModels;
    std::vector<std::string> previousImports;
    std::vector<std::string> currentImports;
    std::uint64_t previousContentRevision = 0;
    std::uint64_t currentContentRevision = 0;
};
```

### 17.2 Impact-Berechnung

Der Impact MUSS atomar aus altem und neuem Headerzustand berechnet werden.

Es müssen mindestens unterschieden werden:

- nur Textinhalt innerhalb einer Source, Modell-/Importset unverändert;
- neue oder entfernte Imports;
- umbenanntes Modell;
- neue oder entfernte Source;
- VersionOnly;
- zuvor unparsebare Source wird parsebar;
- zuvor parsebare Source wird unvollständig;
- Änderung eines Repositorymodells unter stabiler URI.

### 17.3 Invalidierungsregeln

#### VersionOnly

- kein fachlicher Rootcache-Eintrag wird verworfen;
- Materialisierung muss aktuelle Dokumentversionen verwenden;
- ausstehende Requests mit alter Version werden verworfen.

#### ContentChanged bei unverändertem Header

- Roots invalidieren, deren Closure URI enthält;
- andere Roots bleiben gültig;
- Parserartefakte anderer URIs bleiben gültig.

#### Importset geändert

- Roots mit URI in Closure invalidieren;
- Closure neu planen;
- Roots, die durch entfernte/exportierte Modelle indirekt betroffen sind, konservativ prüfen.

#### Modellname geändert

- Roots, die alten oder neuen Namen importieren, invalidieren;
- SourceModelIndex atomar aktualisieren;
- keine Rootanalyse darf zwischen Entfernen und Hinzufügen des Indexzustands laufen.

#### Source entfernt

- alle bekannten Roots mit URI in Closure invalidieren;
- wartende/unresolved Zustände aktualisieren;
- andere Roots bleiben gültig.

#### Repositorykonfiguration gewechselt

P4 soll konkrete Source-Diffs liefern. Nur tatsächlich hinzugefügte, entfernte oder geänderte Sources werden angewendet.

Ist nur ein undifferenzierter Repositorywechsel bekannt, darf konservativ alles invalidiert werden, muss aber als `ConservativeRepositoryReset` gemessen und dokumentiert werden.

---

## 18. Rootanalyse-Schlüssel

### 18.1 `NormalizedCompilationRequest`

```cpp
struct NormalizedCompilationRequest {
    std::vector<std::string> roots;
    bool autoSearch = true;
    bool warningsAsErrors = false;
    std::vector<std::string> modelDirectories;
    std::vector<CompilationRequest::ExternalMetaAttribute> externalMetaAttributes;
};
```

Normalisierung:

- Rootreihenfolge nur dann sortieren, wenn aktuelle Semantik reihenfolgeunabhängig ist;
- Duplikate kontrolliert behandeln;
- Pfade normalisieren ohne Symlinksemantik zu verändern;
- External Meta Attributes deterministisch serialisieren;
- keine localeabhängige Sortierung;
- unbekannte Optionen dürfen nicht still aus dem Schlüssel fallen.

### 18.2 `RootAnalysisKey`

```cpp
struct RootAnalysisKey {
    std::string requestFingerprint;
    std::vector<SourceContentKey> closure;
    std::string compilerVersion;
    std::string grammarFingerprint;
    std::string builtinFingerprint;
};

struct SourceContentKey {
    std::string uri;
    std::uint64_t contentRevision;
    std::string contentHash;
    std::size_t byteLength;
};
```

Dokumentversionen gehören nicht in den fachlichen Inhaltskey. Sie werden bei Resultatmaterialisierung aktualisiert.

### 18.3 Builtins

Builtin-Modelle müssen in den Schlüssel eingehen.

Eine Änderung am eingebauten `INTERLIS`-Modell muss Cache-Misses verursachen, auch wenn keine registrierte Source geändert wurde.

### 18.4 Compiler-/Grammar-Fingerprint

Mindestens:

- Compiler-Version;
- ABI-/Schema-Version;
- generierte Grammar-Version oder Build-Fingerprint;
- relevante Featureflags.

Ein Cacheeintrag darf nicht über inkompatible Buildzustände hinweg verwendet werden.

---

## 19. `CompilationPlanner`

### 19.1 Aufgabe

Der Planner entscheidet vor einer Kompilation:

- Rootset;
- Closure;
- unresolved Models;
- benötigte Parserartefakte;
- Rootcache-Hit oder Miss;
- betroffene Sources;
- topologische Reihenfolge;
- aktuelle Sourcegeneration;
- erwartete Dokumentversionen.

### 19.2 Datenmodell

```cpp
enum class CompilationPlanKind {
    ExactCacheHit,
    RebuildWithParseReuse,
    ColdBuild,
    FailedPlanning,
    Cancelled
};

struct CompilationPlan {
    CompilationPlanKind kind = CompilationPlanKind::FailedPlanning;
    NormalizedCompilationRequest request;
    RootAnalysisKey key;
    RootClosure closure;
    std::map<std::string,std::uint64_t> documentVersions;
    std::uint64_t sourceGeneration = 0;
    std::vector<std::string> parseHits;
    std::vector<std::string> parseMisses;
    std::vector<std::string> reasons;
};
```

### 19.3 Klasse

```cpp
class CompilationPlanner final {
public:
    CompilationPlanner(
        TrackedSourceStore& sources,
        ParsedSourceCache& parsed,
        SourceModelIndex& models,
        SourceDependencyIndex& dependencies,
        RootAnalysisCache& analyses
    );

    CompilationPlan plan(const CompilationRequest& request);
};
```

### 19.4 Planung darf Parserartefakte aufbauen

Um Header und Closure zu bestimmen, darf der Planner für bisher unbekannte Inhaltsrevisionen Parserartefakte erzeugen.

Diese Builds werden als Parsermiss gezählt und später in derselben Kompilation wiederverwendet.

### 19.5 Planstabilität

Ein Plan gilt nur für die darin gespeicherte `sourceGeneration`.

Vor Publikation eines Resultats muss geprüft werden:

```cpp
if (workspace.currentGeneration() != plan.sourceGeneration) {
    // Resultat nicht publizieren oder in Cache einsetzen.
}
```

Da der aktuelle `CompilerSession`-Mutex Operationen serialisiert, kann Generation während eines synchronen nativen Calls normalerweise nicht wechseln. Die Prüfung bleibt dennoch erforderlich für zukünftige Cancellation und asynchrone Adapter sowie für klare Tests.

---

## 20. Rootanalyse-Cache

### 20.1 Kein Cache fertiger request-spezifischer Zeitwerte

Der aktuelle Compilertranscript enthält einen Abschlusszeitpunkt. Ein Exact Hit darf nicht unbemerkt einen alten Laufzeitstempel als neuen manuellen Compile ausgeben.

Daher soll der Cache ein zeitunabhängiges fachliches Artefakt halten.

```cpp
struct CachedRootAnalysis {
    RootAnalysisKey key;
    CompilationCoreResult compilation;
    SemanticSnapshotTemplate semantic;
    std::vector<SyntaxSnapshotTemplate> syntax;
    std::vector<std::string> closureUris;
    std::size_t retainedBytes = 0;
};
```

Eine `CompilationResultMaterializer`-Komponente erzeugt daraus den sichtbaren Run:

```cpp
class CompilationResultMaterializer final {
public:
    CompilationAnalysisResult materialize(
        const CachedRootAnalysis& cached,
        const TrackedSourceStore& sources,
        const CompilationMaterializationContext& context
    ) const;
};
```

### 20.2 `RootAnalysisCache`

```cpp
class RootAnalysisCache final {
public:
    explicit RootAnalysisCache(RootAnalysisCacheOptions options = {});

    std::shared_ptr<const CachedRootAnalysis> find(
        const RootAnalysisKey& key
    );

    void publish(std::shared_ptr<const CachedRootAnalysis> analysis);

    void invalidateUris(const std::vector<std::string>& uris);
    void invalidateRoots(const std::vector<std::string>& roots);
    void clear();
    void trim();

    RootAnalysisCacheStats stats() const noexcept;
};
```

### 20.3 Reverse Lookup

Zur gezielten Invalidierung muss der Cache mindestens abbilden:

```text
URI -> RootAnalysisKey-Einträge, deren Closure URI enthält
Root URI -> RootAnalysisKey-Einträge
unresolved model -> wartende RootAnalysisKey-Einträge
```

### 20.4 Cache auch für fehlgeschlagene Resultate

Deterministische fachlich fehlgeschlagene Resultate dürfen gecacht werden, beispielsweise:

- Syntaxfehler;
- unbekannter Typ;
- fehlendes Modell bei unverändertem Workspace;
- Duplicate Name.

Nicht gecacht werden dürfen:

- Cancellation;
- interner Compilerfehler;
- Out-of-memory;
- fataler ABI-Fehler;
- Resultat einer überholten Sourcegeneration;
- transienter Repository-/Transportfehler, sofern dieser ausserhalb der Sourceclosure liegt und bei gleichem Input anders ausfallen kann.

### 20.5 Budget und Eviction

```cpp
struct RootAnalysisCacheOptions {
    std::size_t maxEntries = 64;
    std::size_t maxRetainedBytes = 512 * 1024 * 1024;
};
```

- deterministische LRU;
- `lastGood` im Language Service ist kein Grund, native Cacheeinträge unendlich zu pinnen;
- öffentliche Snapshots besitzen ihre Daten selbst;
- Eviction darf UI-Snapshots nicht ungültig machen;
- Rootcache-Eviction darf Parsercache unabhängig lassen.

---

## 21. Kompilationsausführung mit Parser-Reuse

### 21.1 `IncrementalCompileExecutor`

```cpp
class IncrementalCompileExecutor final {
public:
    IncrementalCompileExecutor(
        TrackedSourceStore& sources,
        ParsedSourceCache& parsed
    );

    std::shared_ptr<const CachedRootAnalysis> execute(
        const CompilationPlan& plan
    );
};
```

### 21.2 Run-lokaler CompilerContext bleibt

Für einen Cache-Miss darf weiterhin ein neuer `CompilerContext` erzeugt werden.

P5 MUSS nicht mutierbare Metamodellobjekte zwischen Runs teilen.

Der Context MUSS aber Zugriff auf einen Parserartefakt-Provider erhalten:

```cpp
class ParsedSourceProvider {
public:
    virtual ~ParsedSourceProvider() = default;
    virtual std::shared_ptr<const ParsedSourceArtifact> get(
        const std::string& uri
    ) = 0;
};
```

`CompilerContext` beispielhaft:

```cpp
CompilerContext(
    TrackedSourceStore& sessionSources,
    const CompilerOptions& options,
    ParsedSourceProvider& parsedSources
);
```

### 21.3 `IliFileCatalog`

Der Katalog muss Headerinformationen aus `ParsedSourceArtifact::header()` beziehen.

Er darf nicht erneut `IliFileLexer`/`IliFileParser` über die Source laufen lassen.

Mögliche Schnittstelle:

```cpp
class IliFileCatalog final {
public:
    IliFileCatalog(
        CompilationSourceStore& sources,
        ParsedSourceProvider& parsed,
        Logger& logger,
        const CompilerOptions& options
    );

    IliFile* loadByFile(const std::string& uri);
    IliFile* loadByModel(
        const std::string& model,
        const std::string& iliVersion
    );
};
```

`IliFile` darf einen `shared_ptr<const ParsedSourceArtifact>` oder einen lebensdauersicheren Handle halten, nicht einen nackten Pointer auf einen evizierbaren Eintrag.

### 21.4 `compileFile`

Statt erneut zu parsen:

```cpp
artifact->buildMetaModel(context.builder(), context.logger());
```

oder sprachspezifisch:

```cpp
artifact->visitIli1(builder, logger);
artifact->visitIli2(builder, logger);
```

### 21.5 Visitor-Anpassung

Bestehende Visitor-Konstruktoren dürfen erhalten bleiben. Es sollen neue Einstiegsmethoden eingeführt werden:

```cpp
void buildIli2MetaModel(
    const ParsedSourceArtifact& artifact,
    metamodel::MetaModelBuilder& builder,
    util::Logger& logger
);

void buildIli1MetaModel(
    const ParsedSourceArtifact& artifact,
    metamodel::MetaModelBuilder& builder,
    util::Logger& logger
);
```

Diese Methoden dürfen keinen neuen Lexer oder Parser erzeugen.

### 21.6 Diagnostics pro Run

Parserdiagnosen stammen aus dem Artefakt und müssen in den run-lokalen Logger beziehungsweise das Resultat übernommen werden.

Sie dürfen nicht durch Wiederbesuch dupliziert werden.

Semantische Diagnosen bleiben run-lokal.

### 21.7 External Meta Attributes

External Meta Attributes werden nach dem Metamodellaufbau angewendet.

Sie gehören in den Rootanalyse-Schlüssel, aber nicht in den Parsercacheschlüssel.

### 21.8 warningsAsErrors

`warningsAsErrors` gehört in den Rootanalyse-Schlüssel.

Ein Parserartefakt darf unabhängig davon wiederverwendet werden. Die Materialisierung von `treatedAsError`, Erfolg und Counts muss dem bestehenden Compilervertrag entsprechen.

---

## 22. SemanticSnapshot-Erzeugung

### 22.1 Kein erneutes Syntaxparsing

`buildSemanticSnapshot()` darf nicht mehr für jede erreichbare URI `parseSyntax()` ausführen.

Es muss die bereits vorhandenen Parser-/Syntaxartefakte verwenden.

Mögliche Signatur:

```cpp
SemanticSnapshot buildSemanticSnapshot(
    const CompilationRequest& request,
    const CompilationResult& compilation,
    const CompilationPlan& plan,
    const ParsedSourceProvider& parsed,
    const metamodel::MetaModelStore* metaModels,
    std::vector<SyntaxSnapshot>* syntaxSnapshots
);
```

### 22.2 Dependencies

Dependencykanten sollen aus dem `SourceDependencyIndex` beziehungsweise den Headerartefakten stammen.

Ranges müssen weiterhin aus den nativen Syntaximportreferenzen stammen.

### 22.3 Dokumentversionen

`SemanticSnapshot.documentVersions` muss bei Materialisierung die aktuellen Dokumentversionen der inhaltlich identischen Closure enthalten.

Ein VersionOnly-Update darf daher einen Exact Hit liefern und trotzdem die neue Dokumentversion ausgeben.

### 22.4 Symbol- und Referenz-IDs

P5 darf bestehende IDs nicht unnötig ändern.

IDs dürfen nicht von:

- Pointerwerten;
- Cachepositionen;
- ContentRevision;
- LRU-Zustand;
- Workerinstanz

abhängen.

### 22.5 Diagramm und Dokumentation

Diagramm- und Dokumentationsprojektionen gehören zum fachlichen Rootanalyseartefakt.

Sie dürfen bei Exact Hit wiederverwendet werden.

Der Language Service entscheidet weiterhin, ob sie als `fresh`, `stale` oder `lastGood` sichtbar sind.

---

## 23. Resultatmaterialisierung

### 23.1 `CompilationMaterializationContext`

```cpp
struct CompilationMaterializationContext {
    std::string runTimestamp;
    bool cacheHit = false;
    IncrementalMetricsSnapshot metrics;
};
```

### 23.2 Transcript

Der sichtbare Transcript muss mit bestehender Struktur kompatibel bleiben.

Ein optionaler Debughinweis wie:

```text
inf: incremental cache hit
```

ist standardmässig zu vermeiden, weil er Golden Tests und Benutzeroutput verändert. Solche Informationen gehören in separate Metriken oder Debuglogs.

Der Abschlusszeitpunkt wird für jeden sichtbaren Compile neu erzeugt.

### 23.3 Logs

Cacheinterne Logs dürfen nicht die fachlichen Error-/Warning-Counts verändern.

Wenn Debugtracing aktiviert ist, muss es eine getrennte Kategorie verwenden, beispielsweise:

```text
incremental
```

### 23.4 Syntaxreihenfolge

`CompilationAnalysisResult.syntax` muss dieselbe deterministische Reihenfolge wie Cold Compilation liefern.

---

## 24. `IncrementalCompilerWorkspace`

### 24.1 Hauptaggregat

```cpp
class IncrementalCompilerWorkspace final {
public:
    IncrementalCompilerWorkspace(
        IncrementalCompilerOptions options = {}
    );

    SourceUpdateResult updateSource(
        std::string uri,
        std::string utf8,
        std::uint64_t version
    );

    SourceUpdateResult removeSource(const std::string& uri);

    SyntaxSnapshot syntax(const std::string& uri);
    EditorSnapshot editor(const std::string& uri);

    CompilationAnalysisResult compileAndAnalyze(
        const CompilationRequest& request
    );

    IncrementalStats stats() const;
    IncrementalTrace lastTrace() const;

    void clearAnalysisCache();
    void clearParserCache();
    void clearAllCaches();
};
```

### 24.2 Enthaltene Komponenten

- `TrackedSourceStore`;
- `ParsedSourceCache`;
- `IncrementalSnapshotService`;
- `SourceModelIndex`;
- `SourceDependencyIndex`;
- `CompilationPlanner`;
- `RootAnalysisCache`;
- `IncrementalCompileExecutor`;
- Metrik- und Tracezustand.

### 24.3 CompilerSession-Delegation

`CompilerSession::Impl` soll sinngemäss werden:

```cpp
struct CompilerSession::Impl {
    IncrementalCompilerWorkspace workspace;
    std::mutex mutex;
};
```

Öffentliche Methoden delegieren unter dem bestehenden Sessionlock.

### 24.4 Direkter `sources()`-Zugriff

Der heutige öffentliche mutable Zugriff auf `SourceManager& sources()` umgeht Updateklassifikation.

P5 MUSS diesen Vertrag analysieren.

Bevorzugt:

- als deprecated markieren;
- intern nicht mehr verwenden;
- direkte Mutation in Tests/Legacy erkennen;
- nach direkter Mutation konservativ Cachezustand invalidieren.

Falls API-Kompatibilität den Zugriff zwingend erhält, MUSS ein Mutation-Generation-Mechanismus verhindern, dass Caches nach externer Mutation fälschlich gültig bleiben.

Eine unbemerkte direkte Mutation ist nicht zulässig.

---

## 25. Öffentliche C++-API

### 25.1 Bestehende Methoden bleiben

```cpp
void putSource(std::string uri, std::string utf8, std::uint64_t version = 0);
bool removeSource(const std::string& uri);
SyntaxSnapshot parse(const std::string& uri);
EditorSnapshot editorSnapshot(const std::string& uri);
SemanticSnapshot analyze(const CompilationRequest& request);
CompilationAnalysisResult compileAndAnalyze(const CompilationRequest& request);
CompilationResult compile(const CompilationRequest& request);
```

### 25.2 Additive Methoden

Empfohlen:

```cpp
SourceUpdateResult updateSource(
    std::string uri,
    std::string utf8,
    std::uint64_t version = 0
);

IncrementalStats incrementalStats() const;
IncrementalTrace lastIncrementalTrace() const;
void clearIncrementalCaches();
```

`putSource()` darf als kompatibler Wrapper `updateSource()` aufrufen und das Resultat verwerfen.

### 25.3 `IncrementalStats`

```cpp
struct IncrementalStats {
    std::uint64_t sourceAdds = 0;
    std::uint64_t sourceRemoves = 0;
    std::uint64_t sourceNoOps = 0;
    std::uint64_t versionOnlyUpdates = 0;
    std::uint64_t contentChanges = 0;

    std::uint64_t parserBuilds = 0;
    std::uint64_t parserHits = 0;
    std::uint64_t parserEvictions = 0;

    std::uint64_t syntaxMaterializations = 0;
    std::uint64_t editorMaterializations = 0;

    std::uint64_t rootAnalysisHits = 0;
    std::uint64_t rootAnalysisMisses = 0;
    std::uint64_t rootAnalysisEvictions = 0;
    std::uint64_t invalidatedRootEntries = 0;

    std::uint64_t coldCompilations = 0;
    std::uint64_t rebuildsWithParseReuse = 0;
    std::uint64_t cancelledPlans = 0;
};
```

### 25.4 Trace

```cpp
struct IncrementalTrace {
    std::string operation;
    std::string planKind;
    std::vector<std::string> roots;
    std::vector<std::string> closure;
    std::vector<std::string> parserHits;
    std::vector<std::string> parserMisses;
    std::vector<std::string> invalidatedRoots;
    std::vector<std::string> reasons;
};
```

Trace ist für Tests und Debugging bestimmt. Keine Sourceinhalte oder Secrets aufnehmen.

---

## 26. C-ABI

### 26.1 Bestehende ABI bleibt funktionsfähig

Bestehende C-ABI-Aufrufe dürfen ihre Bedeutung nicht verlieren.

### 26.2 Additive Stats-Funktion

Beispielsweise:

```c
ILIC_CAPI_EXPORT uint32_t ilic_incremental_stats(
    uint32_t session,
    const char *request_json,
    size_t request_length
);
```

oder eine passendere bestehende ABI-Konvention.

Resultat JSON:

```json
{
  "schemaVersion": 1,
  "abiVersion": 1,
  "kind": "incremental-stats",
  "sourceAdds": 3,
  "versionOnlyUpdates": 1,
  "parserBuilds": 3,
  "parserHits": 12,
  "rootAnalysisHits": 2,
  "rootAnalysisMisses": 1
}
```

### 26.3 Clear-Funktion

Eine Cache-Clear-Funktion darf additiv eingeführt werden, soll aber nicht im normalen Produktablauf verwendet werden.

### 26.4 ABI-Version

Additive Exports dürfen ABI 1 beibehalten, wenn bestehende Module unverändert funktionieren und Capability Detection verwendet wird.

Eine ABI-Erhöhung ist nur zulässig, wenn tatsächlich inkompatible Layout-/Verhaltensänderungen notwendig sind. Sie darf nicht allein zur Kennzeichnung von P5 erfolgen.

### 26.5 Fehlergrenzen

- keine C++-Exception über C-Grenze;
- ungültige Session kontrolliert;
- Destroy-Lifecycle erhalten;
- keine neuen Handle-Leaks;
- Stats-Aufruf verändert fachlichen Zustand nicht.

---

## 27. WASM- und TypeScript-API

### 27.1 Bestehende High-Level-API bleibt

```ts
session.putSource(uri, source, version);
session.removeSource(uri);
session.parse(uri);
session.editorSnapshot(uri);
session.compileAndAnalyze(request);
session.compile(request);
session.analyze(request);
```

Node-LSP und Web-IDE dürfen nicht gezwungen werden, eine neue Sessionklasse zu verwenden.

### 27.2 Capabilities

```ts
export interface CompilerCapabilities {
  nativeEditorSnapshot: boolean;
  incrementalSession?: boolean;
  incrementalStats?: boolean;
}
```

Neue Felder sind additiv und zunächst optional für alte Mocks.

### 27.3 Stats

```ts
export interface IncrementalStats {
  sourceAdds: number;
  sourceRemoves: number;
  sourceNoOps: number;
  versionOnlyUpdates: number;
  contentChanges: number;
  parserBuilds: number;
  parserHits: number;
  parserEvictions: number;
  syntaxMaterializations: number;
  editorMaterializations: number;
  rootAnalysisHits: number;
  rootAnalysisMisses: number;
  rootAnalysisEvictions: number;
  invalidatedRootEntries: number;
  coldCompilations: number;
  rebuildsWithParseReuse: number;
  cancelledPlans: number;
}
```

Additiv:

```ts
session.incrementalStats?(): IncrementalStats;
session.clearIncrementalCaches?(): void;
```

### 27.4 `putSource`

Die öffentliche Rückgabe bleibt `void`, sofern bestehende API dies verlangt.

Intern darf ein `SourceUpdateResult` ausgewertet werden.

Eine neue additive Methode ist zulässig:

```ts
session.updateSource?(
  uri: string,
  source: string | Uint8Array,
  version?: number,
): SourceUpdateResult;
```

Sie darf kein Consumer-Muss werden.

### 27.5 Kein JavaScript-Fachcache

JavaScript darf keine eigene Entscheidung treffen, ob eine Source semantisch neu geparsed werden muss.

Es darf identische Updates vermeiden, um Kopierkosten zu reduzieren. Die native Session bleibt trotzdem autoritativ.

### 27.6 Sourcekopien

Der Wrapper soll für normale Operationen keinen vollständigen zweiten Sourcebestand nur für Inkrementalitätsentscheidungen halten.

Eine Source-Map ist in Worker-/Recovery-Adaptern zulässig, wenn sie zum Wiederaufbau nach Workercrash benötigt wird.

---

## 28. Compiler-Backend in `interlis-language-tools`

### 28.1 `CompilerBackend`

Erweitere additiv:

```ts
export interface CompilerBackend {
  // bestehende Methoden
  readonly capabilities?: {
    readonly incrementalSession?: boolean;
    readonly incrementalStats?: boolean;
  };

  incrementalStats?(): IncrementalStats | Promise<IncrementalStats>;
  clearIncrementalCaches?(): void | Promise<void>;
}
```

### 28.2 `createWasmCompilerBackend`

Die bestehende Session muss langlebig bleiben.

`restart()` ist nur Recovery. Normale Repositoryauflösung darf `restart()` nicht aufrufen.

Die lokale Source-Map darf für Recovery erhalten bleiben, muss aber:

- nur bei tatsächlichem Restart replayen;
- Replayzähler führen;
- keine normalen Cacheentscheidungen treffen;
- identische `putSource`-Aufrufe möglichst vermeiden;
- Versionsupdates korrekt weitergeben.

### 28.3 Repository-Nachladen

In `LanguageService.#resolveMissingModels()` muss nach erfolgreicher Sourceinstallation direkt erneut kompiliert werden:

```ts
this.#putRepositorySource(source);
analysis = await this.compiler.compileAndAnalyze({ roots: [rootUri] });
```

Folgendes ist im normalen Pfad zu entfernen:

```ts
await this.compiler.restart?.();
```

Ein Test MUSS nachweisen:

- genau die neu aufgelösten Sources werden mit `putSource` registriert;
- kein Restart;
- kein Replay bereits vorhandener Sources;
- zweite Kompilation verwendet Parserhits für vorhandene Sources.

### 28.4 `AnalysisCache`

Der bestehende TypeScript-`AnalysisCache` darf nicht zu einer zweiten fachlichen Cacheautorität werden.

Optionen:

1. entfernen, wenn ungenutzt;
2. als reinen UI-/Request-Deduplizierungscache dokumentieren;
3. auf native Resultatidentitäten verweisen.

Er darf nicht unabhängig entscheiden, dass ein semantisches Resultat trotz nativer Invalidierung gültig ist.

---

## 29. LanguageService-Integration

### 29.1 Bestehende Zuständigkeit

Der Language Service bleibt verantwortlich für:

- Open-/Dirty-/Saved-Zustände;
- Sourcepriorität;
- Scheduling;
- Trigger;
- UI-Freshness;
- `current`, `lastGood`, `saved`;
- Eventpublikation;
- Diagnostics-Merging;
- Worker-Recovery;
- Repository-Orchestrierung.

Der native Compiler wird verantwortlich für:

- Inhaltsrevisionen;
- Parsercache;
- Header-/Dependencyindex;
- Rootanalysecache;
- fachliche Invalidierung;
- Compilation Plan.

### 29.2 Keine parallele fachliche Wahrheit

`#reverseDependencies` darf als UI-Helfer erhalten bleiben, muss aber aus akzeptierten nativen SemanticSnapshots abgeleitet werden.

Es darf nicht verwendet werden, um einen nativen Cachehit zu überstimmen oder eine native Invalidierung zu verhindern.

### 29.3 Source-Revisions

Serviceeigene Versionen bleiben nötig, um Browser-/LSP-Quellen zu koordinieren.

Der Service soll identische Inhalte weiterhin nicht erneut an den Compiler senden, sofern zuverlässig erkannt.

Der native Compiler muss identische Updates dennoch korrekt als No-op klassifizieren, falls sie gesendet werden.

### 29.4 Freshness

Die bestehende Definition bleibt:

```ts
type ResultFreshness = "fresh" | "stale" | "cancelled";
```

Ein nativer Exact Hit kann ein `fresh`-Resultat sein, sofern sein Inhaltskey aktuell ist und die Materialisierung aktuelle Dokumentversionen enthält.

### 29.5 Events

`AnalysisEvent.affectedUris` soll die tatsächlich betroffene Closure beziehungsweise die vom Compiler gemeldete Closure verwenden.

Keine nicht betroffenen Roots oder URIs dürfen nur wegen eines globalen Generationincrements als geändert gemeldet werden.

### 29.6 Invalidation

`#invalidateSource()` soll weiterhin sichtbare Snapshots unverzüglich als stale markieren.

Es darf aber keine globale Invalidierung auslösen, wenn der native Compiler und vorhandene Versionsvektoren die Auswirkung eindeutig bestimmen.

### 29.7 Compilation Queue

Die bestehende Priorität bleibt:

```text
manual
save
diagram
dependency
open/startup
```

P5 darf Superseding und Deduplizierung verbessern, aber:

- manuelle Runs nicht still verwerfen;
- alte automatische Runs nicht publizieren;
- Rootisolation erhalten.

### 29.8 Resultatannahme

Ein Resultat wird nur akzeptiert, wenn:

- Run noch aktuell;
- `compilationEpoch` aktuell;
- Rootsource-Version aktuell;
- alle vom SemanticSnapshot gemeldeten Dokumentversionen aktuell;
- kein neuerer automatischer Run für Root existiert;
- Resultat nicht cancelled.

Native Cachehits ändern diese Regeln nicht.

---

## 30. Worker-Protokoll

### 30.1 Bestehende Messages bleiben

```text
putSource
removeSource
compileAndAnalyze
editorSnapshot
dispose
```

### 30.2 Additive Messages

Zulässig:

```ts
| { id: number; method: "incrementalStats" }
| { id: number; method: "clearIncrementalCaches" }
```

Diese dürfen bestehende Worker nicht unverständlich brechen. Capability Detection oder verständlicher Fehler ist erforderlich.

### 30.3 Queue

`runCompilerWorker()` serialisiert Calls weiterhin pro Worker.

P5 soll optionale Coalescing-Regeln im Adapter implementieren:

- mehrere noch nicht verarbeitete `putSource` für gleiche URI: nur neueste Version vor nächster Analyse senden;
- `removeSource` nach pending `putSource`: finalen Zustand erhalten;
- `compileAndAnalyze` bildet eine Barriere;
- keine Sourceänderung über eine Barriere hinweg verschieben;
- Responses behalten Request-ID.

### 30.4 Worker-Recovery

Bei Workercrash:

1. pending Requests kontrolliert ablehnen;
2. Worker ersetzen;
3. alle aktuell effektiven Sources einmal replayen;
4. Replaygrund und Sourceanzahl messen;
5. keine alten Resultate publizieren;
6. danach inkrementelle Session neu aufbauen.

### 30.5 Normale Operationen

Normale Sourceupdates oder Repositoryauflösung dürfen nicht:

- Worker terminieren;
- neuen Worker erzeugen;
- sämtliche Sources replayen;
- `restart()` auslösen.

### 30.6 Getrennte Editor- und Compile-Worker

P5 darf die Trennung beibehalten.

Sie ermöglicht Live-Editoranalyse, während der Compile-Worker beschäftigt ist.

Die Spezifikation verlangt nicht, beide Worker zu fusionieren. Jede Session besitzt eigene Cachebudgets.

### 30.7 Local Fallback

Das lokale Backend darf als Fallback erhalten bleiben.

Bei einem Workerfehler muss klar sein, ob der lokale Fallback bereits aktuelle Sources enthält. Sourceversions- und Replaytests sind erforderlich.

---

## 31. LSP-Integration

### 31.1 Text Sync

`TextDocumentSyncKind.Full` darf in P5 erhalten bleiben.

Native Inkrementalität basiert auf Sourceinhalten und Dokumentversionen und benötigt keine LSP-Deltas.

Eine Umstellung auf inkrementelle LSP-Textänderungen ist nicht Teil von P5.

### 31.2 Öffentliche Protokolle

Bestehende INTERLIS-Protokollmethoden und Notifications bleiben kompatibel:

- compile;
- compilationCompleted;
- semanticSnapshotChanged;
- workspaceSources;
- workspaceSourceChanged;
- repositoryConfiguration;
- liveAnalysisStatus;
- diagramSnapshot;
- repositorySource.

### 31.3 Optionale Debugdiagnostik

Ein optionaler Debugrequest für Inkrementalitätsmetriken darf eingeführt werden, darf aber nicht Teil normaler UI-Logik sein.

### 31.4 Save-Verhalten

`didSave` bleibt:

```text
markSaved
compileDocument(root, "save")
```

Der Compile darf intern Exact Hit liefern, falls der gespeicherte Inhalt bereits analysiert wurde und fachlich identisch ist.

### 31.5 DidChange

`didChange` aktualisiert weiterhin den Dirty-Buffer und startet keine Vollkompilation.

Es darf nur Editor-Snapshot-Arbeit auslösen.

### 31.6 Diagnostics

Die Diagnostics-Publikation bleibt root- und URI-korrekt.

Ein Cachehit darf nicht zu doppelten Problems führen.

### 31.7 Node-LSP-Kompatibilität

Der Node-LSP darf nicht gezwungen werden:

- neue LSP-Methoden aufzurufen;
- Source-Deltas zu senden;
- CompilerSession direkt zu verwalten;
- Cachekeys zu bilden;
- native Metriken auszuwerten.

---

## 32. Repository-Integration nach P4

### 32.1 Stable Repository URI

Eine Repository-Source wird über ihre stabile virtuelle oder originäre URI identifiziert.

Der Inhalt bestimmt die Inhaltsrevision.

### 32.2 Sourceaddition

Nach Auflösung eines fehlenden Modells:

- Source direkt registrieren;
- SourceModelIndex aktualisieren;
- unresolved watchers prüfen;
- betroffene Rootanalyse invalidieren;
- Compile erneut planen;
- bestehende Parserartefakte anderer Sources wiederverwenden.

### 32.3 Repositorymodell geändert

Gleiche URI, neue Bytes:

- `ContentChanged`;
- betroffene Roots invalidieren;
- kein globaler Reset;
- read-only/UI-Eigenschaft bleibt.

### 32.4 Nur Cachemetadaten geändert

Ändert sich `fromCache`, `stale` oder ein lokaler Cachepfad ohne Sourcebytesänderung, darf der Compiler keine fachliche Invalidierung auslösen.

### 32.5 Repositorykonfiguration

Eine neue Konfiguration soll über Source-Diffs angewendet werden.

Der Language Service darf Repositoryquellen entfernen, die nicht mehr erreichbar sein sollen. Nur betroffene Roots werden stale.

### 32.6 Browser-URI-Vertrag

Die in der Web-IDE verwendeten `interlis-repository:`-URIs müssen stabil bleiben, damit:

- Tabs;
- Diagnostics;
- Navigation;
- Cacheidentität;
- Dependencykanten

nicht bei jeder Auflösung neue URIs erhalten.

---

## 33. `interlis-web-ide`: Workspace-Source-Synchronisierung

### 33.1 Problem

`#syncWorkspaceSources()` kann den vollständigen Workspace scannen. Der Language Service filtert identische Inhalte, aber OPFS-/Local-Folder-I/O und Sourceobjekterzeugung bleiben unnötig.

### 33.2 `WorkspaceSourceSynchronizer`

Implementiere eine klar abgegrenzte Klasse, beispielsweise:

```ts
interface WorkspaceSourceFingerprint {
  readonly uri: string;
  readonly size: number;
  readonly hash: string;
  readonly version: number;
}

interface WorkspaceSourceDiff {
  readonly added: readonly WorkspaceSource[];
  readonly changed: readonly WorkspaceSource[];
  readonly removed: readonly string[];
  readonly unchanged: readonly string[];
}

class WorkspaceSourceSynchronizer {
  constructor(
    private readonly service: LanguageService,
    private readonly uriForPath: (path: string) => string,
  ) {}

  syncAll(fileSystem: WorkspaceFileSystem): Promise<WorkspaceSourceDiff>;
  put(path: string, bytes: Uint8Array): Promise<void>;
  remove(path: string): void;
  reset(): void;
}
```

### 33.3 Integration

Der Synchronizer soll verwendet werden bei:

- Startup;
- Workspacewechsel;
- Dateiimport;
- ZIP-Import;
- Delete;
- Local Folder Refresh;
- Git-Operationen, die Working Tree verändern;
- Recovery, falls Hintergrundsource geändert wird.

### 33.4 Save

Beim Save eines offenen Modells ist der Editorbuffer bereits autoritativ und im Language Service registriert.

Der anschliessende Workspacewrite darf nicht dieselbe Source erneut invalidieren.

### 33.5 Full Rescan als Recovery

Ein vollständiger Scan bleibt zulässig:

- Startup;
- Workspacewechsel;
- externer Folderzustand unbekannt;
- Git Checkout/Reset mit unbekannter Diffmenge;
- Watcher-/Permission-Recovery.

Er muss einen Diff erzeugen und nur Änderungen an den Language Service weitergeben.

### 33.6 Kein zusätzlicher fachlicher Cache

Der Synchronizer cached nur Dateifingerprints und vermeidet I/O/Updates. Er entscheidet nicht über Parser- oder SemanticSnapshot-Gültigkeit.

---

## 34. `interlis-web-ide`: sichtbares Verhalten

### 34.1 Tippen

Beim Tippen:

- Status bleibt `outdated — save or compile`;
- gespeichertes Diagramm wird stale markiert;
- kein Full Compile;
- ein neuer EditorSnapshot für aktuelle Version;
- unveränderte andere EditorSources werden nicht geparsed;
- Suggestions und Outline folgen aktuellem EditorSnapshot.

### 34.2 Save

Beim Save:

- Workspacewrite;
- `markSaved`;
- Rootcompile;
- betroffene Rootclosure neu analysieren oder Exact Hit;
- Output/Problems atomar aktualisieren;
- gültiges Diagramm neu publizieren;
- nicht betroffene Rootzustände bleiben frisch.

### 34.3 Manual Compile

Manueller Compile erzeugt weiterhin ein sichtbares neues CompilationEvent und einen neuen Timestamp, auch bei fachlichem Rootcache-Hit.

### 34.4 Diagramm

- `lastGood` bleibt erhalten;
- stale Diagramm bleibt sichtbar;
- nur akzeptiertes gültiges Resultat ersetzt Diagramm;
- Dependency-Trigger nur für tatsächlich betroffene offene Diagramme;
- Rootcache-Hit darf Layout neu auslösen, wenn UI dies heute tut;
- Viewportanker bleiben erhalten.

### 34.5 DOCX

DOCX verwendet weiterhin gespeicherten SemanticSnapshot.

P5 darf Dirty-Editor-Snapshot nicht als vollständigen DOCX-Input verwenden.

### 34.6 Repositorytabs

Read-only bleibt. Sourceaddition oder Cachehit darf keinen doppelten Tab erzeugen.

### 34.7 PWA

WASM-, Worker- und JavaScript-Assets müssen aus konsistentem Paketstand stammen.

Ein Service-Worker-Update darf keine alte Workerdatei mit neuer WASM-ABI mischen.

---

## 35. Kompatibilitätsvertrag

### 35.1 `ilic-fork`

Bestehende C++-, C-ABI- und JS-Methoden bleiben verwendbar.

### 35.2 `interlis-language-tools`

Bestehende Exporte bleiben:

- `LanguageService`;
- `createWasmCompilerBackend`;
- `createWorkerCompilerBackend`;
- `createWorkerEditorAnalysisBackend`;
- `runCompilerWorker`;
- `CompilerBackend`;
- `EditorAnalysisBackend`;
- LSP-/Monaco-Pakete.

### 35.3 `interlis-web-ide`

`src/main.ts` soll grundsätzlich denselben High-Level-Aufbau behalten können.

Eine kleine interne Anpassung zur Nutzung von Stats oder SourceSynchronizer ist zulässig. Eine fachliche Neustrukturierung der Anwendung ist nicht erforderlich.

### 35.4 Resultat-JSON

Bestehende Felder dürfen nicht entfernt oder umgedeutet werden.

Neue Inkrementalitätsmetadaten sollen über separates Stats-Resultat geliefert werden, nicht durch verpflichtende Felder in jedem SemanticSnapshot.

### 35.5 Mocks und Fixtures

Neue Capability- und Stats-Felder sind zunächst optional in öffentlichen TypeScript-Interfaces, damit bestehende Mocks weiter kompilieren.

Runtime des neuen Compilers muss die Capability korrekt melden.

---

## 36. Cancellation und Generationen

### 36.1 Native Session

Der Sessionmutex serialisiert weiterhin öffentliche Operationen.

Langfristig darf ein Cancellation-Token eingeführt werden:

```cpp
class CompilationCancellation {
public:
    virtual ~CompilationCancellation() = default;
    virtual bool cancelled() const noexcept = 0;
};
```

### 36.2 Cachepublikation

Ein Resultat darf nur in den Rootcache publiziert werden, wenn:

- Plan nicht cancelled;
- Sourcegeneration unverändert;
- kein interner Fehler;
- Resultat vollständig materialisierbar.

### 36.3 Language Service

`compilationEpoch`, Run-ID und Versionsvektor bleiben zusätzliche UI-/Async-Grenzen.

Native Cachegültigkeit ersetzt diese Checks nicht.

### 36.4 Workertermination

Da synchrones WASM während einer Kompilation keine neue Message verarbeitet, darf Cancellation weiterhin durch Workertermination erfolgen.

Danach ist ein Voll-Replay als Recovery zulässig und muss gemessen werden.

### 36.5 Cancelled Cacheeinträge

Cancelled Resultate werden niemals gecacht.

---

## 37. Thread-Safety

### 37.1 Sessionintern

Der bestehende Sessionlock darf erhalten bleiben.

Cacheklassen dürfen darauf vertrauen, nur unter diesem Lock verwendet zu werden, müssen dies aber dokumentieren.

### 37.2 Mehrere Sessions

Mehrere Sessions müssen parallel arbeiten können.

Keine globale Cachemutex.

### 37.3 Öffentliche Snapshots

Nach Rückkehr dürfen Snapshots in anderen Threads gelesen werden, sofern ihre Typen bereits dafür geeignet sind. Sie dürfen keine sessioninternen Referenzen enthalten.

### 37.4 Stats

Stats lesen unter Lock oder über atomaren Snapshot. Keine Datenrennen.

### 37.5 TSan

Mindestens folgende Szenarien:

- zwei Sessions parallel;
- Parse und Compile in getrennten Sessions;
- gleichzeitige Workerinstanzen;
- Cacheeviction während anderer Session arbeitet;
- wiederholte Erstellung/Dispose;
- Crash-Recovery im JS-Adapter.

---

## 38. Speicher- und Eviction-Vertrag

### 38.1 Kein unbeschränktes Halten aller Historien

P5 darf nicht für jede Tastenänderung dauerhaft einen Parse Tree behalten.

Pro URI sollen standardmässig nur aktive und begrenzt wenige zuletzt verwendete Inhaltsrevisionen verbleiben.

### 38.2 Retained Bytes

Jedes Parser- und Rootanalyseartefakt soll seine geschätzten retained bytes melden.

Die Schätzung muss mindestens umfassen:

- Sourcebytes;
- Tokenanzahl;
- Parse Tree grob;
- Snapshotvektoren;
- Strings;
- Semantic-/Diagramm-/Dokumentationsprojektionen.

### 38.3 Eviction nach Dirty Typing

Bei vielen Editorversionen:

- alte Parserartefakte evizieren;
- aktuelle EditorSnapshot-Ausgabe bleibt selbstbesitzend;
- höchstens konfiguriertes Budget;
- kein linearer Heapwachstumstest über tausende Änderungen.

### 38.4 Rootcache

Rootcache-Einträge können gross sein. Eviction muss Root- und Parsercache getrennt behandeln.

### 38.5 Clear

Cache-Clear dient:

- Tests;
- Debugging;
- Recovery;
- Speicherpressure.

Es darf nicht automatisch bei jeder Sourceänderung aufgerufen werden.

---

## 39. Metriken und Tracing

### 39.1 Harte Abnahmemetriken

Tests müssen über Zähler beweisen:

1. erste `editorSnapshot(A)`:
   - `parserBuilds += 1`;
2. anschliessendes `parse(A)`:
   - kein weiterer Parserbuild;
   - `parserHits += 1`;
3. erste Rootkompilation A mit Dependency B:
   - B wird höchstens einmal geparsed;
4. zweite unveränderte Rootkompilation:
   - `rootAnalysisHits += 1`;
   - kein Parserbuild;
5. VersionOnly A:
   - `versionOnlyUpdates += 1`;
   - kein Parserbuild;
   - kein Rootrebuild;
6. ContentChanged A:
   - genau A neu parsen;
   - unverändertes B Parse-Hit;
7. Änderung eines unabhängigen C:
   - Root A bleibt Cachehit;
8. Repositorysource D hinzufügen:
   - kein Sessionrestart;
   - kein Replay A/B/C.

### 39.2 Wall-Clock

Wall-Clock-Benchmarks sind zusätzlich zu reporten:

- Median;
- p95;
- Warmup;
- Anzahl Wiederholungen;
- Hardware/Runtime;
- native und WASM;
- Cold vs Warm.

Keine extrem engen, flakigen CI-Grenzen.

### 39.3 Debugtrace

Ein Trace darf bei Testfehlern enthalten:

```text
operation=compileAndAnalyze
plan=RebuildWithParseReuse
roots=[A]
closure=[B,A]
parserHits=[B]
parserMisses=[A]
invalidatedRootEntries=[A]
reason=content changed: A
```

Keine kompletten Sourceinhalte loggen.

---

## 40. Native Unit-Tests

Mindestens neue Testgruppen:

### 40.1 Sourceupdates

- Added;
- Unchanged;
- VersionOnly;
- ContentChanged;
- Reintroduced;
- Removed;
- stale version rejected;
- Unicode;
- empty source;
- large version values.

### 40.2 ParsedSourceArtifact

- Ili1;
- Ili2.3;
- Ili2.4;
- incomplete source;
- parser diagnostics;
- repeated snapshot materialization;
- repeated metamodel visitor;
- artifact lifetime after source update;
- no mutation across visits.

### 40.3 ParsedSourceCache

- miss then hit;
- version-only hit;
- content change miss;
- eviction;
- byte budget;
- negative parse cache;
- reintroduced same bytes;
- deterministic stats.

### 40.4 Snapshotservice

- syntax then editor one parse;
- editor then syntax one parse;
- both one parse;
- correct documentVersion after VersionOnly;
- parity with uncached P3 baseline;
- unknown URI.

### 40.5 SourceModelIndex

- one model;
- multiple models per file;
- rename;
- duplicate names;
- remove;
- version-specific lookup;
- atomic update.

### 40.6 Dependencyindex

- chain;
- diamond;
- cycle;
- missing model;
- source addition resolves missing;
- import removal;
- model rename;
- unrelated source;
- deterministic closure.

### 40.7 Planner

- ColdBuild;
- RebuildWithParseReuse;
- ExactCacheHit;
- failed closure;
- changed options;
- changed external meta attribute;
- builtin fingerprint;
- stale generation;
- multiple roots.

### 40.8 Rootcache

- exact hit;
- version-only materialization;
- URI invalidation;
- root invalidation;
- unresolved watcher;
- failed deterministic result cached;
- cancellation not cached;
- internal error not cached;
- eviction;
- current timestamp materialized.

### 40.9 Compileexecutor

- dependencies use cached parse trees;
- new run-local MetaModelStore;
- diagnostics not duplicated;
- same result as cold;
- warningsAsErrors;
- external meta attributes;
- multiple sequential mutations.

### 40.10 Sessionisolation

- identical sessions independent stats;
- mutation A does not affect B;
- parallel sessions under TSan;
- dispose frees caches.

---

## 41. C-ABI- und WASM-Tests

### 41.1 C-ABI

- bestehende Aufrufe unverändert;
- Stats-JSON;
- invalid session;
- result lifecycle;
- clear cache;
- repeated stats;
- no leak;
- no exception crossing.

### 41.2 WASM

- `putSource` + editor + parse one native parserbuild;
- repeated compile exact hit;
- VersionOnly;
- content change;
- dependency parse reuse;
- stats capability;
- old module mock ohne Stats funktioniert;
- neues Wrapper + altes Modul verständlich;
- Browserworker;
- Node.

### 41.3 Native/WASM-Parität

Für jede Mutationssequenz:

- gleiche Statslogik soweit plattformneutral;
- identische fachliche Snapshots;
- identische Invalidationsgründe;
- keine JS-Normalisierung von IDs/Ranges/Diagnosen.

---

## 42. Tests in `interlis-language-tools`

### 42.1 Compilerbackend

- langlebige Session;
- normaler `putSource` kein Restart;
- Repositorysource kein Restart;
- Restart replayed genau aktuellen Sourcebestand;
- Replay nur Recovery;
- Stats durchgereicht;
- alte Compiler-Mock ohne Stats.

### 42.2 LanguageService

- bestehende Save-/Dirty-Semantik unverändert;
- Syntaxcache folgt Dokumentversion;
- Root A bleibt fresh bei unabhängiger Änderung C;
- Root A stale bei Dependency B;
- `lastGood` bleibt;
- Fehlerresultat current, lastGood erhalten;
- VersionOnly erzeugt keine unnötige Analyse;
- Repository-Nachladen ohne Restart;
- neu aufgelöstes Modell macht vorher fehlgeschlagenen Root analysierbar;
- Diagnostics nicht doppelt;
- affectedUris korrekt;
- Outdated Result verworfen.

### 42.3 Workeradapter

- normale Updates kein Workerersatz;
- Coalescing gleicher URI;
- Compilebarriere;
- Crash-Recovery replayed einmal;
- Sourceanzahl gemessen;
- Editorworker unabhängig;
- local fallback aktuell;
- Dispose.

### 42.4 LSP

- Full Sync bleibt;
- DidChange kein Full Compile;
- DidSave Compile;
- Semantic notification unverändert;
- Diagnostics root-isoliert;
- Document Symbols aktuelle Version;
- Node- und Browser-LSP.

### 42.5 Monaco Adapter

- keine API-Migration;
- Completion aktuelle Editorversion;
- Outline Sticky Baseline;
- Hover/Definition/References;
- Dirty Rename;
- keine leere Projektion während Cacheeviction.

### 42.6 Package-API

- Public API diff;
- neue optionale Typen;
- alte Mocks kompilieren;
- Pack/consumer test;
- Snapshotversionen konsistent.

---

## 43. Tests in `interlis-web-ide`

### 43.1 Startup

- Workspace einmal synchronisiert;
- Startupcompile;
- Worker bleiben bestehen;
- Sourceanzahl korrekt;
- keine doppelten Repositorysources.

### 43.2 Typing

- `outdated` sichtbar;
- kein `compileAndAnalyze`;
- EditorSnapshot aktuelle Version;
- nur geänderte URI neuer Parserbuild im Editorworker;
- Diagramm stale;
- Outline stabil.

### 43.3 Save

- genau ein sichtbarer Save-Compile;
- kein Workerrestart;
- unveränderte Dependencies Parse-Hits;
- Output/Problems aktuell;
- Diagramm nur bei gültigem Resultat ersetzt.

### 43.4 Unabhängige Modelle

Workspace mit Root A und unabhängigem C:

- C ändern/speichern;
- A-SemanticSnapshot bleibt fresh;
- offenes A-Diagramm löst keinen Dependency-Compile aus;
- A-Parserartefakte bleiben.

### 43.5 Gemeinsame Dependency

A und C importieren B:

- B ändern;
- A und C stale;
- nur offene/benötigte Roots werden entsprechend bestehender Trigger kompiliert;
- keine globalen UI-Resets.

### 43.6 Repositoryauflösung

- fehlendes Modell wird geladen;
- nur neue Repositorysource registriert;
- kein Workerrestart;
- kein Voll-Replay;
- anschliessende Kompilation erfolgreich;
- Repositorytab read-only.

### 43.7 WorkspaceSourceSynchronizer

- initial full scan;
- zweiter Scan ohne Änderungen liefert nur unchanged;
- Save gleicher Bytes kein Sourceupdate;
- Add;
- Change;
- Delete;
- ZIP import;
- Workspace switch reset;
- Git Working Tree change;
- Local Folder recovery.

### 43.8 Workercrash

- sichtbares letztes Resultat bleibt;
- Worker wird ersetzt;
- Sources einmal replayed;
- neuer Editor-/Compilelauf funktioniert;
- keine ältere Antwort überschreibt neue.

### 43.9 PWA

- production build;
- gehashte Worker-/WASM-Assets;
- Offline reload;
- kein Mixed-Version-Scheinresultat;
- Service Worker Update.

### 43.10 Browser-E2E

Mindestens Chromium; vorhandene Gates für Firefox und WebKit erhalten.

---

## 44. Cross-Repository-Contract-Tests

### 44.1 Gemeinsame Mutationsszenarien

Lege ein gemeinsames Szenarioformat an, beispielsweise:

```json
{
  "name": "leaf-change",
  "sources": [
    { "uri": "Base.ili", "version": 1, "textFile": "Base-v1.ili" },
    { "uri": "Root.ili", "version": 1, "textFile": "Root-v1.ili" }
  ],
  "operations": [
    { "op": "compile", "root": "Root.ili" },
    { "op": "put", "uri": "Root.ili", "version": 2, "textFile": "Root-v2.ili" },
    { "op": "compile", "root": "Root.ili" }
  ],
  "expect": {
    "parserBuildDelta": 1,
    "rootMissDelta": 1,
    "coldEquivalent": true
  }
}
```

### 44.2 Runner

- nativer Runner in `ilic-fork`;
- WASM-Runner;
- LanguageService-Runner;
- optional Web-IDE harness.

### 44.3 Vergleich

Verglichen werden:

- fachliche Resultate;
- Metrikdeltas;
- Eventreihenfolge;
- Restart-/Replayzählung;
- Freshness;
- affected URIs.

### 44.4 Kein Copy-Paste der Erwartungen

Die drei Runner sollen dieselben Fixtures lesen. Erwartungslogik darf nicht in jedem Repository unterschiedlich hardcodiert werden.

---

## 45. Externe Corpus-Tests

### 45.1 Conformance-Corpus

Verwende Modelle aus der externen Compiler-Testsuite.

Erzeuge deterministische Workspaces mit:

- Ili1;
- Ili2.3;
- Ili2.4;
- Chains;
- Diamonds;
- mehreren Modellen pro Datei;
- Fehlerfällen;
- Unicode;
- grossen Modellen.

### 45.2 Mutationen

Mindestens:

- Kommentar ändern;
- Whitespace ändern;
- Attributtyp ändern;
- Modellname ändern;
- Import hinzufügen;
- Import entfernen;
- Dependency Source ändern;
- Semikolon entfernen;
- Fehler reparieren;
- Source löschen;
- Source wieder hinzufügen;
- nur Dokumentversion erhöhen.

### 45.3 Fresh-Session-Orakel

Nach jeder Mutation:

1. inkrementelles Resultat;
2. neue Session mit aktuellem Sourcebestand;
3. struktureller Vergleich.

### 45.4 Berichte

Mindestens:

```text
build/p5-corpus/summary.md
build/p5-corpus/results.json
build/p5-corpus/diffs/
```

---

## 46. Benchmarks

### 46.1 Deterministische Szenarien

#### Small

- 5 Sources;
- Chain;
- ein Leaf Change.

#### Medium

- 50 Sources;
- mehrere Diamonds;
- 5 Roots.

#### Large

- mindestens 100 reale Modelle, soweit Corpus verfügbar;
- gemeinsame Basisdependencies;
- Repositorysources.

### 46.2 Operationen

- Cold editor snapshot;
- warm editor snapshot;
- Cold root compile;
- unchanged root compile;
- VersionOnly;
- leaf source change;
- common base change;
- unrelated source change;
- repository source add;
- remove/readd;
- 1000 Dirty-Editor-Updates mit Eviction.

### 46.3 Harte Gates

Harte Gates basieren auf Zählern:

- unveränderte Rootkompilation: `rootAnalysisHits +1`, keine Parserbuilds;
- Leaf Change: genau geänderte Source Parsermiss;
- VersionOnly: keine Parser-/Rootmisses;
- Repository Add: kein Restart/Replay;
- unrelated source: Rootcache bleibt Hit.

### 46.4 Zeitbudgets

Zeitwerte reporten, aber nur grobe Regressiongrenzen verwenden.

Empfehlung:

- Warm compile soll signifikant schneller als Cold sein;
- CI warnt bei grosser Regression;
- keine plattformsensitive Millisekunden-Hartgrenze ohne stabile Benchmarkumgebung.

### 46.5 WASM

Erfasse zusätzlich:

- Main-thread synchronous parse time;
- Compile worker time;
- Editor worker time;
- JSON-Grösse;
- WASM heap high-water mark;
- Source replay count.

---

## 47. Architektur-Guards

Implementiere automatische Guards gegen:

- neue globale Parsercache-Registry;
- `thread_local` Compilerworkspace;
- erneutes `parseSyntax()` in SemanticSnapshot-Aufbau;
- neuer Lexer/Parser im `IliFileCatalog`;
- neuer Lexer/Parser in `compileFile`;
- JavaScript-Parser;
- LanguageService als fachliche Rootcache-Autorität;
- `compiler.restart()` im normalen Repositoryresolve-Pfad;
- Workerrestart bei normalem `putSource`;
- unbeschränkte Sourcehistorie;
- Cachekey nur aus Dokumentversion;
- Cachekey ohne Compiler-/Grammar-Fingerprint;
- öffentliche Snapshots mit Parserpointer;
- Cross-Session mutable Caches;
- Web-IDE-Full-Rescan ohne Diff im normalen Savepfad;
- Node-LSP- oder Web-IDE-Produktionsmigration auf Low-Level-Cache-APIs.

Der Guard MUSS einen negativen Canary besitzen:

1. temporär verbotenes Muster einfügen;
2. Guard muss fehlschlagen;
3. temporäre Änderung entfernen;
4. Guard muss grün sein.

---

## 48. CI über drei Repositories

### 48.1 `ilic-fork`

- Release;
- Debug;
- repository enabled/disabled;
- WASM;
- native unit tests;
- incremental contract;
- native/WASM parity;
- ASan/UBSan/LSan;
- TSan;
- external conformance;
- corpus mutation tests;
- metrics benchmark report;
- architecture guard.

### 48.2 `interlis-language-tools`

- build;
- lint;
- typecheck;
- unit tests;
- coverage;
- worker tests;
- LSP Node/browser tests;
- incremental contract runner;
- real WASM bundle integration;
- pack verify;
- VSIX;
- public API diff.

### 48.3 `interlis-web-ide`

- install local verified tarballs;
- lint;
- typecheck;
- unit tests;
- production build;
- Chromium E2E;
- bestehende Firefox-/WebKit-Gates;
- offline/PWA;
- worker crash recovery;
- incremental workspace scenario;
- bundle scan.

### 48.4 Koordinierter Paketstand

Alle Cross-Repo-Läufe müssen exakt dokumentieren:

- Compiler SHA;
- Language Tools SHA;
- Web IDE SHA;
- Snapshotversionen;
- Tarballpfade;
- WASM hash;
- Lockfile-/Installationsmodus.

Keine Tests dürfen nur wegen Workspace-Hoisting grün sein.

---

## 49. Migrationsphasen

### P5.0 – Inventar und Baseline

- Status aller Repositories;
- aktuelle SHAs;
- P4 prüfen;
- bestehende Tests;
- aktuelle Parserlaufzahlen instrumentieren;
- Cold/Warm-Baseline;
- Workerrestart-/Replay-Baseline;
- sichtbare UI-Verträge erfassen.

### P5.1 – Sourceupdate-Klassifikation

- `SourceIdentity`;
- `SourceUpdateResult`;
- Inhaltsrevision;
- VersionOnly;
- Tests;
- API-Kompatibilität.

### P5.2 – ParsedSourceArtifact

- ein autoritativer Parse;
- Ili1/Ili2;
- Ownership;
- Header;
- Syntax-/Editor-Templates;
- repeated visitor tests;
- Memory accounting.

### P5.3 – ParsedSourceCache

- Schlüssel;
- Hits/Misses;
- negative entries;
- Budget;
- LRU;
- Stats;
- Sanitizer.

### P5.4 – Snapshotintegration

- `parse()` und `editorSnapshot()` umstellen;
- VersionOnly materialization;
- P3 parity;
- corpus.

### P5.5 – IliFileCatalog und Compilervisitor

- Header aus Artefakt;
- keine Header-Neuparses;
- MetaModel visitor aus Artefakt;
- keine Vollparser-Neuerzeugung;
- Cold equivalence.

### P5.6 – Dependencyindex und Planner

- SourceModelIndex;
- forward/reverse edges;
- unresolved watchers;
- closure;
- plans;
- invalidation matrix.

### P5.7 – Rootanalyse-Cache

- normalisierte Requestkeys;
- exact hits;
- failed deterministic results;
- materializer;
- fresh document versions;
- budget/eviction.

### P5.8 – SemanticSnapshot ohne Reparse

- Syntaxartefakte wiederverwenden;
- Dependencies;
- gleiche IDs/Ranges;
- Native/WASM parity.

### P5.9 – C-ABI/WASM/Stats

- exports;
- capabilities;
- wrapper;
- mocks;
- package tests.

### P5.10 – Language Tools

- Backend capabilities;
- Repositoryresolve ohne restart;
- Worker protocol;
- coalescing;
- recovery metrics;
- LanguageService tests;
- LSP compatibility.

### P5.11 – Web-IDE

- WorkspaceSourceSynchronizer;
- no-op saves;
- worker lifecycle;
- UI contract;
- PWA/E2E;
- memory benchmark.

### P5.12 – Cross-Repo Corpus und Abschluss

- shared fixtures;
- fresh-session oracle;
- external conformance;
- release/debug/sanitizers/TSan;
- package tarballs;
- web IDE full gates;
- docs;
- final report.

Nach jeder Phase sind fokussierte Tests auszuführen. Nach P5.5, P5.7, P5.8 und final ist die vollständige Conformance auszuführen.

---

## 50. Verbotene Abkürzungen

Ausdrücklich verboten:

- nur ein `Map<uri, SyntaxSnapshot>` in JavaScript;
- Rootcache nur im Language Service;
- Parsercache nur anhand Dokumentversion;
- vollständiges Neuparsing hinter gefälschten Hit-Zählern;
- Metriken manuell inkrementieren, ohne reale Operationen abzubilden;
- erneutes Headerparsing;
- erneutes Syntaxparsing im SemanticSnapshot;
- erneutes Vollparsing im Compilervisitor;
- Parserartefakte global teilen;
- mutable MetaModelStore-Objekte unkontrolliert über Runs teilen;
- Pointerbasierte Cachekeys;
- unbeschränkte Parse-Tree-Historie;
- Cache-Clear bei jeder Sourceänderung;
- globale Invalidierung bei eindeutig unabhängiger Source;
- VersionOnly als ContentChanged behandeln;
- alter Versionsvektor bei Cachehit;
- alte Transcriptzeit als neuer manueller Lauf;
- Cancellationresultat cachen;
- internen Compilerfehler cachen;
- Repositorysource durch Sessionrestart installieren;
- alle Sources bei normalem Save replayen;
- Worker pro Compile neu erstellen;
- Editor- und Compileworker zwingend fusionieren;
- sichtbare save-driven Semantik ändern;
- Dirty-Code für Diagramm/DOCX als vollständige Semantik ausgeben;
- LSP zwingend auf Textdeltas umstellen;
- Node-LSP zu Low-Level-Stats verpflichten;
- Web-IDE zu direkter CompilerSession-Verwaltung verpflichten;
- P1-Baseline erweitern;
- P2-Isolation schwächen;
- P3-Doppelparser wiedereinführen;
- P4-URI-/Source-Vertrag umgehen;
- Sanitizer-/TSan-Fehler unterdrücken;
- Tests lockern oder löschen;
- nur einen Prototyp oder TODO-Gerüst liefern.

---

## 51. Verifikationsmatrix

### 51.1 Native Builds

```bash
cmake -S . -B build/p5-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON
cmake --build build/p5-release --parallel
ctest --test-dir build/p5-release --output-on-failure
```

```bash
cmake -S . -B build/p5-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=ON
cmake --build build/p5-debug --parallel
ctest --test-dir build/p5-debug --output-on-failure
```

Repository-disabled Build erhalten.

### 51.2 Sanitizer

ASan/UBSan/LSan mit Leak Detection. Mindestens:

- 1000 Sourceupdates;
- Cacheevictions;
- repeated compile;
- remove/readd;
- Sessiondispose;
- corpus mutations.

### 51.3 TSan

Mehrere Sessions und Worker-nahe native Tests.

### 51.4 WASM

```bash
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Tatsächliche Repositoryskripte sind Quelle der Wahrheit.

### 51.5 Language Tools

```bash
cd ../interlis-language-tools
corepack pnpm install --frozen-lockfile
corepack pnpm build
corepack pnpm lint
corepack pnpm typecheck
corepack pnpm test
corepack pnpm test:snapshot
corepack pnpm pack:verify
corepack pnpm package:vsix
```

### 51.6 Web-IDE

```bash
cd ../interlis-web-ide
corepack pnpm install --no-frozen-lockfile --force --update-checksums
corepack pnpm check
corepack pnpm e2e
```

Mindestens fokussierte Chromium-E2E; bestehende vollständige Browsergates erhalten.

### 51.7 Wiederholung

```bash
ctest --test-dir build/p5-debug \
  --repeat until-fail:20 \
  -R "Incremental|ParsedSource|RootAnalysis|CompilerSession" \
  --output-on-failure
```

### 51.8 Abschluss

In allen drei Repositories:

```bash
git diff --check
git status --short
```

---

## 52. Definition of Done

P5 ist nur abgeschlossen, wenn alle Aussagen wahr und durch ausgeführte Tests belegt sind:

### Native Core

- Sourceupdates werden korrekt klassifiziert;
- VersionOnly parst nicht neu;
- ein Parserartefakt pro aktive Inhaltsrevision;
- Syntax und Editor teilen Artefakt;
- Header teilt Artefakt;
- Compilervisitor teilt Artefakt;
- SemanticSnapshot parst nicht erneut;
- Parsercache begrenzt;
- Rootcache begrenzt;
- Rootanalyse-Key vollständig;
- unveränderte Rootanalyse liefert Exact Hit;
- geänderte Rootanalyse verwendet Dependency-Parse-Hits;
- unabhängige Roots bleiben gültig;
- Cold/Incremental-Äquivalenz;
- Snapshots lebensdauersicher;
- keine globalen Caches;
- Stats korrekt;
- Trace korrekt.

### Compiler-Verträge

- bestehende API bleibt;
- neue Stats additiv;
- C-ABI sauber;
- WASM sauber;
- P3-Parität erhalten;
- P1-Conformance ohne neue Abweichung;
- P2-Concurrency erhalten;
- P4-Repositorysource ohne Tempfile-/Restart-Zwang.

### Language Tools

- save-driven Semantik unverändert;
- Repositoryauflösung ohne Restart;
- keine normalen Source-Replays;
- Workercrash-Recovery funktioniert;
- Root-Freshness korrekt;
- `lastGood` korrekt;
- LSP-Protokoll kompatibel;
- Monaco kompatibel;
- Packages und VSIX grün.

### Web-IDE

- Tippen löst keinen Full Compile aus;
- Save löst genau erwarteten Rootlauf aus;
- Status `outdated` bleibt;
- Diagramm last-good/stale bleibt;
- Workspace Source Diffing funktioniert;
- Repositorysource ohne Workerrestart;
- PWA-Build grün;
- Chromium-E2E grün;
- bestehende Firefox-/WebKit-Gates erhalten;
- kein unbeschränktes WASM-Heapwachstum.

### Qualität

- Release grün;
- Debug grün;
- ASan/UBSan/LSan grün;
- TSan grün;
- Architecture Guard grün;
- negativer Canary ausgeführt;
- Dokumentation aktualisiert;
- keine fremden Änderungen beschädigt.

---

## 53. Verhalten bei Blockern

Bei einem echten externen Blocker:

1. alle unabhängigen Teile implementieren;
2. Repositorys konsistent hinterlassen;
3. Blocker pro Repository einzeln dokumentieren;
4. exakten Befehl und Exitcode nennen;
5. betroffene Datei/Symbol nennen;
6. keine Tests als erfolgreich behaupten;
7. P5 nicht als abgeschlossen bezeichnen.

Fehlende lokale npm-Sichtbarkeit ist kein Grund, lokale Tarballtests auszulassen.

Fehlender Browser ist kein Grund, alle Web-IDE-Unit- und Buildtests auszulassen.

P4 noch nicht integriert ist ein Blocker für P4-abhängige P5-Teile, nicht für die Analyse oder Parsercache-Grundlagen. Der Agent darf P4 aber nicht ungefragt inkompatibel ersetzen.

---

## 54. Abschlussbericht

Der Agent MUSS am Ende einen Bericht mit exakt diesen Abschnitten liefern:

1. **Ausgangszustand aller drei Repositories**
2. **Verwendete Commits und Branches**
3. **P0–P4-Voraussetzungen**
4. **Baseline-Messungen**
5. **Sourceupdate-Klassifikation**
6. **Source-Identität und Inhaltsrevisionen**
7. **ParsedSourceArtifact**
8. **Parserartefakt-Ownership**
9. **ParsedSourceCache**
10. **Snapshot-Wiederverwendung**
11. **Header- und Modellindex**
12. **Dependencyindex**
13. **Invalidierungsregeln**
14. **CompilationPlanner**
15. **RootAnalysisKey**
16. **RootAnalysisCache**
17. **CompilationResultMaterializer**
18. **IliFileCatalog-Migration**
19. **Ili1-Compilervisitor**
20. **Ili2-Compilervisitor**
21. **SemanticSnapshot ohne Reparse**
22. **Öffentliche C++-API**
23. **C-ABI**
24. **WASM-/TypeScript-API**
25. **Metriken und Trace**
26. **CompilerBackend in interlis-language-tools**
27. **LanguageService-Integration**
28. **Repository-Nachladen ohne Restart**
29. **Worker-Protokoll und Coalescing**
30. **Worker-Recovery und Replay**
31. **Node-LSP-Kompatibilität**
32. **Monaco-Adapter-Kompatibilität**
33. **Web-IDE-WorkspaceSourceSynchronizer**
34. **Web-IDE Save-/Dirty-Verhalten**
35. **Diagramm- und DOCX-Verhalten**
36. **PWA- und Assetkonsistenz**
37. **Neue und geänderte Dateien in ilic-fork**
38. **Neue und geänderte Dateien in interlis-language-tools**
39. **Neue und geänderte Dateien in interlis-web-ide**
40. **Native Unit-Tests**
41. **C-ABI- und WASM-Tests**
42. **Language-Service-Tests**
43. **Language-Server-Tests**
44. **Web-IDE-Tests**
45. **Cross-Repository-Contract-Tests**
46. **Cold/Incremental-Äquivalenz**
47. **Externe Compiler-Conformance**
48. **Benchmarkresultate**
49. **Speicher- und Evictionresultate**
50. **Release-Ergebnisse**
51. **Debug-Ergebnisse**
52. **ASan-/UBSan-/LSan-Ergebnisse**
53. **TSan-Ergebnisse**
54. **npm-/Pack-/VSIX-Ergebnisse**
55. **Web-IDE-Build- und E2E-Ergebnisse**
56. **Architecture Guard**
57. **Ausgeführte Befehle**
58. **Abweichungen von der Spezifikation**
59. **Externe Blocker**
60. **Verbleibende Risiken**
61. **Abschliessender Git-Status aller drei Repositories**

Für jeden Test-/Buildlauf:

- Repository;
- Arbeitsverzeichnis;
- exakter Befehl;
- Commit;
- Buildtyp;
- Exitcode;
- Testzahl;
- Fehlerzahl;
- Reportpfad;
- verwendete Paket-/Suiteversionen.

Keine geschätzten Zahlen.

---

## 55. Explizite Erfolgsaussage

P5 darf nur als abgeschlossen bezeichnet werden, wenn folgende Aussage wahr und belegt ist:

> Die langlebige `CompilerSession` verwaltet Source-Inhaltsrevisionen, Parserartefakte, Abhängigkeitsclosure und root-granulare Analyseartefakte als sessionlokalen autoritativen Zustand. Unveränderte Sources werden zwischen Syntax-, Editor-, Header-, Kompilations- und SemanticSnapshot-Pfaden nicht erneut gelexed oder geparsed. Eine unveränderte Rootanalyse wird exakt wiederverwendet; nach einer Änderung werden nur betroffene Roots neu aufgebaut und unveränderte Dependency-Parserartefakte wiederverwendet. Incremental-Resultate sind für den geprüften Korpus fachlich identisch zu Resultaten einer frischen Session. `interlis-language-tools` behält save-getriebene Semantik, Root-Freshness und Worker-Recovery bei, lädt Repositorymodelle ohne normalen Sessionrestart nach und exponiert keine zweite fachliche Cacheautorität. `interlis-web-ide` behält ihr sichtbares Dirty-/Last-Good-/Diagramm-/Problems-Verhalten, synchronisiert Workspace-Sources differenziell und benötigt keine fachliche Consumer-Migration. P1-Conformance, P2-Isolation, P3-Single-Parser und P4-Repository-Verträge zeigen keine Regression.

---

## Anhang A – Minimale Abnahmesequenz

```text
1. Session erstellen
2. Base v1 registrieren
3. Root v1 registrieren
4. editorSnapshot(Root)
5. parse(Root)
6. compileAndAnalyze(Root)
7. compileAndAnalyze(Root) erneut
8. Root v2 mit identischen Bytes registrieren
9. compileAndAnalyze(Root)
10. Root v3 mit geändertem Attribut registrieren
11. compileAndAnalyze(Root)
12. unabhängiges Other registrieren und ändern
13. compileAndAnalyze(Root)
14. Repositorydependency hinzufügen
15. compileAndAnalyze(Root)
16. Stats prüfen
17. Fresh Session mit finalem Sourcebestand kompilieren
18. Resultate vergleichen
```

Erwartung:

```text
Root editor + parse: ein Parserbuild
zweiter unveränderter Compile: Rootcache-Hit
VersionOnly: kein Parse, kein Rootrebuild
Root ContentChange: nur Root Parsermiss, Base Parse-Hit
Other Change: Root bleibt Cachehit
Repository Add: kein Restart/Replay
final Incremental == final Fresh
```

---

## Anhang B – Invalidierungsmatrix

| Ereignis | Parserartefakt URI | Rootcache betroffener Closure | Unabhängige Roots | Dokumentversion materialisieren | Workerrestart |
|---|---:|---:|---:|---:|---:|
| identisches put | behalten | behalten | behalten | unverändert | nein |
| VersionOnly | behalten | behalten | behalten | aktualisieren | nein |
| ContentChanged | neue Revision | invalidieren | behalten | aktualisieren | nein |
| Import hinzugefügt | neue Revision | invalidieren/neu planen | nur bei Modellkonflikt | aktualisieren | nein |
| Modellname geändert | neue Revision | alte/neue Importer invalidieren | sonst behalten | aktualisieren | nein |
| Source entfernt | deaktivieren | betroffene invalidieren | behalten | URI entfernen | nein |
| Source gleich wieder hinzugefügt | darf Parse-Reuse | neu planen | behalten | aktualisieren | nein |
| Repositorysource hinzugefügt | bauen oder hit | wartende Roots invalidieren | behalten | aufnehmen | nein |
| Optionen geändert | Parser behalten | Key-Miss | andere Keys behalten | aktuell | nein |
| Workercrash | neue Worker-Caches | neu aufbauen | UI last-good behalten | replay aktuell | ja |
| explizite Cancellation per Termination | neue Worker-Caches | cancelled nicht cachen | UI last-good behalten | replay aktuell | ja |

---

## Anhang C – Reviewfragen

Ein Reviewer muss mindestens beantworten können:

1. Wo ist die einzige Source-Inhaltsrevision definiert?
2. Wie wird VersionOnly erkannt?
3. Welche Klasse besitzt den ANTLR Parse Tree?
4. Wer verhindert doppelte Parserläufe zwischen Editor und Compile?
5. Wie wird Headerinformation ohne zweiten Parser gewonnen?
6. Wie baut der Compilervisitor aus einem bestehenden Artefakt?
7. Warum ist ein Rootcache-Hit fachlich sicher?
8. Welche Optionen sind im Key?
9. Wie werden Builtins berücksichtigt?
10. Wie wird ein Modellrename invalidiert?
11. Wie werden unresolved dependencies beobachtet?
12. Warum bleiben öffentliche Snapshots nach Eviction gültig?
13. Was passiert bei VersionOnly mit `documentVersions`?
14. Was passiert bei einem Cachehit mit dem Transcriptzeitpunkt?
15. Welche Resultate werden nicht gecacht?
16. Wie wird Speicher begrenzt?
17. Wie wird Worker-Replay von normalem Sourceupdate unterschieden?
18. Warum benötigt der Node-LSP keine Migration?
19. Warum bleibt die Web-IDE save-driven?
20. Welche Zähler beweisen echte Wiederverwendung?
21. Wie wird Cold/Incremental-Äquivalenz geprüft?
22. Welche Tests beweisen Unabhängigkeit zweier Roots?
23. Welche Tests beweisen Unabhängigkeit zweier Sessions?
24. Wo wird P4-Repositorysource ohne Restart installiert?
25. Welche Architecture Guards verhindern Rückfälle?

---

## Anhang D – Agenten-Startsequenz

Der spätere Coding-Agent soll in dieser Reihenfolge beginnen:

```bash
cd ../ilic-fork
git status --short
git branch --show-current
git rev-parse HEAD

cd ../interlis-language-tools
git status --short
git branch --show-current
git rev-parse HEAD

cd ../interlis-web-ide
git status --short
git branch --show-current
git rev-parse HEAD
```

Danach vollständig lesen:

```text
ilic-fork:
  ilic-p5-incremental-compilation-spec.md
  ilic-p4-repository-modularization-spec-v2.md
  include/ilic/Compiler.h
  include/ilic/SourceManager.h
  include/ilic/Syntax.h
  include/ilic/Editor.h
  include/ilic/Semantic.h
  source/core/
  source/input/ili1/
  source/input/ili2/
  source/util/IliFile.*
  source/abi/
  packages/compiler-wasm/
  test/core/
  test/abi/
  .github/workflows/ci.yml

interlis-language-tools:
  README.md
  docs/adr/0002-save-driven-editor-synchronization.md
  packages/language-service/src/
  packages/language-server/src/
  packages/monaco-adapter/src/
  packages/*/test/
  apps/
  scripts/
  .github/workflows/

interlis-web-ide:
  README.md
  src/main.ts
  src/language-worker.ts
  src/interlis-compiler.worker.ts
  src/language-repository.ts
  src/workbench/
  src/workspace/
  test/
  e2e/
  vite.config.*
  package.json
  .github/workflows/
```

