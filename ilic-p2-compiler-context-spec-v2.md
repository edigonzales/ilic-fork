
# P2-Spezifikation: Expliziter `CompilerContext`, eindeutige Ownership und reentrante Compilerarchitektur

**Zielrepository:** `edigonzales/ilic-fork`  
**Referenzstand der Core-Bestandsaufnahme:** `d57f12942e936c9485bd6494a38b198e255cb144`  
**Referenz der realen Conformance-Integration:** `.github/workflows/ci.yml` auf `main`, geprüft am 31. Juli 2026  
**Zielphase:** P2  
**Sprache der Implementierung:** C++17  
**Primäre Zielplattformen:** Linux, macOS, Windows und WebAssembly/Emscripten  
**Adressat:** selbständig arbeitender LLM-Coding-Agent  
**Status dieses Dokuments:** normative Implementierungsspezifikation, Revision 2  
**Revision 2:** Die real umgesetzte P1-Absicherung erfolgt über die externe `interlis-compiler-testsuite`; frühere Annahmen über ein in `ilic-fork` eingebettetes P1-Korpus sind ersetzt.

---

## 1. Auftrag in einem Satz

Beseitige den impliziten, veränderlichen Prozesszustand des Compilers, führe einen pro Kompilationslauf erzeugten, expliziten `ilic::detail::CompilerContext` ein, überführe Metamodellobjekte und `.ili`-Dateimetadaten in eindeutig besitzende RAII-Container und ermögliche dadurch echte, datenrassenfreie Parallelität mehrerer unabhängiger `CompilerSession`-Instanzen, ohne Compilersemantik, öffentliche Resultatschemata, C-ABI oder Native/WASM-Verhalten zu verändern.

---

## 2. Normative Sprache

Die Schlüsselwörter **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT** und **KANN** sind normativ.

- **MUSS / DARF NICHT**: zwingendes Abnahmekriterium.
- **SOLL / SOLL NICHT**: darf nur mit einer konkreten, im Abschlussbericht belegten technischen Begründung abweichen.
- **KANN**: zulässige Option.
- Beispielcode ist normativ bezüglich Verantwortlichkeiten und Lebensdauer. Bezeichner dürfen nur dann abweichen, wenn die alternative Benennung mindestens gleich klar ist und die Abweichung dokumentiert wird.
- Die Spezifikation verlangt eine tatsächlich implementierte und verifizierte Lösung. Ein Architekturentwurf, ein teilweises Gerüst oder Kompatibilitätswrapper über den alten Globals reichen nicht.

---

## 2.1 Revisionshinweis

Revision 2 ersetzt die ursprüngliche Annahme, P1 habe innerhalb von `ilic-fork`
ein eigenes importiertes `ili2c`-Korpus, Manifeste und einen Native/WASM-
Conformance-Runner erzeugt. Tatsächlich wird die umfassende Compilerregression durch
das separate Codeberg-Repository `interlis-compiler-testsuite` ausgeführt. Alle
P2-Test-, CI- und Abnahmeregeln in diesem Dokument sind entsprechend auf diese reale
Architektur ausgerichtet.

---

## 3. Voraussetzungen und Reihenfolge

P2 baut auf P0 und der externen INTERLIS-Conformance-Absicherung auf.

### 3.1 P0 muss vorhanden sein

Vor Beginn MUSS der Agent verifizieren:

- dauerhaft aktives Test-Harness `ilic::test-support`;
- keine Standard-`assert`-Abhängigkeit im Testbereich;
- Release-Canary;
- Debug-CI;
- ASan-/UBSan-CI;
- aktive Leak Detection.

### 3.2 Reale P1-Absicherung: externe `interlis-compiler-testsuite`

P1 ist im realen Projekt **nicht** als in `ilic-fork` eingebettetes, selbst importiertes
Conformance-Korpus umgesetzt. Die verbindliche fachliche Regression wird durch das
separate Repository ausgeführt:

```text
https://codeberg.org/edigonzales/interlis-compiler-testsuite
```

Für lokale Arbeiten wird die Suite standardmässig als Nachbarrepository erwartet:

```text
../interlis-compiler-testsuite
```

Die Suite enthält sehr viele INTERLIS-Modelle und führt den Compiler als externen
Prozess aus. Sie ist für P2 der primäre End-to-End-Nachweis, dass das Refactoring
Compilersemantik, Ladeverhalten, Diagnoseverhalten und Prozessstabilität nicht
verändert.

Der Agent MUSS vor Beginn die reale Integration aus dem aktuellen Branch ermitteln:

1. `.github/workflows/ci.yml` vollständig lesen;
2. den dort gepinnten Suite-Commit ermitteln;
3. den dort verwendeten `InterlisConformance.java run`-Befehl unverändert als
   Referenz übernehmen;
4. den tatsächlich über `--baseline ilic=...` referenzierten Baseline-Pfad ermitteln;
5. den lokalen Nachbarcheckout prüfen, ohne ihn zu verändern.

Im Referenzstand dieser Spezifikation verwendet die GitHub Action:

```text
Suite-Repository: https://codeberg.org/edigonzales/interlis-compiler-testsuite
Suite-Commit:     88576ffc4cd491dd72d2375eb897ea9f3f55d2c3
Suite-Manifest:   suite/test-cases.json
Runner:           InterlisConformance.java
Baseline:         conformance/known-failures.json
```

Der Benutzer bezeichnet die Baseline teilweise als `conformance/known-issues.json`.
Der Dateiname ist **nicht** zu erraten: Normativ ist der Pfad, der im aktuellen
`ci.yml` tatsächlich an `--baseline ilic=...` übergeben wird. Existiert im
Arbeitsbranch inzwischen `conformance/known-issues.json` und verwendet der Workflow
diese Datei, ist genau diese zu verwenden. Der Agent DARF nicht parallel eine zweite
Baseline-Datei anlegen oder beide Namen synchron halten.

Im oben genannten Referenzstand enthält die Baseline elf Einträge. Diese Zahl ist
nur eine Bestandsaufnahme und kein dauerhaft fest codiertes Akzeptanzkriterium. Der
Agent MUSS die aktuelle Zahl und die Test-IDs bei jeder P2-Ausführung aus der realen
Baseline neu bestimmen.

### 3.3 Verbindliche Eigenschaften der externen Suite

Die externe Suite MUSS während P2:

- vor der ersten Refactoringänderung vollständig laufen;
- nach jeder abgeschlossenen, semantisch relevanten Migrationsphase vollständig
  laufen;
- vor dem Abschluss gegen den Release-Build vollständig laufen;
- gegen Debug, ASan/UBSan/LSan und TSan ausgeführt werden, soweit die jeweilige
  Buildvariante einen ausführbaren nativen `ilic` erzeugt;
- in der GitHub Action weiterhin vollständig laufen;
- dieselbe Baseline wie vor P2 verwenden;
- alle nicht bekannten neuen Abweichungen als Fehler behandeln;
- veraltete Baseline-Einträge sichtbar machen, soweit der Suite-Runner dies
  unterstützt;
- ohne Modifikation des Suite-Repositorys ausgeführt werden.

P2 DARF NICHT:

- Modelle aus der externen Suite in `ilic-fork` kopieren, nur um die Suite zu
  ersetzen;
- ein zweites in-repo P1-Korpus oder eine zweite Erwartungsdatenbank aufbauen;
- die Suite verkleinern, filtern oder auf einen Smoke-Test reduzieren;
- neue bekannte Fehler eintragen, um eine Refactoringregression zu kaschieren;
- den Suite-Commit nebenbei aktualisieren;
- die Baseline lockern;
- den Runner patchen;
- eine fehlgeschlagene Suite mit `continue-on-error` endgültig ignorieren.

Das bestehende CI-Muster, bei dem der eigentliche Lauf für den Reportupload zunächst
mit `continue-on-error` ausgeführt und der gespeicherte Exitcode danach in einem
eigenen Schritt erzwungen wird, ist zulässig. Entscheidend ist, dass der Job am Ende
mit genau diesem Exitcode fehlschlägt.

### 3.4 Lokaler Suite-Checkout

Der Agent MUSS zunächst prüfen:

```bash
test -d ../interlis-compiler-testsuite
test -f ../interlis-compiler-testsuite/InterlisConformance.java
test -f ../interlis-compiler-testsuite/suite/test-cases.json
git -C ../interlis-compiler-testsuite status --short
git -C ../interlis-compiler-testsuite rev-parse HEAD
```

Der lokale Checkout ist eine externe Arbeitskopie und DARF von P2 nicht verändert,
formatiert, bereinigt, zurückgesetzt oder auf einen anderen Commit geschaltet werden.

Weicht sein HEAD vom in `ci.yml` gepinnten Commit ab, SOLL der Agent den gepinnten
Stand in einem separaten temporären Worktree verwenden, sofern der Commit lokal
vorhanden ist:

```bash
suite_repo=../interlis-compiler-testsuite
suite_worktree="$PWD/build/p2-conformance-suite"
suite_commit=<AUS_CI_YML>
git -C "$suite_repo" cat-file -e "${suite_commit}^{commit}"
git -C "$suite_repo" worktree add --detach "$suite_worktree" "$suite_commit"
```

Ist der gepinnte Commit lokal nicht verfügbar und Netzwerkzugriff nicht möglich,
MUSS der Agent:

1. die vorhandene lokale Suite trotzdem als zusätzliche Entwicklungsprüfung laufen
   lassen;
2. die Abweichung zwischen lokalem HEAD und CI-Pin exakt dokumentieren;
3. den unveränderten CI-Workflow als endgültigen Nachweis bestehen lassen;
4. niemals behaupten, den CI-gepinnten Stand lokal ausgeführt zu haben.

### 3.5 Ausgangsbaseline

Vor der ersten Änderung MUSS der Agent mindestens festhalten:

```bash
git status --short
git rev-parse HEAD
git submodule status
cmake --version
c++ --version
node --version
java --version
git -C ../interlis-compiler-testsuite rev-parse HEAD
git -C ../interlis-compiler-testsuite status --short
```

Danach MUSS er ausführen:

1. die vollständige interne P0-/CTest-Suite;
2. die vollständige externe `interlis-compiler-testsuite` mit dem realen Befehl aus
   `.github/workflows/ci.yml`;
3. die bestehenden WASM- und JavaScript-Pakettests.

Der komplette Ausgabeordner der externen Suite MUSS als Vorher-Baseline aufbewahrt
werden, beispielsweise unter:

```text
build/p2-baseline/conformance/
```

Zusätzlich sind zu protokollieren:

- Suite-Commit;
- Suite-Manifestpfad;
- Baseline-Pfad;
- SHA-256 der Baseline-Datei;
- Anzahl aktueller Baseline-Einträge;
- vollständige Liste der Baseline-Test-IDs;
- Compilerbinary und dessen SHA-256;
- exakter Runnerbefehl;
- Exitcode;
- sämtliche erzeugten Reports.

Ein bereits vor P2 fehlschlagender interner oder externer Test MUSS als solcher
belegt werden. Er darf weder still P2 angelastet noch ohne Nachweis zur Baseline
hinzugefügt werden.

---

# Teil A – Problemdefinition und Architekturgrenzen

## 4. Beobachtete Ausgangslage

Die Bestandsaufnahme des genannten Referenzstands zeigt veränderlichen Zustand an mehreren Stellen. Diese Liste ist ein Startpunkt, keine abschliessende Inventarliste.

| Bereich | Beobachteter Zustand | Risiko |
|---|---|---|
| `source/core/Compiler.cpp` | prozessweiter `compilerMutex` | alle Sessions werden serialisiert; verdeckt Datenrassen |
| `source/util/Logger.cpp` | globales Objekt `Logger Log` | Diagnosen, Source-URI, Optionen und Sinks können sich vermischen |
| `source/core/SourceManager.cpp` | `thread_local` aktiver SourceManager | implizite Abhängigkeit; verschachtelte/async Nutzung fragil |
| `source/util/IliFile.cpp` | globale Listen, Maps, Suchoptionen und manuelles Löschen | Session-Leaks, Stale State, Doppelbesitz |
| `source/metamodel/MetaModel.cpp` | globale Objektarena, Registries und Kontextstack | keine Reentranz; globale Lebensdauer |
| `source/metamodel/MetaModelInput.cpp` | weitere globale Registries, Sprachflags, Metadatenpuffer | doppelte Wahrheiten und Quellkontext-Leaks |
| `source/input/ili1/*`, `source/input/ili2/*` | globale `input_file`, globale Builderfunktionen | Parserinstanzen sind nicht selbständig |
| Semantic-/Translation-Checker | freie Funktionen über globale Registries und `Log` | versteckte Inputs und Outputs |
| `MetaModelTreeVisitor` | globaler Modellzugriff und globales Logging | Generatoren/Visitor nicht parallel verwendbar |
| Output-Klassen | dateiweite mutable `static`-Variablen | parallele Ausgaben überschreiben sich |
| CLI | globaler Logger und globale Optionen wie `model_filter` | Prozesszustand statt lokaler Requestdaten |
| C-ABI | globales Handle-Registry | grundsätzlich zulässig, aber Core-Session muss intern isoliert sein |

Der Agent MUSS zusätzlich einen vollständigen maschinellen und manuellen Global-State-Bericht erzeugen, bevor er refaktoriert.

## 5. Zielgrenze

Nach P2 gilt:

> Innerhalb von `ilic-core` existiert kein veränderlicher Prozesszustand, der den Inhalt, die Diagnoseausgabe, die Namensauflösung, die Objektlebensdauer, den Modellkatalog oder die Ausgabe eines Compilerlaufs beeinflusst.

Ausnahmen sind nur:

1. unveränderliche Compile-Time-Konstanten;
2. unveränderliche, nach C++-Initialisierung nie mutierte Lookup-Tabellen;
3. von ANTLR generierte, effektiv unveränderliche Laufzeittabellen;
4. die C-ABI-Handleverwaltung ausserhalb von `ilic-core`, exakt dokumentiert und mutexgeschützt;
5. Betriebssystem-/C++-Runtime-Zustand ausserhalb der Kontrolle des Projekts.

Nicht zulässige Ersatzlösungen:

- ein anderer globaler Mutex;
- `thread_local CompilerContext`;
- `thread_local Logger`;
- Singleton;
- Service Locator;
- „current context“-Zeiger;
- Reset vor und nach jedem Lauf;
- globale Map nach Session-ID;
- globaler Objektpool;
- Leaks als Lebensdauerstrategie.

## 6. P2-In-Scope

P2 MUSS mindestens umfassen:

1. `CompilerSession`-Synchronisierung;
2. Compiler-Orchestrierung;
3. Logger-/Diagnosezustand;
4. aktive Quellkontexte;
5. `.ili`-Header- und Modellkatalog;
6. Metamodellobjekt-Ownership;
7. Metamodellregistries;
8. Parser-/Builder-Kontextstack;
9. INTERLIS-Sprachflags;
10. Metadaten- und Dokumentationspuffer;
11. Name-Resolution-Hilfen;
12. Semantic Checker;
13. Translation Checker;
14. Semantic-Snapshot-Aufbau;
15. `MetaModelTreeVisitor`;
16. Output-Generatoren;
17. CLI-Legacypfad;
18. C-ABI-Sessionparallelität;
19. Native/WASM-Gleichheit;
20. Architekturtests;
21. ThreadSanitizer-CI.

## 7. Explizit nicht Teil von P2

P2 DARF nicht zu einer verdeckten Umsetzung späterer Phasen werden.

Nicht Teil von P2:

- vollständige inkrementelle Kompilierung oder Cache-Wiederverwendung;
- neuer Parser oder Entfernen des doppelten JavaScript-Parsers;
- komplette Repository-Abstraktionsarchitektur;
- Änderung der INTERLIS-Semantik;
- INTERLIS-2.2-Unterstützung;
- öffentliches AST-/Metamodell-API;
- neues Plugin-System;
- Wechsel des C++-Standards;
- Generalüberholung aller Namenskonventionen;
- Umstellung des gesamten Projekts auf `shared_ptr`;
- grossflächige Neuformatierung unbeteiligter Dateien.

P2 MUSS jedoch so gestaltet sein, dass P5 später einen sessionlokalen inkrementellen Cache ergänzen kann, ohne erneut Prozessglobals einzuführen.

---

# Teil B – Zielarchitektur

## 8. Lebensdauermodell

Es gibt drei klar getrennte Lebensdauern.

### 8.1 Prozesslebensdauer

Darf enthalten:

- unveränderlichen Programmcode;
- unveränderliche Grammatiktabellen;
- C-ABI-Handle-Registry in `ilic-capi`;
- keine Compilerfachdaten.

### 8.2 `CompilerSession`-Lebensdauer

Enthält:

- `SourceManager`;
- per-Session-Mutex;
- rein sessionlokale Zähler/Diagnostik für Tests;
- zukünftig mögliche inkrementelle Caches;
- keine Metamodellobjekte eines abgeschlossenen Laufs;
- keine Diagnosen eines abgeschlossenen Laufs.

### 8.3 Kompilationslauf-Lebensdauer

Ein neuer `CompilerContext` wird pro `compile`, `analyze` oder `compileAndAnalyze` erzeugt und besitzt:

