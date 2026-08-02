# P6 – Diagnostic Quality

## Verbindliche Spezifikation für einen autonomen LLM-Coding-Agenten

**Projektverbund**

```text
../ilic-fork
../interlis-language-tools
../interlis-web-ide
```

**Status:** normative Implementierungsspezifikation  
**Schwerpunkt:** Qualität, Stabilität, Nachvollziehbarkeit und Darstellung von Diagnosen  
**Explizit nicht Bestandteil:** Änderungen der fachlichen Accept-/Reject-Entscheidungen

---

## 1. Zweck dieser Phase

P6 verbessert ausschliesslich die Diagnosequalität des bestehenden INTERLIS-Compilers und seiner Folgeprojekte.

Die Phase muss dafür sorgen, dass Benutzerinnen und Benutzer bei einem ungültigen, unvollständigen oder problematischen Modell möglichst genau verstehen:

- was falsch ist;
- wo der Fehler liegt;
- welche Deklarationen beteiligt sind;
- welche Meldung die eigentliche Ursache beschreibt;
- welche Meldungen nur Folgeprobleme wären;
- ob es sich um Syntax, Auflösung, Semantik, Übersetzung, Repository oder Infrastruktur handelt;
- wie dieselbe Diagnose in CLI, C++, C-ABI, WASM, Node-LSP, Monaco und Web-IDE dargestellt wird.

Die Phase darf nicht als versteckte Conformance-Korrektur verwendet werden.

Insbesondere bleiben bewusst unangetastet:

- die beiden aktuell bekannten Conformance-Fälle mit `internal-error`;
- alle aktuell bekannten False Accepts;
- alle fachlich abstimmungsbedürftigen Accept-/Reject-Entscheidungen;
- die bestehende Conformance-Baseline, soweit keine rein technische Metadatenpflege notwendig ist.

P6 darf die Diagnose eines bereits kontrolliert abgelehnten Modells verbessern. P6 darf aber nicht aus einem bisher akzeptierten Modell ein abgelehntes Modell machen oder umgekehrt.

---

## 2. Normative Schlüsselwörter

Die Wörter **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT** und **DARF** sind normativ zu verstehen.

- **MUSS** bezeichnet eine zwingende Abnahmebedingung.
- **DARF NICHT** bezeichnet ein zwingendes Verbot.
- **SOLL** darf nur mit dokumentierter technischer Begründung abweichend umgesetzt werden.
- **DARF** bezeichnet eine ausdrücklich erlaubte Variante.
- Eine Abweichung von einem MUSS gilt als offene Restarbeit und verhindert die Bezeichnung der Phase als abgeschlossen.

---

## 3. Informative Ausgangsreferenz

Zum Zeitpunkt der Erstellung dieser Spezifikation waren folgende öffentliche Stände sichtbar:

```text
ilic-fork:
e3d7f666c3340527b6b860a20349bde96e342cc6

interlis-language-tools:
e0d2094d21fd5c3d6360aac0a0a382e0ce135b92

interlis-web-ide:
ad92df63089f7df9b65519fb34a5b3f891c0cffe
```

Diese Werte sind nur eine informative Referenz.

Der Coding-Agent MUSS zu Beginn die tatsächlichen lokalen Commits, Branches und Änderungen ermitteln. Der lokale Arbeitsstand ist die Quelle der Wahrheit.

---

## 4. Unverhandelbare fachliche Grenze: Decision Lock

### 4.1 Grundsatz

P6 ist eine Diagnosephase, keine Sprachsemantikphase.

Für jeden unveränderten Testfall MUSS nach P6 gelten:

```text
Accept bleibt Accept.
Reject bleibt Reject.
Internal Error bleibt ausserhalb des P6-Änderungsumfangs.
Cancellation bleibt Cancellation.
Missing-Model-Verhalten bleibt fachlich unverändert.
Erzeugte beziehungsweise erkannte Modelle bleiben unverändert.
```

### 4.2 Decision-Lock-Daten

Für jeden relevanten Testfall ist mindestens folgender fachlicher Ausgang zu vergleichen:

```cpp
struct DiagnosticDecisionFingerprint {
    bool success;
    bool cancelled;
    int acceptedOrRejected;
    std::vector<std::string> missingModels;
    std::vector<std::string> compiledModelNames;
    std::vector<std::string> compiledModelUris;
    std::string outcomeClass;
};
```

Die tatsächliche Struktur darf dem Projektstil angepasst werden.

Der Fingerprint DARF bewusst nicht enthalten:

- Diagnosemeldung;
- Diagnosecode, sofern ein bisher generischer Code präzisiert wird;
- Diagnose-Reihenfolge;
- Zahl unterdrückter Folgediagnosen;
- Related Information;
- Notes;
- verbesserte Source-Ranges.

### 4.3 Erlaubte Änderungen

P6 DARF verändern:

- generische Diagnosecodes in präzisere stabile Codes;
- Meldungstexte;
- Gross-/Kleinschreibung und Terminologie;
- primäre Source-Range;
- Related Information;
- Notes;
- `source`;
- `treatedAsError`, aber nur wenn das bestehende `warningsAsErrors`-Verhalten exakt erhalten bleibt;
- deterministische Reihenfolge;
- exakte Duplikate;
- klar nachgewiesene Kaskadenmeldungen;
- Darstellung in CLI, LSP und Web-IDE;
- optionale additive JSON-Felder;
- interne Diagnosearchitektur.

### 4.4 Verbotene Änderungen

P6 DARF NICHT:

- Grammatikregeln ändern;
- Parserakzeptanz ändern;
- semantische Prüfregeln hinzufügen oder entfernen;
- einen bekannten False Accept korrigieren;
- einen bekannten Internal Error fachlich korrigieren;
- einen bekannten False Reject ohne separaten Auftrag korrigieren;
- die Conformance-Baseline verkleinern oder erweitern;
- `warningsAsErrors` semantisch ändern;
- aus einer Warnung einen Fehler machen;
- aus einem Fehler eine Warnung machen;
- Repositoryauflösung fachlich ändern;
- Modellwahl, Versionswahl oder Dependency-Reihenfolge ändern;
- Fehler durch `try/catch` verschlucken, nur um eine schönere Meldung zu erzeugen;
- einen Compilerfehler als normale Benutzermeldung tarnen;
- Tests löschen oder fachliche Erwartungen lockern.

### 4.5 Sonderfall Kaskadenunterdrückung

Kaskadenunterdrückung darf die Anzahl veröffentlichter Diagnosen reduzieren, wenn alle folgenden Bedingungen erfüllt sind:

1. Die primäre Ursache bleibt sichtbar.
2. Das Compile-Ergebnis bleibt unverändert.
3. Der unterdrückte Fehler ist nachweislich nur eine Folge der primären Ursache.
4. Die Beziehung wird strukturell erzeugt, nicht durch Vergleich freier Meldungstexte.
5. Ein fokussierter Test beweist die Unterdrückung.
6. Ein Cold-/WASM-/Consumer-Vergleich zeigt dieselbe veröffentlichte Diagnosemenge.
7. Debug- oder Trace-Instrumentierung kann die Unterdrückungsentscheidung nachvollziehbar machen.

---

## 5. Qualitätsziele

### 5.1 Stabilität

- Jeder öffentlich sichtbare Diagnosefall besitzt einen stabilen Code.
- Ein Code ändert seine fachliche Bedeutung nicht.
- Meldungstexte dürfen verbessert werden, ohne dass Consumer darauf parsen müssen.
- Quick Fixes und UI-Logik verwenden Codes und strukturierte Daten, nicht Meldungstexte.

### 5.2 Lokalität

- Die primäre Range zeigt auf das Token oder die Deklaration, die der Benutzer ändern muss.
- Ein Fehler in einer importierten oder referenzierten Deklaration enthält Related Information.
- Ranges verweisen auf die autoritative Source-URI.
- Repositoryquellen behalten ihre Repository-URI.
- UTF-8-Byteoffsets und UTF-16-Editorpositionen bleiben konsistent.

### 5.3 Verständlichkeit

- Meldungen beschreiben Problem, Kontext und beteiligte Namen.
- Meldungen vermeiden interne C++-, ANTLR- oder Metamodell-Bezeichner.
- Meldungen enthalten keine Pointer, Typnamen von `typeid`, Stacktraces oder rohe Exceptiontexte.
- Eine Meldung enthält keine unnötigen Pfadwiederholungen, wenn die Range bereits die URI trägt.
- Meldungen verwenden konsistente INTERLIS-Terminologie.

### 5.4 Handlungsorientierung

- Die Diagnose benennt nach Möglichkeit das erwartete Konstrukt.
- Notes dürfen eine kurze Erläuterung oder zulässige Alternativen enthalten.
- Related Information zeigt Konfliktpartner oder Ursprungsdeklarationen.
- Eine Diagnose darf keine spekulative Reparatur vorschlagen.

### 5.5 Signal-Rausch-Verhältnis

- Exakte Duplikate werden entfernt.
- Folgediagnosen werden nur unterdrückt, wenn ihre Abhängigkeit strukturell bekannt ist.
- Unabhängige Fehler bleiben sichtbar.
- Mehrere Fehler in derselben Datei werden deterministisch angeordnet.
- Live-, Saved- und Semantic-Diagnosen werden nicht doppelt veröffentlicht.

### 5.6 Plattformparität

Dieselbe native Diagnose muss in folgenden Oberflächen semantisch identisch erscheinen:

```text
C++ API
C-ABI JSON
WASM TypeScript API
Language Service
Node-LSP
Monaco Marker
Web-IDE Problems
Web-IDE Navigation
```

---

## 6. Zu untersuchende bestehende Architektur

Der Agent MUSS vor Änderungen die realen Diagnosepfade vollständig inventarisieren.

### `ilic-fork`

```text
include/ilic/Diagnostic.h
include/ilic/Compiler.h
include/ilic/Syntax.h
include/ilic/Editor.h
include/ilic/Semantic.h
include/ilic/Repository.h
include/ilic/capi.h

source/util/DiagnosticCode.*
source/util/Logger.*
source/input/parser/IliParserErrorListener.*
source/input/ili1/
source/input/ili2/
source/metamodel/MetaModelBuilder.*
source/metamodel/SemanticChecker.*
source/metamodel/TranslationChecker.*
source/core/Compiler.*
source/core/SnapshotPipeline.*
source/core/Semantic.*
source/repository/
source/abi/Capi.cpp
source/abi/Json.*

test/core/DiagnosticCodeTest.cpp
test/core/DiagnosticQualityTest.cpp
test/core/SyntaxSnapshotTest.cpp
test/core/EditorSnapshotTest.cpp
test/core/SemanticSnapshotTest.cpp
test/abi/
test/repository/
conformance/known-failures.json
docs/diagnostik-und-logging.md
```

### `interlis-language-tools`

```text
packages/language-service/src/service.ts
packages/language-service/src/types.ts
packages/language-service/src/live-analysis.ts
packages/language-service/src/features.ts
packages/language-service/src/compiler.ts
packages/language-service/src/compiler-worker.ts
packages/language-service/src/repository.ts
packages/language-server/src/
packages/monaco-adapter/src/
packages/vscode-extension/src/
tests and fixtures for diagnostics, quick fixes and problems
```

### `interlis-web-ide`