- Logger/Diagnoseengine;
- kombinierte Source-Sicht;
- `.ili`-Dateikatalog;
- Metamodellstore/Arena;
- Metamodellbuilder;
- Sprachversion;
- Kontextstack;
- Metaattribut-/Dokumentationspuffer;
- alle temporären Namens- und Compile-Order-Daten.

Nach Rückgabe eines wertbasierten `CompilationResult`, `SemanticSnapshot` oder `CompilationAnalysisResult` MUSS der Kontext vollständig zerstört sein. Kein Pointer oder `string_view` in öffentlichen Resultaten darf in den zerstörten Kontext zeigen.

## 9. Zielabhängigkeitsgraph

```text
CompilerSession::Impl
  ├── std::mutex
  ├── SourceManager
  └── session counters

per operation:
CompilerContext
  ├── borrows SourceManager
  ├── owns Logger
  ├── owns CompilationSourceStore
  ├── owns IliFileCatalog
  ├── owns MetaModelStore
  └── owns MetaModelBuilder

CompilerPipeline
  ├── uses CompilerContext
  ├── invokes Ili1ParserDriver / Ili2ParserDriver
  ├── invokes SemanticChecker
  ├── invokes TranslationChecker
  └── returns value-only CompilationExecution

Semantic snapshot:
  buildSemanticSnapshot(
      CompilationSourceStore const&,
      MetaModelStore const&,
      request,
      compilation)

Output:
  OutputGenerator(
      MetaModelStore const&,
      Logger&,
      output options)
```

`CompilerContext` DARF nur an der Orchestrierungsgrenze als Bündel verwendet werden. Untergeordnete Klassen und Funktionen MÜSSEN konkrete Abhängigkeiten erhalten; sie dürfen nicht überall einen Context als Service Locator entgegennehmen.

---

# Teil C – Verbindliche Klassen und Methoden

## 10. `CompilerSession`

### 10.1 Öffentliche Semantik

Die bestehende öffentliche Funktionalität MUSS erhalten bleiben:

```cpp
namespace ilic {

class CompilerSession {
public:
    CompilerSession();
    ~CompilerSession();

    CompilerSession(const CompilerSession&) = delete;
    CompilerSession& operator=(const CompilerSession&) = delete;
    CompilerSession(CompilerSession&&) = delete;
    CompilerSession& operator=(CompilerSession&&) = delete;

    void putSource(std::string uri, std::string utf8,
                   std::uint64_t version = 0);
    bool removeSource(const std::string& uri);

    SourceManager& sources();
    const SourceManager& sources() const;

    SyntaxSnapshot parse(const std::string& uri);
    CompilationResult compile(const CompilationRequest& request);
    SemanticSnapshot analyze(const CompilationRequest& request);
    CompilationAnalysisResult compileAndAnalyze(
        const CompilationRequest& request);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
```

PImpl ist verbindlich, sofern der aktuelle Branch nicht bereits eine gleichwertige interne Kapselung besitzt. Eine Abweichung MUSS erklären, wie öffentliche Header vom neuen internen Context/Store entkoppelt bleiben.

### 10.2 `CompilerSession::Impl`

```cpp
class CompilerSession::Impl final {
public:
    std::mutex mutex;
    SourceManager sources;
    std::uint64_t compileInvocationCount = 0;
    std::uint64_t parseInvocationCount = 0;
};
```

Die genauen Testzähler KÖNNEN abweichen. Fachlicher Compilerzustand DARF nicht zwischen Läufen in `Impl` verbleiben.

### 10.3 Synchronisationsvertrag

- `putSource`, `removeSource`, `parse`, `compile`, `analyze` und `compileAndAnalyze` MÜSSEN denselben per-Session-Mutex verwenden.
- Zwei verschiedene Sessions DÜRFEN keinen gemeinsamen Core-Mutex verwenden.
- Operationen derselben Session werden serialisiert.
- Operationen verschiedener Sessions können echt parallel laufen.
- Der bisherige prozessweite `compilerMutex` MUSS vollständig entfernt werden.
- Locking DARF nicht rekursiv erforderlich sein.
- Callbacks/Sinks dürfen möglichst nicht unter einem Registry-Mutex der C-ABI ausgeführt werden.
- `sources()` bleibt aus Kompatibilitätsgründen bestehen. Die Dokumentation MUSS klar sagen, dass ein vom Aufrufer direkt verwendeter mutabler `SourceManager&` extern synchronisiert werden muss.

### 10.4 Implementierungsmuster

```cpp
CompilationResult CompilerSession::compile(
    const CompilationRequest& request)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    ++impl_->compileInvocationCount;

    detail::CompilerContext context(impl_->sources, request.options);
    detail::CompilationExecution execution =
        detail::CompilerPipeline(context).compile(request);

    return std::move(execution.result);
}
```

`compileAndAnalyze` MUSS denselben Context für Kompilierung und Snapshot-Aufbau halten:

```cpp
CompilationAnalysisResult CompilerSession::compileAndAnalyze(
    const CompilationRequest& request)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    ++impl_->compileInvocationCount;

    detail::CompilerContext context(impl_->sources, request.options);
    detail::CompilationExecution execution =
        detail::CompilerPipeline(context).compile(request);

    CompilationAnalysisResult result;
    result.compilation = std::move(execution.result);
    result.semantic = buildSemanticSnapshot(
        context.compilationSources(),
        context.models(),
        request,
        result.compilation,
        execution.compilationSourceUris,
        &result.syntax);
    return result;
}
```

Die bestehende `retainMetamodel`-/Cleanup-Flag-Logik MUSS verschwinden.

---

## 11. `ilic::detail::CompilerContext`

### 11.1 Ablage und Sichtbarkeit

Neue Dateien:

```text
source/core/CompilerContext.h
source/core/CompilerContext.cpp
```

Der Header ist intern. Er DARF nicht installiert und nicht über die öffentliche C++-API exportiert werden.

### 11.2 Verbindliche Schnittstelle

```cpp
namespace ilic::detail {

class CompilerContext final {
public:
    CompilerContext(
        SourceManager& sessionSources,
        const CompilerOptions& options);
    ~CompilerContext() noexcept;

    CompilerContext(const CompilerContext&) = delete;
    CompilerContext& operator=(const CompilerContext&) = delete;
    CompilerContext(CompilerContext&&) = delete;
    CompilerContext& operator=(CompilerContext&&) = delete;

    SourceManager& sessionSources() noexcept;
    const SourceManager& sessionSources() const noexcept;

    util::Logger& logger() noexcept;
    const util::Logger& logger() const noexcept;

    CompilationSourceStore& compilationSources() noexcept;
    const CompilationSourceStore& compilationSources() const noexcept;

    util::IliFileCatalog& files() noexcept;
    const util::IliFileCatalog& files() const noexcept;

    metamodel::MetaModelStore& models() noexcept;
    const metamodel::MetaModelStore& models() const noexcept;

    metamodel::MetaModelBuilder& builder() noexcept;
    const CompilerOptions& options() const noexcept;

private:
    SourceManager& sessionSources_;
    CompilerOptions options_;
    util::Logger logger_;
    CompilationSourceStore compilationSources_;
    util::IliFileCatalog files_;
    metamodel::MetaModelStore models_;
    metamodel::MetaModelBuilder builder_;
};

}
```

### 11.3 Konstruktionsreihenfolge

Die Memberreihenfolge MUSS die Abhängigkeiten widerspiegeln:

1. geborgter Session-SourceManager;
2. kopierte unveränderliche Optionen;
3. Logger;
4. kombinierte Source-Sicht;
5. Dateikatalog;
6. Metamodellstore;
7. Builder.

Destruktion erfolgt automatisch in umgekehrter Reihenfolge. Kein explizites globales `reset()` ist zulässig.

### 11.4 Context-Invarianten

Während der Lebensdauer:

- alle erzeugten Metamodell-Heapobjekte gehören genau `models_`;
- alle rohen Graphzeiger zeigen auf Objekte desselben Stores oder auf eingebettete Value-Member;
- alle `.ili`-Metadaten gehören `files_`;
- alle Diagnosen gehören `logger_`;
- jeder Parser-Source-Scope ist beim Methodenende geschlossen;
- der Builder-Kontextstack ist nach jedem vollständig geparsten File leer;
- Optionen ändern sich während des Laufs nicht;
- der Context ist nie global, `thread_local`, geteilt oder nach aussen gespeichert.

### 11.5 Kein Service Locator

Folgendes ist verboten:

```cpp
void parseSomething(CompilerContext& context); // wenn nur Logger/Builder nötig
```

Stattdessen:

```cpp
void parseSomething(
    metamodel::MetaModelBuilder& builder,
    util::Logger& logger);
```

Der Context darf an `CompilerPipeline` übergeben werden, weil diese Klasse Orchestrator ist. Parser-Visitor, Checker und Generatoren erhalten konkrete Abhängigkeiten.

---

## 12. `CompilationSourceStore`

### 12.1 Zweck

Die bisherige implizite Mischung aus:

- `SourceManager`;
- `activeSourceManager`;
- Dateisystem;
- internem INTERLIS-Modell;
- temporären Fallback-SourceManagern

wird durch eine explizite, pro Lauf gültige Source-Sicht ersetzt.

### 12.2 Dateien

```text
source/core/CompilationSourceStore.h
source/core/CompilationSourceStore.cpp
```

### 12.3 Schnittstelle

```cpp
namespace ilic::detail {

enum class SourceOrigin {
    Session,
    FileSystem,
    Builtin
};

struct CompilationSource {
    SourceBuffer buffer;
    SourceOrigin origin = SourceOrigin::Session;
};

class CompilationSourceStore final {
public:
    explicit CompilationSourceStore(SourceManager& sessionSources);

    CompilationSourceStore(const CompilationSourceStore&) = delete;
    CompilationSourceStore& operator=(const CompilationSourceStore&) = delete;

    const SourceBuffer* get(const std::string& uri) const noexcept;
    bool contains(const std::string& uri) const noexcept;
    std::vector<std::string> uris() const;

    const SourceBuffer& rememberFileSystemSource(
        std::string uri,
        std::string utf8);
    const SourceBuffer& rememberBuiltinSource(
        std::string uri,
        std::string utf8);

    SourcePosition position(
        const std::string& uri,
        std::size_t byteOffset) const;

private:
    SourceManager& sessionSources_;
    std::map<std::string, CompilationSource> ownedSources_;
};

}
```

### 12.4 Regeln

- Sessionquellen werden nicht kopiert, sofern kein stabiler Snapshot nötig ist.
- Dateisystem- und Built-in-Quellen werden vom Store besessen.
- Eine Source-URI darf nicht still mit anderem Inhalt überschrieben werden.
- Bei Kollision zwischen Sessionquelle und Dateisystemquelle hat die explizite Sessionquelle Vorrang.
- `position()` MUSS UTF-8-Byteoffset und UTF-16-Spalte identisch zur bisherigen öffentlichen Semantik berechnen.
- `activeSourceManager`, `setActiveSourceManager` und `ActiveSourceManagerScope` MÜSSEN entfernt werden.
- Kein Ersatzzeiger „current source store“ ist zulässig.

---

## 13. `util::Logger`

### 13.1 Global entfernen

Folgendes MUSS verschwinden:

```cpp
extern util::Logger Log;
Logger Log;
```

Jeder Logger ist eine normale Instanz mit klarer Ownership.

### 13.2 Kernverhalten

Die bestehenden Diagnosen, Counts, Logevents und Transcriptinhalte MÜSSEN semantisch erhalten bleiben.

### 13.3 Fehlerbehandlung

`Logger::internal_error` DARF im Core weder `exit`, `abort` noch `std::terminate` aufrufen. Es MUSS einen typisierten Fehler werfen:

```cpp
class CompilerAbort final : public std::runtime_error {
public:
    CompilerAbort(std::string message, int code);
    int code() const noexcept;
};
```

Empfohlene Methoden:

```cpp
[[noreturn]] void internalError(std::string message, int code = 1);
```

Der CLI-Rand übersetzt `CompilerAbort` in einen Prozess-Exitcode. C++-API und C-ABI übersetzen ihn in `ILIC-COMPILER-INTERNAL` beziehungsweise das bestehende öffentliche Fehlerformat.

### 13.4 RAII-Scopes

Der Logger MUSS verschachtelte Zustände ohne manuelles Zurücksetzen verwalten:

```cpp
class Logger::SourceScope final {
public:
    SourceScope(Logger&, std::string uri);
    ~SourceScope() noexcept;
    SourceScope(const SourceScope&) = delete;
    SourceScope& operator=(const SourceScope&) = delete;
private:
    Logger* logger_;
    std::string previous_;
};

class Logger::CategoryScope final {
    // analog
};

class Logger::IndentScope final {
public:
    explicit IndentScope(Logger&);
    ~IndentScope() noexcept;
};
```

Erlaubtes Muster:

```cpp
auto sourceScope = logger.sourceScope(source.uri);
auto categoryScope = logger.categoryScope("parser");
auto indent = logger.indentScope();
```

Manuelle, paarweise `incNestLevel`/`decNestLevel`-Folgen SOLLEN in produktivem Compilerpfad verschwinden, soweit sie echten Scope darstellen.

### 13.5 Logger-Invarianten

- Counter sind instanzlokal.
- aktuelle Source und Kategorie sind instanzlokal.
- Sinks sind instanzlokal.
- `reset()` ist im neuen per-Run-Loggerpfad nicht erforderlich.
- Sinkcallbacks dürfen keine Referenzen auf Contextdaten über dessen Lebensdauer hinaus speichern.
- Logger ist nicht kopierbar.
- Logger wird nicht zwischen parallelen Contexts geteilt.

---

## 14. `util::IliFile`

### 14.1 Trennung von Daten und Parser

`IliFile` DARF nicht zugleich global registrierter Datensatz und ANTLR-Visitor sein.

Ziel:

```cpp
namespace util {

class IliFile final {
public:
    const std::string& uri() const noexcept;
    const std::string& iliVersion() const noexcept;
    const std::vector<std::string>& imports() const noexcept;
    const std::vector<std::string>& translationModels() const noexcept;
    const std::vector<std::string>& requiredModels() const noexcept;
    const std::vector<std::string>& models() const noexcept;
    bool autoSearched() const noexcept;
    const ilic::SourceBuffer& source() const noexcept;

private:
    friend class IliFileCatalog;
    // reine Daten
};

}
```

Die bisherigen Getter dürfen als Übergang intern delegieren, aber die endgültige Implementierung SOLL const-korrekte Referenzen statt Containerkopien liefern.

### 14.2 `IliFileHeaderVisitor`

Neue interne Klasse:

```cpp
class IliFileHeaderVisitor final
    : public parser::IliFileParserBaseVisitor {
public:
    IliFileHeaderVisitor(util::Logger& logger, IliFile& target);

    antlrcpp::Any visitIliFile(...) override;
    antlrcpp::Any visitVersion(...) override;
    antlrcpp::Any visitModelName(...) override;
    antlrcpp::Any visitTranslationOf(...) override;
    antlrcpp::Any visitModelImport(...) override;

private:
    util::Logger& logger_;
    IliFile& target_;
};
```

Der Visitor DARF keinen globalen Modellindex verändern. Der Katalog indexiert erst nach erfolgreichem Parsen.

---

## 15. `util::IliFileCatalog`

### 15.1 Dateien

```text
source/util/IliFileCatalog.h
source/util/IliFileCatalog.cpp
```

`IliFile.h/.cpp` dürfen entsprechend aufgeteilt oder ersetzt werden. In der finalen Struktur darf es nur eine aktive Implementierung geben.

### 15.2 Schnittstelle

```cpp
namespace util {

class IliFileCatalog final {
public:
    IliFileCatalog(
        ilic::detail::CompilationSourceStore& sources,
        Logger& logger,
        const ilic::CompilerOptions& options);

    IliFileCatalog(const IliFileCatalog&) = delete;
    IliFileCatalog& operator=(const IliFileCatalog&) = delete;

    IliFile* loadRoot(const std::string& uri);
    IliFile* resolveModel(
        const std::string& modelName,
        const std::string& iliVersion);

    IliFile* findByUri(const std::string& uri) const noexcept;
    IliFile* findByModel(
        const std::string& modelName,
        const std::string& iliVersion) const noexcept;

    const std::vector<IliFile*>& selectedFiles() const noexcept;
    const std::vector<std::unique_ptr<IliFile>>& allFiles() const noexcept;

    void select(IliFile& file);
    std::size_t fileCount() const noexcept;

private:
    IliFile* load(const std::string& uri, bool autoSearched);
    IliFile* builtinInterlis(const std::string& iliVersion);
    void index(IliFile& file);
    void validateModelDirectories();

    ilic::detail::CompilationSourceStore& sources_;
    Logger& logger_;
    const ilic::CompilerOptions& options_;

    std::vector<std::unique_ptr<IliFile>> files_;
    std::vector<IliFile*> selected_;
    std::map<std::string, IliFile*> byUri_;
    std::map<std::pair<std::string, std::string>, IliFile*> byModelAndVersion_;
};

}
```

### 15.3 Verhaltensregeln

- Kein `extern all_ilifiles`.
- Keine globale `all_ilifiles_full`.
- Keine globale `all_ilimodels`.
- Keine globalen `auto_search`-/`ilidirs`-Werte.
- Kein manuelles `delete` in Resetfunktionen.
- Die Optionen stammen aus dem Context.
- Verzeichnisse werden validiert; Fehler erzeugen Diagnosen oder `CompilerAbort`, niemals direkt `exit`.
- Sessionquellen werden vor Dateisystemquellen durchsucht.
- Dateisystemsuche ist nur bei `autoSearch=true` erlaubt.
- Suchreihenfolge und Fehlermeldungen bleiben soweit fachlich beobachtbar stabil.
- Das Built-in-Modell wird pro Context katalogisiert.
- Ein Modellname mit falscher Sprachversion wird nicht als passender Treffer behandelt.
- Der Katalog liefert stabile Pointer bis zu seiner Destruktion.
- Derselbe URI wird pro Context höchstens einmal geparst.
- Gross-/Kleinschreibungsverhalten bleibt plattformübergreifend definiert und durch Tests gedeckt.

---

## 16. `metamodel::MetaModelStore`

### 16.1 Zweck

Der Store ist der alleinige Owner aller dynamisch erzeugten Metamodellobjekte und die einzige Quelle für runlokale Registries.

### 16.2 Dateien

```text
source/metamodel/MetaModelStore.h
source/metamodel/MetaModelStore.cpp
```

### 16.3 Virtueller Destruktor

`MMObject` besitzt bereits virtuelle Methoden, aber keinen virtuellen Destruktor. P2 MUSS ergänzen:

```cpp
virtual ~MMObject() = default;
```

Damit kann der Store `std::unique_ptr<MMObject>` sicher besitzen.

### 16.4 Arena

```cpp
class MetaModelStore final {
public:
    MetaModelStore();
    ~MetaModelStore() noexcept;

    MetaModelStore(const MetaModelStore&) = delete;
    MetaModelStore& operator=(const MetaModelStore&) = delete;
    MetaModelStore(MetaModelStore&&) = delete;
    MetaModelStore& operator=(MetaModelStore&&) = delete;

    template<class T, class... Args>
    T* make(Args&&... args)
    {
        static_assert(std::is_base_of_v<MMObject, T>);
        auto object = std::make_unique<T>(
            std::forward<Args>(args)...);
        T* result = object.get();
        objects_.push_back(std::move(object));
        owned_.insert(result);
        return result;
    }

    bool owns(const MMObject* object) const noexcept;
    std::size_t objectCount() const noexcept;

    bool discardUnlinked(MMObject* object) noexcept;
    void clear() noexcept;

    // Registries
    void addModel(Model&);
    void addPackage(Package&);
    void addType(Type&);
    void addUnit(Unit&);
    void addImport(Import&);
    void addDependency(Dependency&);
    void addAxisSpec(AxisSpec&);
    void addDataUnit(DataUnit&);
    void addFunction(FunctionDef&);
    void addLineForm(LineForm&);
    void addGraphic(Graphic&);

    const std::vector<Model*>& models() const noexcept;
    const std::vector<Package*>& packages() const noexcept;
    const std::vector<Type*>& types() const noexcept;
    const std::vector<Unit*>& units() const noexcept;
    const std::vector<Import*>& imports() const noexcept;
    const std::vector<Dependency*>& dependencies() const noexcept;
    const std::vector<AxisSpec*>& axisSpecs() const noexcept;
    const std::vector<DataUnit*>& dataUnits() const noexcept;
    const std::vector<FunctionDef*>& functions() const noexcept;
    const std::vector<LineForm*>& lineForms() const noexcept;
    const std::vector<Graphic*>& graphics() const noexcept;

    Model* interlisModel() const noexcept;
    void setInterlisModel(Model&);

    Class& anyClass();
    Class& anyStructure();

private:
    std::vector<std::unique_ptr<MMObject>> objects_;
    std::unordered_set<const MMObject*> owned_;

    std::vector<Model*> models_;
    std::vector<Package*> packages_;
    std::vector<Type*> types_;
    std::vector<Unit*> units_;
    std::vector<Import*> imports_;
    std::vector<Dependency*> dependencies_;
    std::vector<AxisSpec*> axisSpecs_;
    std::vector<DataUnit*> dataUnits_;
    std::vector<FunctionDef*> functions_;
    std::vector<LineForm*> lineForms_;
    std::vector<Graphic*> graphics_;

    Model* interlis_ = nullptr;
    Class* anyClass_ = nullptr;
    Class* anyStructure_ = nullptr;
};
```

Containerwahl kann abweichen, sofern Pointerstabilität, eindeutiger Besitz, effiziente Iteration und testbare Destruktion gewährleistet sind.

### 16.5 Ownership-Invarianten

- Jedes dynamische `MMObject` wird genau einmal durch `MetaModelStore::make<T>` erzeugt.
- Kein produktiver Parserpfad verwendet direkt `new` für `MMObject`-Subtypen.
- Kein produktiver Pfad löscht Metamodellobjekte direkt.
- Graphkanten bleiben nicht-besitzende rohe Pointer.
- `shared_ptr` im Metamodellgraph ist verboten.
- Objekte verschiedener Stores dürfen nicht miteinander verknüpft werden.
- `discardUnlinked` darf nur für noch nicht publizierte/registrierte Objekte verwendet werden.
- Die Destruktion darf keine Graphkante dereferenzieren.
- `clear()` ist idempotent und normalerweise nur für fokussierte Tests nötig; der Compiler verlässt sich auf den Destruktor.
- Der Store gibt keine Containerkopien zurück.
- Die bisher getrennten Import-Registries in `MetaModel.cpp` und `MetaModelInput.cpp` werden zu exakt einer Registry.

### 16.6 Alte API entfernen

Diese API MUSS verschwinden:

```cpp
register_mmobject(...)
destroy_mmobject(...)
reset_mmobjects()
make_mmobject<T>(...)
reset()
init(...)
extern Model* INTERLIS
```

Eine gleichnamige Kompatibilitätsschicht, die intern einen aktuellen globalen Store sucht, ist verboten.

### 16.7 Clone

`MMObject::clone()` MUSS den Zielstore explizit erhalten:

```cpp
MMObject* clone(MetaModelStore& destination);
```

Alternativ:

```cpp
MMObject* MetaModelStore::clone(const MMObject& source);
```

Alle intern erzeugten Clone-Unterobjekte MÜSSEN in denselben Zielstore gelangen. Kein Clone darf im alten globalen Objektregister landen.

---

## 17. `metamodel::MetaModelBuilder`

### 17.1 Verantwortung

Der Builder ersetzt die zustandsbehafteten freien Funktionen aus `MetaModelInput.*` und die globalen Kontextfunktionen aus `MetaModel.*`.

### 17.2 Dateien

```text
source/metamodel/MetaModelBuilder.h
source/metamodel/MetaModelBuilder.cpp
```

### 17.3 Sprachversion

```cpp
enum class IliLanguageVersion {
    Ili10,
    Ili23,
    Ili24
};
```

String-Konvertierung erfolgt zentral und getestet.

### 17.4 Schnittstelle – Zustände und Scopes

```cpp
class MetaModelBuilder final {
public:
    class SourceScope;
    class ContextScope;

    MetaModelBuilder(
        MetaModelStore& store,
        util::Logger& logger);

    MetaModelBuilder(const MetaModelBuilder&) = delete;
    MetaModelBuilder& operator=(const MetaModelBuilder&) = delete;

    void setLanguageVersion(IliLanguageVersion version) noexcept;
    IliLanguageVersion languageVersion() const noexcept;
    bool isIli23() const noexcept;
    bool isIli24() const noexcept;

    SourceScope enterSource(const ilic::SourceBuffer& source);
    ContextScope enterContext(MetaElement& element);

    MMObject* current() const noexcept;
    Class* currentClass() const noexcept;
    Package* currentPackage() const noexcept;
    SubModel* currentTopic() const noexcept;
    Model* currentModel() const noexcept;
    std::size_t contextDepth() const noexcept;

    MetaModelStore& store() noexcept;
    const MetaModelStore& store() const noexcept;

    // Quellen/Metadaten
    void prepareMetaAttributes(std::string_view source);
    ilic::SourceRange tokenRange(antlr4::Token* token) const;
    void setSelectionSource(MetaElement&, antlr4::Token*);
    void setEndSelectionSource(MetaElement&, antlr4::Token*);
    void setReferenceSource(
        MMObject&, std::string kind, antlr4::Token*);
    void setReferenceSource(
        MMObject&, std::string kind,
        antlr4::ParserRuleContext*);

    // Initialisierung
    void initObject(MMObject&, int line);
    void initMetaElement(MetaElement&, int line);
    void initExtendable(ExtendableME&, int line);
    void initPackage(Package&, int line);
    void initType(Type&, int line);
    void initDomainType(DomainType&, int line);
    void initClass(Class&, int line);
    void initGraphic(Graphic&, int line);
    void initFunction(FunctionDef&, int line);
    void initLineForm(LineForm&, int line);
    void initExpression(Expression&, int line);
    void initFactor(Factor&, int line);
    void initConstraint(Constraint&, int line);

    // Registrierung/Auflösung – siehe Mappingtabelle
};
```

### 17.5 `SourceScope`

```cpp
class MetaModelBuilder::SourceScope final {
public:
    SourceScope(
        MetaModelBuilder& builder,
        const ilic::SourceBuffer& source);
    ~SourceScope() noexcept;

    SourceScope(const SourceScope&) = delete;
    SourceScope& operator=(const SourceScope&) = delete;
    SourceScope(SourceScope&& other) noexcept;
    SourceScope& operator=(SourceScope&&) = delete;

private:
    MetaModelBuilder* builder_;
    // vorherige Source und vorherige Pending-Daten
};
```

Anforderungen:

- Source-URI und Source-Text werden gemeinsam gesetzt.
- Logger-Source-Scope wird synchron gehalten.
- Pending MetaAttributes/Documentation gehören zum aktiven Source-Scope.
- Beim Verlassen wird der vorherige Zustand exakt wiederhergestellt.
- Ein Fehler/Exception im Parser darf keinen aktiven Sourcezustand hinterlassen.

### 17.6 `ContextScope`

```cpp
class MetaModelBuilder::ContextScope final {
public:
    ContextScope(
        MetaModelBuilder& builder,
        MetaElement& element);
    ~ContextScope() noexcept;

    ContextScope(const ContextScope&) = delete;
    ContextScope& operator=(const ContextScope&) = delete;
    ContextScope(ContextScope&& other) noexcept;
    ContextScope& operator=(ContextScope&&) = delete;

private:
    MetaModelBuilder* builder_;
    MetaElement* expected_;
};
```

Anforderungen:

- Konstruktor pusht genau ein Element.
- Destruktor entfernt genau dasselbe Element.
- Unterlauf oder falsche Reihenfolge sind programmatische Invariantenverletzungen.
- Produktionscode SOLL keine manuellen `push_context`/`pop_context`-Paare mehr enthalten.
- Verschachtelte Scopes und Exception-Unwinding werden getestet.

### 17.7 Kein globaler Sprachzustand

Diese Symbole MÜSSEN verschwinden:

```cpp
extern bool ili23;
extern bool ili24;
extern std::string iliversion;
```

Parser und Builder verwenden die Instanzmethoden.

---

## 18. Mapping der alten Metamodell-API

Die folgenden Zuordnungen sind verbindlich. Ein Agent darf Methoden zusammenfassen, muss aber jede alte Verantwortung explizit migrieren.

### 18.1 `MetaModelInput`-Zuordnung

| Alte API | Ziel |
|---|---|
| `reset_input_state()` | entfällt; Destruktor von `CompilerContext`/Store/Builder |
| `init_mmobject(o,line)` | `MetaModelBuilder::initObject(*o,line)` |
| `set_selection_source(e,t)` | `MetaModelBuilder::setSelectionSource(*e,t)` |
| `set_end_selection_source(e,t)` | `MetaModelBuilder::setEndSelectionSource(*e,t)` |
| `set_reference_source(o,k,t)` | `MetaModelBuilder::setReferenceSource(*o,k,t)` |
| `prepare_meta_attributes(source)` | `MetaModelBuilder::prepareMetaAttributes(source)` innerhalb `SourceScope` |
| `init_metaelement(e,line)` | `MetaModelBuilder::initMetaElement(*e,line)` |
| `init_extendableme(e,line)` | `MetaModelBuilder::initExtendable(*e,line)` |
| `init_package(p,line)` | `MetaModelBuilder::initPackage(*p,line)` |
| `find_model(name,line)` | `MetaModelBuilder::findModel(name,line)` |
| `find_dataunit(name,line)` | `MetaModelBuilder::findDataUnit(name,line)` |
| `add_package(p)` | `MetaModelBuilder::addPackage(*p)` / Store-Registry |
| `find_package(name,line)` | `MetaModelBuilder::findPackage(name,line)` |
| `find_topic(name,line)` | `MetaModelBuilder::findTopic(name,line)` |
| `add_unit(u)` | `MetaModelBuilder::addUnit(*u)` |
| `find_unit(name,line)` | `MetaModelBuilder::findUnit(name,line)` |
| `init_type(t,line)` | `MetaModelBuilder::initType(*t,line)` |
| `add_type(t)` | `MetaModelBuilder::addType(*t)` |
| `find_type(name,line)` | `MetaModelBuilder::findType(name,line)` |
| `get_type_string(t)` | stateless `typeString(t)` oder Builder-Methode ohne globalen Zugriff |
| `init_domaintype(t,line)` | `MetaModelBuilder::initDomainType(*t,line)` |
| `find_domaintype(name,line)` | `MetaModelBuilder::findDomainType(name,line)` |
| `init_class(c,line)` | `MetaModelBuilder::initClass(*c,line)` |
| `add_class(c)` | `MetaModelBuilder::addClass(*c)` |
| `find_class(...)` | `MetaModelBuilder::findClass(...)` mit allen bisherigen Overloads |
| `find_class_type(name,line)` | `MetaModelBuilder::findClassType(name,line)` |
| `find_class_or_view(name,line)` | `MetaModelBuilder::findClassOrView(name,line)` |
| `find_class_or_structure(name,line)` | `MetaModelBuilder::findClassOrStructure(name,line)` |
| `find_structure(...)` | `MetaModelBuilder::findStructure(...)` |
| `find_view(name,line)` | `MetaModelBuilder::findView(name,line)` |
| `find_association(...)` | `MetaModelBuilder::findAssociation(...)` |
| `find_attribute(c,name)` | `MetaModelBuilder::findAttribute(c,name)` |
| `find_role(c,name)` | `MetaModelBuilder::findRole(c,name)` |
| `find_parameter(c,name,line)` | `MetaModelBuilder::findParameter(c,name,line)` |
| `init_graphic(g,line)` | `MetaModelBuilder::initGraphic(*g,line)` |
| `add_graphic(g)` | `MetaModelBuilder::addGraphic(*g)` |
| `find_graphic(name,line)` | `MetaModelBuilder::findGraphic(name,line)` |
| `init_function(f,line)` | `MetaModelBuilder::initFunction(*f,line)` |
| `add_function(f)` | `MetaModelBuilder::addFunction(*f)` |
| `find_function(name,line)` | `MetaModelBuilder::findFunction(name,line)` |
| `init_lineform(f,line)` | `MetaModelBuilder::initLineForm(*f,line)` |
| `add_lineform(f)` | `MetaModelBuilder::addLineForm(*f)` |
| `find_lineform(name,line)` | `MetaModelBuilder::findLineForm(name,line)` |
| `init_expression(e,line)` | `MetaModelBuilder::initExpression(*e,line)` |
| `init_factor(f,line)` | `MetaModelBuilder::initFactor(*f,line)` |
| `init_constraint(c,line)` | `MetaModelBuilder::initConstraint(*c,line)` |
| `debug(ctx,message)` | Parser-Visitor nutzt instanzlokalen `Logger&` und RAII-Indent |
| `get_line(...)` | stateless `sourceLine(...)` kann frei bleiben |
| `is_reserved_name(name)` | stateless, unveränderliche Lookup-Tabelle |

### 18.2 `MetaModel`-Zuordnung

| Alte API | Ziel |
|---|---|
| `add_dataunit` | `MetaModelStore::addDataUnit` über Builder |
| `get_all_dataunits` | `MetaModelStore::dataUnits() const` |
| `add_model` | `MetaModelStore::addModel` über Builder |
| `get_all_models` | `MetaModelStore::models() const` |
| `add_import` | `MetaModelStore::addImport` über Builder |
| `get_all_imports` | `MetaModelStore::imports() const` |
| `get_all_unqualified_imports` | `MetaModelBuilder::unqualifiedImports(modelName) const` |
| `add_dependency` | `MetaModelStore::addDependency` |
| `get_all_dependencies` | `MetaModelStore::dependencies() const` |
| `depends_on` | `MetaModelBuilder::dependsOn(currentTopic,target) const` |
| `add_axisspec` | `MetaModelStore::addAxisSpec` |
| `get_all_axisspecs` | `MetaModelStore::axisSpecs() const` |
| `push_context` | `MetaModelBuilder::enterContext` |
| `pop_context` | RAII-Destruktor `ContextScope` |
| `get_context` | `MetaModelBuilder::current()` |
| `get_class_context` | `MetaModelBuilder::currentClass()` |
| `get_package_context` | `MetaModelBuilder::currentPackage()` |
| `get_topic_context` | `MetaModelBuilder::currentTopic()` |
| `get_model_context` | `MetaModelBuilder::currentModel()` |
| `get_path/get_parent_path` | dürfen stateless bleiben, sofern kein Store-/Contextzugriff |