```text
src/workbench/workbench.ts
src/language-worker.ts
src/interlis-compiler.worker.ts
src/language-repository.ts
src/styles/
src/problems or diagnostics-related modules
e2e/language.spec.ts
all diagnostic-navigation and Problems tests
```

---

## 7. Verbindliches Ausgangsinventar

Vor der ersten produktiven Änderung MUSS ein Inventar erstellt werden.

### 7.1 Diagnoseproduzenten

Für jeden Produzenten erfassen:

- Datei;
- Klasse oder Funktion;
- Phase;
- aktueller Code;
- aktuelle Severity;
- aktueller Meldungstext;
- Rangequelle;
- Related Information;
- Notes;
- mögliche Duplikate;
- mögliche Kaskaden;
- Verhalten unter `warningsAsErrors`;
- Native-/WASM-Ausgabe;
- Consumer-Verwendung.

### 7.2 Phasen

Mindestens klassifizieren:

```text
lexical
syntax
editor-recovery
model-discovery
model-resolution
name-resolution
type-resolution
semantic
translation
repository
formatting
ABI/request
internal
infrastructure
```

### 7.3 Codeinventar

Der Agent MUSS alle im Produktionscode vorkommenden Diagnosecodes erfassen.

Zu prüfen:

- doppelt verwendete Codes mit unterschiedlicher Bedeutung;
- gleiche Bedeutung mit mehreren Codes;
- leere Codes;
- generische Fallbackcodes;
- dynamisch zusammengesetzte Codes;
- Codes, die nur in Tests existieren;
- Codes ohne Test;
- Codes ohne Dokumentation;
- Quick Fixes, die Meldungstexte statt Codes verwenden.

### 7.4 Rangeinventar

Zu erfassen:

- Fehler ohne Range;
- Fehler mit ungültiger Range;
- Fehler mit Range auf das ganze Dokument;
- Fehler mit Range auf falsche URI;
- Konfliktfehler ohne Related Information;
- parserseitige Off-by-one-Fehler;
- EOF-Ranges;
- Zero-width-Ranges;
- Unicode- und CRLF-Fälle.

### 7.5 Kaskadeninventar

Für repräsentative ungültige Modelle dokumentieren:

- erste Ursache;
- alle nachfolgenden Diagnosen;
- welche davon unabhängig sind;
- welche nur Folgefehler sind;
- welche Ursache-ID oder Abhängigkeit heute verfügbar ist;
- wo zusätzliche strukturierte Ursacheinformationen erzeugt werden müssen.

### 7.6 Consumerinventar

Erfassen:

- LSP-Severity-Mapping;
- LSP-Code-Mapping;
- `source`;
- `relatedInformation`;
- `tags`;
- `codeDescription`;
- Monaco Marker;
- Quick-Fix-Auswahl;
- Problems-Gruppierung;
- Problems-Sortierung;
- Navigation;
- stale/live/saved Priorität;
- Deduplizierung im Language Service;
- Verhalten bei Dokumentversionen.

---

## 8. Zielarchitektur

```text
Parser / Resolver / Semantic Rule / Repository
                 |
                 v
        DiagnosticCandidate
                 |
                 v
        DiagnosticBuilder
                 |
                 v
       DiagnosticPipeline
          |      |      |
          |      |      +--> deterministic ordering
          |      +---------> cascade suppression
          +----------------> validation and deduplication
                 |
                 v
          published Diagnostic
                 |
       +---------+---------+
       |                   |
       v                   v
C++ CompilationResult   C-ABI JSON
                            |
                            v
                         WASM API
                            |
                            v
                      Language Service
                       |            |
                       v            v
                    Node-LSP      Monaco
                                      |
                                      v
                                Web-IDE Problems
```

Die Pipeline darf keine Accept-/Reject-Entscheidung treffen.

---

## 9. Öffentlicher Diagnosevertrag

Der bestehende Vertrag ist soweit möglich zu erhalten.

Sinngemäss:

```cpp
enum class DiagnosticSeverity {
    Error,
    Warning,
    Information,
    Hint
};

struct RelatedInformation {
    SourceRange range;
    std::string message;
};

struct Diagnostic {
    DiagnosticSeverity severity;
    std::string code;
    std::string source;
    std::string message;
    SourceRange range;
    std::vector<RelatedInformation> relatedInformation;
    std::vector<std::string> notes;
    bool treatedAsError;
};
```

Die realen Namen und Felder des Projekts sind massgeblich.

### 9.1 Additive Felder

Optionale additive Felder dürfen eingeführt werden, wenn sie klar benötigt werden:

```cpp
enum class DiagnosticPhase {
    Lexical,
    Syntax,
    Resolution,
    Semantic,
    Translation,
    Repository,
    Formatting,
    Request,
    Internal
};

enum class DiagnosticTag {
    Primary,
    Cascaded,
    Recovery,
    Deprecated,
    Unnecessary
};
```

Mögliche additive Felder:

```cpp
DiagnosticPhase phase;
std::vector<DiagnosticTag> tags;
std::string helpId;
std::string fingerprint;
```

Diese Felder dürfen bestehende Consumer nicht brechen.

### 9.2 Interne und öffentliche Typen trennen

```cpp
struct DiagnosticCandidate {
    Diagnostic diagnostic;
    DiagnosticCauseId cause;
    std::vector<DiagnosticCauseId> dependsOn;
    DiagnosticPublicationClass publicationClass;
    std::string producer;
};

struct PublishedDiagnostics {
    std::vector<Diagnostic> values;
    DiagnosticPipelineStats stats;
};
```

---
## 10. Diagnosecode-Katalog

### 10.1 Ziel

Alle öffentlichen Codes müssen zentral registriert, eindeutig und testbar sein.

Empfohlene Typen:

```cpp
struct DiagnosticDescriptor {
    std::string_view code;
    DiagnosticSeverity defaultSeverity;
    DiagnosticPhase phase;
    std::string_view title;
    std::string_view documentationId;
    bool mayBeTreatedAsError;
    bool requiresPrimaryRange;
};

class DiagnosticCatalog final {
public:
    static const DiagnosticDescriptor* find(std::string_view code) noexcept;
    static const DiagnosticDescriptor& require(std::string_view code);
    static std::vector<DiagnosticDescriptor> all();
    static bool isRegistered(std::string_view code) noexcept;
};
```

### 10.2 Codeformat

Codes sollen dem bestehenden Stil folgen.

Falls das bestehende Schema `ILIC-<AREA>-<NAME>` verwendet, ist es beizubehalten.

Empfohlene Bereiche:

```text
ILIC-LEX-...
ILIC-SYN-...
ILIC-RESOLVE-...
ILIC-SEM-...
ILIC-TRANS-...
ILIC-REPO-...
ILIC-FMT-...
ILIC-ABI-...
ILIC-INTERNAL-...
```

### 10.3 Regeln

- Ein Code bezeichnet genau eine fachliche Diagnoseart.
- Codes werden nicht lokal als freie Strings dupliziert.
- Codes dürfen als `constexpr std::string_view` oder typisierte IDs bereitgestellt werden.
- Ein Code darf nicht anhand einer Meldung generiert werden.
- Ein bestehender Code darf nicht still umgedeutet werden.
- Ein neuer Code benötigt Descriptor, Test und mindestens ein Fixture.
- Ein entfernter Code benötigt einen dokumentierten Migrationsgrund.
- Quick Fixes referenzieren Codes.
- UI-Filter referenzieren Codes oder Phasen.
- Tests vergleichen Codes, nicht vollständige englische Meldungen, ausser bei gezielten Message-Quality-Tests.

### 10.4 Guard

Ein Architecture Guard MUSS erkennen:

- nicht registrierte `ILIC-...`-Literale im Produktionscode;
- doppelte Katalogcodes;
- Katalogcodes ohne gültiges Format;
- Codes ohne Descriptor;
- leere Diagnosecodes;
- Quick-Fix-Logik anhand von `message.includes(...)`.

---

## 11. `DiagnosticBuilder`

Diagnoseerzeugung soll nicht aus wiederholten Aggregate-Initialisierungen bestehen.

Empfohlene API:

```cpp
class DiagnosticBuilder final {
public:
    explicit DiagnosticBuilder(DiagnosticCode code);

    DiagnosticBuilder& severity(DiagnosticSeverity value);
    DiagnosticBuilder& source(std::string value);
    DiagnosticBuilder& message(std::string value);
    DiagnosticBuilder& primaryRange(SourceRange value);
    DiagnosticBuilder& related(SourceRange range, std::string message);
    DiagnosticBuilder& note(std::string value);
    DiagnosticBuilder& treatedAsError(bool value);
    DiagnosticBuilder& phase(DiagnosticPhase value);
    DiagnosticBuilder& tag(DiagnosticTag value);
    DiagnosticBuilder& cause(DiagnosticCauseId value);
    DiagnosticBuilder& dependsOn(DiagnosticCauseId value);
    DiagnosticBuilder& publicationClass(DiagnosticPublicationClass value);
    DiagnosticBuilder& producer(std::string value);

    DiagnosticCandidate build() &&;
};
```

### 11.1 Invarianten

`build()` MUSS:

- Code im Katalog prüfen;
- Default-Severity übernehmen, wenn nicht explizit gesetzt;
- leere Meldungen ablehnen;
- Range normalisieren;
- `source` setzen;
- leere Notes entfernen;
- identische Related-Information-Einträge deduplizieren;
- keine Accept-/Reject-Entscheidung treffen.

### 11.2 Convenience-Factories

Für häufige Muster sind kleine typisierte Factories erlaubt:

```cpp
DiagnosticCandidate missingModelDiagnostic(
    std::string_view model,
    SourceRange importRange
);

DiagnosticCandidate duplicateDefinitionDiagnostic(
    std::string_view qualifiedName,
    SourceRange duplicate,
    SourceRange original
);
```

Die Factories dürfen keine fachliche Regel selbst ausführen. Sie formatieren nur eine bereits getroffene Diagnoseentscheidung.

### 11.3 Producer-Migration

Die Migration der Produzenten MUSS schrittweise erfolgen.

Nach jeder Gruppe ist auszuführen:

- fokussierte native Tests;
- Decision Lock;
- Native/WASM-Diagnosevergleich;
- relevante Consumer-Tests.

Ein Mischzustand ist vorübergehend erlaubt, wenn der Guard eine kleine explizite Allowlist besitzt. Die Allowlist MUSS am Ende leer oder begründet minimal sein.

---

## 12. `DiagnosticRangeResolver`

### 12.1 Zweck

Alle Phasen sollen Ranges über eine zentrale, klar getestete Abstraktion erzeugen.

Empfohlene API:

```cpp
class DiagnosticRangeResolver final {
public:
    explicit DiagnosticRangeResolver(const SourceManager& sources);

    SourceRange token(
        std::string_view uri,
        const antlr4::Token* token
    ) const;

    SourceRange context(
        std::string_view uri,
        const antlr4::ParserRuleContext* context
    ) const;

    SourceRange insertionPoint(
        std::string_view uri,
        std::size_t byteOffset
    ) const;

    SourceRange name(const MetaElement& element) const;
    SourceRange declaration(const MetaElement& element) const;
    SourceRange normalize(SourceRange range) const;
};
```

Die tatsächlichen Metamodelltypen sind an den Code anzupassen.

### 12.2 Range-Regeln