### 18.3 Abschlussregel für alte Header

`MetaModelInput.h` und die globalen Abschnitte von `MetaModel.h` dürfen am Ende nicht eine zweite API parallel zur neuen Architektur anbieten.

Zulässig:

- Löschen der alten Headerteile;
- Umbenennen in stateless Hilfen;
- kleine freie Funktionen, die ausschliesslich ihre Parameter verwenden.

Nicht zulässig:

- Wrapper, die einen globalen/current Store suchen;
- Wrapper mit `thread_local`;
- Wrapper, die einen Context über versteckte Registry beziehen;
- Deprecated-Funktionen, die weiterhin mutable Globals bedienen.

---

## 19. Parser-Driver und Visitor

### 19.1 Ili2

Zielschnittstelle:

```cpp
namespace input {

class Ili2Input final
    : public parser::Ili2ParserBaseVisitor {
public:
    Ili2Input(
        metamodel::MetaModelBuilder& builder,
        util::Logger& logger);

    // bestehende visit*-Overrides

private:
    metamodel::MetaModelBuilder& builder_;
    util::Logger& logger_;
};

void parseIli2(
    const ilic::SourceBuffer& source,
    metamodel::MetaModelBuilder& builder,
    util::Logger& logger);

}
```

### 19.2 Ili1

Analog:

```cpp
class Ili1Input final
    : public parser::Ili1ParserBaseVisitor {
public:
    Ili1Input(
        metamodel::MetaModelBuilder& builder,
        util::Logger& logger);
private:
    metamodel::MetaModelBuilder& builder_;
    util::Logger& logger_;
};

void parseIli1(
    const ilic::SourceBuffer& source,
    metamodel::MetaModelBuilder& builder,
    util::Logger& logger);
```

### 19.3 Verbindliche Migrationsregeln

Für alle Dateien:

```text
source/input/ili1/*.cpp
source/input/ili2/Ili2Input.cpp
source/input/ili2/Ili2Input_assoc.cpp
source/input/ili2/Ili2Input_class.cpp
source/input/ili2/Ili2Input_constraint.cpp
source/input/ili2/Ili2Input_expression.cpp
source/input/ili2/Ili2Input_function.cpp
source/input/ili2/Ili2Input_graphic.cpp
source/input/ili2/Ili2Input_helper.cpp
source/input/ili2/Ili2Input_package.cpp
source/input/ili2/Ili2Input_path.cpp
source/input/ili2/Ili2Input_type.cpp
source/input/ili2/Ili2Input_unit.cpp
source/input/ili2/Ili2Input_view.cpp
```

gilt:

- `Log.*` wird `logger_.*`.
- `make_mmobject<T>()` wird `builder_.store().make<T>()`.
- globale Builderfunktionen werden Buildermethoden.
- `input_file` wird entfernt.
- `ili23`, `ili24`, `iliversion` werden Builderzustand.
- manueller Kontextpush/-pop wird RAII.
- Parserquellen werden als `SourceBuffer` übergeben, nicht erneut über Dateinamen geladen.
- keine Parserinstanz speichert Referenzen länger als der Aufruf.
- Fehlerpfade lassen Logger-, Source- und Kontextzustand sauber zurück.
- der Built-in-INTERLIS-Text wird wie jede andere explizite Source geparst.
- `using namespace std` in öffentlichen/intern breit eingebundenen Headern SOLL entfernt werden, soweit die bearbeiteten Header ohnehin geändert werden.

### 19.4 Helper

Reine Helper wie String-Decodierung dürfen freie Funktionen bleiben, wenn sie:

- keinen Logger verwenden;
- keine Registries lesen;
- keinen Builderkontext lesen;
- deterministisch nur von Parametern abhängen.

Ein Helper, der eine Diagnose erzeugt, erhält `Logger&` oder wird Buildermethode.

---

## 20. `IliParserErrorListener`

### 20.1 Zielsignatur

```cpp
class IliParserErrorListener final
    : public antlr4::ANTLRErrorListener {
public:
    IliParserErrorListener(
        util::Logger& logger,
        const ilic::SourceBuffer& source);

    void syntaxError(...) override;
    // übrige Overrides

private:
    util::Logger& logger_;
    const ilic::SourceBuffer& source_;
};
```

### 20.2 Anforderungen

- Kein Defaultkonstruktor, der globales Logging voraussetzt.
- Rangeberechnung verwendet die explizite Source.
- Lexer- und Parserlistener können getrennte Instanzen sein.
- Source-URI stammt nicht aus globalem `current_source`.
- Listener darf nicht länger leben als die Parser-Source.
- Diagnosereihenfolge und Codes bleiben stabil.

---

## 21. `SemanticChecker`

### 21.1 Zielklasse

```cpp
namespace metamodel {

class SemanticChecker final {
public:
    SemanticChecker(
        MetaModelStore& store,
        util::Logger& logger);

    void run();

private:
    MetaModelStore& store_;
    util::Logger& logger_;

    // stateful/private helpers
};

Multiplicity effectiveRoleCardinality(const Role* role);
Multiplicity attributeCardinality(const Type* type);

}
```

Reine Berechnungsfunktionen dürfen frei bleiben. Alles, was Registries liest oder Diagnosen schreibt, gehört in die Instanz oder erhält explizite Parameter.

### 21.2 Anforderungen

- `check_model_semantics()` verschwindet.
- Kein `get_all_models()`/`get_all_*`.
- Kein globales `Log`.
- Kein globaler Current Topic.
- Checkerzustand wird nach `run()` nicht geteilt.
- Mehrere Checker über verschiedene Stores dürfen parallel laufen.
- Diagnosecodes, Ranges, Related Information und Reihenfolge bleiben erhalten.
- Der Checker darf den Store nur dort mutieren, wo die bestehende Semantik bereits resolved type/link data schreibt; diese Mutationen sind dokumentiert.

---

## 22. `TranslationChecker`

```cpp
class TranslationChecker final {
public:
    TranslationChecker(
        MetaModelStore& store,
        util::Logger& logger);

    void run();

private:
    MetaModelStore& store_;
    util::Logger& logger_;
};
```

Anforderungen:

- `check_model_translations()` verschwindet.
- `_translationOf`-Links gehören zu Objekten desselben Stores.
- Keine statischen Lookupmaps mit runabhängigen Pointern.
- Übersetzungsdiagnosen bleiben semantisch identisch.
- Reentranz wird mit parallelen Übersetzungsmodellen getestet.

---

## 23. Semantic Snapshot

### 23.1 Neue Signatur

```cpp
SemanticSnapshot buildSemanticSnapshot(
    const ilic::detail::CompilationSourceStore& sources,
    const metamodel::MetaModelStore& models,
    const CompilationRequest& request,
    const CompilationResult& compilation,
    const std::vector<std::string>& compilationSourceUris,
    std::vector<SyntaxSnapshot>* syntaxSnapshots);
```

### 23.2 Regeln

- Kein Zugriff auf `metamodel::get_all_models()`.
- `SnapshotBuilder` erhält den Store oder die Modellliste explizit.
- Snapshot enthält nur Values/Strings/IDs, keine Pointer in den Store.
- Alle Source-URIs werden aus expliziter Source-Sicht bestimmt.
- Die resultierenden JSON-Schemata bleiben unverändert.
- Die Reihenfolge bleibt deterministisch.
- Snapshot-Aufbau ist abgeschlossen, bevor der Context zerstört wird.
- Bei fehlgeschlagener Kompilierung darf kein ungültiger Storepointer dereferenziert werden.

---

## 24. `MetaModelTreeVisitor`

### 24.1 Zielkonstruktor

```cpp
class MetaModelTreeVisitor {
public:
    MetaModelTreeVisitor(
        const MetaModelStore& store,
        util::Logger& logger);
    virtual ~MetaModelTreeVisitor() = default;

    void visitAllModels();

protected:
    const MetaModelStore& store() const noexcept;
    util::Logger& logger() noexcept;

private:
    const MetaModelStore& store_;
    util::Logger& logger_;
};
```

### 24.2 Migration

- `visitAllModels()` iteriert `store_.models()`.
- Imports werden aus `store_.imports()` gelesen.
- AxisSpecs werden aus `store_.axisSpecs()` gelesen.
- Debugausgabe nutzt `logger_`.
- `ignoreVisit` darf als internes Kontrollsignal bleiben, SOLL aber durch einen typisierten privaten Exceptiontyp statt `std::string` ersetzt werden.
- `accept_exception` darf bei unerwartetem Fehler `CompilerAbort` werfen, nicht den Prozess beenden.
- Der Visitor ist nicht kopierbar.
- Generated/erzeugte Visitor-Dateien und Generatorvorlagen müssen gemeinsam aktualisiert werden, damit eine Regeneration die Änderung nicht zurücksetzt.

---

## 25. Output-Generatoren

### 25.1 Allgemeine Konstruktorregel

Jeder Generator erhält:

- `const MetaModelStore&`;
- `Logger&`;
- eigene Outputoptionen;
- ausschliesslich instanzlokalen veränderlichen Zustand.

Beispiel:

```cpp
class Ili2Output final : public MetaModelTreeVisitor {
public:
    Ili2Output(
        const MetaModelStore& store,
        util::Logger& logger,
        std::string outputFile,
        std::string modelVersion);

private:
    std::string outputFile_;
    std::string modelVersion_;
    util::TextWriter writer_;
    Model* activeModel_ = nullptr; // nicht besitzend, runlokal
};
```

### 25.2 Verbindlich zu prüfen

Mindestens:

```text
source/output/Ili1Output.*
source/output/Ili2Output.*
source/output/ImdOutput.*
source/output/XsdOutput.*
source/output/GmlOutput.*
source/metamodel/MetaModelOutput.*
source/metamodel/MetaModelTreeVisitor.*
source/util/TextWriter.*
source/util/XMLWriter.*
source/util/XTFWriter.*
```

### 25.3 Zu entfernende Muster

Beispiele aus dem Ausgangscode:

- dateiweites `static std::string ili_file`;
- dateiweites `static std::string model_version`;
- dateiweites `static TextWriter ili2`;
- `static Model* act_model`;
- `static bool domainheader_written`;
- `static bool visitClassFlag`;
- mutable dateiweite Writer;
- globales `model_filter`.

Alle müssen Instanzmember oder lokale Variablen werden.

### 25.4 Ausgabegleichheit

- Bei identischen Inputs und Optionen MUSS der erzeugte Dateiinhalt bytegleich bleiben.
- Pfade/Zeiten, die bereits vor P2 variabel sind, dürfen nur gemäss den bestehenden Golden- und Suite-Vergleichsregeln normalisiert werden. P2 DARF keine neue Normalisierung einführen, die fachliche Unterschiede verdeckt.
- Parallele Generatoren mit verschiedenen Zieldateien dürfen sich nicht beeinflussen.
- Gleiche Zieldatei gleichzeitig ist ein Aufruferfehler und muss nicht koordiniert werden; dies ist zu dokumentieren.
- Writer werden im Destruktor zuverlässig geschlossen.
- Fehler beim Öffnen/Schreiben werden als typisierte Fehler oder Diagnosen gemeldet, nicht per `exit`.

---

## 26. `CompilerPipeline`

### 26.1 Dateien

```text
source/core/CompilerPipeline.h
source/core/CompilerPipeline.cpp
```

`Compiler.cpp` darf die öffentliche Sessionfassade enthalten. Die eigentliche runlokale Orchestrierung SOLL in `CompilerPipeline` liegen.

### 26.2 Schnittstelle

```cpp
namespace ilic::detail {

struct CompilationExecution {
    CompilationResult result;
    std::vector<std::string> compilationSourceUris;
};

class CompilerPipeline final {
public:
    explicit CompilerPipeline(CompilerContext& context);

    CompilationExecution compile(
        const CompilationRequest& request);

private:
    bool compileFile(
        util::IliFile& file,
        std::vector<util::IliFile*>& compiledFiles,
        std::set<std::string>& compiledModels,
        std::vector<std::string>& transcript,
        std::size_t& diagnosticIndex,
        std::size_t& logIndex);

    void applyExternalMetaAttributes(
        const CompilationRequest& request);

    CompilerContext& context_;
};

}
```

### 26.3 Compile-Ablauf

Der Ablauf MUSS explizit sein:

1. Context ist frisch konstruiert.
2. Logger wird für API-Kompilierung stumm konfiguriert, Sinks bleiben instanzlokal.
3. Rootquellen werden über `IliFileCatalog::loadRoot` geladen.
4. Sprachversion wird validiert.
5. Importmodelle werden über den Katalog aufgelöst.
6. Built-in-INTERLIS wird in denselben Katalog aufgenommen.
7. Dateien werden topologisch/iterativ in stabiler Reihenfolge kompiliert.
8. Pro Datei:
   - expliziter SourceBuffer;
   - Logger Source-/Category-Scope;
   - Builder SourceScope;
   - Ili1- oder Ili2-Parserdriver.
9. External MetaAttributes werden über Store/Builder aufgelöst.
10. `SemanticChecker::run()`.
11. `TranslationChecker::run()`.
12. wertbasierte `CompiledModel`-Liste wird erzeugt.
13. Diagnosen/Logs/Transcript werden kopiert/verschoben.
14. `CompilationExecution` wird zurückgegeben.
15. Contextdestruktion räumt alle internen Objekte auf.

### 26.4 Entfernen alter Cleanup-Logik

Muss verschwinden:

- `resetCompilerState`;
- `CompilerStateCleanup`;
- `retainMetamodel`;
- Reset bei CLI-Abbruch;
- globale `reset_*`-Reihenfolge;
- Cleanup, der korrekte Reihenfolge manuell kennen muss.

RAII ist die einzige normale Cleanupstrategie.

---

# Teil D – CLI, C-ABI und WASM

## 27. CLI

### 27.1 Lokale Optionen

`model_filter` und andere Requestwerte werden Felder einer lokalen Struktur:

```cpp
struct CliOptions {
    std::string modelFilter;
    // bestehende Optionen
};
```

### 27.2 Logger

`main()` konstruiert einen lokalen CLI-Logger oder verwendet direkte, klar getrennte Console-Ausgabe. Help-/Versionfunktionen erhalten `Logger&` beziehungsweise `std::ostream&`.

Verboten:

```cpp
Log.message(...);
```

ohne explizites Objekt.

### 27.3 Compilerpfad

Der CLI darf nicht zusätzlich zur neuen Pipeline den alten globalen Parserpfad behalten.

Für Outputgenerierung darf der CLI intern:

1. einen lokalen `SourceManager` füllen;
2. einen `CompilerContext` erzeugen;
3. `CompilerPipeline::compile` ausführen;
4. vor Contextdestruktion die gewählten Generatorinstanzen über `context.models()` ausführen.

Die JSON-Compile-Option bleibt auf der öffentlichen C-ABI aufgebaut und muss unverändert funktionieren.

### 27.4 Exitcodes

Nur der äusserste CLI-Rand darf Prozess-Exitcodes zurückgeben. Corebibliotheken dürfen nicht `exit()` aufrufen.

Ein Architekturtest MUSS direkte `exit`, `_Exit`, `quick_exit`, `abort` in Corepfaden erkennen. Zulässige Nutzung im echten `main` wird eng allowlisted.

---

## 28. C-ABI

### 28.1 Zulässige globale Registry

Folgende prozessweite Daten in `ilic-capi` dürfen bleiben:

- Handle-Map für Sessions;
- Handle-Map für Resultate;
- nächste Handle-ID;
- Registry-Mutex.

Begründung: Eine C-ABI benötigt prozessweit auflösbare opaque Handles.

### 28.2 Bedingungen

- Registry ist nicht Teil von `ilic-core`.
- Die Allowlist nennt exakte Datei und Symbole.
- `getSession()` kopiert einen `shared_ptr` unter kurzem Registrylock.
- Die eigentliche Kompilierung läuft nach Freigabe des Registrylocks.
- Jede `CompilerSession` serialisiert nur sich selbst.
- Zwei C-ABI-Sessions können parallel kompilieren.
- Sessionzerstörung während eines bereits gestarteten Calls bleibt durch den lokalen `shared_ptr` speichersicher.
- JSON-Schema, ABI-Version und exportierte Funktionsnamen bleiben unverändert.
- Handleoverflow wird nicht durch P2 verschlechtert.

### 28.3 Tests

Mindestens:

- zwei Sessions parallel, beide erfolgreich;
- eine gültige und eine ungültige Session parallel;
- Warnungsoptionen isoliert;
- Quellen mit gleichen Modellnamen, aber verschiedenen URIs/Inhalten isoliert;
- Resulthandles unabhängig;
- Destroy nach abgeschlossenem Call ohne Leak;
- wiederholte parallele Calls unter TSan.

---

## 29. WASM

- WebAssembly verwendet denselben C++-Core und denselben `CompilerContext`.
- Kein WASM-spezifischer globaler Compilerzustand darf eingeführt werden.
- Bestehende Exports und ABI bleiben erhalten.
- Node-/Browser-Wrapper behalten Verhalten und JSON-Schema.
- Die bestehende WASM-/JavaScript-Testsuite muss unverändert grün bleiben. Die externe `interlis-compiler-testsuite` ist im aktuellen Projekt ein nativer Prozessrunner; P2 DARF daraus keine nicht vorhandene WASM-Conformance-Infrastruktur erfinden. Falls der aktuelle Branch bereits einen WASM-Lauf dieser externen Suite besitzt, muss auch dieser unverändert grün bleiben.
- Emscripten ist typischerweise single-threaded; dies entschuldigt keine Globals im gemeinsamen Core.
- ThreadSanitizer wird nicht für Emscripten aktiviert.
- Contextdestruktion muss in wiederholten WASM-Sessionzyklen Speicher freigeben.
- Ein Node-Stresstest soll viele Session-create/compile/destroy-Zyklen ausführen und darf keinen monotonen, offensichtlichen Handle-/Result-Leak zeigen.

---

# Teil E – Global-State-Architekturwächter

## 30. Maschineller Source-Guard

Neue Datei:

```text
scripts/check-core-global-state.py
```

CTest-Name:

```text
ilic_no_mutable_compiler_globals
```

### 30.1 Zu scannender Bereich

Mindestens:

```text
include/ilic
source/core
source/input
source/metamodel
source/output
source/util
source/main
```

Generated ANTLR-Dateien dürfen eng ausgeschlossen werden. Ganze handgeschriebene Verzeichnisse dürfen nicht ausgeschlossen werden.

### 30.2 Explizit verbotene Symbole

Der Guard MUSS mindestens diese Muster erkennen:

```text
extern util::Logger Log
Logger Log
compilerMutex
activeSourceManager
setActiveSourceManager
ActiveSourceManagerScope
all_ilifiles
all_ilifiles_full
all_ilimodels
resetCompilerState
reset_compiler_state
reset_input_state
reset_mmobjects
register_mmobject
destroy_mmobject
push_context
pop_context
get_class_context
get_package_context
get_topic_context
get_model_context
input_file
UniversalClassesInitialized
CurrentSourceText
PendingMetaAttributes
PendingDocumentation
```

Alte Namen dürfen auch nicht nur in neue „Compatibility“-Dateien verschoben werden.

### 30.3 Mutable Namespace-/File-Statics

Der Guard SOLL Kommentare und Stringliterale entfernen und handgeschriebene mutable Variablendefinitionen auf Namespace-/Dateiebene erkennen.

Verdächtig:

```cpp
static std::string current;
static bool flag;
std::map<...> registry;
thread_local Foo* current;
```

Zulässig:

```cpp
constexpr ...
inline constexpr ...
static const unveränderliche Lookup-Tabelle
function-local static const ...
```

`const` reicht nur, wenn das referenzierte Objekt effektiv unveränderlich ist.

### 30.4 Allowlist

Neue Datei, falls nötig:

```text
scripts/core-global-state-allowlist.json
```

Jeder Eintrag enthält:

```json
{
  "path": "source/abi/Capi.cpp",
  "symbol": "sessions",
  "reason": "C ABI opaque handle registry; protected by registryMutex",
  "scope": "ilic-capi, not ilic-core"
}
```

Regeln:

- exakter Pfad;
- exakter Symbolname;
- konkrete Begründung;
- keine Wildcards;
- keine Verzeichnisallowlist;
- kein „generated“ ohne Beleg;
- ungenutzte/veraltete Allowlisteinträge lassen den Test fehlschlagen.

### 30.5 Optionaler Symboltest

Auf ELF-/Mach-O-Plattformen KANN zusätzlich ein `nm`-/`objdump`-Test die Corebibliothek auf bekannte mutable globale Symbole prüfen. Dieser Test ersetzt den Source-Guard nicht.

---

# Teil F – ThreadSanitizer und Buildsystem

## 31. ThreadSanitizer-Option

P0 hat ASan/UBSan. P2 ergänzt:

```cmake
option(
  ILIC_ENABLE_THREAD_SANITIZER
  "Enable ThreadSanitizer for native project targets"
  OFF
)
```

### 31.1 Gegenseitiger Ausschluss

CMake MUSS abbrechen, wenn gleichzeitig aktiv:

```text
ILIC_ENABLE_SANITIZERS=ON
ILIC_ENABLE_THREAD_SANITIZER=ON
```

### 31.2 Flags

Für unterstütztes Clang/GCC:

```text
-fsanitize=thread
-fno-omit-frame-pointer
-fno-sanitize-recover=all
```

Compile- und Linkflags müssen konsistent auf alle projektkontrollierten nativen Targets wirken, die im Testprozess zusammen geladen werden.

### 31.3 Nicht unterstützt

Klare Fehlermeldung bei:

- Emscripten;
- MSVC;
- unbekanntem Compiler;
- inkompatibler Sanitizerkombination.

### 31.4 Keine Unterdrückung

Nicht zulässig:

- globale TSan-Suppressions für Projektcode;
- `TSAN_OPTIONS=halt_on_error=0`;
- Ignorieren des Exitcodes;
- `continue-on-error`;
- Auslassen gerade der Concurrency-Tests;
- Mutex um den ganzen Compiler, nur um TSan grün zu machen.

Drittanbieterbefunde müssen untersucht werden. Eine eng begrenzte Suppression ist nur mit exaktem externem Stack, dokumentierter Version und separater Begründung zulässig; Projektframes dürfen dadurch nicht verdeckt werden.

---

## 32. CMake-Integration

Neue/angepasste Core-Sources mindestens:

```cmake
source/core/CompilationSourceStore.cpp
source/core/CompilerContext.cpp
source/core/CompilerPipeline.cpp
source/metamodel/MetaModelStore.cpp
source/metamodel/MetaModelBuilder.cpp
source/util/IliFileCatalog.cpp
```

Neue Testtargets siehe Testteil.

Alle neuen internen Header bleiben privat. `ilic-core` exportiert `CompilerContext` nicht.

CTest-Labels:

```text
architecture
concurrency
lifetime
metamodel
sources
output
capi
```

Empfohlene Tests:

```cmake
add_test(NAME ilic_no_mutable_compiler_globals ...)
set_tests_properties(... PROPERTIES LABELS "architecture")

add_test(NAME ilic_compiler_concurrency ...)
set_tests_properties(... PROPERTIES LABELS "concurrency")

add_test(NAME ilic_compiler_isolation ...)
set_tests_properties(... PROPERTIES LABELS "concurrency;core")

add_test(NAME ilic_compiler_lifetime ...)
set_tests_properties(... PROPERTIES LABELS "lifetime;core")
```

P0 und alle internen Tests bleiben Bestandteil von CTest. Die externe `interlis-compiler-testsuite` bleibt ein separater vollständiger End-to-End-Lauf und darf nicht durch CTest-Smoke-Tests ersetzt werden.

---

# Teil G – Detaillierte Testanforderungen

## 33. Neue Testdateien

Mindestens:

```text
test/core/CompilerConcurrencyTest.cpp
test/core/CompilerIsolationTest.cpp
test/core/CompilerLifetimeTest.cpp
test/metamodel/MetaModelStoreTest.cpp
test/metamodel/MetaModelBuilderTest.cpp
test/util/IliFileCatalogTest.cpp
test/output/OutputConcurrencyTest.cpp
test/abi/CapiConcurrencyTest.cpp
test/architecture/GlobalStateTest.py
```

Dateinamen dürfen an die bestehende Struktur angepasst werden. Der Inhalt ist verbindlich.

Alle C++-Tests verwenden P0 `ILIC_REQUIRE*`, nie Standard-`assert`.

---

## 34. Synchronisationshilfen für Tests

C++17 besitzt kein `std::barrier`. Implementiere testlokal:

```cpp
class StartGate final {
public:
    explicit StartGate(std::size_t participants);

    bool arriveAndWait(
        std::chrono::milliseconds timeout =
            std::chrono::seconds(10));

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::size_t remaining_;
    std::uint64_t generation_ = 0;
};
```

Tests dürfen nicht nur mit `sleep_for` Parallelität „erzwingen“.

Für Workerfehler:

- Threads schreiben Resultate/Exceptions in eigene Slots.
- Nach `join()` prüft der Hauptthread.
- Kein `ILIC_REQUIRE` in Workerthreads, wenn das Test-Harness den Prozess sofort beendet und dadurch Debugging verschlechtert; stattdessen `std::exception_ptr`.
- Alle Threads werden auch auf Fehlerpfaden gejoint, vorzugsweise RAII.

---

## 35. `CompilerConcurrencyTest`

### 35.1 Verschiedene gültige Sessions

Erzeuge mindestens 8 Sessions mit eindeutigen:

- Source-URIs;
- Modellnamen;
- Topicnamen;
- Attributen;
- Dokumentationen;
- Metaattributen.

Starte sie gleichzeitig. Prüfe pro Resultat:

- `success=true`;
- `errorCount=0`;
- exakt erwartetes Modell;
- keine fremde URI;
- keine fremde Dokumentation;
- keine fremden Metaattribute;
- keine Diagnose aus einer anderen Session.

Mindestens 50 Wiederholungen in einem Prozess oder konfigurierbare Wiederholungszahl.

### 35.2 Gültig und ungültig parallel

Parallel:

- Session A: gültiges Modell;
- Session B: unbekannter Domainname;
- Session C: Syntaxfehler;
- Session D: Translation-Mismatch.

Prüfe:

- A bleibt erfolgreich;
- B erhält nur erwarteten Namensdiagnosecode;
- C erhält Syntaxdiagnose;
- D erhält Translationcode;
- Counts, Ranges und URIs sind isoliert.

### 35.3 `parse` und `compile`

- `parse` in Session A;
- `compileAndAnalyze` in Session B;
- mehrere Iterationen;
- Syntax-Snapshot von A darf keine B-Daten enthalten.

### 35.4 Gleichzeitige unterschiedliche Sprachversionen

Parallel mindestens:

- INTERLIS 1.0;
- INTERLIS 2.3;
- INTERLIS 2.4.

Prüfe, dass keine globalen `ili23`/`ili24`-Flags übersprechen.

### 35.5 Übersetzungsmodelle

Zwei unabhängige Übersetzungsmodellpaare parallel. Prüfe `_translationOf` indirekt über Semantic Snapshot und Diagnosen.

### 35.6 Kein globales Serialisieren

Der Source-Guard MUSS den globalen Mutex verhindern. Zusätzlich SOLL der Test eine interne, test-only Phasenbeobachtung oder eine andere deterministische Methode verwenden, um zu belegen, dass zwei Contexts gleichzeitig die Pipeline betreten können.

Eine solche Beobachtung:

- bleibt in internen Testzugängen;
- wird nicht Teil der öffentlichen API;
- beeinflusst Produktionsverhalten nicht;
- verwendet keine Zeitmessung als alleinigen Beleg.

---

## 36. `CompilerIsolationTest`

### 36.1 Wiederholte Läufe derselben Session

Ablauf:

1. gültiges Modell A kompilieren;
2. Source entfernen;
3. ungültiges Modell B hinzufügen/kompilieren;
4. B korrigieren/kompilieren;
5. A wieder hinzufügen/kompilieren.

Nach jedem Lauf:

- Counts beginnen bei null;
- Diagnosen gehören nur zum Lauf;
- Modelle gehören nur zu aktuell erreichbaren Sources;
- keine alten Imports;
- keine alten Metaattribute;
- keine alte Warnungsoption;
- keine alten Missing Models.

### 36.2 Optionen

Parallel unterschiedliche Sessions:

- `warningsAsErrors=true/false`;
- `autoSearch=true/false`;
- verschiedene `modelDirectories`;
- verschiedene External MetaAttributes.

Kein Optionswert darf die andere Session beeinflussen.

### 36.3 Gleiche Namen, verschiedene Quellen

Zwei Sessions enthalten jeweils `MODEL SameName`, aber unterschiedliche Struktur. Beide Snapshots müssen ihre eigene Struktur zeigen.

### 36.4 Source-Ranges

Verwende UTF-8 mit:

- Umlaut;
- Emoji ausserhalb BMP;
- verschiedene Zeilenenden, soweit unterstützt.

Prüfe URI, Byteoffset, Zeile und UTF-16-Spalte pro Session.

### 36.5 Context-Exception

Erzeuge kontrolliert einen internen Parser-/Sinkfehler. Danach muss ein neuer Lauf derselben Session sauber funktionieren. Keine Source-/Kategorie-/Indent-/Context-Tiefe darf hängen bleiben.

---

## 37. `CompilerLifetimeTest`

### 37.1 Viele Sessions

Mindestens:

- 1000 create/compile/destroy-Zyklen in Release/ASan;
- kleine gültige und ungültige Modelle alternierend;
- keine Leaks;
- keine Use-after-free;
- keine Double-free.

Die Zahl darf für TSan reduziert werden, nicht auf null.

### 37.2 `compileAndAnalyze`

Snapshotwerte werden nach Zerstörung des internen Contexts weiter gelesen:

- Symbole;
- Referenzen;
- Dokumentation;
- Diagnosen;
- Source-Ranges;
- Diagramm.

Der Test darf keinen internen Pointerzugriff benötigen.

### 37.3 Sink-Lebensdauer

Logger-Sinks erhalten Kopien/const Referenzen nur während des Calls. Nach Rückgabe werden Resultwerte verwendet, nicht gespeicherte interne Referenzen.

### 37.4 Destruktionsbeleg

Test-only Objektzähler oder ein spezieller `MMObject`-Subtyp belegt:

- jedes arena-owned Objekt genau einmal zerstört;
- `objectCount()` vor Destruktion plausibel;
- nach `clear()` null;
- wiederholtes `clear()` sicher.

---

## 38. `MetaModelStoreTest`

Pflichtfälle:

1. `make<T>` liefert stabilen, vom Store besessenen Pointer.
2. verschiedene Typen werden korrekt polymorph zerstört.
3. `owns(nullptr)==false`.
4. Objektzahl steigt/fällt korrekt.
5. `discardUnlinked` löscht genau ein Objekt.
6. registrierte Objekte dürfen nicht versehentlich discarded werden.
7. zwei Stores besitzen disjunkte Objekte.
8. AnyClass/AnyStructure sind pro Store unterschiedlich.
9. Built-in-INTERLIS-Pointer ist pro Store.
10. Registrygetter liefern const Referenzen.
11. Import existiert nur in einer Registry.
12. Destruktionsreihenfolge ist deterministisch oder mindestens speichersicher.
13. Clone landet im Zielstore.
14. Clone enthält keine Pointer auf temporär zerstörte Clone-Unterobjekte.
15. Store ist compile-time nicht kopierbar/nicht bewegbar.

---

## 39. `MetaModelBuilderTest`

Pflichtfälle:

### 39.1 ContextScope

- leerer Stack;
- ein Model;
- Model → Topic → Class → Attribute;
- Getter liefern korrekten Typ;
- Verschachtelung wird beim Scopeende zurückgesetzt;
- Exception-Unwinding setzt Tiefe zurück;
- zwei Builder parallel haben getrennte Stacks.

### 39.2 SourceScope

- URI/Text gesetzt;
- verschachtelte Source wird restauriert;
- UTF-8-Tokenrange korrekt;
- Pending MetaAttributes gehören nur zur Source;
- Dokumentationskommentare leaken nicht;
- Exception-Unwinding restauriert Source.

### 39.3 Sprachversion

Zwei Builder gleichzeitig mit 2.3 und 2.4; Flags bleiben getrennt.

### 39.4 Registries

- add/find in Store A;
- gleichnamige Objekte in Store B;
- keine Cross-Store-Auflösung.

### 39.5 Fehlerdiagnosen

Fehlende Typen/Modelle erzeugen Diagnosen im zugehörigen Logger, nicht in einem anderen Logger.

---

## 40. `IliFileCatalogTest`

Pflichtfälle:

1. Root aus Session-SourceManager.
2. Root aus Dateisystem.
3. Sessionquelle gewinnt bei gleicher URI.
4. gleicher URI wird einmal geparst.
5. Modellindex nach erfolgreichem Headerparse.
6. mehrere Modelle in einer Datei.
7. Imports plus `TRANSLATION OF`.
8. Version 1.0/2.3/2.4.
9. falsche Version wird nicht geliefert.
10. `autoSearch=false`.
11. mehrere `modelDirectories` in stabiler Reihenfolge.
12. fehlendes Verzeichnis erzeugt Fehler ohne Prozessende.
13. Built-in INTERLIS pro Context.
14. Dateinamen-Gross-/Kleinschreibung auf Windows/Linux.
15. Katalogdestruktion löscht alle Files.
16. zwei Kataloge parallel mit gleichen Modellnamen.
17. keine globalen selected files.
18. keine doppelten Deletes.