- `start <= end`.
- URI muss gesetzt sein, wenn die Source bekannt ist.
- Byteoffsets müssen innerhalb der Source liegen.
- EOF-Fehler erhalten eine gültige Zero-width-Range am Dokumentende.
- Mehrbyte-UTF-8 darf die UTF-16-Position nicht verschieben.
- CRLF und LF müssen dieselbe logische Position liefern.
- Eine unbekannte Range bleibt explizit ungültig und wird nicht als `(0,0)` vorgetäuscht.
- Primäre Range soll möglichst klein sein.
- Related Range soll auf die beteiligte Deklaration zeigen.
- Ein Konflikt darf nicht beide Ranges in den Meldungstext schreiben.

### 12.3 Tests

Mindestens:

- ASCII;
- Umlaute;
- Emoji vor dem Fehler;
- CRLF;
- leere Datei;
- EOF;
- fehlendes Semikolon;
- Zero-width-Insertion;
- Repository-URI;
- Datei-URI;
- Memory-URI;
- ungültiger Tokenpointer;
- synthetischer Editor-Token;
- Range nach Cache-Hit;
- Range nach Worker-Serialisierung.

---

## 13. Parser- und Lexerdiagnosen

### 13.1 Grundsatz

Rohe ANTLR-Meldungen dürfen nicht unverändert die primäre öffentliche Meldung bilden.

ANTLR darf technische Details liefern. Die öffentliche Diagnose muss auf stabile Codes und kontrollierte Formulierungen abgebildet werden.

### 13.2 Klassifikation

Mindestens zu unterscheiden:

```text
unrecognized character
unexpected token
extraneous token
missing token
mismatched input
no viable alternative
unexpected end of file
incomplete construct
invalid INTERLIS version syntax
lexer failure
```

### 13.3 Empfohlene Codes

Die endgültigen Codes müssen zum bestehenden Katalog passen.

Beispiel:

```text
ILIC-LEX-UNRECOGNIZED-CHARACTER
ILIC-SYN-UNEXPECTED-TOKEN
ILIC-SYN-EXTRANEOUS-TOKEN
ILIC-SYN-MISSING-TOKEN
ILIC-SYN-MISMATCHED-INPUT
ILIC-SYN-NO-VIABLE-ALTERNATIVE
ILIC-SYN-UNEXPECTED-EOF
ILIC-SYN-INCOMPLETE-CONSTRUCT
```

### 13.4 `ParserDiagnosticTranslator`

Empfohlene API:

```cpp
struct ParserDiagnosticInput {
    std::string uri;
    std::string recognizerKind;
    const antlr4::Token* offendingToken;
    std::string rawMessage;
    std::vector<std::string> expectedTokens;
    std::exception_ptr exception;
};

class ParserDiagnosticTranslator final {
public:
    DiagnosticCandidate translate(
        const ParserDiagnosticInput& input
    ) const;

private:
    DiagnosticCandidate unexpectedToken(...) const;
    DiagnosticCandidate missingToken(...) const;
    DiagnosticCandidate extraneousToken(...) const;
    DiagnosticCandidate mismatchedInput(...) const;
    DiagnosticCandidate noViableAlternative(...) const;
    DiagnosticCandidate unexpectedEof(...) const;
};
```

### 13.5 Meldungsregeln

Gute Form:

```text
Unexpected token 'END'; expected ';'.
```

Nicht erwünscht:

```text
mismatched input 'END' expecting T__42
```

Tokenanzeigen sollen:

- lesbare Symbolnamen verwenden;
- interne `T__42`-Bezeichnungen vermeiden;
- maximal eine sinnvolle Auswahl erwarteter Tokens zeigen;
- sehr grosse Expected-Sets zusammenfassen;
- Strings sicher quoten;
- keine unkontrollierten Sourcefragmente ausgeben.

### 13.6 Editor-Recovery

Tolerante Editor-Recovery und strikte Compilerdiagnostik müssen getrennt bleiben.

Ein synthetischer Recovery-Token darf:

- Editor-Snapshot-Recovery erklären;
- als interne Recovery-Information markiert sein;
- keine falsche Compilerdiagnose erzeugen;
- nicht als echte Source-Range erscheinen.

P6 darf die in P5.1 definierte Strict-/Tolerant-Trennung nicht aufweichen.

### 13.7 Expected-Token-Darstellung

Implementiere eine kontrollierte Übersetzung:

```cpp
class ExpectedTokenFormatter final {
public:
    std::string format(
        const std::vector<TokenKind>& expected,
        std::size_t maximumItems = 5
    ) const;
};
```

Regeln:

- INTERLIS-Schlüsselwörter in lesbarer Form.
- Interpunktion als Zeichen.
- Namen als „a name“ oder projektspezifisch konsistent.
- Keine Grammar-Indizes.
- Doppelte Tokens entfernen.
- Deterministische Reihenfolge.
- Bei grosser Menge Kategorien statt vollständiger Liste verwenden.

---

## 14. Modell- und Importauflösung

### 14.1 Fehlendes Modell

Eine Missing-Model-Diagnose soll enthalten:

- Modellname;
- primäre Range des Imports oder der Referenz;
- Source-URI des importierenden Modells;
- stabilen Code;
- keine wiederholten Meldungen pro nachfolgender Typreferenz;
- Missing-Models-Liste im CompilationResult unverändert.

Beispiel:

```text
Imported model 'Base' could not be resolved.
```

### 14.2 Doppelte Modelldefinition

Primär:

- zweite beziehungsweise konfliktverursachende Definition.

Related Information:

- erste bekannte Definition.

Beispiel:

```text
Model 'Base' is defined more than once.
```

Related:

```text
First definition of model 'Base'.
```

### 14.3 Unbekannter Name oder Typ

Eine Diagnose muss unterscheiden, soweit die bestehende fachliche Entscheidung dies bereits tut:

- unbekanntes Modell;
- unbekanntes Topic;
- unbekannte Klasse;
- unbekannte Domain;
- unbekanntes Attribut;
- nicht auflösbarer qualifizierter Pfad.

P6 darf diese Unterscheidung nicht als neue semantische Prüfung einführen. Es darf nur bereits vorhandene interne Informationen präziser abbilden.

### 14.4 Keine spekulative Ähnlichkeitssuche

Ein „Did you mean …?“ ist ausserhalb von P6, sofern keine bereits deterministisch vorhandene eindeutige Kandidatenlogik existiert.

Keine Levenshtein-basierte automatische Empfehlung ohne separaten Auftrag.

### 14.5 Range des Namens

Die primäre Range soll die nicht auflösbare Namenskomponente markieren, nicht zwingend den gesamten Pfad.

Bei:

```text
External.Topic.Class
```

und unbekanntem `Topic` soll nach Möglichkeit `Topic` markiert werden.

Wenn der aktuelle Resolver nur den Gesamtpfad kennt, darf P6 die vorhandene strukturierte Parserinformation zur Rangepräzisierung verwenden, solange die Auflösungsentscheidung unverändert bleibt.

---

## 15. Semantische Diagnosen

### 15.1 Migrationsprinzip

Jede bestehende semantische Fehlermeldung ist auf folgende Struktur zu prüfen:

```text
code
severity
primary subject
primary range
conflicting subject
related range
message template
notes
cascade cause
```

### 15.2 Konfliktdiagnosen

Für Konflikte wie doppelte Namen, Rollen-/Attributkonflikte oder inkompatible Übersetzungen gilt:

- primäre Range zeigt auf die neuere oder beanstandete Deklaration;
- Related Information zeigt auf den Konfliktpartner;
- beide qualifizierten Namen werden genannt, wenn nötig;
- keine Dateipfade im Meldungstext;
- keine mehrfachen identischen Konfliktmeldungen in beiden Richtungen.

### 15.3 Typdiagnosen

Typfehler sollen unterscheiden, soweit intern bekannt:

- erwarteter Typ;
- tatsächlicher Typ;
- betroffene Deklaration;
- Ursprung eines geerbten oder referenzierten Typs.

Beispiel:

```text
Attribute 'Parcel.area' has type 'TEXT', but a numeric type is required.
```

Related Information kann auf die Typdefinition zeigen.

### 15.4 Kardinalität

Kardinalitätsmeldungen sollen:

- die vorhandene Kardinalität zeigen;
- die verletzte Bedingung in normaler Sprache erklären;
- auf die Kardinalitätsangabe zeigen;
- keine internen Min-/Max-Variablennamen nennen.

### 15.5 Übersetzungen

Translation-Diagnosen sollen:

- übersetztes Element;
- Basiselement;
- Art der Abweichung;
- primäre und Related Range

enthalten.

Die fachlichen Übersetzungsregeln dürfen nicht geändert werden.

### 15.6 Producer-Adapter

Bestehende Semantic-Checker-Methoden sollen keine UI-spezifische Formatierung enthalten.

Empfohlen:

```cpp
class SemanticDiagnosticEmitter final {
public:
    DiagnosticCauseId emitDuplicateName(...);
    DiagnosticCauseId emitUnknownType(...);
    DiagnosticCauseId emitTypeMismatch(...);
    DiagnosticCauseId emitCardinalityViolation(...);
    DiagnosticCauseId emitTranslationMismatch(...);
};
```

Der Emitter formatiert Diagnosen. Der Checker trifft weiterhin die bestehende fachliche Entscheidung.

---

## 16. Repositorydiagnosen

P6 soll vorhandene Repositorydiagnosen in denselben Qualitätsvertrag integrieren.

Mindestens:

```text
ILIC-REPO-INDEX
ILIC-REPO-SITE
ILIC-REPO-NOT-FOUND
ILIC-REPO-CYCLE
ILIC-REPO-PATH
ILIC-REPO-DOWNLOAD
ILIC-REPO-CHECKSUM
ILIC-REPO-CACHE
ILIC-REPO-OFFLINE
ILIC-REPO-LIMIT
ILIC-REPO-CANCELLED
ILIC-REPO-XML
ILIC-REPO-VERSION
```

### Anforderungen

- URI separat im strukturierten Kontext halten.
- Credentials, Tokens und Query-Secrets redigieren.
- Checksumdiagnose darf expected und actual enthalten.
- Transportfehler darf keine kompletten Response-Header veröffentlichen.
- Cachewarnung soll die Modellauflösung nicht als fehlgeschlagen darstellen, wenn gültige Source vorliegt.
- Optional fehlende `ilisite.xml` darf keine irreführende Fehlermeldung erzeugen.
- Native und JavaScript-Repository-Core sollen dieselben Codes und vergleichbare Meldungsparameter verwenden.
- Diese Phase darf Repositoryentscheidungen nicht verändern.

### Secret Redaction

Empfohlene Hilfsfunktion:

```cpp
std::string redactRepositoryUri(std::string_view uri);
std::string redactTransportError(std::string_view message);
```

Zu redigieren:

- Userinfo;
- Bearer Tokens;
- API Keys in bekannten Queryparametern;
- Basic-Auth-Header;
- lokale sensible Homepfade, sofern sie nicht für die Diagnose notwendig sind.

---

## 17. ABI- und Requestdiagnosen

Fehlerhafte C-ABI-Requests und JSON-Requests sollen kontrolliert diagnostiziert werden.

Mindestens:

- ungültiger Sessionhandle;
- null request;
- ungültiges JSON;
- falsche `schemaVersion`;
- fehlende Roots;
- falscher Feldtyp;
- ungültige Optionen;
- zerstörte Session;
- ungültiger Resultathandle.