---

## 41. `OutputConcurrencyTest`

Für jeden aktiv genutzten Generator:

- zwei unabhängige Stores;
- zwei Logger;
- zwei temporäre Zielverzeichnisse/-dateien;
- parallele Generierung;
- Ausgabe A enthält nur Modell A;
- Ausgabe B enthält nur Modell B;
- wiederholter Lauf ist bytegleich;
- keine fremden Warnungen;
- Writer sauber geschlossen;
- Ausgabe kann anschliessend umbenannt/gelöscht werden, auch auf Windows.

Mindestens Ili1, Ili2, XSD, IMD und GML, soweit diese Generatoren in der bestehenden Suite aktiv unterstützt sind.

---

## 42. C-ABI-Concurrency-Test

Der Test MUSS die echte C-ABI verwenden, nicht direkt `CompilerSession`.

Ablauf:

1. N Sessionhandles erzeugen.
2. pro Handle eigene Source setzen.
3. StartGate.
4. parallele `ilic_compile`-/`ilic_compile_and_analyze`-Calls.
5. JSON lesen.
6. Resulthandles zerstören.
7. Sessionhandles zerstören.
8. wiederholen.

Prüfe:

- gültiges JSON;
- korrekte Fallzuordnung;
- keine Handleverwechslung;
- keine globale Core-Serialisierung;
- keine TSan-Befunde;
- kein Leak.

---

## 43. Same-Session-Vertragstest

Zwei Threads rufen dieselbe `CompilerSession` auf.

Erwartung:

- Operationen werden sauber serialisiert;
- keine Datenrasse;
- beide Resultate sind gültig;
- keine Deadlocks;
- die Sourcebasis ist konsistent zum Zeitpunkt des jeweiligen Locks.

Ein gleichzeitiges direktes Mutieren des über `sources()` erhaltenen `SourceManager&` ist ausserhalb des garantierten Vertrags und wird dokumentiert, nicht künstlich unterstützt.

---

## 44. Verbindliche Regression über `interlis-compiler-testsuite`

### 44.1 Rolle der Suite

Die externe Suite ist der verbindliche fachliche Regressionstest des P2-Refactorings.
Sie enthält eine grosse Zahl realer und aus `ili2c` abgeleiteter Modelle. Interne
Unit- und Architekturtests beweisen Ownership, Lebensdauer und Parallelität; die
externe Suite beweist, dass der umgebaute Compiler weiterhin dieselben Modelle
akzeptiert beziehungsweise ablehnt.

Beide Ebenen sind notwendig. Die externe Suite ersetzt keine fokussierten P2-Tests,
und fokussierte P2-Tests ersetzen nicht die externe Suite.

### 44.2 Quelle der Wahrheit für den Aufruf

Der Agent MUSS den Befehl aus dem aktuellen `.github/workflows/ci.yml` lesen. Im
Referenzstand lautet der semantisch relevante Aufruf:

```bash
java conformance-suite/InterlisConformance.java run \
  --suite conformance-suite/suite/test-cases.json \
  --compiler ilic=build/conformance-ilic/ilic \
  --baseline ilic=conformance/known-failures.json \
  --out build/conformance
```

Lokal mit dem Nachbarrepository ist derselbe Lauf sinngemäss:

```bash
suite=../interlis-compiler-testsuite
baseline=<AUS_CI_YML>
binary=build/p2-conformance-release/ilic
out=build/p2-conformance/release

java "$suite/InterlisConformance.java" run \
  --suite "$suite/suite/test-cases.json" \
  --compiler "ilic=$binary" \
  --baseline "ilic=$baseline" \
  --out "$out"
```

Der Agent MUSS Pfade an Generator und Plattform anpassen, aber nicht die Bedeutung
des Befehls verändern.

### 44.3 Referenz-Build des Compilers

Der CI-Conformance-Build ist absichtlich schlank und vom internen CTest-Build
getrennt. Im Referenzstand:

```bash
cmake -S . -B build/p2-conformance-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF \
  -DILIC_STATIC_DISTRIBUTION=OFF \
  -DBUILD_TESTING=OFF
cmake --build build/p2-conformance-release --target ilic --parallel
```

P2 MUSS diesen Modus erhalten. Die Suite prüft das tatsächlich ausgelieferte
Compilerprogramm und darf nicht versehentlich gegen ein Test-Doppel, einen Wrapper
mit abweichendem Verhalten oder ein anderes Binary laufen.

Vor jedem Lauf MUSS der Agent dokumentieren:

```bash
realpath "$binary"
sha256sum "$binary"              # auf macOS entsprechend shasum -a 256
"$binary" -version || "$binary" --version
```

### 44.4 Baseline der bekannten Probleme

Die Baseline ist eine enge Liste bereits bekannter `ilic`-Abweichungen. Sie ist kein
allgemeines XFAIL-System für P2.

Normative Regeln:

1. Der Baseline-Pfad wird aus `ci.yml` ermittelt.
2. Die Datei wird vor P2 gehasht und vollständig inventarisiert.
3. Der Inhalt bleibt während eines reinen Refactorings byteidentisch.
4. Ein neuer Eintrag wegen einer P2-Regression ist verboten.
5. Ein geänderter `actualOutcome` zur Anpassung an eine P2-Regression ist verboten.
6. Ein breiterer Matcher, Wildcard oder Kategorien-Ausschluss ist verboten.
7. Wird ein bekannter Fehler tatsächlich behoben, darf der exakte Eintrag entfernt
   werden, aber nur mit fokussiertem Regressionstest und erfolgreichem vollständigem
   Suite-Lauf.
8. Eine Entfernung darf nicht mit einer anderen neuen Ausnahme verrechnet werden.
9. Jeder Baseline-Diff wird im Abschlussbericht zeilenweise erklärt.
10. Ohne absichtliche Fehlerbehebung MUSS gelten:

```bash
git diff --exit-code -- <BASELINE_DATEI>
```

### 44.5 Aktuelle Referenzfälle

Im Referenzstand `conformance/known-failures.json` sind folgende elf Test-IDs
registriert. Der Agent MUSS sie aus der Datei neu lesen und darf diese Liste nicht
als dauerhaft aktuell annehmen:

```text
ili10.InterlisConversion23Test.iliconversion_CheckConversionOfIli1ToIli2
ili10.InterlisConversion23Test.iliconversion_Ili2StructForili1Line
ili10.InterlisFmt10Test.ilifmt_AllIli1Datatypes
ili10.References10Test.references_DetectRecursiveRefAttr_Fail
ili10.Schema10Test.datatypes_GenerateIli1Datatypes
ili10.Schema10Test.decimal_GenerateIli1DecWithScaling
ili10.Surface10Test.surface_AcceptIli1Surface
ili10.Translation10Test.attrTypeFail
ili23.Attribute10Test.attributeExplFail
ili23.Association23Test.association_RolenameConflict_Fail
ili23.MainRule23Test.mainRules_IllegalIliVersion
```

Besonders wichtig für P2 sind Fälle mit `actualOutcome: internal-error`. Das
Refactoring von Context, Ownership und Abbruchbehandlung DARF diese nicht in Crash,
Deadlock, Datenvermischung oder unkontrollierten Prozessabbruch verschlechtern. Werden
sie zu einer normalen fachlichen Ablehnung verbessert, ist dies eine mögliche echte
Fehlerbehebung und muss bewusst adjudiziert werden.

### 44.6 Ausführungszeitpunkte

Die vollständige Suite MUSS laufen:

- einmal vor jeder P2-Änderung;
- nach Migration der Diagnoseengine;
- nach Migration von Source-/IliFileCatalog;
- nach Einführung von `MetaModelStore` und Builder;
- nach vollständiger Ili1-Parsermigration;
- nach vollständiger Ili2-Parsermigration;
- nach Migration von Semantic-/Translation-Checker;
- nach Umbau von `CompilerSession` und Entfernung des globalen Mutex;
- nach Migration der CLI, da die Suite das Executable aufruft;
- nach Output-/Visitoränderungen, sofern diese den normalen Compilerpfad berühren;
- im endgültigen Release-Build;
- im Debug-Build;
- im ASan-/UBSan-/LSan-Build;
- im TSan-Build;
- in GitHub Actions.

Bei einem sehr kleinen, rein lokalen Zwischenschritt darf zunächst ein fokussierter
Unit-Test laufen. Vor dem nächsten abgeschlossenen Migrations-Commit ist jedoch die
vollständige externe Suite erforderlich.

### 44.7 Vergleich vor und nach P2

Der Agent MUSS nicht nur den Exitcode vergleichen. Er MUSS die vom Runner erzeugten
Reports aufbewahren und die verfügbaren maschinenlesbaren Zusammenfassungen
vergleichen.

Mindestens zu vergleichen:

- Gesamtzahl der Testfälle;
- akzeptiert/abgelehnt/internal-error/timeout, soweit vom Runner ausgewiesen;
- bekannte Baselinefälle;
- unerwartete neue Fehler;
- unerwartet behobene Fälle;
- fehlende oder nicht ausgeführte Fälle;
- Laufzeitabbrüche;
- Compiler-Exitcodes;
- Änderungen an Test-IDs oder Suite-Manifest.

Die konkreten Reportdateinamen sind aus dem tatsächlichen Suite-Output zu ermitteln.
Der Agent DARF keine Dateinamen erfinden und danach nur leere Dateien vergleichen.

### 44.8 Fehlertriage

Bei einem neuen Suitefehler MUSS der Agent:

1. Test-ID nennen;
2. betroffene Modellpfade aus dem Suite-Manifest ermitteln;
3. den exakten `ilic`-Aufruf oder einen minimal äquivalenten Aufruf reproduzieren;
4. Vorher- und Nachherverhalten vergleichen;
5. Diagnoseausgabe, Exitcode und Sanitizerausgabe sichern;
6. den zuletzt migrierten Zustandsbereich untersuchen;
7. die Implementierung korrigieren;
8. den fokussierten Fall erneut laufen lassen;
9. danach die vollständige Suite erneut ausführen.

Nicht zulässig ist, zuerst die Baseline zu erweitern und die Ursache später zu
untersuchen.

### 44.9 Suite-Unveränderlichkeit

P2 arbeitet ausschliesslich im `ilic-fork`-Repository. Der Agent DARF im
Nachbarrepository nicht:

- Dateien ändern;
- Fixtures anpassen;
- Erwartungen umschreiben;
- Tests deaktivieren;
- generierte Dateien committen;
- `git clean`, `git reset`, `git checkout` oder `git switch` ausführen;
- den Branch wechseln;
- den bestehenden Arbeitsbaum auf den CI-Pin zurücksetzen.

Ein temporärer detached Worktree ist zulässig und nach Abschluss sauber zu entfernen:

```bash
git -C ../interlis-compiler-testsuite worktree remove "$suite_worktree"
```

Nur der vom Agenten selbst erzeugte Worktree darf entfernt werden.

### 44.10 Sanitizerläufe

Für ASan/UBSan/LSan:

```bash
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:strict_string_checks=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
java "$suite/InterlisConformance.java" run \
  --suite "$suite/suite/test-cases.json" \
  --compiler "ilic=$asan_binary" \
  --baseline "ilic=$baseline" \
  --out build/p2-conformance/sanitizers
```

Für TSan:

```bash
TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
java "$suite/InterlisConformance.java" run \
  --suite "$suite/suite/test-cases.json" \
  --compiler "ilic=$tsan_binary" \
  --baseline "ilic=$baseline" \
  --out build/p2-conformance/tsan
```

Der Runner startet `ilic` als Kindprozess. Sanitizerumgebungsvariablen müssen daher
an diese Prozesse vererbt werden. Der Agent MUSS prüfen, dass dies tatsächlich
geschieht und dass Sanitizerfehler nicht vom Runner als normale fachliche Ablehnung
fehlklassifiziert werden.

### 44.11 Concurrency-Grenze der Suite

Die externe Suite ist umfangreich, beweist aber nicht automatisch echte Parallelität
mehrerer `CompilerSession`-Instanzen, weil sie den CLI-Compiler typischerweise als
separate Prozesse startet. Deshalb bleiben die P2-internen Threadtests zwingend:

- `CompilerConcurrencyTest`;
- `DiagnosticIsolationTest`;
- `CapiConcurrencyTest`;
- Lifetime- und Storetests;
- TSan-Ausführung dieser Tests.

Die externe Suite belegt semantische Breite; interne Tests belegen In-Process-
Reentranz. Keine der beiden Ebenen darf als Ersatz für die andere dargestellt werden.

### 44.12 CI-Vertrag

Der bestehende `conformance`-Job MUSS erhalten bleiben. P2 darf ihn refaktorieren,
aber nicht semantisch abschwächen.

Zu erhalten sind mindestens:

- gepinnter Suite-Commit;
- Download oder Checkout genau dieses Commits;
- Java 21 oder die im aktuellen Workflow verwendete Version;
- Build von `ilic` mit den bestehenden Conformance-Optionen;
- vollständiger Runnerlauf;
- Verwendung der ilic-Baseline;
- Reportupload auch bei Fehlern;
- anschliessende Erzwingung des Runner-Exitcodes.

Der Suite-Pin DARF in P2 nur geändert werden, wenn dies ausdrücklich Teil des
Auftrags wird. Ein zufälliges Update des externen Korpus würde die Refactoringbaseline
unbrauchbar machen.

P2 SOLL zusätzlich im TSan-Job die vollständige externe Suite gegen das
TSan-instrumentierte Binary ausführen. Falls die Laufzeitgrenze dies nachweislich
verhindert, muss mindestens ein separater vollständiger TSan-Conformance-Job mit
angemessenem Timeout eingerichtet werden; eine reduzierte Fallmenge ist nicht die
bevorzugte Lösung.

---

# Teil H – Migrationsplan

## 45. Phase 0: Baseline und Inventar

Erzeuge:

```text
docs/architecture/p2-global-state-inventory.md
build/p2-baseline/
```

Inventar enthält pro Fund:

- Symbol;
- Datei/Zeile;
- Lebensdauer;
- Leser;
- Schreiber;
- Ownership;
- Synchronisation;
- Zielklasse;
- Migrationsphase;
- Test, der die Migration belegt.

Führe P0/CTest, die externe `interlis-compiler-testsuite` und die bestehenden WASM-/Pakettests vollständig aus.

## 46. Phase 1: Logger instanzfähig

- globales `Log` nicht sofort blind löschen;
- zuerst Konstruktorinjektion in ErrorListener und neue Klassen;
- Logger-intern `exit` durch `CompilerAbort`;
- RAII-Scopes;
- fokussierte Tests;
- danach alle Core-Aufrufer migrieren;
- am Phasenende kein globales `Log` im Core.

Keine temporäre globale Loggerbrücke darf über die nächste Phase hinaus bestehen.

## 47. Phase 2: Explizite Source-Sicht

- `CompilationSourceStore`;
- IliFile-Lader auf explizite Source-Sicht;
- Parserrange auf SourceBuffer;
- `activeSourceManager` entfernen;
- Syntax-/UTF-8-Tests.

## 48. Phase 3: `IliFileCatalog`

- Daten/Visitor trennen;
- Ownership via `unique_ptr`;
- Indexe;
- Optionen instanzlokal;
- Dateisystemfehler ohne `exit`;
- alte Globals entfernen;
- Catalog-Tests.

## 49. Phase 4: `MetaModelStore`

- virtueller `MMObject`-Destruktor;
- Arena;
- alle Registries;
- Universalobjekte;
- Built-in-Pointer;
- Clone explizit;
- alte Objektregistry entfernen;
- Store-Tests/ASan.

## 50. Phase 5: `MetaModelBuilder`

- Sprachversion;
- SourceScope;
- ContextScope;
- Pending Metadata;
- Init-/Add-/Find-Methoden;
- alte Globals und Kontextfunktionen entfernen;
- Builder-Tests.

## 51. Phase 6: Parsermigration

Reihenfolge empfohlen:

1. IliFile-Headerparser;
2. Ili1;
3. Ili2 Hauptdatei;
4. Ili2 package/model;
5. type;
6. class/association;
7. path/expression;
8. constraints;
9. views/graphics;
10. helper.

Nach jeder Gruppe fokussierte interne Tests. Nach jeder abgeschlossenen Parser-Migrationsgruppe zusätzlich die vollständige externe `interlis-compiler-testsuite`; ein selbst definierter Teil-Shard ersetzt diesen Lauf nicht.

## 52. Phase 7: Checker und Snapshot

- SemanticChecker;
- TranslationChecker;
- SnapshotBuilder;
- keine globale Registry mehr;
- Diagnose-/Metamodelltests.

## 53. Phase 8: Visitor und Outputs

- Store/Logger-Konstruktor;
- mutable statics in Member;
- alle Generatoren;
- Paralleloutputtests;
- Golden Outputs.

## 54. Phase 9: CompilerContext und Pipeline

- Context zusammensetzen;
- Pipeline;
- `CompilerSession::Impl`;
- per-Session-Mutex;
- globalen Mutex entfernen;
- Cleanupflags entfernen;
- compile/analyze/compileAndAnalyze.

## 55. Phase 10: CLI, C-ABI, WASM

- CLI lokal;
- C-ABI-Concurrency;
- WASM;
- keine alte Parallelpipeline.

## 56. Phase 11: Guard und TSan

- Source-Guard;
- exakte Allowlist;
- ThreadSanitizer-CMake;
- CI;
- Stress.

## 57. Phase 12: Endbereinigung

- alte Header/API löschen;
- keine Compatibility-Bridges;
- Includebereinigung;
- Dokumentation;
- vollständige Matrix;
- Diffkontrolle.

---

# Teil I – CI

## 58. Bestehende Jobs erhalten

Müssen bleiben:

- separater vollständiger `conformance`-Job mit der externen Suite;
- Native Release Linux;
- Native Release macOS;
- Native Release Windows;
- Native Debug Linux;
- Native ASan+UBSan Linux;
- WASM/npm;
- P0 und die vollständige interne CTest-Suite.

Der separate `conformance`-Job darf nicht in einen kleinen CTest-Smoke-Test umgewandelt werden.

## 59. Neuer TSan-Job

Empfohlen:

```yaml
native-thread-sanitizer-linux:
  name: Native TSan (Linux x86_64)
  runs-on: ubuntu-latest
  timeout-minutes: 90
  steps:
    - uses: actions/checkout@v6
    - name: Install dependencies
      run: sudo apt-get update &&
           sudo apt-get install --yes clang ninja-build
    - name: Configure
      run: >
        cmake -S . -B build/tsan -G Ninja
        -DCMAKE_BUILD_TYPE=Debug
        -DBUILD_TESTING=ON
        -DILIC_ENABLE_THREAD_SANITIZER=ON
        -DILIC_ENABLE_SANITIZERS=OFF
        -DILIC_STATIC_DISTRIBUTION=OFF
    - name: Build
      run: cmake --build build/tsan --parallel
    - name: Run architecture and concurrency tests
      env:
        TSAN_OPTIONS: halt_on_error=1:second_deadlock_stack=1
      run: >
        ctest --test-dir build/tsan
        --output-on-failure
        -L "architecture|concurrency|lifetime"
    - name: Run remaining compiler tests
      env:
        TSAN_OPTIONS: halt_on_error=1:second_deadlock_stack=1
      run: ctest --test-dir build/tsan --output-on-failure
    - name: Download pinned conformance suite
      shell: bash
      run: |
        set -euo pipefail
        suite_commit=<SAME_PIN_AS_CONFORMANCE_JOB>
        archive="$RUNNER_TEMP/interlis-conformance-suite.tar.gz"
        curl --fail --location --retry 3 \
          "https://codeberg.org/edigonzales/interlis-compiler-testsuite/archive/${suite_commit}.tar.gz" \
          --output "$archive"
        mkdir -p conformance-suite
        tar --extract --gzip --strip-components=1 \
          --file="$archive" --directory=conformance-suite
    - name: Run full conformance suite under TSan
      env:
        TSAN_OPTIONS: halt_on_error=1:second_deadlock_stack=1
      run: |
        set -euo pipefail
        java conformance-suite/InterlisConformance.java run \
          --suite conformance-suite/suite/test-cases.json \
          --compiler ilic=build/tsan/ilic \
          --baseline ilic=<SAME_BASELINE_AS_CONFORMANCE_JOB> \
          --out build/conformance-tsan
    - name: Upload TSan conformance reports
      if: always()
      uses: actions/upload-artifact@v6
      with:
        name: ilic-conformance-tsan
        path: build/conformance-tsan/
        if-no-files-found: warn
```

Native Repository darf im TSan-Job nur entsprechend dem bereits für den Conformance-Build verwendeten, dokumentierten Setup deaktiviert werden. Die vollständige externe `interlis-compiler-testsuite` muss trotzdem gegen das TSan-instrumentierte `ilic` laufen. Dies ist im Workflow zu kommentieren.

## 60. CI-Artefakte

Bei Fehlern sollen hochgeladen werden:

- TSan-Log;
- vollständige Reports und Diffs der externen `interlis-compiler-testsuite`;
- Global-State-Inventory/Guard-Ausgabe;
- Output-Golden-Diffs.

Keine Artefakte mit Secrets.

---

# Teil J – Dokumentation

## 61. Neue Architekturdokumentation

Mindestens:

```text
docs/architecture/compiler-context.md
docs/architecture/metamodel-ownership.md
docs/architecture/thread-safety.md
docs/architecture/p2-global-state-inventory.md
```

### 61.1 `compiler-context.md`

- Lebensdauern;
- Abhängigkeitsgraph;
- per-Run-Context;
- Pipeline;
- warum kein Singleton/thread_local;
- Erweiterungspunkt für P5.

### 61.2 `metamodel-ownership.md`

- Owner vs non-owning raw pointers;
- Arena;
- Storegrenzen;
- Clone;
- Destruktion;
- keine Pointer in öffentlichen Snapshots.

### 61.3 `thread-safety.md`

Vertrag:

| Operation | gleiche Session | verschiedene Sessions |
|---|---|---|
| `putSource/removeSource` | serialisiert | parallel |
| `parse` | serialisiert | parallel |
| `compile` | serialisiert | parallel |
| `compileAndAnalyze` | serialisiert | parallel |
| direkter mutabler `sources()`-Zugriff | externe Synchronisation | unabhängig |
| C-ABI verschiedene Handles | parallel | parallel |
| Generator gleiche Zieldatei | nicht garantiert | nicht garantiert |

### 61.4 Global-State-Inventar

Vorher-/Nachher-Tabelle; jeder alte Fund muss entweder migriert oder exakt begründet allowlisted sein.

---

# Teil K – Qualitäts- und Akzeptanzkriterien

## 62. Funktionale Gleichheit

- Der `conformance`-Job aus `.github/workflows/ci.yml` bleibt semantisch erhalten.
- Der gepinnte Commit der externen Suite bleibt unverändert, sofern kein separater Auftrag ihn aktualisiert.
- Die vollständige externe Suite läuft vor und nach P2 mit demselben Manifest und derselben Baseline.
- Anzahl und Identität der Baseline-Einträge bleiben unverändert, ausser ein Altfehler wird nachweislich behoben.
- Kein neuer unerwarteter Accepted/Rejected/Internal-Error/Timeout-Unterschied.
- Keine Suite-Test-ID verschwindet und kein Suite-Lauf wird gefiltert.
- Alle P0-Tests grün.
- Die vollständige externe `interlis-compiler-testsuite` ist mit unveränderter Baseline grün.
- Kein neuer Baseline-/Known-Issue-Eintrag für Refactorregressionen.
- Bestehende native C-ABI-/CLI-Resultate und die durch die vorhandenen WASM-Pakettests geprüften JSON-Resultate bleiben unverändert.
- CLI-Exitcodes unverändert.
- C-ABI-JSON unverändert.
- Golden Outputs unverändert.
- Diagnosecodes, Ranges und relevante Reihenfolge unverändert.

## 63. Architektur

- kein `compilerMutex`;
- kein globales `Log`;
- kein aktiver SourceManager;
- keine globalen IliFile-Registries;
- keine globale Metamodellarena;
- kein globaler Contextstack;
- keine globalen Sprachflags;
- keine runabhängigen mutable Outputstatics;
- kein Core-`exit`;
- Context intern, nicht öffentlich;
- eindeutige Ownership;
- kein globales Resetprotokoll.

## 64. Parallelität

- verschiedene Sessions parallel korrekt;
- gleiche Session serialisiert;
- C-ABI verschiedene Handles parallel;
- TSan ohne Projektbefund;
- keine Deadlocks im Stresslauf;
- kein globaler Lock um vollständige Kompilierung.

## 65. Lebensdauer

- ASan/UBSan grün;
- Leak Detection grün;
- 1000 Sessionzyklen ohne Leak;
- Storeobjekte genau einmal zerstört;
- Snapshots nach Contextdestruktion gültig;
- keine Use-after-free;
- keine Double-free;
- keine Cross-Store-Pointer.

## 66. Portabilität

- Linux Release/Debug;
- macOS Release;
- Windows Release;
- Clang ASan/UBSan;
- Clang TSan Linux;
- Emscripten/WASM;
- C++17.

## 67. Wartbarkeit

- neue Klassen haben klare Headerkommentare zu Ownership/Lebensdauer;
- keine generische Context-Service-Locator-Nutzung;
- keine breite Allowlist;
- keine doppelte alte/neue API;
- keine unbeteiligte Massenformatierung;
- Commits logisch trennbar.

---

# Teil L – Verbotene Abkürzungen

## 68. Strikt verboten

1. globalen Mutex nur umbenennen;
2. einen Mutex in `Logger`/Store legen und Globals behalten;
3. `thread_local CompilerContext`;
4. `thread_local` Metamodellstore;
5. Singleton;
6. globale Map `sessionId -> context`;
7. Reset vor/nach jedem Lauf;
8. CompilerContext als öffentliches API-Objekt;
9. Context überall als Service Locator übergeben;
10. `shared_ptr` für alle Graphkanten;
11. Leaks oder „process lifetime ownership“;
12. Raw `new` ohne Storeownership;
13. direkte `delete`-Aufrufe für publizierte MMObjects;
14. Standard-`assert`;
15. Deaktivieren interner Tests oder Filtern/Verkleinern der externen Conformance-Suite;
16. neue XFAILs für Refactorfehler;
17. TSan-Suppressions für Projektcode;
18. `continue-on-error`;
19. `exit()` im Core;
20. zweite CLI-Compilerimplementierung;
21. Native/WASM getrennte Corepfade;
22. Änderung der JSON-Schemaversion ohne Not;
23. C++20-Barrieren/Features;
24. breiter Umbau der INTERLIS-Semantik;
25. inkrementelle Caches als P2-Nebenschauplatz;
26. riesiger unreviewbarer Einmalcommit;
27. Source-Guard mit Wildcard-Allowlist;
28. blosses Dokumentieren statt Entfernen der Globals.

---

# Teil M – Verifikation

## 69. Source-Suchen

Müssen leer sein oder nur explizit zulässige Dokumentations-/Testreferenzen zeigen:

```bash
rg -n 'extern\s+util::Logger\s+Log|(?:^|[^:[:alnum:]_])Logger\s+Log\b' \
  include source

rg -n 'compilerMutex|activeSourceManager|setActiveSourceManager|ActiveSourceManagerScope' \
  include source

rg -n 'all_ilifiles|all_ilifiles_full|all_ilimodels' source

rg -n 'resetCompilerState|reset_compiler_state|reset_input_state|reset_mmobjects' \
  include source

rg -n 'push_context|pop_context|get_(class|package|topic|model)_context' \
  include source

rg -n '\b(input_file|ili23|ili24|iliversion)\b' \
  source/input source/metamodel

rg -n '\b(exit|abort|quick_exit|_Exit)\s*\(' \
  source/core source/input source/metamodel source/output source/util
```

Der maschinelle Guard ist zusätzlich auszuführen.

## 70. Release

```bash
cmake -S . -B build/p2-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=OFF \
  -DILIC_ENABLE_THREAD_SANITIZER=OFF

cmake --build build/p2-release --parallel
ctest --test-dir build/p2-release --output-on-failure
```

## 71. Debug

```bash
cmake -S . -B build/p2-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON

cmake --build build/p2-debug --parallel
ctest --test-dir build/p2-debug --output-on-failure
```

## 72. ASan/UBSan

```bash
cmake -S . -B build/p2-asan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON \
  -DILIC_ENABLE_THREAD_SANITIZER=OFF

cmake --build build/p2-asan --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:strict_string_checks=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/p2-asan --output-on-failure
```

## 73. TSan

```bash
cmake -S . -B build/p2-tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=OFF \
  -DILIC_ENABLE_THREAD_SANITIZER=ON

cmake --build build/p2-tsan --parallel

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p2-tsan \
  --output-on-failure \
  -L 'architecture|concurrency|lifetime'

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p2-tsan --output-on-failure
```

## 74. Stress

Beispiel:

```bash
ctest --test-dir build/p2-debug \
  --output-on-failure \
  -R 'ilic_(compiler|capi|output)_concurrency' \
  --repeat until-fail:50
```

Der interne Test hat zusätzlich eigene Wiederholungen. Ein einzelner erfolgreicher Lauf reicht nicht.

## 75. Externe Conformance-Suite und WASM

### 75.1 Reale Suitevariablen ermitteln

```bash
suite_default=../interlis-compiler-testsuite
workflow=.github/workflows/ci.yml

test -f "$workflow"
test -f "$suite_default/InterlisConformance.java"
test -f "$suite_default/suite/test-cases.json"

git -C "$suite_default" status --short
git -C "$suite_default" rev-parse HEAD
```

Der Agent MUSS Suite-Pin und Baselinepfad aus dem Workflow dokumentieren. Er soll
einen kleinen robusten Parser oder eine gezielte manuelle Auswertung verwenden, nicht
einen fragilen `grep`, der bei YAML-Zeilenumbrüchen still den falschen Wert liefert.

### 75.2 Release-Conformance

```bash
cmake -S . -B build/p2-conformance-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF \
  -DILIC_STATIC_DISTRIBUTION=OFF \
  -DBUILD_TESTING=OFF
cmake --build build/p2-conformance-release --target ilic --parallel

java ../interlis-compiler-testsuite/InterlisConformance.java run \
  --suite ../interlis-compiler-testsuite/suite/test-cases.json \
  --compiler ilic=build/p2-conformance-release/ilic \
  --baseline ilic=<BASELINE_AUS_CI_YML> \
  --out build/p2-conformance/release
```

### 75.3 Debug-Conformance

```bash
cmake -S . -B build/p2-conformance-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF \
  -DILIC_STATIC_DISTRIBUTION=OFF \
  -DBUILD_TESTING=OFF
cmake --build build/p2-conformance-debug --target ilic --parallel

java ../interlis-compiler-testsuite/InterlisConformance.java run \
  --suite ../interlis-compiler-testsuite/suite/test-cases.json \
  --compiler ilic=build/p2-conformance-debug/ilic \
  --baseline ilic=<BASELINE_AUS_CI_YML> \
  --out build/p2-conformance/debug
```

### 75.4 Sanitizer-Conformance

Der Agent MUSS den vollständigen Lauf aus Abschnitt 44.10 gegen den
ASan-/UBSan-/LSan-Build ausführen. Der Lauf ist zusätzlich zur internen CTest-Suite
notwendig, weil er viele weitere Modelle und Fehlerpfade erreicht.

### 75.5 TSan-Conformance

Der Agent MUSS den vollständigen Lauf aus Abschnitt 44.10 gegen den TSan-Build
ausführen. Dieser Lauf ersetzt nicht die In-Process-Concurrency-Tests, erweitert aber
die erreichten Parser-, Builder-, Resolver- und Checkerpfade erheblich.

### 75.6 Baseline-Unverändertheit

Vor und nach allen Läufen:

```bash
sha256sum <BASELINE_AUS_CI_YML>
git diff -- <BASELINE_AUS_CI_YML>
```

Ohne bewusst behobenen Altfehler MUSS der Diff leer sein.

### 75.7 WASM- und Pakettests

Verwende die realen bestehenden Befehle, mindestens:

```bash
./scripts/build-wasm.sh
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
node --test test/npm/PrepareNpmSnapshotTest.mjs
node scripts/prepare-npm-snapshot.mjs --timestamp 20260101000000
node scripts/test-npm-packages.mjs
```

Die externe Suite wird nur dann zusätzlich gegen WASM ausgeführt, wenn der aktuelle
Branch dafür bereits einen offiziellen Runner besitzt. P2 soll keine neue
WASM-Conformance-Schicht als Nebenziel entwickeln.


## 76. Plattformmatrix

Mindestens einmal:

- Linux;
- macOS ARM64;
- Windows x64.

Windows muss insbesondere beweisen, dass Writer/Files nach Destruktion geschlossen sind.

## 77. Abschlusskontrollen

```bash
python3 scripts/check-core-global-state.py
git diff --check
git status --short
```

Ausserdem:

- generierte Dateien aktuell;
- keine untracked Buildartefakte im Sourcebaum;
- keine zufälligen Fixtureänderungen;
- keine Lizenzheader entfernt;
- keine unbeabsichtigte Änderung am Suite-Pin, Suite-Befehl oder Baseline-Pfad;

---

# Teil N – Definition of Done

## 78. P2 ist nur abgeschlossen, wenn