### Regeln

- Kein C++-Exceptiontext wird blind publiziert.
- Bekannte Requestfehler erhalten stabile Codes.
- Unbekannte Exceptions bleiben klar als intern klassifiziert.
- Keine Stacktraces in Standardausgaben.
- ABI-Fehler dürfen keine Compilerdiagnose vortäuschen.
- JSON-Resultat bleibt syntaktisch gültig.
- Bestehende Schema- und ABI-Version bleiben kompatibel.

Empfohlene Trennung:

```cpp
enum class RequestErrorKind {
    InvalidSession,
    NullRequest,
    InvalidJson,
    UnsupportedSchema,
    MissingField,
    InvalidFieldType,
    InvalidOption
};

Diagnostic requestDiagnostic(RequestErrorKind kind, ...);
```

---

## 18. Interne Fehler – ausdrücklich ausserhalb der Korrektur

Die aktuell bekannten Conformance-Internal-Errors sind nicht fachlich zu korrigieren.

P6 MUSS:

- diese Testfälle im Decision Lock führen;
- verhindern, dass Diagnose-Refactoring ihr Outcome versehentlich verändert;
- dokumentieren, dass sie bewusst offen bleiben;
- keine Baselineänderung vornehmen.

P6 DARF allgemeine Infrastruktur zur sicheren Diagnoseformatierung verbessern, solange der konkrete Outcome und die bekannte Abweichung unverändert bleiben.

P6 DARF NICHT behaupten, Internal Errors seien gelöst.

---

## 19. `DiagnosticPipeline`

### 19.1 Verantwortung

Empfohlene Klasse:

```cpp
class DiagnosticPipeline final {
public:
    explicit DiagnosticPipeline(
        const DiagnosticCatalog& catalog,
        const SourceManager& sources,
        DiagnosticPipelineOptions options = {}
    );

    PublishedDiagnostics publish(
        std::vector<DiagnosticCandidate> candidates
    ) const;

private:
    void validate(...) const;
    void normalize(...) const;
    void deduplicate(...) const;
    void suppressCascades(...) const;
    void sort(...) const;
};
```

### 19.2 Reihenfolge

Empfohlene deterministische Sortierung:

1. URI;
2. Gültigkeit der Range;
3. Start-Byteoffset;
4. End-Byteoffset;
5. Severity-Rang;
6. Phase;
7. Code;
8. normalisierte Meldung;
9. Producer-Reihenfolge nur als letzter stabiler Tie-Breaker.

Die Sortierung muss dokumentiert und in Native/WASM identisch sein.

### 19.3 Validierung

Die Pipeline MUSS mindestens prüfen:

- registrierter Code;
- nicht leere Meldung;
- gültige Severity;
- Rangegrenzen;
- Related Ranges;
- keine identische Related Information;
- keine leeren Notes;
- `treatedAsError` nur bei Warnungen beziehungsweise gemäss bestehendem Vertrag;
- kein nicht erlaubter interner Text.

Im Release-Build sollen ungültige Kandidaten kontrolliert normalisiert oder mit einem klaren internen Fallback publiziert werden. Im Debug-/Test-Build dürfen Invarianten härter geprüft werden.

### 19.4 Deduplizierung

Empfohlener Schlüssel:

```cpp
struct DiagnosticDeduplicationKey {
    std::string code;
    DiagnosticSeverity severity;
    std::string source;
    std::string uri;
    std::size_t startByte;
    std::size_t endByte;
    std::string normalizedMessage;
};
```

Related Information und Notes sollen für die Gleichheit kontrolliert berücksichtigt werden.

Keine Deduplizierung nur anhand des Codes.

### 19.5 Pipeline-Statistik

```cpp
struct DiagnosticPipelineStats {
    std::uint64_t candidates = 0;
    std::uint64_t published = 0;
    std::uint64_t exactDuplicatesRemoved = 0;
    std::uint64_t cascadesSuppressed = 0;
    std::uint64_t invalidRangesNormalized = 0;
    std::uint64_t unknownCodes = 0;
};
```

Diese Statistik ist Beobachtbarkeit. Sie darf keine Compilerentscheidung beeinflussen.

---

## 20. Kaskadenmodell

### 20.1 Ursache-ID

Empfohlene interne Typen:

```cpp
struct DiagnosticCauseId {
    std::uint64_t value;
};

enum class DiagnosticPublicationClass {
    Primary,
    Independent,
    Cascaded,
    Recovery
};
```

### 20.2 Producer-Vertrag

Ein Producer, der einen Folgefehler erzeugt, muss nach Möglichkeit die Ursache referenzieren:

```cpp
DiagnosticBuilder(code)
    .dependsOn(missingModelCause)
    .publicationClass(DiagnosticPublicationClass::Cascaded);
```

### 20.3 Unterdrückungsregeln

Unterdrückbar sind beispielsweise:

- unbekannter Typ als Folge eines bereits gemeldeten fehlenden importierten Modells;
- ungültiger Pfad als Folge einer nicht auflösbaren ersten Pfadkomponente;
- weitere Typfehler, wenn die Ausgangsdeklaration bereits nicht aufgebaut werden konnte;
- wiederholte Meldung desselben Namenskonflikts aus mehreren Traversierungsrichtungen.

Nicht unterdrückbar:

- unabhängiger zweiter Syntaxfehler;
- Fehler in einer anderen Deklaration;
- zusätzliche Modellmehrdeutigkeit;
- Sicherheits- oder Repositoryintegritätsfehler;
- ein Fehler nur wegen räumlicher Nähe;
- ein Fehler nur weil der Text ähnlich ist.

### 20.4 Kein Message Matching

Folgendes ist verboten:

```cpp
if (diagnostic.message.find("Model not found") != std::string::npos) {
    ...
}
```

Ebenso in TypeScript.

### 20.5 Suppression Trace

Im Test-/Debugmodus muss nachvollziehbar sein:

```text
suppressed code
suppressed range
primary cause code
primary cause range
producer
reason
```

Der Trace ist nicht zwingend Teil der öffentlichen CompilationResult-API.

---

## 21. Meldungstemplates

### 21.1 Ziel

Meldungstexte sollen aus kontrollierten Templates und Parametern entstehen.

Empfohlene Struktur:

```cpp
struct DiagnosticArguments {
    std::map<std::string, std::string> values;
};

class DiagnosticMessageFormatter final {
public:
    std::string format(
        DiagnosticCode code,
        const DiagnosticArguments& arguments
    ) const;
};
```

Eine vollständige Lokalisierungsinfrastruktur ist nicht erforderlich.

### 21.2 Stilregeln

- Ein Satz.
- Punkt am Ende konsistent nach Projektentscheid.
- Namen mit einfachen Quotes.
- Keine Backticks in nativen Meldungen, sofern CLI/LSP dieselben Texte verwenden.
- Keine interne Namespace-Präfixe.
- Keine C++-Klassenbezeichner.
- Keine ANTLR-Regelnamen, ausser es handelt sich um dokumentierte INTERLIS-Begriffe.
- Erwartet/tatsächlich klar benennen.
- Keine vagen Wörter wie „bad“, „wrong“ oder „problem“.
- Keine Aufforderung „contact developer“ für normale Benutzermodellfehler.
- Keine mehrzeiligen Meldungen; zusätzliche Information gehört in Notes.
- Keine URL direkt in der Meldung; optional `helpId`.

### 21.3 Sichere Parameter

Parameter müssen:

- kontrolliert gequotet werden;
- Zeilenumbrüche entfernen oder escapen;
- überlange Sourcefragmente begrenzen;
- Secrets redigieren;
- deterministisch formatiert sein.

### 21.4 Terminologieliste

Die Dokumentation soll eine kleine verbindliche Terminologieliste führen, zum Beispiel:

```text
model
translation
topic
class
structure
association
role
attribute
domain
unit
view
graphic
constraint
import
```

Synonyme sollen nicht zufällig zwischen Meldungen wechseln.

---

## 22. `warningsAsErrors`

Der bestehende Vertrag muss erhalten bleiben.

Empfohlen:

```text
severity = warning
treatedAsError = true
```

Nicht:

```text
severity = error
```

sofern der aktuelle öffentliche Vertrag bereits `treatedAsError` verwendet.

Tests müssen prüfen:

- Code bleibt gleich;
- Meldung bleibt gleich;
- Severity bleibt warning;
- `treatedAsError` ändert sich;
- `success` entspricht dem bisherigen Verhalten;
- LSP und Monaco zeigen die bestehende visuelle Semantik;
- Web-IDE zählt Problems konsistent.

---

## 23. CLI-Darstellung

### 23.1 Trennung

Diagnosedaten und Textdarstellung müssen getrennt sein.

Empfohlene Klasse:

```cpp
struct DiagnosticTextRenderOptions {
    bool showCode = true;
    bool showSource = true;
    bool showRelated = true;
    bool showNotes = true;
    std::size_t contextLines = 0;
};

class DiagnosticTextRenderer final {
public:
    std::string render(
        const Diagnostic& diagnostic,
        const SourceManager* sources,
        DiagnosticTextRenderOptions options = {}
    ) const;

    std::string renderAll(
        const std::vector<Diagnostic>& diagnostics,
        const SourceManager* sources,
        DiagnosticTextRenderOptions options = {}
    ) const;
};
```

### 23.2 Ausgabe

Beispiel:

```text
model.ili:12:8: error ILIC-RESOLVE-MODEL-NOT-FOUND:
Imported model 'Base' could not be resolved.

Related:
  other.ili:3:7: Import originates here.

Note:
  Repository lookup completed without a matching model.
```

Die genaue Formatierung muss zum bestehenden CLI-Stil passen.

### 23.3 Verboten

- JSON-Serialisierung und CLI-Rendering in derselben Funktion.
- ANSI-Farbcodes in strukturierten Ausgaben.
- Terminalbreite als fachlicher Faktor.
- doppelte Ausgabe desselben Fehlers über Logger und Diagnostics.
- Pfade oder Zeilennummern sowohl im Präfix als auch im Meldungstext.

### 23.4 Exitcode

CLI-Rendering darf den Exitcode nicht beeinflussen. Der Exitcode folgt weiterhin dem bestehenden CompilationResult beziehungsweise dem bestehenden CLI-Vertrag.

---

## 24. C-ABI-JSON

Die bestehende JSON-Struktur muss kompatibel bleiben.

Mindestens:

```json
{
  "severity": "error",
  "code": "ILIC-...",
  "source": "ilic",
  "message": "...",
  "range": {},
  "relatedInformation": [],
  "notes": [],
  "treatedAsError": false
}
```

### 24.1 Additive Felder

Optional:

```json
{
  "phase": "semantic",
  "tags": ["primary"],
  "helpId": "diagnostics/ILIC-...",
  "fingerprint": "..."
}
```

Alte Wrapper müssen unbekannte Felder ignorieren können.

### 24.2 Serialisierungsregeln

- Stabile Feldnamen.
- Keine Plattformpfade in Meldungen.
- Gleiche Reihenfolge der Diagnosearrays.
- UTF-8 korrekt.
- Keine ungültigen Zahlen.
- `range: null` nur wenn wirklich unbekannt.
- Related Information vollständig.
- Notes vollständig.
- Kein doppelter JSON-Serializer mit abweichender Semantik.