- [ ] der reale `ci.yml`-Conformance-Befehl wurde dokumentiert und unverändert ausgeführt.
- [ ] der lokale Suite-Checkout `../interlis-compiler-testsuite` wurde nicht verändert.
- [ ] der CI-gepinnte Suite-Commit wurde lokal ausgeführt oder die fehlende lokale Verfügbarkeit ehrlich dokumentiert.
- [ ] Suite-Manifest, Baselinepfad und Baseline-SHA wurden vor und nach P2 festgehalten.
- [ ] die vollständige externe Suite lief gegen Release, Debug, ASan/UBSan/LSan und TSan.
- [ ] kein neuer Baselineeintrag wurde für P2 hinzugefügt.
- [ ] jeder entfernte Baselineeintrag entspricht einem nachweislich behobenen Altfehler.
- [ ] die vollständigen Suite-Reports wurden aufbewahrt und ausgewertet.
- [ ] P0/CTest und die vollständige externe `interlis-compiler-testsuite` wurden vor und nach P2 ausgeführt.
- [ ] `CompilerContext` ist implementiert und pro Lauf.
- [ ] `CompilerSession` besitzt einen per-Session-Mutex.
- [ ] der prozessweite Compiler-Mutex ist entfernt.
- [ ] Logger ist instanzlokal.
- [ ] Core ruft nicht `exit()` auf.
- [ ] SourceManager wird nicht über thread-local „aktiv“ gemacht.
- [ ] `IliFileCatalog` besitzt alle runlokalen Fileobjekte.
- [ ] `MetaModelStore` besitzt alle dynamischen MMObjects.
- [ ] `MMObject` ist polymorph sicher zerstörbar.
- [ ] Metamodellregistries sind instanzlokal und nicht dupliziert.
- [ ] Builder-Kontextstack ist instanzlokal und RAII-basiert.
- [ ] Sprachflags sind instanzlokal.
- [ ] Metaattribute/Dokumentationspuffer sind sourcelokal.
- [ ] Ili1- und Ili2-Visitor erhalten Abhängigkeiten explizit.
- [ ] ErrorListener erhält Logger/Source explizit.
- [ ] SemanticChecker ist instanzlokal.
- [ ] TranslationChecker ist instanzlokal.
- [ ] SnapshotBuilder erhält Store explizit.
- [ ] MetaModelTreeVisitor erhält Store/Logger explizit.
- [ ] Outputgeneratoren besitzen keinen mutable File-Scope-State.
- [ ] CLI verwendet keine globale Compilerpipeline.
- [ ] C-ABI-Sessions laufen parallel.
- [ ] bestehende WASM- und JavaScript-Pakettests bleiben grün.
- [ ] Global-State-Guard ist grün.
- [ ] Release ist grün.
- [ ] Debug ist grün.
- [ ] ASan/UBSan/LSan sind grün.
- [ ] TSan ist grün.
- [ ] macOS ist grün.
- [ ] Windows ist grün.
- [ ] keine Refactor-XFAILs wurden eingeführt.
- [ ] Dokumentation ist vollständig.
- [ ] Abschlussbericht nennt echte Befehle/Ergebnisse.

---

# Teil O – Erwarteter Abschlussbericht des Coding-Agenten

## 79. Pflichtstruktur

Der Agent MUSS am Ende exakt diese Abschnitte liefern:

1. **Ausgangsstand und Baseline**
2. **Global-State-Inventar**
3. **Umgesetzte Zielarchitektur**
4. **CompilerContext**
5. **CompilerSession und Synchronisation**
6. **Source- und IliFile-Ownership**
7. **Metamodell-Ownership**
8. **Parser- und Buildermigration**
9. **Semantic-/Translation-Checker**
10. **Visitor und Outputgeneratoren**
11. **CLI, C-ABI und WASM**
12. **Neue und geänderte Dateien**
13. **Entfernte Globals und alte APIs**
14. **Neue Tests**
15. **Concurrency- und Isolationsergebnisse**
16. **Release-/Debug-Ergebnisse**
17. **ASan-/UBSan-/LSan-Ergebnisse**
18. **TSan-Ergebnisse**
19. **Externe INTERLIS-Conformance-Suite und WASM-Pakettests**
    - lokaler Suite-HEAD und CI-Pin;
    - exakter Runnerbefehl;
    - Manifest und Baseline;
    - Baseline-SHA und Eintragszahl vor/nach P2;
    - Resultate für Release, Debug, Sanitizer und TSan;
    - neue, behobene und unveränderte Abweichungen;
    - Pfade zu den erzeugten Reports;
20. **Plattformmatrix**
21. **Abweichungen von dieser Spezifikation**
22. **Externe Blocker**
23. **Verbleibende Risiken**
24. **Ausgeführte Befehle**
25. **Abschliessender Git-Status**

Jeder behauptete Testlauf benötigt:

- exakten Befehl;
- Exitcode;
- Testanzahl;
- Pass/Fail/Skip;
- relevante Umgebungsvariablen.

Nicht ausgeführte Prüfungen müssen klar als nicht ausgeführt bezeichnet werden.

---

# Anhang A – Verbindliche Ziel-Dateistruktur

```text
include/ilic/
  Compiler.h
  SourceManager.h

source/core/
  CompilationSourceStore.h
  CompilationSourceStore.cpp
  CompilerContext.h
  CompilerContext.cpp
  CompilerPipeline.h
  CompilerPipeline.cpp
  Compiler.cpp
  Semantic.cpp
  SourceManager.cpp

source/metamodel/
  MetaModel.h
  MetaModel.cpp
  MetaModelStore.h
  MetaModelStore.cpp
  MetaModelBuilder.h
  MetaModelBuilder.cpp
  MetaModelInput.h/.cpp          # löschen oder rein stateless
  SemanticChecker.h/.cpp
  TranslationChecker.h/.cpp
  MetaModelTreeVisitor.h/.cpp
  MetaModelOutput.h/.cpp

source/util/
  Logger.h/.cpp
  IliFile.h/.cpp
  IliFileCatalog.h/.cpp

source/input/parser/
  IliParserErrorListener.h/.cpp

test/
  core/CompilerConcurrencyTest.cpp
  core/CompilerIsolationTest.cpp
  core/CompilerLifetimeTest.cpp
  metamodel/MetaModelStoreTest.cpp
  metamodel/MetaModelBuilderTest.cpp
  util/IliFileCatalogTest.cpp
  output/OutputConcurrencyTest.cpp
  abi/CapiConcurrencyTest.cpp

scripts/
  check-core-global-state.py
  core-global-state-allowlist.json

docs/architecture/
  compiler-context.md
  metamodel-ownership.md
  thread-safety.md
  p2-global-state-inventory.md
```

Der Agent darf bestehende Namenskonventionen berücksichtigen. Verantwortlichkeiten und Tests sind verbindlich.

---

# Anhang B – Review-Checkliste auf Klassenebene

## `CompilerSession`

- [ ] noncopyable/nonmovable;
- [ ] PImpl;
- [ ] per-Session-Mutex;
- [ ] keine runlokalen Modelpointer;
- [ ] Context pro Compile;
- [ ] parse ohne globalen Lock;
- [ ] `sources()`-Vertrag dokumentiert.

## `CompilerContext`

- [ ] intern;
- [ ] noncopyable/nonmovable;
- [ ] eindeutige Memberreihenfolge;
- [ ] Logger;
- [ ] SourceStore;
- [ ] IliFileCatalog;
- [ ] MetaModelStore;
- [ ] MetaModelBuilder;
- [ ] kein globaler Zugriff;
- [ ] Destruktor cleanup.

## `MetaModelStore`

- [ ] alleiniger Heapowner;
- [ ] `unique_ptr`;
- [ ] virtueller Base-Destruktor;
- [ ] eine Importregistry;
- [ ] Universalobjekte pro Store;
- [ ] Clone explizit;
- [ ] keine Containerkopien;
- [ ] Tests für Destruktion.

## `MetaModelBuilder`

- [ ] Store/Logger per Referenz;
- [ ] Sprachversion instanzlokal;
- [ ] SourceScope;
- [ ] ContextScope;
- [ ] Pending Metadata instanzlokal;
- [ ] alle find/add/init migriert;
- [ ] keine manuellen Globals.

## `IliFileCatalog`

- [ ] `unique_ptr<IliFile>`;
- [ ] SourceStore;
- [ ] Optionen const;
- [ ] Session vor Filesystem;
- [ ] Built-in pro Context;
- [ ] keine Deletes/Globals/exit.

## `Logger`

- [ ] kein globales Objekt;
- [ ] kein Core-exit;
- [ ] RAII-Scopes;
- [ ] Diagnostics/Events lokal;
- [ ] Counts lokal;
- [ ] Sinks lokal.

## Parser

- [ ] SourceBuffer statt Pfadreload;
- [ ] Builder/Logger Konstruktor;
- [ ] ErrorListener explizit;
- [ ] kein `input_file`;
- [ ] keine globalen Versionflags.

## Checker

- [ ] Store/Logger explizit;
- [ ] runlokal;
- [ ] keine globalen Getter;
- [ ] Diagnosen identisch.

## Outputs

- [ ] Store/Logger explizit;
- [ ] alle mutable statics entfernt;
- [ ] Writer Member;
- [ ] Paralleltest;
- [ ] bytegleiche Golden Outputs.

---

# Anhang C – Empfohlene Commitfolge

1. `test: capture P2 global-state baseline`
2. `refactor: make logger instance-owned and non-exiting`
3. `refactor: replace active source manager with explicit source store`
4. `refactor: introduce owned ili file catalog`
5. `refactor: introduce metamodel store and object arena`
6. `refactor: introduce metamodel builder scopes`
7. `refactor: inject builder into INTERLIS input visitors`
8. `refactor: make semantic and translation checks context-owned`
9. `refactor: make metamodel visitors and outputs instance-owned`
10. `refactor: introduce compiler context and pipeline`
11. `refactor: remove process-wide compiler serialization`
12. `refactor: migrate CLI and C ABI concurrency`
13. `test: add compiler concurrency and lifetime coverage`
14. `ci: add global-state guard and thread sanitizer`
15. `docs: document compiler ownership and thread safety`

Ein Commit darf mehrere eng gekoppelte Schritte enthalten, aber kein einzelner Commit soll die gesamte Migration unreviewbar vermischen.

---

# Anhang D – Kompakter Startprompt für einen Coding-Agenten

Lege diese Datei als `ilic-p2-compiler-context-spec.md` ins Repository-Root und verwende:

```text
Implementiere P2 im aktuell geöffneten Repository vollständig und selbständig.

Lies zuerst die gesamte Datei `ilic-p2-compiler-context-spec.md`. Sie ist normativ.
Prüfe `git status --short` und bewahre fremde Änderungen.

Die reale P1-Regression liegt nicht als Korpus in diesem Repository. Verwende die
externe Suite `https://codeberg.org/edigonzales/interlis-compiler-testsuite`, lokal
unter `../interlis-compiler-testsuite`. Lies `.github/workflows/ci.yml`, ermittle den
dort gepinnten Suite-Commit, den exakten `InterlisConformance.java run`-Befehl und
den tatsächlich verwendeten Baselinepfad. Im Referenzstand ist dies
`conformance/known-failures.json`; falls der aktuelle Branch stattdessen
`conformance/known-issues.json` verwendet, gilt der reale Workflow. Lege keine zweite
Baseline an.

Verändere das Nachbarrepository nicht. Führe vor jeder P2-Änderung die vollständige
interne CTest-Suite, die vollständige externe Conformance-Suite und die bestehenden
WASM-/Pakettests aus. Sichere Suite-Commit, Manifest, Baseline-SHA, Baseline-Test-IDs,
Compiler-SHA, Exitcode und Reports.

Beseitige danach den veränderlichen Prozesszustand des Compilercores. Führe einen pro
Lauf erzeugten internen `CompilerContext`, einen eindeutig besitzenden
`MetaModelStore`, einen instanzlokalen `MetaModelBuilder`, einen besitzenden
`IliFileCatalog`, explizite Source-Abhängigkeiten und instanzlokales Logging ein.
Entferne den globalen Compiler-Mutex, globale Logger-/Metamodell-/IliFile-/Context-/
Versionzustände, thread-local Active-Source-Zugriff, globale Resets und mutable
Outputstatics.

Mehrere verschiedene `CompilerSession`s müssen echt parallel und datenrassenfrei
kompilieren; dieselbe Session wird per Sessionmutex serialisiert. C-ABI,
CLI-Ergebnisse und bestehendes WASM-Verhalten bleiben kompatibel.

Implementiere alle verlangten Architektur-, Ownership-, Lifetime-, Isolation-,
Output- und C-ABI-Concurrency-Tests, den Global-State-Guard und einen Linux-
ThreadSanitizer-Job. Verwende C++17 und das P0-Test-Harness.

Nach jeder abgeschlossenen Migrationsphase muss die vollständige externe Suite laufen.
Führe sie am Ende gegen Release, Debug, ASan/UBSan/LSan und TSan aus, jeweils mit dem
realen Manifest und derselben Baseline. Die Suite darf nicht gefiltert oder durch
Smoke-Tests ersetzt werden. Füge keinen Baselineeintrag hinzu, ändere keinen
`actualOutcome` und lockere keine Erwartung, um eine Refactoringregression zu
kaschieren. Ein bestehender Baselineeintrag darf nur entfernt werden, wenn der
Altfehler wirklich behoben und durch einen fokussierten Test abgesichert ist.

Erhalte den bestehenden GitHub-Actions-Conformance-Job mit gepinntem Suite-Commit,
vollständigem Lauf, Reportupload und abschliessender Erzwingung des Runner-Exitcodes.
Aktualisiere den Suite-Pin nicht nebenbei.

Arbeite nicht nur einen Plan aus. Implementiere, baue, teste, korrigiere und
wiederhole, bis Release, Debug, ASan/UBSan/LSan, TSan, die vollständige externe
Conformance-Suite, die bestehenden WASM-/Pakettests und die Plattformmatrix soweit in
der Umgebung ausführbar erfolgreich sind.

Beende mit dem in der Spezifikation verlangten Abschlussbericht. Nenne jeden exakten
Befehl, Exitcode, Testanzahl, Suite-Pin, Baseline-Diff und Reportpfad. Behaupte keinen
Lauf, den du nicht tatsächlich ausgeführt hast.
```

---

# Anhang E – Entscheidungssätze für schwierige Stellen

Wenn während der Implementierung Unsicherheit entsteht, gelten diese Regeln:

1. **Ownership vor Bequemlichkeit:** Das Objekt gehört einem klaren RAII-Owner.
2. **Explizite Abhängigkeit vor globalem Zugriff:** Parameter/Member statt Current-Singleton.
3. **Per-Session-Lock vor Prozesslock:** unabhängige Sessions dürfen parallel sein.
4. **Value-Snapshot vor Pointerexport:** öffentliche Resultate überleben den Context.
5. **Eine Registry vor doppelter Wahrheit:** insbesondere Imports.
6. **Scope vor manuellem Push/Pop:** Source, Kategorie, Indent und Metamodellkontext.
7. **Throw vor Exit im Core:** Prozessentscheidung nur am CLI-Rand.
8. **Konkrete Dependency vor Context-Service-Locator:** Context nur für Orchestrierung.
9. **Conformance-Gleichheit vor Refactorästhetik:** keine neue Abweichung gegenüber der externen Suite und keine Baseline-Erweiterung.
10. **Beleg vor Ausnahme:** keine Allowlist/Suppression ohne exakten Grund.


---

# Anhang F – Referenzworkflow der externen Suite

Dieser Anhang dokumentiert den bei Erstellung der Revision 2 beobachteten
GitHub-Actions-Vertrag. Der aktuelle Branch bleibt die Quelle der Wahrheit.

## F.1 Download des gepinnten Korpus

```bash
set -euo pipefail
suite_commit=88576ffc4cd491dd72d2375eb897ea9f3f55d2c3
archive="$RUNNER_TEMP/interlis-conformance-suite.tar.gz"
curl --fail --location --retry 3 \
  "https://codeberg.org/edigonzales/interlis-compiler-testsuite/archive/${suite_commit}.tar.gz" \
  --output "$archive"
mkdir -p conformance-suite
tar --extract --gzip --strip-components=1 \
  --file="$archive" --directory=conformance-suite
```

P2 soll diesen Pin nicht aktualisieren. Ein Update der Testsuite ist ein eigener,
fachlich zu adjudizierender Vorgang.

## F.2 Referenzbuild

```bash
cmake -S . -B build/conformance-ilic -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF \
  -DILIC_STATIC_DISTRIBUTION=OFF \
  -DBUILD_TESTING=OFF
cmake --build build/conformance-ilic --target ilic --parallel
```

## F.3 Referenzlauf

```bash
java conformance-suite/InterlisConformance.java run \
  --suite conformance-suite/suite/test-cases.json \
  --compiler ilic=build/conformance-ilic/ilic \
  --baseline ilic=conformance/known-failures.json \
  --out build/conformance
```

## F.4 Reportupload und Exitcode

Der Workflow darf den Runner zunächst mit `continue-on-error: true` starten, damit
Reports auch bei einem Fehlschlag hochgeladen werden. Danach MUSS ein eigener Schritt
den gespeicherten Exitcode erzwingen. Das Endresultat des Jobs darf nicht grün sein,
wenn der Runner einen Fehler meldet.

## F.5 Baseline-Invariante für P2

Bei einem reinen Zustands-/Ownership-Refactoring ist der erwartete Baseline-Diff:

```text
kein Diff
```

Ein wachsender Baselinebestand bedeutet eine P2-Regression und blockiert die Abnahme.
Ein schrumpfender Baselinebestand ist nur zulässig, wenn jeder entfernte Testfall als
echte Fehlerbehebung einzeln belegt wird.

---

_Ende der normativen P2-Spezifikation._