### 24.3 Aufteilung von `Capi.cpp`

Die Diagnose-JSON-Erzeugung soll mindestens logisch extrahiert werden, beispielsweise:

```text
source/abi/DiagnosticJson.h
source/abi/DiagnosticJson.cpp
```

Empfohlene Funktionen:

```cpp
json::Value diagnosticToJson(const Diagnostic&);
json::Value diagnosticsToJson(const std::vector<Diagnostic>&);
```

## 25. WASM-Vertrag

`@ilic/compiler-wasm` muss:

- alle bestehenden Diagnosefelder typisieren;
- additive Felder optional typisieren;
- keine Meldungen neu interpretieren;
- keine eigenen Diagnosecodes erzeugen;
- keine Ranges neu berechnen;
- keine Deduplizierung anhand von Meldungen durchführen;
- Native-/WASM-Paritätstests besitzen.

Empfohlene TypeScript-Typen:

```ts
export type DiagnosticPhase =
  | "lexical"
  | "syntax"
  | "resolution"
  | "semantic"
  | "translation"
  | "repository"
  | "formatting"
  | "request"
  | "internal";

export interface Diagnostic {
  readonly severity: DiagnosticSeverity;
  readonly code: string;
  readonly source?: string;
  readonly message: string;
  readonly range: SourceRange | null;
  readonly relatedInformation: readonly DiagnosticRelatedInformation[];
  readonly notes: readonly string[];
  readonly treatedAsError: boolean;
  readonly phase?: DiagnosticPhase;
  readonly tags?: readonly string[];
  readonly helpId?: string;
  readonly fingerprint?: string;
}
```

Bestehende reale Typen sind zu erweitern, nicht zu duplizieren.

### 25.1 Mischversionen

Mindestens prüfen:

- neuer Wrapper mit altem WASM;
- alter Wrapper mit neuem WASM;
- fehlende optionale Felder;
- unbekannte Tags;
- unbekannte Phase;
- Diagnose ohne `helpId`;
- Diagnose ohne `fingerprint`.

Die Kernfelder müssen kompatibel bleiben.

---

## 26. `interlis-language-tools`: Diagnosearchitektur

### 26.1 Ziel

Der Language Service soll Diagnosen verwalten, nicht fachlich neu erzeugen.

Er darf weiterhin konservative Live-Diagnosen und Quick Fixes erzeugen, sofern diese klar von nativen semantischen Diagnosen getrennt sind.

### 26.2 Interne Komponenten

Die bestehende `LanguageService`-Fassade darf erhalten bleiben.

Empfohlene Extraktion:

```ts
class DiagnosticStore;
class DiagnosticMerger;
class DiagnosticPublicationPolicy;
class DiagnosticVersionGate;
class DiagnosticFingerprint;
```

### 26.3 `DiagnosticStore`

Empfohlene Verantwortung:

```ts
interface StoredDiagnostics {
  readonly uri: string;
  readonly documentVersion: number;
  readonly rootUri?: string;
  readonly origin: "live" | "saved" | "semantic" | "repository";
  readonly diagnostics: readonly Diagnostic[];
}

class DiagnosticStore {
  put(value: StoredDiagnostics): void;
  removeByUri(uri: string): void;
  removeByRoot(rootUri: string): void;
  current(uri: string, documentVersion: number): readonly Diagnostic[];
  clear(): void;
}
```

### 26.4 Versionsgate

Eine Diagnose darf nur publiziert werden, wenn:

- URI passt;
- Dokumentversion aktuell ist;
- Run-ID aktuell ist;
- Compilation-Epoch aktuell ist;
- Workspacegeneration aktuell ist;
- Root-Snapshot nicht überholt ist.

Überholte Diagnosen müssen verworfen werden.

Empfohlene API:

```ts
interface DiagnosticPublicationToken {
  readonly uri: string;
  readonly documentVersion: number;
  readonly runId: number;
  readonly compilationEpoch: number;
  readonly generation: number;
  readonly rootUri?: string;
}

class DiagnosticVersionGate {
  accepts(token: DiagnosticPublicationToken): boolean;
}
```

### 26.5 Merge-Priorität

Die bestehende Produktsemantik ist zu bewahren.

Sinngemäss:

```text
dirty document:
  conservative live diagnostics
  plus weiterhin gültige unabhängige workspace diagnostics
  keine veralteten saved syntax diagnostics

saved document:
  native semantic/syntax diagnostics
  plus repository or workspace diagnostics

failed compilation:
  current errors visible
  last-good semantic data bleibt für Navigation/Diagramm getrennt erhalten
```

Die exakte bestehende Semantik ist vor Änderungen zu erfassen und durch Tests zu sperren.

### 26.6 Deduplizierung

Deduplizierung erfolgt anhand:

- Code;
- Range;
- Severity;
- Origin;
- stabiler Fingerprint oder normalisierter strukturierter Identität.

Nicht allein anhand der Meldung.

### 26.7 Quick Fixes

Quick Fixes müssen an mindestens Folgendes gebunden sein:

```ts
interface DiagnosticQuickFixKey {
  readonly code: string;
  readonly uri: string;
  readonly range: EditorRange;
  readonly documentVersion: number;
}
```

Ein Quick Fix darf nicht durch englische Meldungstextsuche aktiviert werden.

### 26.8 Native versus Live

Live-Diagnosen müssen eine eigene `source` beziehungsweise Origin besitzen. Sie dürfen native Diagnosen nicht imitieren, wenn sie nur heuristisch oder konservativ sind.

Empfohlen:

```text
source = ilic-live
source = ilic
source = ilic-repository
```

Die tatsächlichen Werte sind projektweit festzulegen und zu dokumentieren.

---

## 27. Node-LSP

### 27.1 Mapping

Native Diagnosen müssen vollständig auf LSP abgebildet werden:

```ts
function toLspDiagnostic(value: Diagnostic): LspDiagnostic;
```

Zu mappen:

- severity;
- range;
- code;
- source;
- message;
- relatedInformation;
- tags, falls vorhanden;
- codeDescription, falls `helpId` stabil auflösbar ist;
- optional strukturierte `data` nur additiv.

### 27.2 LSP-Regeln

- Zero-based Zeilen und Zeichen.
- UTF-16-Zeichenpositionen.
- Ungültige native Range erhält kontrollierten Fallback.
- Keine Diagnose für falsche URI publizieren.
- `publishDiagnostics` enthält immer die aktuelle vollständige Liste pro URI.
- Leere Liste entfernt alte Problems.
- Keine Meldungsparsinglogik.
- Codes bleiben Strings.
- Related Information erhält korrekte Location.
- LSP-Mapping darf Severity nicht eigenmächtig ändern.

### 27.3 Tests

Mindestens:

- Code sichtbar;
- Source sichtbar;
- Related Information;
- Unicode Range;
- EOF Range;
- alte Diagnose wird entfernt;
- stale Ergebnis wird verworfen;
- gleiche Diagnose nicht doppelt;
- `warningsAsErrors`;
- mehrere Roots;
- Repositorysource;
- Dokument geschlossen;
- Workspace gewechselt.

---

## 28. Monaco Adapter

Monaco Marker müssen denselben semantischen Inhalt tragen.

Zu mappen:

```ts
interface MonacoMarkerData {
  severity;
  message;
  code;
  source;
  startLineNumber;
  startColumn;
  endLineNumber;
  endColumn;
  relatedInformation?;
  tags?;
}
```

Regeln:

- Kein separates Codeinventar.
- Kein Message Matching.
- Marker werden atomar pro Owner aktualisiert.
- Live- und Semantic-Marker dürfen nicht doppelt erscheinen.
- Ownernamen müssen stabil sein.
- Quick Fixes prüfen Dokumentversion.
- Repositorytabs bleiben read-only.
- Related Information darf navigierbar gemacht werden, ohne den Markertext zu duplizieren.

### 28.1 Fallback Range

Monaco benötigt immer eine darstellbare Range. Für native Diagnosen ohne Range muss eine zentrale, klar gekennzeichnete Projektion verwendet werden.

Empfohlen:

```ts
function markerRange(
  diagnostic: Diagnostic,
  model: MonacoModel | undefined,
): MonacoRange;
```

Ein Fallback darf nicht zurück in die native Diagnose geschrieben werden.

---

## 29. `interlis-web-ide`: Problems und Navigation

### 29.1 Problems-Datenmodell

Die Web-IDE soll nicht aus gerendertem Text zurückparsen.

Empfohlene Projektion:

```ts
interface ProblemItem {
  readonly id: string;
  readonly uri: string;
  readonly severity: DiagnosticSeverity;
  readonly code: string;
  readonly source?: string;
  readonly message: string;
  readonly range: EditorRange | null;
  readonly relatedInformation: readonly ProblemRelatedInformation[];
  readonly notes: readonly string[];
  readonly documentVersion?: number;
}
```

### 29.2 Stabile ID

Empfohlen:

```text
uri + code + start + end + semantic fingerprint
```

Nicht:

```text
array index
```

### 29.3 Sortierung

Empfohlen:

1. Severity;
2. URI beziehungsweise sichtbarer Dateiname;
3. Range;
4. Code;
5. Message.

Die Sortierung muss stabil sein.

### 29.4 Darstellung

Mindestens sichtbar:

- Severity-Icon;
- Meldung;
- Code;
- Datei;
- Zeile/Spalte;
- Source, sofern hilfreich.

Optional aufklappbar:

- Related Information;
- Notes.

### 29.5 Navigation

Klick auf primäre Diagnose:

- öffnet oder aktiviert Source;
- setzt Cursor;
- selektiert Range;
- fokussiert Editor;
- funktioniert für Workspace- und Repositorysource;
- verhindert Bearbeitung einer read-only Source.

Klick auf Related Information:

- navigiert zu Related Range;
- zeigt den Related-Text;
- kehrt nicht unbeabsichtigt zur primären Range zurück.

### 29.6 Produktverträge

P6 darf nicht brechen:

- Last-Good-Diagramm;
- Problems-Zähler;
- Output;
- Completion;
- Outline;
- Save-driven Compilation;
- stale status;
- Auto-Save;
- Recovery;
- Offline;
- PWA;
- Worker-Fallback.

### 29.7 Extraktion aus `WebIdeWorkbench`

Wenn Problems-Logik derzeit direkt in der Workbench liegt, soll sie in klar begrenzte Komponenten extrahiert werden:

```ts
class ProblemStore;
class ProblemProjector;
class ProblemRenderer;
class ProblemNavigator;
```

Die Workbench bleibt Fassade und Lifecycle-Owner.

---

## 30. Diagnosequalität-Bericht

Implementiere einen maschinenlesbaren Qualitätsbericht.

Empfohlene Struktur:

```json
{
  "schemaVersion": 1,
  "totalDiagnostics": 0,
  "registeredCodeCoverage": 1.0,
  "validPrimaryRangeCoverage": 1.0,
  "relatedInformationCount": 0,
  "exactDuplicateCount": 0,
  "unknownCodeCount": 0,
  "messageLeakCount": 0,
  "cascadeSuppressedCount": 0,
  "diagnosticsByPhase": {},
  "diagnosticsByCode": {}
}
```

### 30.1 Metriken

Mindestens:

- Anzahl verschiedener Codes;
- unregistrierte Codes;
- Diagnose ohne Code;
- Diagnose ohne gültige Range;
- Diagnose mit Related Information;
- exakte Duplikate;
- unterdrückte Kaskaden;
- Meldungen mit ANTLR-internen Tokennamen;
- Meldungen mit C++-Typnamen;
- Meldungen mit absoluten Temp-Pfaden;
- Native-/WASM-Abweichungen.

### 30.2 Verwendung

Der Bericht ist ein Qualitätssignal, kein Ersatz für fachliche Tests.

CI soll insbesondere verhindern:

- neue unregistrierte Codes;
- neue leere Codes;
- neue exakte Duplikate;
- neue Implementierungsleaks;
- sinkende Rangequalität für geschützte Fixtures.

### 30.3 Report-API

Ein Reportgenerator darf als Test-/Scriptkomponente implementiert werden:

```text
scripts/generate-diagnostic-quality-report.*
```

Er darf nicht den normalen Compilerpfad verlangsamen oder zusätzliche öffentliche Laufzeitabhängigkeiten einführen.

---

## 31. Diagnose-Fixtures

Erstelle einen dedizierten Fixturebestand:

```text
test/diagnostics/
    syntax/
    resolution/
    semantic/
    translation/
    repository/
    ranges/
    cascades/
    ordering/
    decision-lock/
```

Jedes Fixture soll enthalten:

```text
source files
compile request
expected decision fingerprint
expected published diagnostics
optional expected suppressed diagnostics
optional consumer projection
```

Empfohlenes Manifest:

```json
{
  "schemaVersion": 1,
  "id": "missing-imported-model",
  "roots": ["memory:///Root.ili"],
  "options": {},
  "expectedDecision": {
    "success": false,
    "outcomeClass": "rejected"
  },
  "expectedDiagnostics": [
    {
      "code": "ILIC-RESOLVE-MODEL-NOT-FOUND",
      "severity": "error",
      "uri": "memory:///Root.ili"
    }
  ]
}
```

Vollständige Meldungstexte sollen nur dort fest geprüft werden, wo Formulierung und Parameter Teil des Qualitätsziels sind.

---

## 32. Pflichtszenarien

| ID | Phase | Fall | Verbindliche Prüfung |
|---|---|---|---|
| `lexical-unrecognized-character` | Lexer | Unbekanntes Zeichen | Code, kleinstmögliche Range, sicher gequotetes Zeichen |
| `syntax-unexpected-token` | Parser | Unerwartetes Token | Lesbarer Tokenname und begrenzte Erwartungsliste |
| `syntax-missing-semicolon` | Parser | Fehlendes Semikolon | Zero-width-Insertion-Range am erwarteten Ort |
| `syntax-unexpected-eof` | Parser | Unerwartetes Dateiende | Range am EOF und verständlicher unvollständiger Kontext |
| `syntax-two-independent-errors` | Parser | Zwei unabhängige Fehler | Beide bleiben sichtbar und stabil sortiert |
| `editor-recovery-vs-compiler` | Editor/Compiler | Toleranter Editor, strikter Compiler | Keine Vermischung der Diagnosepfade |
| `missing-imported-model` | Resolution | Importiertes Modell fehlt | Eine Primärdiagnose; Folgekaskaden kontrolliert |
| `missing-model-repository-offline` | Repository | Modell fehlt offline | Repositorycode und Import-Range bleiben getrennt |
| `duplicate-model-definition` | Resolution | Doppelte Modelldefinition | Primär- und Related Range |
| `unknown-domain` | Resolution | Unbekannte Domain | Präziser Code und Referenzrange |
| `unknown-qualified-path` | Resolution | Pfad nicht auflösbar | Range der ersten nicht auflösbaren Komponente |
| `duplicate-attribute` | Semantic | Doppeltes Attribut | Konfliktpartner als Related Information |
| `role-attribute-conflict-controlled` | Semantic | Kontrollierter bestehender Konfliktfall | Keine Internal-Error-Korrektur |
| `invalid-cardinality` | Semantic | Ungültige Kardinalität | Range der Kardinalität und klare Bedingung |
| `type-mismatch` | Semantic | Typinkompatibilität | Erwartet/tatsächlich ohne interne Typnamen |
| `inheritance-conflict` | Semantic | Konflikt durch Vererbung | Related Range der geerbten Deklaration |
| `translation-mismatch` | Translation | Übersetzungsabweichung | Basis und Übersetzung verknüpfen |
| `repository-checksum` | Repository | Prüfsummenabweichung | Expected/actual und keine Secrets |
| `repository-cycle` | Repository | Repositoryzyklus | Deterministischer Zykluspfad |
| `repository-xml` | Repository | Ungültige Metadaten | Sicherer XML-Fehler ohne Parserinternas |
| `warnings-as-errors` | Compiler | Warnung als Fehler behandelt | Severity und treatedAsError-Vertrag |
| `unicode-before-error` | Ranges | Mehrbytezeichen vor Fehler | Byte- und UTF-16-Position korrekt |
| `crlf-range` | Ranges | CRLF | Identische logische Position |
| `empty-file` | Ranges | Leere Datei | Gültige EOF-Range |
| `repository-uri-range` | Ranges | Repositorysource | URI bleibt autoritativ |
| `exact-duplicate` | Pipeline | Identische Diagnose zweimal erzeugt | Genau eine Publikation |
| `cascade-missing-model` | Pipeline | Fehlendes Modell mit Folgefehlern | Strukturelle Suppression |
| `same-location-independent` | Pipeline | Unabhängige Fehler an gleicher Stelle | Keine falsche Deduplizierung |
| `deterministic-ordering` | Pipeline | Mehrere Dateien und Phasen | Identische Reihenfolge über Wiederholungen |
| `capi-json` | C-ABI | Diagnose-JSON | Alle Felder und Unicode |
| `wasm-parity` | WASM | Native/WASM | Strukturell identische Diagnosen |
| `lsp-related-information` | Language Server | Related Information | Korrekte Location und Message |
| `stale-run-discard` | Language Service | Überholter Compile | Keine alten Diagnostics |
| `live-vs-saved-dedup` | Language Service | Live und saved | Keine Dopplung |
| `quick-fix-by-code` | Language Service | Quick Fix | Keine Meldungstextabhängigkeit |
| `monaco-marker` | Monaco | Markerprojektion | Code, Source, Range |
| `web-problems-navigation` | Web IDE | Problems-Klick | Korrekte Cursorposition |
| `web-related-navigation` | Web IDE | Related-Klick | Konfliktpartner öffnen |
| `last-good-diagram` | Web IDE | Compilefehler nach gültigem Snapshot | Diagramm bleibt last-good |
| `workspace-switch-clears` | Web IDE | Workspacewechsel | Alte Problems verschwinden |

Für jedes Szenario sind mindestens Native, C-ABI/WASM und der relevante Consumerpfad zu prüfen.

---

## 33. Decision-Lock-Suite

### 33.1 Quellen

Die Suite muss mindestens umfassen:

- vollständige externe Conformance-Suite;
- bestehende native Tests;
- bestehende WASM-Snapshottests;
- Diagnose-Fixtures;
- relevante Language-Tools-Tests;
- relevante Web-IDE-E2E-Fälle.

### 33.2 Vergleich

Empfohlener Runner:

```text
scripts/capture-diagnostic-decisions.*
scripts/compare-diagnostic-decisions.*
```

Er soll vor und nach P6 canonical Decision-Fingerprints erzeugen.

### 33.3 Bekannte Conformanceabweichungen

Alle bestehenden Baselinefälle bleiben im Vergleich enthalten.

Der Report muss deutlich zeigen:

```text
outcome unchanged
diagnostics changed
baseline unchanged
```

Kein Eintrag darf still verschwinden oder neu hinzukommen.

### 33.4 Verbot

Ein Decision-Lock-Diff darf nicht mit einer Allowlist für P6 „grün gemacht“ werden.

Wenn eine Diagnoseänderung eine fachliche Entscheidung beeinflusst, ist sie zurückzunehmen oder ausserhalb von P6 separat zu behandeln.

---

## 34. Native Tests

Mindestens folgende Testgruppen sind zu implementieren oder zu erweitern.

### 34.1 Katalog

```text
DiagnosticCatalogTest
DiagnosticCodeUniquenessTest
DiagnosticCodeCoverageTest
DiagnosticCodeFormatTest
```

### 34.2 Builder und Pipeline

```text
DiagnosticBuilderTest
DiagnosticValidationTest
DiagnosticDeduplicationTest
DiagnosticCascadeSuppressionTest
DiagnosticOrderingTest
DiagnosticMessageSanitizationTest
```

### 34.3 Ranges

```text
DiagnosticRangeResolverTest
DiagnosticUnicodeRangeTest
DiagnosticCrLfRangeTest
DiagnosticEofRangeTest
DiagnosticRelatedRangeTest
```

### 34.4 Parser

```text
LexerDiagnosticTest
ParserDiagnosticTranslatorTest
ParserExpectedTokensTest
ParserRecoveryIsolationTest
```

### 34.5 Semantik

```text
ResolutionDiagnosticTest
SemanticConflictDiagnosticTest
SemanticTypeDiagnosticTest
TranslationDiagnosticTest
```

### 34.6 Repository

```text
RepositoryDiagnosticContractTest
RepositorySecretRedactionTest
RepositoryDiagnosticParityTest
```

### 34.7 Decision Lock

```text
DiagnosticDecisionLockTest
DiagnosticConformanceOutcomeTest
```

### 34.8 Concurrency und Lebensdauer

Diagnoseobjekte müssen unter parallelen unabhängigen Sessions sicher sein.

Mindestens:

- zwei Sessions mit gleichen Fehlern;
- zwei Sessions mit verschiedenen Fehlern;
- Cachehit;
- Cacheclear;
- Sourceupdate;
- TSan;
- ASan/LSan;
- keine global mutable Diagnosepipeline;
- kein global mutable Meldungsformatter.

---

## 35. C-ABI- und WASM-Tests

Mindestens:

- jeder Severity-Wert;
- Code;
- Source;
- null Range;
- gültige Range;
- Related Information;
- Notes;
- `treatedAsError`;
- Unicode;
- additive Felder;
- unbekannte additive Felder im alten JS-Consumer;
- ungültiger Sessionhandle;
- ungültiges Request-JSON;
- Native/WASM canonical JSON;
- deterministische Reihenfolge;
- keine Duplikate;
- bekannte Decision-Lock-Fälle;
- Memory ownership der JSON-Resultate.

---

## 36. Tests in `interlis-language-tools`

Mindestens:

```text
DiagnosticStoreTest
DiagnosticVersionGateTest
DiagnosticMergerTest
DiagnosticFingerprintTest
LspDiagnosticMappingTest
MonacoDiagnosticMappingTest
QuickFixDiagnosticCodeTest
StaleDiagnosticDiscardTest
LiveSavedDiagnosticDedupTest
RootDiagnosticIsolationTest
RepositoryDiagnosticTest
WorkerDiagnosticParityTest
```

### Regressionsverträge

Explizit erhalten:

- Completion während verzögerter Editoranalyse;
- unused-import warning in dirty und saved states;
- gespeicherte Diagnose nach Save;
- Entfernung alter Diagnose nach Korrektur;
- Root-spezifische Diagnosepublikation;
- keine Worker-Replay-bedingten Duplikate;
- kein Message Matching für Quick Fixes.

---

## 37. Tests in `interlis-web-ide`

Mindestens Unit-/Integrationstests für:

- Problems-Projektion;
- Sortierung;
- stabile IDs;
- Primärnavigation;
- Related-Navigation;
- Notes;
- Codeanzeige;
- Sourceanzeige;
- leere Problems nach Korrektur;
- Workspacewechsel;
- Repositorysource;
- read-only Verhalten;
- Last-Good-Diagramm;
- Problems-Zähler;
- Output bleibt korrekt;
- stale Status.

Mindestens E2E:

1. Syntaxfehler erzeugen und Problem anklicken.
2. Fehler korrigieren und Problem verschwindet.
3. Doppelte Deklaration mit Related Information.
4. Missing Model ohne Kaskadenlawine.
5. Unicode vor Fehler und korrekte Navigation.
6. Last-Good-Diagramm bleibt sichtbar.
7. Workspacewechsel entfernt alte Problems.
8. Reload/PWA zeigt keine persistierten veralteten Problems.
9. Worker-Fallback liefert gleiche Codes.
10. Save-driven Semantik bleibt erhalten.

---

## 38. Dokumentation

Mindestens aktualisieren:

```text
docs/diagnostik-und-logging.md
docs/native-api.md
docs/wasm.md
docs/architecture/diagnostic-pipeline.md
```

In Language Tools:

```text
docs/diagnostics.md
docs/language-service.md
docs/lsp.md
```

In Web-IDE:

```text
docs/diagnostics-and-problems.md
```

### Dokumentationsinhalt

- Diagnosevertrag;
- Codes;
- Severity;
- `treatedAsError`;
- Range;
- Related Information;
- Notes;
- Kaskaden;
- Decision Lock;
- Native/WASM-Parität;
- LSP-Projektion;
- Web-IDE Problems;
- Anleitung zum Hinzufügen eines neuen Codes;
- verbotene Message-Matching-Logik.

---

## 39. Architecture Guards

Implementiere automatisierte Guards.

Mindestens erkennen:

1. `ILIC-...`-Code ausserhalb des Katalogs.
2. Doppelte Codes.
3. Leerer Code.
4. Rohe ANTLR-Meldung als direkte öffentliche Diagnose.
5. `T__<number>` in öffentlichen Meldungen.
6. `typeid(...).name()` in Diagnosen.
7. Pointerdarstellung in Diagnosen.
8. absolute Temp-Pfade in erwarteten öffentlichen Meldungen.
9. Quick-Fix- oder UI-Logik anhand von Meldungstext.
10. direkte freie Diagnose-Aggregatinitialisierung ausserhalb erlaubter Adapter.
11. Diagnose-Deduplizierung nur nach Meldung.
12. Kaskadenunterdrückung nur nach Meldung.
13. mutable globale Diagnosepipeline.
14. `thread_local` Diagnosezustand.
15. abweichender C-ABI- und WASM-Feldvertrag.
16. Consumer, der `code` verwirft.
17. Web-IDE, die Problems aus gerendertem Text parst.
18. Diagnoseänderung mit Conformance-Baselineänderung im selben P6-Scope.

Jeder Guard benötigt einen negativen Canary.

---

## 40. CI

Erhalte alle bestehenden Jobs.

Ergänze mindestens:

```text
diagnostic-catalog
diagnostic-quality
diagnostic-decision-lock
diagnostic-native-wasm-parity
diagnostic-language-tools
diagnostic-web-ide
diagnostic-architecture-guards
```

### CI-Regeln

- Keine Diagnoseprüfung dauerhaft mit `continue-on-error`.
- Reports dürfen bei Fehlern hochgeladen werden.
- Der ursprüngliche Exitcode muss danach erzwungen werden.
- Decision-Lock-Report als Artefakt.
- Quality-Report als Artefakt.
- Native/WASM-Diff als Artefakt.
- Web-IDE-Screenshots/Traces bei E2E-Fehlern.
- Conformance-Baseline darf im P6-PR nicht geändert werden, ausser rein dokumentierter technischer Notwendigkeit; standardmässig Guard-Fehler.

---

## 41. Empfohlene Dateistruktur

### `ilic-fork`

```text
include/ilic/
    Diagnostic.h
    DiagnosticCatalog.h

source/diagnostics/
    DiagnosticBuilder.h
    DiagnosticBuilder.cpp
    DiagnosticCatalog.cpp
    DiagnosticPipeline.h
    DiagnosticPipeline.cpp
    DiagnosticRangeResolver.h
    DiagnosticRangeResolver.cpp
    DiagnosticMessageFormatter.h
    DiagnosticMessageFormatter.cpp
    DiagnosticTextRenderer.h
    DiagnosticTextRenderer.cpp
    ParserDiagnosticTranslator.h
    ParserDiagnosticTranslator.cpp

source/abi/
    DiagnosticJson.h
    DiagnosticJson.cpp

test/diagnostics/
    ...
```

Die tatsächliche Struktur darf angepasst werden.

Die Architektur muss aber klar trennen:

- Produktion;
- Normalisierung;
- Pipeline;
- Rendering;
- Serialisierung;
- Consumer-Projektion.

### `interlis-language-tools`

```text
packages/language-service/src/diagnostics/
    diagnostic-store.ts
    diagnostic-merger.ts
    diagnostic-version-gate.ts
    diagnostic-fingerprint.ts

packages/language-server/src/
    diagnostic-mapping.ts

packages/monaco-adapter/src/
    diagnostic-mapping.ts
```

### `interlis-web-ide`

```text
src/problems/
    problem-model.ts
    problem-store.ts
    problem-renderer.ts
    problem-navigation.ts
```

Die bestehende `WebIdeWorkbench` darf Fassade bleiben, soll Diagnoseverantwortung aber delegieren.

## 42. Umsetzungsphasen

### P6.0 – Inventar und Baseline

- Gitstatus aller Repositories.
- Commits.
- Toolchain.
- bestehende Tests.
- Codeinventar.
- Rangeinventar.
- Kaskadeninventar.
- Consumerinventar.
- Decision-Fingerprint-Baseline.
- Quality-Baseline.
- vollständige Conformance.

### P6.1 – Diagnosekatalog

- Descriptor.
- Registrierung.
- Code-Guard.
- Dokumentation.
- keine Produceränderung ausser mechanischer Registrierung.

### P6.2 – Builder und Range Resolver

- Builder.
- Range-Normalisierung.
- Unicode/CRLF/EOF.
- bestehende Produzenten schrittweise migrieren.

### P6.3 – Parserdiagnosen

- Translator.
- Expected Tokens.
- stabile Codes.
- Strict-/Editor-Trennung.
- Native/WASM-Tests.

### P6.4 – Auflösungsdiagnosen

- Missing Model.
- Duplicate Definitions.
- Unknown Names.
- Related Information.
- keine Regeländerung.

### P6.5 – Semantik und Translation

- bestehende Regeln inventarisieren;
- Codes und Ranges verbessern;
- Konfliktpartner verknüpfen;
- keine Accept-/Reject-Änderung.

### P6.6 – Pipeline

- Validation.
- Deduplizierung.
- Structured Cause.
- Kaskadenunterdrückung.
- Sortierung.
- Quality Metrics.

### P6.7 – CLI, C-ABI und WASM

- Renderer.
- Diagnostic JSON extrahieren.
- TypeScript-Typen.
- Parität.
- Mischversionen.

### P6.8 – Language Tools

- Store.
- Version Gate.
- Merge.
- LSP.
- Monaco.
- Quick Fixes.
- Worker.

### P6.9 – Web-IDE

- Problem-Modell.
- Darstellung.
- Navigation.
- Related Information.
- Last Good.
- E2E.

### P6.10 – Guards, Dokumentation und vollständige Abnahme

- Guards und Canaries.
- Decision Lock.
- Conformance.
- Release/Debug.
- Sanitizer/TSan.
- WASM/npm.
- Language Tools.
- Web-IDE.
- PWA.

---

## 43. Verifikationsbefehle

Der Agent muss reale Projektskripte verwenden. Folgende Befehle sind Richtwerte.

### `ilic-fork` Release

```bash
cmake -S . -B build/p6-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON

cmake --build build/p6-release --parallel
ctest --test-dir build/p6-release --output-on-failure
```

### Debug

```bash
cmake -S . -B build/p6-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON

cmake --build build/p6-debug --parallel
ctest --test-dir build/p6-debug --output-on-failure
```

### Repository deaktiviert

```bash
cmake -S . -B build/p6-no-repository -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF

cmake --build build/p6-no-repository --parallel
ctest --test-dir build/p6-no-repository --output-on-failure
```

### Sanitizer

```bash
cmake -S . -B build/p6-sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON

cmake --build build/p6-sanitizers --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/p6-sanitizers --output-on-failure -j2
```

### TSan

```bash
cmake -S . -B build/p6-tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_THREAD_SANITIZER=ON

cmake --build build/p6-tsan --parallel

TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1 \
ctest --test-dir build/p6-tsan \
  -R "Diagnostic|CompilerConcurrency|CapiConcurrency" \
  --output-on-failure
```

### WASM und npm

```bash
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
node --test test/npm/*.mjs
```

### Language Tools

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

### Web-IDE

```bash
cd ../interlis-web-ide
corepack pnpm install --no-frozen-lockfile --force --update-checksums
corepack pnpm check
corepack pnpm e2e
corepack pnpm build
```

### Abschluss

```bash
git -C ../ilic-fork diff --check
git -C ../interlis-language-tools diff --check
git -C ../interlis-web-ide diff --check

git -C ../ilic-fork status --short
git -C ../interlis-language-tools status --short
git -C ../interlis-web-ide status --short
```

---

## 44. Verbotene Abkürzungen

Nicht zulässig:

- Meldungen nur kosmetisch global ersetzen.
- Diagnosecodes aus Meldungstext ableiten.
- Quick Fixes auf englische Sätze matchen.
- ANTLR-Rohmeldungen unverändert publizieren.
- alle Folgefehler pauschal nach dem ersten Fehler unterdrücken.
- Diagnoseanzahl als alleinige Qualitätsmetrik verwenden.
- Ranges auf Dokumentanfang setzen, wenn die echte Range unbekannt ist.
- unbekannte Range vortäuschen.
- Related Information in Meldungstext einkopieren und strukturiertes Feld leer lassen.
- bestehende False Accepts korrigieren.
- bekannte Internal Errors korrigieren.
- Conformance-Baseline ändern.
- Grammatik oder Semantic Checker fachlich ändern.
- Severity ändern.
- `warningsAsErrors` ändern.
- C-ABI-Felder entfernen oder umbenennen.
- LSP-/Monaco-Verträge brechen.
- Last-Good-Semantik entfernen.
- überholte Diagnosen publizieren.
- Tests löschen oder lockern.
- Live-Internettests einführen.
- fremde Änderungen überschreiben.
- nur Katalog und Interfaces ohne Producer-Migration liefern.
- P6 als abgeschlossen bezeichnen, wenn Decision Lock nicht grün ist.

---

## 45. Umgang mit unklaren Fällen

Wenn eine Meldungsverbesserung eine fachliche Interpretation benötigt:

1. Verhalten nicht ändern.
2. aktuellen Code und aktuelle Meldung dokumentieren.
3. fokussierten Test für den Status quo hinzufügen.
4. Fall als `needs-domain-agreement` markieren.
5. alle unabhängigen Diagnoseverbesserungen fortsetzen.

Der Agent darf nicht wegen eines einzelnen fachlich unklaren Falls die gesamte Phase abbrechen.

### 45.1 Zulässige Statuswerte

```text
clear-diagnostic-improvement
needs-domain-agreement
out-of-scope-false-accept
out-of-scope-internal-error
out-of-scope-semantic-change
blocked-by-missing-range-data
```

Diese Statuswerte gehören in das Inventar oder den Abschlussbericht, nicht zwingend in die öffentliche Compiler-API.

---

## 46. Definition of Done

P6 ist nur abgeschlossen, wenn alle folgenden Aussagen wahr und durch ausgeführte Tests belegt sind:

- Accept-/Reject-Verhalten ist unverändert.
- Alle bekannten Conformance-Outcomes sind unverändert.
- Conformance-Baseline ist unverändert.
- Die bekannten Internal-Error-Fälle bleiben bewusst offen.
- Die bekannten False Accepts bleiben bewusst offen.
- Öffentliche Diagnosen besitzen registrierte stabile Codes.
- Codes sind eindeutig.
- Parserdiagnosen publizieren keine ANTLR-internen Tokennamen.
- Primäre Ranges sind für geschützte Fixtures korrekt.
- Unicode-, CRLF- und EOF-Ranges sind getestet.
- Konfliktdiagnosen besitzen Related Information.
- Exakte Duplikate werden entfernt.
- Kaskadenunterdrückung ist strukturell und getestet.
- Unabhängige Fehler bleiben sichtbar.
- Diagnose-Reihenfolge ist deterministisch.
- `warningsAsErrors` bleibt kompatibel.
- CLI-Rendering und Datenmodell sind getrennt.
- C-ABI-JSON bleibt kompatibel.
- Native/WASM-Diagnosen sind strukturell gleich.
- Language Service verwirft stale Diagnosen.
- Live- und Saved-Diagnosen erscheinen nicht doppelt.
- Quick Fixes verwenden Codes, nicht Meldungstexte.
- Node-LSP mappt Code, Source, Range und Related Information.
- Monaco mappt dieselben Daten.
- Web-IDE Problems zeigt Code und korrekte Navigation.
- Related Information ist in der Web-IDE navigierbar oder bewusst dokumentiert, falls UI-seitig nicht umgesetzt.
- Last-Good-Diagramm bleibt erhalten.
- Problems-Zähler bleibt korrekt.
- Release ist grün.
- Debug ist grün.
- Repository-disabled-Build ist grün.
- ASan/UBSan/LSan sind grün oder ein echter Plattformblocker ist dokumentiert.
- TSan ist grün oder ein echter Plattformblocker ist dokumentiert.
- WASM/npm ist grün.
- Language Tools sind grün.
- Package-Verifikation ist grün.
- Web-IDE-Check ist grün.
- relevante E2E-Tests sind grün.
- PWA-/Browserbuild ist grün.
- Architecture Guards und negative Canaries sind grün.
- Dokumentation ist aktualisiert.
- keine fremden Änderungen wurden beschädigt.

---

## 47. Abschlussbericht

Der Coding-Agent muss einen Abschlussbericht mit exakt folgenden Abschnitten liefern:

1. **Ausgangszustand**
2. **Repositorypfade**
3. **Ausgangs-Commits**
4. **Fremde Änderungen**
5. **Diagnoseinventar**
6. **Codeinventar**
7. **Rangeinventar**
8. **Kaskadeninventar**
9. **Consumerinventar**
10. **Decision-Lock-Baseline**
11. **Quality-Baseline**
12. **Diagnosekatalog**
13. **DiagnosticBuilder**
14. **DiagnosticRangeResolver**
15. **ParserDiagnosticTranslator**
16. **Lexerdiagnosen**
17. **Syntaxdiagnosen**
18. **Auflösungsdiagnosen**
19. **Semantische Diagnosen**
20. **Translation-Diagnosen**
21. **Repositorydiagnosen**
22. **ABI- und Requestdiagnosen**
23. **DiagnosticPipeline**
24. **Deduplizierung**
25. **Kaskadenunterdrückung**
26. **Deterministische Reihenfolge**
27. **Meldungstemplates**
28. **warningsAsErrors**
29. **CLI-Darstellung**
30. **C-ABI-JSON**
31. **WASM-Vertrag**
32. **Language-Service-DiagnosticStore**
33. **Versionsgate**
34. **Live-/Saved-/Semantic-Merge**
35. **Quick Fixes**
36. **Node-LSP**
37. **Monaco**
38. **Web-IDE Problems**
39. **Primärnavigation**
40. **Related-Information-Navigation**
41. **Last-Good-Verhalten**
42. **Quality Report**
43. **Decision-Lock-Ergebnis**
44. **Conformance-Ergebnis**
45. **Native-/WASM-Parität**
46. **Neue und geänderte Dateien**
47. **Neue Tests**
48. **Architecture Guards**
49. **Release-Ergebnis**
50. **Debug-Ergebnis**
51. **Repository-disabled-Ergebnis**
52. **ASan-/UBSan-/LSan-Ergebnis**
53. **TSan-Ergebnis**
54. **WASM-/npm-Ergebnis**
55. **Language-Tools-Ergebnis**
56. **Package-Verifikation**
57. **Web-IDE-Ergebnis**
58. **Browser-/PWA-Ergebnis**
59. **Ausgeführte Befehle**
60. **Bewusst unveränderte Conformancefälle**
61. **Abweichungen von der Spezifikation**
62. **Externe Blocker**
63. **Verbleibende Risiken**
64. **Abschliessender Gitstatus aller drei Repositories**

Für jeden Lauf nennen:

- Repository;
- exakter Befehl;
- Exitcode;
- Testzahl;
- Fehlerzahl;
- Buildtyp;
- Umgebung;
- Reportpfad;
- Conformance-Suite-Commit;
- Baseline-Datei;
- verwendete npm-Tarballs;
- Browserprojekt;
- relevante Qualitätsmetriken.

Keine Testzahl schätzen.

Keinen Lauf behaupten, der nicht ausgeführt wurde.

---

## 48. Explizite Erfolgsaussage

P6 darf nur dann als abgeschlossen bezeichnet werden, wenn folgende Aussage wahr und belegt ist:

> Der INTERLIS-Compiler und seine Folgeprojekte liefern stabile, präzise, deterministische und plattformübergreifend konsistente Diagnosen. Jede öffentliche Diagnose besitzt einen registrierten Code, einen kontrollierten Meldungstext und nach Möglichkeit eine genaue primäre Range. Konflikte verwenden Related Information. Exakte Duplikate und strukturell nachgewiesene Folgekaskaden werden kontrolliert reduziert, ohne unabhängige Fehler zu verbergen. C++ API, C-ABI, WASM, Language Service, Node-LSP, Monaco und Web-IDE transportieren denselben Diagnosevertrag. Quick Fixes und UI-Logik hängen nicht von Meldungstexten ab. Save-driven Semantik, stale handling, Problems und Last-Good-Diagramme bleiben erhalten. Die fachlichen Accept-/Reject-Entscheidungen, die bekannten Internal Errors, die bekannten False Accepts und die Conformance-Baseline bleiben bewusst unverändert.

---

## Anhang A – Startbefehle für den Coding-Agenten

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
2. alle genannten Diagnosepfade inventarisieren;
3. unveränderte Baseline ausführen;
4. Decision Lock erzeugen;
5. schrittweise implementieren;
6. nach jeder Phase fokussierte Tests ausführen;
7. nach jeder Producer-Migrationsgruppe Decision Lock prüfen;
8. final alle drei Repositories vollständig abnehmen.

---

## Anhang B – Reviewcheckliste für einen neuen Diagnosecode

- [ ] Code im Katalog registriert.
- [ ] Codeformat korrekt.
- [ ] Bedeutung eindeutig.
- [ ] Default-Severity korrekt.
- [ ] Phase korrekt.
- [ ] Primäre Range definiert.
- [ ] Related Information geprüft.
- [ ] Notes geprüft.
- [ ] Meldungstemplate kontrolliert.
- [ ] Keine internen Typnamen.
- [ ] Keine Secrets.
- [ ] Keine Message-Matching-Consumer.
- [ ] Native Test.
- [ ] C-ABI/WASM-Test.
- [ ] Language-Service-Test, falls relevant.
- [ ] LSP-/Monaco-Test, falls relevant.
- [ ] Web-IDE-Test, falls relevant.
- [ ] Decision Lock grün.
- [ ] Conformance-Baseline unverändert.
- [ ] Dokumentation ergänzt.

---

## Anhang C – Reviewcheckliste für Kaskadenunterdrückung

- [ ] Primärursache strukturell identifiziert.
- [ ] Folgefehler referenziert Ursache-ID.
- [ ] Keine Textheuristik.
- [ ] Unabhängige Fehler bleiben sichtbar.
- [ ] Compile-Outcome unverändert.
- [ ] Missing Models unverändert.
- [ ] Native Test.
- [ ] WASM-Parität.
- [ ] Language-Service-Merge geprüft.
- [ ] Web-IDE Problems geprüft.
- [ ] Quality Report weist Suppression aus.
- [ ] Debugtrace erklärt Entscheidung.

---

## Anhang D – Reviewcheckliste für Ranges

- [ ] Autoritative URI.
- [ ] Start kleiner oder gleich Ende.
- [ ] Byteoffset innerhalb der Source.
- [ ] UTF-16-Position korrekt.
- [ ] CRLF getestet.
- [ ] Unicode getestet.
- [ ] EOF getestet.
- [ ] Zero-width bewusst.
- [ ] Primäre Range möglichst klein.
- [ ] Related Range korrekt.
- [ ] Native/WASM gleich.
- [ ] LSP/Monaco gleich.
- [ ] Web-IDE-Navigation getestet.

---

## Anhang E – Nichtziele

Diese Phase implementiert ausdrücklich nicht:

- neue INTERLIS-Sprachregeln;
- Korrektur von False Accepts;
- Korrektur der beiden bekannten Internal Errors;
- allgemeine Conformance Closure;
- automatische Rechtschreib- oder Ähnlichkeitsvorschläge;
- vollständige Internationalisierung der Meldungen;
- Telemetrie an externe Dienste;
- serverseitige Diagnoseanalyse;
- persistente Diagnosehistorie;
- neue Diagrammsemantik;
- neue Repositoryauflösung;
- neue Compileroptimierungen;
- Änderung der Inkrementalitätsregeln;
- neue Quick Fixes, ausser sie sind für die codebasierte Migration bestehender Fixes notwendig.

---

## Anhang F – Agentenarbeitsregel

Der Agent soll autonom arbeiten, aber keine fachlich unklare Entscheidung erfinden.

Bei klarer Diagnoseverbesserung:

```text
implementieren
→ fokussiert testen
→ Decision Lock prüfen
→ Consumer testen
```

Bei fachlich unklarer Semantik:

```text
Status quo sperren
→ needs-domain-agreement dokumentieren
→ nicht verändern
→ mit unabhängigen Arbeiten fortfahren
```

Bei technischer Regression:

```text
kleinsten Fall reproduzieren
→ Ursache lokalisieren
→ Diagnosearchitektur korrigieren
→ vollständige relevante Suite wiederholen
```

