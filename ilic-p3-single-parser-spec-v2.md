# P3-Spezifikation: Ein autoritativer Parser, toleranter nativer Editor-Snapshot und Native/WASM-Parität

**Zielrepository:** `edigonzales/ilic-fork`  
**Beobachteter Referenzstand der Bestandsaufnahme:** `5ed35f8129fc6f984e2ca56bac8fb9d42a9418cf`  
**Zielphase:** P3  
**Voraussetzungen:** P0 abgeschlossen, externe P1-Conformance-Suite integriert, P2-CompilerContext/Ownership/Reentranz abgeschlossen  
**Sprache der Kernimplementierung:** C++17  
**Zielplattformen:** Linux, macOS, Windows, WebAssembly/Emscripten, Node.js, Browser und Web Worker  
**Adressat:** selbständig arbeitender LLM-Coding-Agent  
**Status:** normative Implementierungsspezifikation  
**Revision:** 2, 31. Juli 2026

**Änderung gegenüber Revision 1:** verbindlicher Consumer-Kompatibilitäts- und Migrationsvertrag für `@ilic/compiler-wasm`, Node-LSP, Language Service, Worker, Web-IDE/PWA, TypeScript-Fixtures und gemischte Wrapper-/WASM-Versionen.

---

## 1. Auftrag in einem Satz

Beseitige die zweite, in JavaScript implementierte INTERLIS-Parserlogik des WASM-Pakets, mache den C++-/ANTLR-Parser zur einzigen autoritativen Quelle für Syntax- und Editorinformationen, erweitere ihn zu einem robusten fehlertoleranten Editor-Snapshot, exponiere diesen Snapshot über C++-API, C-ABI, WASM und Worker, erhalte die bestehende öffentliche Consumer-API für Node-LSP, Language Service und Web-IDE und beweise durch exakte Native/WASM-Parität, Consumer-Kompatibilitätstests, Corpus-Tests sowie die vollständige externe `interlis-compiler-testsuite`, dass weder Compilersemantik noch Editorverhalten regressieren.

---

## 2. Normative Sprache

Die Schlüsselwörter **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT** und **KANN** sind normativ.

- **MUSS / DARF NICHT** bezeichnet zwingende Abnahmekriterien.
- **SOLL / SOLL NICHT** darf nur mit einer konkreten, technisch belegten Begründung abweichen.
- **KANN** bezeichnet eine zulässige Variante.
- Beispielcode ist normativ bezüglich Verantwortlichkeiten, Datenfluss, Lebensdauer und Fehlersemantik.
- Konkrete Bezeichner dürfen nur abweichen, wenn die Alternative mindestens gleich klar ist.
- Jede Abweichung von dieser Spezifikation MUSS im Abschlussbericht mit Datei, Symbol, Grund und Auswirkung dokumentiert werden.
- Eine reine Analyse, ein Architekturpapier, ein Kompatibilitätswrapper über den bestehenden JavaScript-Parser oder ein unvollständiges Gerüst erfüllen P3 nicht.

---

## 3. Projektkontext und Reihenfolge

### 3.1 P0

Der Agent MUSS verifizieren, dass die P0-Testhärtung weiterhin vorhanden ist:

- `ilic::test-support`;
- Release-Canary;
- keine Standard-`assert`-Abhängigkeit in Tests;
- Debug-CI;
- ASan-/UBSan-CI;
- aktive Leak Detection;
- bestehende CTest-Tests.

P3 DARF diese Infrastruktur nicht entfernen oder abschwächen.

### 3.2 Reale P1-Absicherung

Die breite Compiler-Conformance-Suite liegt im separaten Repository:

```text
https://codeberg.org/edigonzales/interlis-compiler-testsuite
```

Der lokale Standardpfad ist:

```text
../interlis-compiler-testsuite
```

Der Agent MUSS aus der aktuellen `.github/workflows/ci.yml` ermitteln:

- den gepinnten Suite-Commit;
- den Suite-Manifestpfad;
- den tatsächlichen Baselinepfad;
- den exakten `InterlisConformance.java run`-Aufruf;
- die CMake-Optionen des Conformance-Builds;
- den Reportpfad;
- die Exitcode-Erzwingung.

Im beobachteten Referenzstand lauten die wesentlichen Werte:

```text
Suite-Commit:   88576ffc4cd491dd72d2375eb897ea9f3f55d2c3
Suite-Manifest: conformance-suite/suite/test-cases.json
Runner:         conformance-suite/InterlisConformance.java
Baseline:       conformance/known-failures.json
```

Diese Werte sind eine Bestandsaufnahme. Normativ ist immer der aktuelle Arbeitsbranch.

Die externe Suite MUSS vor und nach P3 vollständig durchlaufen. Die Baseline DARF nicht erweitert werden, um eine P3-Regression zu kaschieren.

### 3.3 P2

P3 setzt die P2-Architektur voraus:

- expliziter pro Lauf erzeugter `CompilerContext`;
- eindeutige Ownership;
- kein globaler Logger;
- kein globaler Metamodellzustand;
- kein globaler SourceManager;
- keine globale Compiler-Mutex;
- voneinander isolierte `CompilerSession`-Instanzen;
- lebensdauersichere öffentliche Snapshots.

Der Agent MUSS den tatsächlichen P2-Zustand des Branches lesen. Wenn P2-Bezeichner von den Beispielen in diesem Dokument abweichen, ist die bestehende saubere P2-Architektur zu verwenden.

P3 DARF keine neuen Globals, Singletons, Service-Locator oder `thread_local`-Aktivkontexte einführen.

---

## 4. Ausgangslage

### 4.1 Autoritativer nativer Syntaxpfad

Der C++-Kern besitzt einen `SyntaxSnapshot` mit:

- Token;
- Parse-Tree-Knoten;
- Editor-Kontexten;
- Imports;
- Import-Referenzen;
- Syntaxdiagnosen;
- URI;
- Dokumentversion;
- INTERLIS-Version.

Der beobachtete native Pfad liegt in:

```text
include/ilic/Syntax.h
source/core/Syntax.cpp
```

`CompilerSession::parse()` verwendet diesen Pfad, und `source/abi/Capi.cpp` serialisiert ihn über `ilic_parse`.

### 4.2 Zweiter Parser im WASM-Paket

`packages/compiler-wasm/index.js` enthält im beobachteten Referenzstand zwei unterschiedliche Editorpfade:

1. `editorProjection(syntax)` projiziert einen nativen `SyntaxSnapshot`.
2. `editorTokens()` und `editorTextProjection()` implementieren einen eigenen linearen INTERLIS-Tokenizer und eine eigene Struktur-, Referenz- und Diagnoseerkennung in JavaScript.

`CompilerSession.editorSnapshot(uri)` verwendet aktuell für jede mit `putSource()` registrierte Quelle den JavaScript-Textparser:

```js
const source = this.#sources.get(uri);
return source
  ? editorTextProjection(uri, source.text, source.version)
  : editorProjection(this.parse(uri));
```

Damit ist der native Pfad faktisch nur ein Fallback.

### 4.3 Doppelte Source-Ownership im JavaScript-Wrapper

Der JavaScript-Wrapper hält jede Quelle zusätzlich zum nativen/WASM-`SourceManager` in:

```js
#sources = new Map();
```

Die Source wird somit:

- als JavaScript-String oder `Uint8Array` empfangen;
- nach WASM kopiert;
- zusätzlich als JavaScript-Text gespeichert;
- für `editorTextProjection()` erneut tokenisiert.

Dies verursacht unnötige Speicherduplikation und zwei semantisch unterschiedliche Parserpfade.

### 4.4 Konkrete Driftgefahren

Der JavaScript-Pfad kennt nur eine selbst definierte Teilmenge der Sprache und besitzt eigene Regeln für:

- Kommentare;
- Strings;
- Namen;
- Zahlen;
- Deklarationskeywords;
- Container;
- `END`;
- Imports;
- `EXTENDS`;
- `REFERENCE`;
- `LIST`;
- `BAG`;
- Typreferenzen;
- Duplikate;
- Endnamenfehler;
- Versionsdetektion;
- UTF-8-Byteoffsets;
- UTF-16-Spalten.

Jede Änderung an den ANTLR-Grammatiken, Tokenarten, INTERLIS-Versionen oder Editoranforderungen kann zu Drift führen.

### 4.5 Ziel des Refactorings

Nach P3 existiert genau eine autoritative Sprachinterpretation:

```text
INTERLIS-Quelltext
        |
        v
C++ SourceManager
        |
        v
ANTLR Lexer + Parser im C++-Kern
        |
        +--> SyntaxSnapshot
        |
        +--> EditorSnapshot
        |
        +--> Compiler / SemanticSnapshot
        |
        v
C-ABI JSON
        |
        v
WASM-JavaScript-Wrapper
```

JavaScript DARF den INTERLIS-Text nicht mehr selbst tokenisieren oder syntaktisch interpretieren.

---

## 5. P3-Ziele

P3 MUSS vollständig erreichen:

1. Der C++-/ANTLR-Pfad ist die einzige autoritative Parserimplementierung.
2. `CompilerSession` besitzt eine native Methode `editorSnapshot()`.
3. Der Editor-Snapshot funktioniert für gültige und unvollständige Dokumente.
4. Der Editor-Snapshot enthält mindestens dieselben fachlichen Informationen wie der heutige JavaScript-Textpfad.
5. `editorTokens()` wird entfernt.
6. `editorTextProjection()` wird entfernt.
7. Die JavaScript-Source-Schattenkopie wird entfernt, soweit sie nur dem zweiten Parser dient.
8. `packages/compiler-wasm/index.js` implementiert keine INTERLIS-Lexik oder -Grammatik mehr.
9. Die C-ABI exponiert den nativen Editor-Snapshot.
10. Emscripten exportiert die neue C-ABI-Funktion.
11. Der JavaScript-Wrapper ruft standardmässig ausschliesslich den nativen/WASM-Editorpfad auf.
12. Ein optionaler Legacy-Fallback darf nur explizit aktiviert werden und darf nur einen nativen `SyntaxSnapshot` projizieren.
13. Der Legacy-Fallback darf keinen Texttokenizer enthalten.
14. Native und WASM liefern für dieselbe Source bytegleich normalisierbare Syntax- und Editor-Snapshots.
15. Unicode-, CRLF-, Fehler- und Recovery-Ranges sind identisch.
16. Die externe `interlis-compiler-testsuite` zeigt keine neue Regression.
17. Alle P0-, P1- und P2-Tests bleiben grün.
18. Das Worker-Protokoll verwendet denselben nativen Snapshot.
19. TypeScript-Typen und Dokumentation werden aktualisiert.
20. Ein Architecture Guard verhindert die Wiedereinführung eines JavaScript-Parsers.

---

## 6. Nichtziele

P3 umfasst ausdrücklich nicht:

- inkrementelles Parsing;
- persistente Parse-Tree-Caches;
- differenzielles Relexing;
- inkrementelle Kompilation;
- LSP-Serverimplementierung;
- Repository-Downloadlogik im WASM-Modul;
- HTTP- oder Dateisystemzugriffe aus WASM;
- kooperative Compiler-Cancellation;
- vollständige semantische Fehlerdiagnose bei jedem Tastendruck;
- Ersetzung der ANTLR-Grammatiken;
- Wechsel auf Tree-sitter;
- Wechsel auf eine andere Parserbibliothek;
- Neuschreiben des Compilers;
- P4-Repositorymodularisierung;
- P5-inkrementelle Kompilation.

Parser- und Snapshotarchitektur MUSS jedoch so gestaltet sein, dass P5 später ohne erneuten JavaScript-Parser möglich ist.

---

## 7. Zentrale Invarianten

### 7.1 Single Source of Truth

Für Syntax und Editorstruktur gilt:

```text
ANTLR-Grammatik + C++-Snapshot-Code = einzige autoritative Sprachinterpretation
```

JavaScript darf nur:

- C-ABI-Aufrufe durchführen;
- Speicher kopieren;
- JSON parsen;
- Resultate weiterreichen;
- einen expliziten Legacy-Syntax-Snapshot rein strukturell projizieren.

### 7.2 Keine zweite Lexik

Produktiver JavaScript-Code DARF NICHT:

- Zeichen für Zeichen durch INTERLIS-Quelltext laufen;
- eigene Namensregeln definieren;
- Strings oder Kommentare lexen;
- Keywords klassifizieren;
- Versionsheader erkennen;
- `END`-Paare verwalten;
- Scope-Stacks aus Text aufbauen;
- Referenzen aus Textmustern ableiten;
- Live-Diagnosen aus Textmustern erzeugen.

### 7.3 Ein Parsevorgang pro Snapshotaufruf

Ein einzelner Aufruf von:

```cpp
CompilerSession::editorSnapshot(uri)
```

MUSS den Quelltext höchstens einmal lexen und parsen.

Es ist zulässig, dass ein später separat aufgerufener `parse()`- oder `compile()`-Aufruf erneut parst. Inkrementelles Caching ist P5.

### 7.4 Resultate sind eigenständig

`SyntaxSnapshot` und `EditorSnapshot` dürfen nach Methodenrückkehr keine Referenzen auf:

- ANTLR-Input;
- Lexer;
- TokenStream;
- Parser;
- ParseTree;
- `CompilerContext`;
- temporäre Builder;
- JavaScript-Source-Schattenkopien

enthalten.

### 7.5 Parität

Bei identischen UTF-8-Bytes, URI und Dokumentversion müssen Native und WASM fachlich identische Resultate liefern.

### 7.6 Fehlertoleranz

Ein Syntaxfehler darf nicht automatisch zu einem leeren Editor-Snapshot führen.

Soweit ANTLR einen partiellen Parse-Tree erzeugt, müssen gültig erkannte Bereiche weiterhin als Deklarationen, Imports, Referenzen und Kontexte verfügbar sein.

### 7.7 Keine Conformance-Baselineausweitung

Neue P3-Regressionsfälle dürfen nicht in `conformance/known-failures.json` oder `conformance/known-issues.json` aufgenommen werden.

---

## 8. Zielarchitektur

### 8.1 Öffentliche Ebene

```text
include/ilic/Compiler.h
include/ilic/Syntax.h
include/ilic/Editor.h
include/ilic/capi.h
```

Öffentliche C++-API:

```cpp
namespace ilic {

class CompilerSession {
public:
    SyntaxSnapshot parse(const std::string& uri);
    EditorSnapshot editorSnapshot(const std::string& uri);
};

}
```

### 8.2 Interne Snapshot-Pipeline

Empfohlene Struktur:

```cpp
namespace ilic::detail {

enum class SnapshotProducts : unsigned {
    Syntax = 1u,
    Editor = 2u
};

struct SnapshotBundle final {
    SyntaxSnapshot syntax;
    std::optional<EditorSnapshot> editor;
};

class SnapshotPipeline final {
public:
    explicit SnapshotPipeline(const SourceManager& sources);

    SyntaxSnapshot syntax(const std::string& uri) const;
    EditorSnapshot editor(const std::string& uri) const;
    SnapshotBundle build(
        const std::string& uri,
        SnapshotProducts products
    ) const;

private:
    SnapshotBundle buildIli1(
        const SourceBuffer& source,
        SnapshotProducts products
    ) const;

    SnapshotBundle buildIli2(
        const SourceBuffer& source,
        SnapshotProducts products
    ) const;

    const SourceManager& sources_;
};

}
```

Der konkrete Typ für Flagkombinationen kann abweichen. Normativ ist:

- ein gemeinsamer Einstieg;
- eine Parserauswahl;
- maximal ein Lexer-/Parserlauf pro `build()`-Aufruf;
- Syntax- und Editorprodukt werden aus demselben TokenStream und Parse-Tree erzeugt.

### 8.3 Interne Lebensdauer

Für einen Editoraufruf:

```text
SourceBuffer
  -> ANTLRInputStream
  -> Lexer
  -> CommonTokenStream
  -> Parser
  -> RootContext
  -> SyntaxSnapshotBuilder
  -> EditorSnapshotBuilder
  -> eigenständige Value Objects
  -> Zerstörung sämtlicher ANTLR-Objekte
```

### 8.4 Keine ANTLR-Typen in öffentlicher API

Öffentliche Header dürfen keine generierten Parserheader oder ANTLR-Klassen exponieren.

ANTLR bleibt Implementierungsdetail in `source/core`.

---

## 9. Öffentliche Editor-Datentypen

Neue Datei:

```text
include/ilic/Editor.h
```

Empfohlene Definition:

```cpp
#pragma once

#include "Diagnostic.h"
#include "Syntax.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ilic {

enum class EditorSymbolKind {
    Model,
    Topic,
    Class,
    Structure,
    Association,
    View,
    Graphic,
    Domain,
    Unit,
    Attribute
};

enum class EditorReferenceKind {
    Extends,
    Type,
    Collection,
    Reference,
    Unit
};

struct EditorDeclaration {
    std::string id;
    std::string name;
    std::string qualifiedName;
    EditorSymbolKind kind = EditorSymbolKind::Attribute;
    std::string containerId;
    bool hasContainer = false;
    SourceRange range;
    SourceRange selectionRange;
    SourceRange endRange;
};

struct EditorReference {
    std::string text;
    EditorReferenceKind kind = EditorReferenceKind::Type;
    std::string sourceId;
    bool hasSource = false;
    SourceRange range;
};

struct EditorSnapshot {
    bool success = false;
    bool recovered = false;
    bool complete = false;
    std::string uri;
    std::uint64_t documentVersion = 0;
    std::string iliVersion;
    std::vector<EditorDeclaration> declarations;
    std::vector<EditorReference> references;
    std::vector<SyntaxImportReference> imports;
    std::vector<SyntaxContext> contexts;
    std::vector<Diagnostic> diagnostics;
};

}
```

### 9.1 Kompatibilität zu TypeScript

Die JSON-Form MUSS weiterhin die bestehenden Stringwerte verwenden:

```text
model
topic
class
structure
association
view
graphic
domain
unit
attribute
```

Referenzarten:

```text
extends
type
collection
reference
unit
```

### 9.2 `containerId`

C++ darf intern `hasContainer` verwenden. JSON MUSS wie bisher liefern:

```json
"containerId": null
```

oder eine String-ID.

### 9.3 `endRange`

Wenn kein expliziter Abschlussname vorhanden ist:

```json
"endRange": null
```

Ein künstlich durch ANTLR eingefügtes Token DARF nicht als echter Endname ausgegeben werden.

### 9.4 `success`

`success` MUSS genau bedeuten:

> Der Quelltext enthält keine Error-Severity-Syntax- oder Editorstrukturdiagnose.

Ein unvollständiges Dokument kann daher:

- einen umfangreichen nutzbaren Snapshot liefern;
- `success == false` besitzen.

### 9.5 `recovered`

`recovered` MUSS `true` sein, wenn mindestens eine der folgenden Bedingungen gilt:

- Lexer- oder Parserdiagnose vorhanden;
- ANTLR Error Recovery wurde verwendet;
- ein Container besitzt keinen expliziten Abschluss;
- ein Editorstrukturfehler wurde erkannt.

### 9.6 `complete`

`complete` MUSS nur `true` sein, wenn:

- der Parser bis EOF gelaufen ist;
- keine Error-Severity-Syntaxdiagnose besteht;
- jeder erkannte Container einen expliziten Abschluss besitzt;
- keine künstlich eingefügten Tokens für relevante Abschlussstellen verwendet wurden.

### 9.7 Schemaerweiterung

Die Felder `recovered` und `complete` sind additiv.

Der bestehende `schemaVersion`-Wert DARF bei einer rein additiven Änderung bei `1` bleiben, sofern:

- keine vorhandenen Felder entfernt werden;
- vorhandene Feldtypen unverändert bleiben;
- bestehende C-ABI-Funktionen unverändert bleiben.

Eine ABI- oder Schemaversion darf nur mit konkreter Begründung erhöht werden.

### 9.8 Verbindlicher Consumer-Kompatibilitätsvertrag

P3 ist grundsätzlich ein Austausch der internen Implementierung hinter einer bereits bestehenden öffentlichen Editor-API.

Folgende bestehende Aufrufoberfläche MUSS ohne Änderung des Consumer-Codes weiter funktionieren:

```ts
const compiler = await createCompiler(options);
const session = compiler.createSession();

session.putSource(uri, source, documentVersion);
const snapshot = session.editorSnapshot(uri);

session.removeSource(uri);
session.dispose();
```

Insbesondere bleiben unverändert:

- der Exportname `createCompiler`;
- `Compiler.createSession()`;
- `CompilerSession.putSource()`;
- `CompilerSession.putWorkspace()`;
- `CompilerSession.removeSource()`;
- `CompilerSession.compile()`;
- `CompilerSession.parse()`;
- `CompilerSession.editorSnapshot()`;
- `CompilerSession.analyze()`;
- `CompilerSession.compileAndAnalyze()`;
- `CompilerSession.format()`;
- `CompilerSession.dispose()`;
- das Worker-Kommando `editorSnapshot`;
- die synchrone Rückgabe von `session.editorSnapshot(uri)` innerhalb der bereits initialisierten Session;
- die URI-basierte Auswahl der zuvor mit `putSource()` registrierten Quelle;
- die Dokumentversion im zurückgegebenen Snapshot.

P3 DARF von Anwendern nicht verlangen:

- die private Emscripten-Funktion `_ilic_editor_snapshot` direkt aufzurufen;
- C-ABI-Result-Handles selbst zu verwalten;
- Quelltext zusätzlich zum `putSource()`-Aufruf an `editorSnapshot()` zu übergeben;
- einen neuen Worker-Methodennamen zu verwenden;
- eine zweite Parserbibliothek zu installieren;
- EditorSnapshot-Daten aus einem `SyntaxSnapshot` selbst zu projizieren;
- Node-LSP- oder Web-IDE-Anwendungslogik auf eine neue Compilerabstraktion umzuschreiben.

### 9.9 Unveränderte Signatur und Synchronitätssemantik

Die öffentliche TypeScript-Signatur MUSS bleiben:

```ts
editorSnapshot(uri: string): EditorSnapshot;
```

Nicht zulässig sind im Rahmen von P3:

```ts
editorSnapshot(uri: string): Promise<EditorSnapshot>;
editorSnapshot(uri: string, source: string): EditorSnapshot;
editorSnapshot(request: EditorSnapshotRequest): EditorSnapshot;
```

Eine spätere asynchrone High-Level-API darf separat ergänzt werden, aber P3 darf die bestehende synchrone Methode nicht ersetzen.

Für Browser-Anwendungen SOLL weiterhin der Worker empfohlen werden, damit der synchrone WASM-Aufruf nicht auf dem UI-Thread ausgeführt wird. Diese Empfehlung ist keine API-Änderung.

### 9.10 Rückwärtskompatible TypeScript-Erweiterung

Die neue Laufzeit MUSS in jedem nativen EditorSnapshot stets liefern:

```json
{
  "recovered": false,
  "complete": true
}
```

oder die fachlich korrekten anderen Booleanwerte.

Damit bestehende Consumer-Mocks, Test-Fixtures und manuell konstruierte `EditorSnapshot`-Objekte nicht sofort brechen, MUSS die erste mit P3 veröffentlichte TypeScript-Definition die Felder additiv und zunächst optional deklarieren:

```ts
export interface EditorSnapshot {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "editor";
  success: boolean;
  uri: string;
  documentVersion: number;
  iliVersion: "1.0" | "2.3" | "2.4" | "unknown";

  /** Present in every P3 runtime result. Optional only for source compatibility
   *  with pre-P3 mocks, fixtures and serialized snapshots. */
  recovered?: boolean;

  /** Present in every P3 runtime result. Optional only for source compatibility
   *  with pre-P3 mocks, fixtures and serialized snapshots. */
  complete?: boolean;

  declarations: EditorDeclaration[];
  references: EditorReference[];
  imports: SyntaxImportReference[];
  contexts: SyntaxContext[];
  diagnostics: Diagnostic[];
}
```

Diese Optionalität ist ausschliesslich eine TypeScript-Quellkompatibilitätsmassnahme.

Sie DARF NICHT dazu führen, dass die P3-Laufzeit die Felder auslässt.

Die Dokumentation MUSS für Consumer folgende Normalisierung angeben, wenn sie gleichzeitig alte und neue serialisierte Snapshots einlesen:

```ts
const recovered = snapshot.recovered ?? !snapshot.success;
const complete = snapshot.complete ?? snapshot.success;
```

Diese Fallbackwerte sind nur für alte persistierte Daten beziehungsweise Testdoubles gedacht. Ein P3-Runtime-Test MUSS verlangen, dass beide Properties als eigene Boolean-Properties vorhanden sind.

Eine spätere Major-Version KANN die TypeScript-Felder verpflichtend machen. Das ist nicht Bestandteil von P3.

### 9.11 Bedeutung von `success` für Editor-Consumer

P3 MUSS die Bedeutung von `success` ausdrücklich dokumentieren und testen:

- `success == true` bedeutet: keine Error-Severity-Syntax- oder Editorstrukturdiagnose;
- `success == false` bedeutet nicht: der Snapshot ist leer oder unbrauchbar;
- `declarations`, `references`, `imports`, `contexts` und `diagnostics` bleiben bei Recovery nutzbar;
- ein LSP oder eine IDE SOLL partielle Informationen auch dann verwenden, wenn `success == false` ist.

Consumer-Code nach diesem Muster ist mit dem P3-Vertrag nicht korrekt:

```ts
const snapshot = session.editorSnapshot(uri);
if (!snapshot.success) return [];
return snapshot.declarations;
```

Der dokumentierte Standard ist:

```ts
const snapshot = session.editorSnapshot(uri);
publishDiagnostics(snapshot.diagnostics);
updateDocumentSymbols(snapshot.declarations);
updateReferences(snapshot.references);
```

P3 MUSS jedoch bestehende Consumer nicht automatisch ändern. Stattdessen MUSS das `ilic-fork`-Repository einen Kompatibilitätstest enthalten, der einen fehlerhaften, aber partiell auswertbaren Snapshot erzeugt und sicherstellt, dass die Daten vorhanden sind.

### 9.12 Stabilität der bestehenden JSON-Felder

Folgende bestehende Felder des EditorSnapshot dürfen in P3 nicht entfernt, umbenannt oder in ihrem Basistyp geändert werden:

```text
schemaVersion
abiVersion
compilerVersion
kind
success
uri
documentVersion
iliVersion
declarations
references
imports
contexts
diagnostics
```

Für `EditorDeclaration` bleiben erhalten:

```text
id
name
qualifiedName
kind
containerId
range
selectionRange
endRange
```

Für `EditorReference` bleiben erhalten:

```text
text
kind
sourceId
range
```

Für Imports bleiben erhalten:

```text
model
unqualified
range
```

Folgende Werte bleiben stabil:

```text
kind == "editor"
schemaVersion == 1
```

Bestehende Enum-Stringwerte dürfen nicht geändert werden.

Eine Präzisierung von Ranges, zusätzliche korrekt erkannte Deklarationen, zusätzliche korrekt erkannte Referenzen und zusätzliche native Diagnosen sind fachlich zulässige Ergebnisverbesserungen. Sie müssen jedoch über Golden-Diff-Tests sichtbar gemacht und im Abschlussbericht beschrieben werden.

### 9.13 Stabilität von IDs und Reihenfolge

Viele LSP- und IDE-Consumer verwenden Snapshot-IDs und Arrayreihenfolgen für Caches, Tree-Diffs und UI-Selektion.

Darum MUSS P3 folgende Stabilität anstreben:

- gleiche gültige Source plus gleiche Dokumentversion liefert deterministisch gleiche IDs;
- ein Wechsel vom JS-Parser zum nativen Parser darf IDs nicht willkürlich bei jedem Lauf verändern;
- Deklarationen stehen in Source-Reihenfolge;
- Referenzen stehen in Source-Reihenfolge;
- Imports stehen in Source-Reihenfolge;
- Contexts stehen in Source-Reihenfolge;
- Diagnosen sind deterministisch sortiert;
- `containerId` verweist nur auf eine im selben Snapshot vorhandene Deklaration oder ist `null`;
- `sourceId` verweist nur auf eine im selben Snapshot vorhandene Deklaration oder ist `null`.

Wenn die bisherige JS-ID exakt übernommen werden kann, SOLL das bestehende Format erhalten bleiben:

```text
<kind>:<selection-start-byte-offset>:<name>
```

Eine Änderung des ID-Formats ist nur zulässig, wenn sie zur korrekten nativen Implementierung erforderlich ist und folgende Nachweise vorliegen:

1. deterministische neue IDs;
2. dokumentierte Migrationswirkung;
3. aktualisierte Consumer-Golden-Tests;
4. keine Änderung innerhalb derselben P3-Laufzeit bei identischem Input;
5. kein Einsatz zufälliger UUIDs.

### 9.14 Diagnosekompatibilität

Bestehende Diagnosefelder bleiben erhalten:

```text
severity
code
message
range
relatedInformation
notes
treatedAsError
source
tags
```

P3 darf zusätzliche Diagnosecodes einführen, muss aber bestehende Codes soweit technisch möglich erhalten.

Für Editorstrukturdiagnosen, die bisher vom Live-Editorpfad erzeugt wurden, SOLL `source: "live"` erhalten bleiben. Dazu gehören insbesondere:

- falscher `END`-Name;
- doppelte Deklaration im gleichen Scope;
- editorbezogene Strukturdiagnosen.

Lexer- und Parserdiagnosen SOLLEN `source: "compiler"` verwenden.

Consumer dürfen Diagnosequellen filtern. Darum ist eine pauschale Änderung aller Diagnosen von `live` auf `compiler` oder umgekehrt nicht zulässig, ohne dass bestehende Consumer-Tests angepasst und die Änderung begründet wird.

### 9.15 Node-LSP-Vertrag

Ein Node-LSP, der das öffentliche Paket verwendet, soll nach P3 weiterhin denselben Ablauf verwenden können:

```ts
session.putSource(document.uri, document.getText(), document.version);
const snapshot = session.editorSnapshot(document.uri);
```

P3 MUSS dafür sicherstellen:

- LSP-Positionen bleiben UTF-16-basiert;
- `byteOffset` bleibt zusätzlich verfügbar;
- `documentVersion` wird unverändert transportiert;
- URI bleibt exakt erhalten;
- bei nicht gefundenem Dokument entsteht ein strukturiertes Fehlerresultat oder die bisher dokumentierte Exception;
- ein Snapshot einer alten Dokumentversion darf nicht irrtümlich als neue Version zurückgegeben werden;
- mehrere Sessions vermischen keine Sources;
- Worker ist nicht zwingend für Node.js;
- Node.js benötigt keinen DOM- oder Browser-Globalzustand;
- die API bleibt ESM-kompatibel wie bisher.

Wenn im Arbeitsumfeld ein konkreter Node-LSP-Checkout verfügbar ist, SOLL der Agent dessen Build und fokussierte Compilerintegration gegen das lokal erzeugte P3-Snapshotpaket ausführen. Der Agent DARF den Consumer-Checkout nicht verändern, nur um den Test grün zu machen.

Falls Anpassungen am Consumer tatsächlich notwendig erscheinen, muss zuerst bewiesen werden, dass der P3-Kompatibilitätsvertrag technisch nicht eingehalten werden kann. Normale Snapshotpräzisierungen oder neue optionale Felder rechtfertigen keine Consumer-API-Änderung.

### 9.16 `@ilic/language-service`-Vertrag

Wenn ein Language-Service-Paket zwischen Compiler und IDE liegt, MUSS P3 dessen bestehende Compileradapteroberfläche respektieren.

Der Adapter SOLL weiterhin:

- Sources mit `putSource()` synchronisieren;
- `editorSnapshot(uri)` aufrufen;
- Deklarationen in Document Symbols projizieren;
- Referenzen für Definition, Rename und Completion nutzen;
- Diagnosen publizieren;
- Dokumentversionen prüfen.

P3 DARF den Language Service nicht zwingen, selbst:

- SyntaxSnapshot zu EditorSnapshot zu konvertieren;
- Rohtext zu lexen;
- INTERLIS-Keywords zu interpretieren;
- C-ABI-Handles zu verwalten;
- Emscripten-Speicher direkt zu lesen.

Wenn `@ilic/language-service` ausserhalb von `ilic-fork` veröffentlicht wird, MUSS die P3-Dokumentation festhalten, welche minimale kompatible Paketversion benötigt wird. Bei unveränderter High-Level-API SOLL keine neue Mindestversion allein wegen P3 erforderlich sein.

### 9.17 Web-IDE-Vertrag

Eine Web-IDE soll nach P3 ohne fachliche Änderung des Anwendungscodes weiterarbeiten.

Insbesondere bleiben möglich:

- Compilerinitialisierung beim Start oder im Worker;
- Sourceaktualisierung bei Monaco-Änderungen;
- EditorSnapshot für Outline, Navigation, Rename und Diagnostics;
- SemanticSnapshot für semantische Funktionen;
- parallele UI-Arbeit über Worker.

P3 MUSS für Browser/PWA-Szenarien zusätzlich berücksichtigen:

- JavaScript-Wrapper und `.wasm` müssen aus demselben Paketbuild stammen;
- Worker und Hauptthread dürfen keine unterschiedlichen Compilerassets laden;
- Service Worker dürfen keine alte `.wasm` mit neuem Wrapper kombinieren;
- Assets sollen content-gehashte Namen oder eine gleichwertige Cache-Busting-Strategie verwenden;
- eine inkompatible Kombination muss früh und verständlich fehlschlagen;
- es darf keinen stillen Rückfall auf den entfernten JS-Parser geben.

Empfohlene Fehlermeldung:

```text
The @ilic/compiler-wasm JavaScript wrapper and WebAssembly module are incompatible: native editor snapshot export is missing.
```

Die Dokumentation MUSS einen Hinweis für PWA-Deployments enthalten:

1. Wrapper, Worker und WASM gemeinsam veröffentlichen;
2. alte Service-Worker-Caches invalidieren;
3. keine Dateien einzelner Paketversionen mischen.

### 9.18 Worker-Protokollkompatibilität

Das bestehende Requestformat bleibt:

```ts
{
  id: string,
  method: "editorSnapshot",
  args: [sessionId, uri]
}
```

Das Responseformat bleibt:

```ts
{ id, value: editorSnapshot }
```

oder bei Fehler:

```ts
{ id, error: message }
```

P3 DARF nicht verlangen, dass ein Consumer einen neuen Worker initialisiert, nur weil eine neue Source gesetzt wurde.

Die Session-ID, Methodenbezeichnung und Argumentreihenfolge bleiben unverändert.

Ein optionales Capability- oder Versionshandshake darf additiv ergänzt werden, muss aber bestehende Requests weiterhin akzeptieren.

### 9.19 Emscripten-Low-Level-API

Die Low-Level-Schnittstelle wird additiv erweitert um:

```ts
_ilic_editor_snapshot(
  session: number,
  request: number,
  requestLength: number
): number;
```

Diese Änderung betrifft nur:

- den internen JS-Wrapper;
- Tests mit Fake-Emscripten-Modulen;
- Consumer, die entgegen der empfohlenen API das rohe Emscripten-Modul direkt mocken oder verwenden.

Die High-Level-API MUSS die Low-Level-Erweiterung vollständig kapseln.

Testdoubles des Typs `EmscriptenIlicModule` müssen die neue Funktion ergänzen, sofern sie `editorSnapshot()` testen. Das ist die einzige bewusst akzeptierte Quellcodeanpassung ausserhalb des eigentlichen Wrappers.

### 9.20 Gemischte Wrapper-/WASM-Versionen

Der P3-Wrapper MUSS beim ersten EditorSnapshot-Aufruf oder bereits bei der Compilerinitialisierung prüfen, ob `_ilic_editor_snapshot` vorhanden ist.

Bei fehlendem Export gilt standardmässig:

- klarer Fehler;
- kein Zugriff auf uninitialisierten Funktionspointer;
- kein stiller JS-Parserfallback;
- kein leeres Snapshotresultat.

Wenn der explizite Legacy-Fallback aktiviert wurde, darf nur die in dieser Spezifikation definierte native SyntaxSnapshot-Projektion verwendet werden.

Zusätzlich SOLL eine Buildkennung oder Capability verfügbar sein:

```ts
compiler.capabilities.nativeEditorSnapshot === true
```

Optional kann die Runtime eine Paket-/Modulkennung vergleichen. Eine solche Prüfung darf keine nichtdeterministischen Buildpfade oder lokale absolute Pfade in das Paket schreiben.

### 9.21 Paketveröffentlichung und atomare Aktualisierung

P3 MUSS sicherstellen, dass folgende Bestandteile aus demselben Build publiziert werden:

```text
packages/compiler-wasm/index.js
packages/compiler-wasm/index.d.ts
packages/compiler-wasm/worker.js
packages/compiler-wasm/ilic.js
packages/compiler-wasm/ilic.wasm
```

Die tatsächlichen generierten Dateinamen sind aus dem Projekt zu übernehmen.

Der Snapshot-Publishing-Prozess MUSS den neuen WASM-Export und die aktualisierten Typen gemeinsam stagen.

Nicht zulässig:

- neuen Wrapper mit altem WASM veröffentlichen;
- neues WASM mit alten Typen veröffentlichen;
- Worker aus einer anderen Paketversion einbetten;
- nur `index.js` in einer bestehenden Web-IDE-Installation auszutauschen.

### 9.22 Consumer-Kompatibilitätsmatrix

Der Agent MUSS die folgende Matrix im Abschlussbericht mit tatsächlichen Ergebnissen ausfüllen:

| Consumer-Art | Erwartete Produktionscodeänderung | Verbindlicher Nachweis |
|---|---:|---|
| Direkter High-Level-Nutzer von `@ilic/compiler-wasm` | keine | bestehender Aufruf kompiliert und läuft |
| Node-LSP über High-Level-API | keine | Smoke-/Integrationstest |
| `@ilic/language-service` | keine fachliche Änderung | Pakettests gegen P3-Build |
| Web-IDE über Language Service/Worker | keine fachliche Änderung | Build und fokussierter Browser-/Worker-Test |
| Worker-Client | keine Protokolländerung | bestehender `editorSnapshot`-Request |
| Rohes Emscripten-Modul-Mock | neue Fake-Funktion nötig | aktualisierte Unit-Tests |
| Vollständige Snapshot-Golden-Files | erwartbare Aktualisierung | überprüfter Diff |
| Persistierte alte EditorSnapshots | keine zwingende Migration | optionale TS-Felder und Normalisierung |

Ein Ergebnis „Produktionscodeänderung erforderlich“ für die ersten fünf Zeilen ist ein P3-Kompatibilitätsfehler und muss vor Abschluss behoben oder als echter, genehmigungsbedürftiger Blocker ausgewiesen werden.

### 9.23 Keine versteckte Consumer-Migration

Der Coding-Agent DARF nicht stillschweigend Consumer-APIs ändern und anschliessend lediglich die internen Beispiele aktualisieren.

Vor Abschluss MUSS eine öffentliche API-Diff-Prüfung mindestens vergleichen:

- Exportnamen;
- Klassen- und Methodennamen;
- Parameteranzahl;
- Rückgabetypen;
- synchrone versus asynchrone Methoden;
- Worker-Methodennamen;
- JSON-Pflichtfelder;
- Enum-Stringwerte;
- `schemaVersion` und `abiVersion`.

Der Diff MUSS im Abschlussbericht zusammengefasst werden.

---

## 10. Source- und Range-Infrastruktur

### 10.1 Problem

Die aktuelle Syntaximplementierung berechnet für viele Tokens wiederholt:

- ANTLR-Codepointindex;
- UTF-8-Byteoffset;
- Zeile;
- UTF-16-Spalte.

P3 MUSS diese Berechnung zentralisieren, damit Syntax-, Editor- und Diagnoseranges identisch sind.

### 10.2 `SourceRangeMapper`

Empfohlene interne Klasse:

```cpp
namespace ilic::detail {

class SourceRangeMapper final {
public:
    SourceRangeMapper(
        const SourceManager& sources,
        const SourceBuffer& source
    );

    SourceRange bytes(
        std::size_t startByte,
        std::size_t endByte
    ) const;

    SourceRange codepoints(
        std::size_t startCodepoint,
        std::size_t endCodepoint
    ) const;

    SourceRange token(const antlr4::Token* token) const;
    SourceRange context(const antlr4::ParserRuleContext* context) const;
    SourceRange eof() const;

    std::size_t byteOffsetForCodepoint(
        std::size_t codepointOffset
    ) const;

private:
    const SourceManager& sources_;
    const SourceBuffer& source_;
    std::vector<std::size_t> codepointToByte_;
};

}
```

### 10.3 Anforderungen

`SourceRangeMapper` MUSS:

- UTF-8-Byteoffsets exakt berechnen;
- UTF-16-Spalten exakt berechnen;
- astrale Unicode-Zeichen als zwei UTF-16-Codeunits zählen;
- kombinierende Zeichen nicht normalisieren;
- LF und CRLF korrekt behandeln;
- leere Dateien unterstützen;
- EOF-Ranges erzeugen;
- ungültige oder negative ANTLR-Indizes defensiv behandeln;
- Start und Ende auf Sourcegrenzen begrenzen;
- niemals einen Endoffset vor dem Startoffset liefern.

### 10.4 Kein doppelter Range-Code

Nach P3 SOLL es in der Snapshot-Pipeline keine parallelen Implementierungen von:

- `utf8ByteOffset`;
- `rangeAt`;
- `editorPosition`;
- `editorRange`;
- JavaScript-`codePointAt`-Offsetlogik

geben.

### 10.5 SourceManager

Wenn P2 bereits einen effizienteren Sourceindex eingeführt hat, MUSS dieser verwendet werden. Es darf keine zweite Range-Infrastruktur daneben entstehen.

---

## 11. Sprachversionsdetektion

### 11.1 Problem des aktuellen heuristischen Pfads

Eine Suche wie:

```cpp
source.text.find("2.4") != std::string::npos
```

ist nicht zulässig, weil `2.4` in:

- Kommentaren;
- Strings;
- Versionsmetadaten;
- URLs

auftreten kann.

### 11.2 `LanguageDetector`

Empfohlene interne API:

```cpp
namespace ilic::detail {

enum class InterlisLanguage {
    Unknown,
    Ili1,
    Ili23,
    Ili24
};

class LanguageDetector final {
public:
    static InterlisLanguage fromPrefix(
        const SourceBuffer& source
    );
};

}
```

### 11.3 Regeln

- Erstes signifikantes Token `TRANSFER` → INTERLIS 1.0.
- Erstes signifikantes Token `INTERLIS` und Versionstoken `2.3` → 2.3.
- Erstes signifikantes Token `INTERLIS` und Versionstoken `2.4` → 2.4.
- Fehlender oder unvollständiger Header → `Unknown`.
- Bei `Unknown` darf für fehlertolerante Editorzwecke der Ili2-Parser verwendet werden.
- Das Resultatfeld `iliVersion` MUSS bei unbekannter Version explizit `"unknown"` oder den im bestehenden Projekt vereinbarten leeren Wert liefern.
- TypeScript MUSS denselben Wert abbilden.
- Compilersemantik für vollständig kompilierte Modelle darf nicht verändert werden.

### 11.4 Kein JavaScript-Headerparser

JavaScript DARF die Version nicht aus dem Quelltext bestimmen.

---

## 12. Syntax-Pipeline

### 12.1 `SnapshotErrorCollector`

Ersetze oder erweitere den bisherigen `SnapshotErrorListener`.

Empfohlene API:

```cpp
class SnapshotErrorCollector final : public antlr4::BaseErrorListener {
public:
    SnapshotErrorCollector(
        const SourceRangeMapper& ranges,
        DiagnosticOrigin origin
    );

    void syntaxError(
        antlr4::Recognizer* recognizer,
        antlr4::Token* offendingSymbol,
        std::size_t line,
        std::size_t charPositionInLine,
        const std::string& message,
        std::exception_ptr error
    ) override;

    const std::vector<Diagnostic>& diagnostics() const noexcept;
    bool hasErrors() const noexcept;

private:
    const SourceRangeMapper& ranges_;
    DiagnosticOrigin origin_;
    std::vector<Diagnostic> diagnostics_;
};
```

### 12.2 Diagnosecodes

Bestehende öffentliche Diagnosecodes sollen stabil bleiben.

Mindestens:

```text
ILIC-SYNTAX
ILIC-SOURCE-NOT-FOUND
```

Falls Lexer und Parser unterschieden werden, ist eine additive, dokumentierte Erweiterung zulässig:

```text
ILIC-SYNTAX-LEXER
ILIC-SYNTAX-PARSER
```

Eine Änderung bestehender Tests oder Consumer darf nicht ohne Migrationsbegründung erfolgen.

### 12.3 Diagnosereihenfolge

Diagnosen müssen deterministisch sortiert sein nach:

1. URI;
2. Start-Byteoffset;
3. End-Byteoffset;
4. Severity;
5. Code;
6. Meldung;
7. Erfassungsreihenfolge als stabiler Tie-Breaker.

### 12.4 Deduplizierung

Identische Lexer- und Parserdiagnosen dürfen dedupliziert werden, wenn alle folgenden Werte identisch sind:

- Code;
- Severity;
- URI;
- Start;
- Ende;
- Meldung.

Ähnliche, aber nicht identische Diagnosen dürfen nicht willkürlich zusammengelegt werden.

### 12.5 Parser Error Strategy

Der Editorpfad MUSS die normale fehlertolerante ANTLR-Recovery verwenden.

Nicht zulässig:

- `BailErrorStrategy` als Standard;
- Abbruch beim ersten Fehler;
- Wegwerfen des partiellen Parse-Trees;
- Catch-all mit leerem Snapshot.

Eine eigene `TrackingErrorStrategy` KANN Recovery-Nutzung markieren:

```cpp
class TrackingErrorStrategy final : public antlr4::DefaultErrorStrategy {
public:
    bool recovered() const noexcept;

    void recover(
        antlr4::Parser* recognizer,
        std::exception_ptr error
    ) override;

    antlr4::Token* recoverInline(
        antlr4::Parser* recognizer
    ) override;

private:
    bool recovered_ = false;
};
```

Die tatsächlichen Signaturen müssen zur eingebetteten ANTLR-Version passen.

### 12.6 Syntax-Token-Builder

Empfohlene Klasse:

```cpp
class SyntaxTokenBuilder final {
public:
    SyntaxTokenBuilder(
        const SourceRangeMapper& ranges,
        const antlr4::Vocabulary& vocabulary
    );

    void append(
        antlr4::CommonTokenStream& tokens,
        SyntaxSnapshot& snapshot
    ) const;
};
```

MUSS:

- EOF nicht als normales Token ausgeben;
- alle Kanäle erhalten;
- symbolischen Namen bevorzugen;
- Literalnamen als Fallback verwenden;
- Text exakt erhalten;
- Range aus dem gemeinsamen Mapper verwenden;
- stabile Tokenreihenfolge gewährleisten.

### 12.7 Syntax-Tree-Builder

Empfohlene Klasse:

```cpp
class SyntaxTreeBuilder final {
public:
    SyntaxTreeBuilder(
        const SourceRangeMapper& ranges,
        const std::vector<std::string>& ruleNames
    );

    void append(
        antlr4::tree::ParseTree* root,
        SyntaxSnapshot& snapshot
    ) const;

private:
    std::size_t appendNode(
        antlr4::tree::ParseTree* tree,
        SyntaxSnapshot& snapshot,
        std::size_t parent,
        bool hasParent
    ) const;
};
```

MUSS:

- nur Parserregelkontexte als `SyntaxNode` ausgeben;
- IDs fortlaufend und deterministisch vergeben;
- Parent-IDs korrekt setzen;
- bei unvollständigen Kontexten sinnvolle Ranges liefern;
- synthetische Error Nodes nicht als echte Deklarationen ausgeben;
- tiefe Bäume ohne willkürliche Rekursionsgrenze verarbeiten;
- Stackoverflowrisiken bei pathologischen Dateien berücksichtigen.

### 12.8 Editor-Kontexte im Syntax-Snapshot

Die Liste der `SyntaxContext`-Regeln MUSS zentral definiert sein.

Sie darf nicht parallel in C++ und JavaScript gepflegt werden.

Empfohlene Funktion:

```cpp
bool isEditorContextRule(std::string_view ruleName);
```

---

## 13. Interner Parse-Aufbau

### 13.1 Ein gemeinsamer Parsevorgang

Empfohlener Ablauf für Ili2:

```cpp
SnapshotBundle SnapshotPipeline::buildIli2(
    const SourceBuffer& source,
    SnapshotProducts products
) const
{
    SourceRangeMapper ranges(sources_, source);
    antlr4::ANTLRInputStream input(source.text);
    lexer::Ili2Lexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    parser::Ili2Parser parser(&tokens);

    // Listener und Recovery konfigurieren.
    auto* root = parser.interlis2Def();

    SnapshotBundle bundle;
    SyntaxSnapshotBuilder(...).build(lexer, tokens, parser, root, bundle.syntax);

    if (includes(products, SnapshotProducts::Editor)) {
        bundle.editor = Ili2EditorCollector(...).collect(
            parser,
            tokens,
            root,
            bundle.syntax
        );
    }

    return bundle;
}
```

### 13.2 Kein erneutes Tokenisieren

Der Editorcollector MUSS denselben `CommonTokenStream` verwenden.

Er darf nicht:

- `SourceBuffer::text` erneut lexen;
- einen zweiten Lexer instanziieren;
- Textregexe als Grammatik verwenden;
- Keywords aus dem Rohtext suchen.

### 13.3 Parserobjekt-Lebensdauer

Der Parse-Tree darf nur verwendet werden, solange Parser und TokenStream leben.

Alle Snapshotdaten müssen vor deren Zerstörung kopiert sein.

### 13.4 Exceptions

Der Pipeline-Einstieg MUSS:

- `std::exception` kontrolliert in eine strukturierte Diagnose übersetzen;
- Source-URI und Dokumentversion erhalten;
- partielle Resultate nicht unnötig verwerfen;
- unbekannte Exceptions in eine interne Snapshotdiagnose umwandeln;
- niemals `exit()` aufrufen.

---

## 14. Editor-Snapshot-Builder

### 14.1 Gemeinsamer Akkumulator

Empfohlene Klasse:

```cpp
namespace ilic::detail {

class EditorSnapshotAccumulator final {
public:
    EditorSnapshotAccumulator(
        const SourceBuffer& source,
        std::string iliVersion
    );

    std::size_t addDeclaration(EditorDeclaration declaration);
    void bindNode(
        const antlr4::ParserRuleContext* context,
        std::size_t declarationIndex
    );

    void addReference(EditorReference reference);
    void addImport(SyntaxImportReference importReference);
    void addContext(SyntaxContext context);
    void addDiagnostic(Diagnostic diagnostic);

    const EditorDeclaration* declarationFor(
        const antlr4::tree::ParseTree* node
    ) const;

    const EditorDeclaration* nearestOwner(
        const antlr4::tree::ParseTree* node
    ) const;

    EditorSnapshot finish(
        bool parserRecovered,
        bool parserComplete
    );

private:
    EditorSnapshot snapshot_;
    std::unordered_map<
        const antlr4::tree::ParseTree*,
        std::size_t
    > declarationsByNode_;
    std::unordered_set<std::string> referenceKeys_;
};

}
```

Ungeordnete Container sind intern zulässig, sofern sie nicht direkt die Ausgabereihenfolge bestimmen.

### 14.2 Stabile Ausgabereihenfolge

Deklarationen müssen in Source-Reihenfolge sortiert sein nach:

1. `selectionRange.start.byteOffset`;
2. `selectionRange.end.byteOffset`;
3. Kind;
4. Name.

Referenzen müssen in Source-Reihenfolge sortiert sein.

Imports müssen in Source-Reihenfolge sortiert sein.

Diagnosen müssen nach Abschnitt 12.3 sortiert sein.

### 14.3 Stabile IDs

Die bestehende kompatible Form SOLL erhalten bleiben:

```text
<kind>:<selection-start-byte-offset>:<name>
```

Beispiel:

```text
class:91:Parcel
```

Anforderungen:

- Native und WASM exakt identisch;
- keine Pointeradressen;
- keine Parse-Tree-IDs;
- keine Hashes mit plattformabhängigem Seed;
- keine zufälligen UUIDs;
- gleiche Source ergibt bei wiederholtem Lauf gleiche IDs.

URI muss nicht Bestandteil der ID sein, weil ein `EditorSnapshot` genau eine URI repräsentiert.

### 14.4 Qualified Name

`qualifiedName` wird aus den editorrelevanten Containerdeklarationen aufgebaut.

Beispiel:

```text
Model.Topic.Class.Attribute
```

Ein Parser-Recovery-Container darf verwendet werden, wenn Name und Scope eindeutig erkannt wurden.

### 14.5 Containerregeln

Container sind:

```text
model
topic
class
structure
association
view
graphic
```

Domain, Unit und Attribute sind keine Container für `qualifiedName`.

### 14.6 Source Range

`range` umfasst:

- Deklarationskeyword;
- Header;
- Body;
- expliziten Abschluss;
- abschliessendes `;` oder `.` soweit im Parse-Kontext enthalten.

Bei unvollständigen Containern endet `range` am letzten tatsächlich vorhandenen Token oder EOF.

### 14.7 Selection Range

`selectionRange` umfasst exakt den deklarierten Namen.

Synthetisch eingefügte Namen dürfen nicht als echte Selection Range ausgegeben werden.

### 14.8 End Range

`endRange` umfasst exakt den expliziten Namen nach `END`.

Bei anonymen Deklarationen oder unvollständigem Abschluss bleibt sie ungültig/null.

---

## 15. Ili2-Editorcollector

Empfohlene Klasse:

```cpp
class Ili2EditorCollector final
    : public parser::Ili2ParserBaseVisitor {
public:
    Ili2EditorCollector(
        const SourceRangeMapper& ranges,
        antlr4::CommonTokenStream& tokens,
        EditorSnapshotAccumulator& output
    );

    antlrcpp::Any visitInterlis2Def(
        parser::Ili2Parser::Interlis2DefContext* context
    ) override;

    antlrcpp::Any visitModelDef(
        parser::Ili2Parser::ModelDefContext* context
    ) override;

    antlrcpp::Any visitTopicDef(
        parser::Ili2Parser::TopicDefContext* context
    ) override;

    antlrcpp::Any visitClassDef(
        parser::Ili2Parser::ClassDefContext* context
    ) override;

    antlrcpp::Any visitStructureDef(
        parser::Ili2Parser::StructureDefContext* context
    ) override;

    antlrcpp::Any visitAssociationDef(
        parser::Ili2Parser::AssociationDefContext* context
    ) override;

    antlrcpp::Any visitViewDef(
        parser::Ili2Parser::ViewDefContext* context
    ) override;

    antlrcpp::Any visitGraphicDef(
        parser::Ili2Parser::GraphicDefContext* context
    ) override;

    antlrcpp::Any visitDomainDef(
        parser::Ili2Parser::DomainDefContext* context
    ) override;

    antlrcpp::Any visitUnitDef(
        parser::Ili2Parser::UnitDefContext* context
    ) override;

    antlrcpp::Any visitAttributeDef(
        parser::Ili2Parser::AttributeDefContext* context
    ) override;

    antlrcpp::Any visitImporting(
        parser::Ili2Parser::ImportingContext* context
    ) override;

    antlrcpp::Any visitPath(
        parser::Ili2Parser::PathContext* context
    ) override;

private:
    template<class Context>
    antlrcpp::Any declaration(
        Context* context,
        EditorSymbolKind kind,
        antlr4::tree::TerminalNode* name,
        bool container
    );

    EditorReferenceKind classifyPath(
        parser::Ili2Parser::PathContext* context
    ) const;

    std::string pathText(
        parser::Ili2Parser::PathContext* context
    ) const;

    SourceRange explicitEndRange(
        antlr4::ParserRuleContext* context,
        std::string_view declarationName
    ) const;

    const SourceRangeMapper& ranges_;
    antlr4::CommonTokenStream& tokens_;
    EditorSnapshotAccumulator& output_;
};
```

Die tatsächlichen Kontexttypen sind gegen die generierten Header zu verifizieren.

### 15.1 Deklarationsnamen

Der Collector SOLL benannte Grammar-Felder oder Terminalmethoden verwenden.

Tokenheuristiken sind nur zulässig, wenn die Grammatik keinen benannten Zugriff anbietet.

Nicht zulässig ist eine allgemeine Suche:

```text
erstes NAME nach irgendeinem Keyword
```

wenn der Kontext einen eindeutigen Namensknoten bereitstellt.

### 15.2 Anonyme Associations

Anonyme Associations dürfen:

- keine erfundene Deklarations-ID erhalten;
- weiterhin relevante Rollen und Referenzen liefern;
- keine `???`-Namen erzeugen.

Wenn Consumer eine anonyme Association als Container benötigen, KANN eine deterministische interne Container-ID verwendet werden, die nicht als benannte Deklaration ausgegeben wird.

### 15.3 `VIEW TOPIC`

Die spezielle Syntax `VIEW TOPIC` MUSS korrekt als Topic-Deklaration klassifiziert werden, ohne doppelte Topic-Deklaration.

### 15.4 `DOMAIN`

Der Lexer verwendet gegebenenfalls den Tokenkindnamen `ILIDOMAIN`. Der Editorcollector MUSS sich auf die Grammarregel und nicht auf JavaScript-Keywordlisten verlassen.

### 15.5 Attribute

Attribute müssen über `AttributeDefContext` erkannt werden.

Ein beliebiges Muster `NAME :` im Rohtext ist nicht mehr zulässig.

Dadurch dürfen Konstrukte in:

- Strings;
- Kommentaren;
- Funktionssignaturen;
- Constraints;
- Metadaten

keine falschen Attributedeklarationen erzeugen.

---

## 16. Ili1-Editorcollector

Empfohlene Klasse:

```cpp
class Ili1EditorCollector final
    : public parser::Ili1ParserBaseVisitor {
public:
    Ili1EditorCollector(
        const SourceRangeMapper& ranges,
        antlr4::CommonTokenStream& tokens,
        EditorSnapshotAccumulator& output
    );

    antlrcpp::Any visitInterlis1Def(
        parser::Ili1Parser::Interlis1DefContext* context
    ) override;

    antlrcpp::Any visitModelDef(
        parser::Ili1Parser::ModelDefContext* context
    ) override;

    antlrcpp::Any visitTopicDef(
        parser::Ili1Parser::TopicDefContext* context
    ) override;

    antlrcpp::Any visitTableDef(
        parser::Ili1Parser::TableDefContext* context
    ) override;

    antlrcpp::Any visitDomainDef(
        parser::Ili1Parser::DomainDefContext* context
    ) override;

    antlrcpp::Any visitAttribute(
        parser::Ili1Parser::AttributeContext* context
    ) override;

    antlrcpp::Any visitType(
        parser::Ili1Parser::TypeContext* context
    ) override;

private:
    // Entsprechende gemeinsame Helfer.
};
```

### 16.1 Mapping von TABLE

INTERLIS-1-`TABLE` wird für den bestehenden kanonischen Editorvertrag als:

```text
class
```

abgebildet, sofern die semantische API dies ebenfalls als Class modelliert.

Eine neue öffentliche Art `table` darf nur eingeführt werden, wenn alle Consumer und Snapshots gemeinsam migriert werden. Standard ist die kompatible Abbildung auf `class`.

### 16.2 Ili1-Referenzen

Der Collector MUSS mindestens erfassen:

- Domain-/Typreferenzen von Attributen;
- Basistypreferenzen;
- Linienattributreferenzen soweit durch Grammarregeln eindeutig;
- relevante Topic-/Tabellenbezüge.

Er darf keine Ili2-Konstrukte vortäuschen.

### 16.3 Fehlertoleranz

Unvollständige INTERLIS-1-Modelle müssen ebenfalls partielle Deklarationen liefern.

---

## 17. Referenzklassifikation

### 17.1 Erlaubte Referenzarten

```text
extends
type
collection
reference
unit
```

### 17.2 Klassifikation anhand Parse-Tree

Die Art MUSS aus dem Grammar-Kontext beziehungsweise dessen Vorfahren bestimmt werden.

Beispielhafte Regeln:

| Parse-Kontext oder Vorfahr | EditorReferenceKind |
|---|---|
| `baseExtensionDef`, explizite Extensionregel | `extends` |
| `referenceAttr`, `restrictedRef`, Reference-Typ | `reference` |
| `bagOrListType` | `collection` |
| Unit-Referenz oder Unit-Ausdruck | `unit` |
| Domain-/Attributtyp | `type` |

### 17.3 Nicht zulässige Heuristik

Nicht zulässig ist eine allgemeine Regel:

> Jeder `path` innerhalb eines Class- oder Topic-Definitionsknotens ist `extends`.

### 17.4 Referenztext

`text` MUSS aus den tatsächlichen Tokens der Path-Regel aufgebaut werden.

Erlaubte Bestandteile:

- Namen;
- Punkte;
- von der Grammar explizit zugelassene Pfadbestandteile.

Whitespace und Kommentare dürfen den kanonischen Pfadtext nicht verändern.

### 17.5 Source Owner

`sourceId` ist die nächste umschliessende benannte Editordeklaration.

Bei fehlendem Owner:

```json
"sourceId": null
```

### 17.6 Deduplizierung

Eine Referenz darf höchstens einmal erscheinen.

Empfohlener Schlüssel:

```text
kind:startByte:endByte:text:sourceId
```

### 17.7 Imports

Importnamen werden ausschließlich als `imports` ausgegeben und nicht zusätzlich als normale EditorReference, sofern der bestehende Vertrag dies nicht verlangt.

---

## 18. Imports

### 18.1 Native Quelle

Imports müssen aus dem Parse-Tree stammen.

### 18.2 Felder

Jeder Import enthält:

```json
{
  "model": "Geometry",
  "unqualified": true,
  "range": { ... }
}
```

### 18.3 `UNQUALIFIED`

Der Boolwert MUSS zum konkreten Importnamen gehören.

Ein vorangehendes `UNQUALIFIED` darf nicht irrtümlich auf alle nachfolgenden Namen übertragen werden, wenn die Grammar dies nicht so definiert.

### 18.4 Doppelte Imports

Doppelte Imports dürfen mehrfach im Syntax-/Editor-Snapshot erscheinen, wenn sie mehrfach im Source stehen. Semantische Diagnose ist Aufgabe des Compilers.

### 18.5 Unvollständige Importliste

Bereits vollständig erkannte Importnamen müssen auch bei fehlendem Semikolon verfügbar sein.

---

## 19. Editorstrukturdiagnosen

P3 MUSS die bisher vom JavaScript-Textparser gelieferten fachlichen Live-Diagnosen nativ erzeugen oder bewusst durch gleichwertige Parserdiagnosen ersetzen.

### 19.1 Endnamenfehler

Bestehender Code:

```text
ILIC-LIVE-END-NAME
```

MUSS erhalten bleiben, wenn Consumer oder Tests darauf beruhen.

Anforderungen:

- Range auf dem tatsächlich vorhandenen falschen Endnamen;
- Meldung enthält den erwarteten Namen;
- optional Related Information auf die ursprüngliche Deklaration;
- kein Fehler bei anonymen Konstrukten;
- kein Fehler durch synthetisches Recovery-Token.

### 19.2 Doppelte Deklaration

Bestehender Code:

```text
ILIC-LIVE-DUPLICATE
```

MUSS auf Basis tatsächlicher Deklarationskontexte erzeugt werden.

Schlüssel:

```text
containerId + case-normalized name
```

Diagnose:

- Range auf zweiter Deklaration;
- Related Information auf erster Deklaration;
- deterministische Meldung;
- keine falschen Treffer in verschiedenen Scopes.

### 19.3 Parserdiagnosen

ANTLR-Syntaxdiagnosen und Editorstrukturdiagnosen dürfen gemeinsam im Snapshot stehen.

### 19.4 `source`

Falls der öffentliche `Diagnostic`-Typ bereits ein `source`-Feld besitzt oder P2 es ergänzt hat:

- Parserdiagnosen → `compiler` oder `parser`;
- Live-Strukturdiagnosen → `live`.

Andernfalls darf die Information nicht nur im JavaScript nachträglich erfunden werden.

---

## 20. Verhalten bei unvollständigen Dokumenten

P3 MUSS einen expliziten Testkatalog für typische Editorzustände besitzen.

### 20.1 Leere Datei

Erwartung:

- kein Crash;
- korrekte URI und Dokumentversion;
- `success == false` oder definierter leerer Zustand;
- keine erfundenen Deklarationen;
- sinnvoller Syntaxhinweis;
- `recovered == true`;
- `complete == false`.

### 20.2 Nur Header

```ili
INTERLIS 2.4;
```

Erwartung:

- Version 2.4;
- keine Deklaration;
- keine internen Fehler;
- partieller Snapshot.

### 20.3 Unvollständiger Header

```ili
INTERLIS 2.
```

Erwartung:

- Parserdiagnose;
- keine JavaScript-Heuristik;
- Version `unknown` oder nach Tokenlage 2.3/2.4 nur bei eindeutiger Erkennung.

### 20.4 Begonnenes Modell

```ili
INTERLIS 2.3;
MODEL Example
```

Erwartung:

- Modelldeklaration mit Name `Example`, soweit Parse-Tree-Kontext vorhanden;
- Selection Range korrekt;
- kein End Range;
- `complete == false`.

### 20.5 Offener Topic-/Class-Body

Bereits erkannte Container und Attribute müssen verfügbar bleiben.

### 20.6 Halb geschriebener Typ

```ili
Value : REF
```

Erwartung:

- Attribute bleibt sichtbar;
- keine erfundene Referenz;
- Syntaxdiagnose korrekt lokalisiert.

### 20.7 Unvollständiger String

Der Inhalt darf keine falschen Deklarationen oder Referenzen erzeugen.

### 20.8 Kommentar mit Keywords

```ili
!! MODEL Fake = END Fake.
```

Darf keine Deklaration erzeugen.

### 20.9 Fehlender END-Name

Container bleibt sichtbar, `endRange == null`.

### 20.10 Falscher END-Name

Container bleibt sichtbar und erhält `ILIC-LIVE-END-NAME`.

### 20.11 Mehrere Fehler

Ein Fehler am Anfang darf nicht alle später recoverbaren Deklarationen unterdrücken.

### 20.12 Keine zweite Recovery-Grammatik

Falls die ANTLR-Recovery für einzelne Fälle unzureichend ist, MUSS zuerst geprüft werden:

- Error Strategy;
- Parseraufruf;
- Listener;
- Parse-Tree-Traversierung;
- Kontext-Range;
- generierte Grammarregeln.

Ein neuer Textscanner ist keine zulässige Lösung.

---

## 21. C++-CompilerSession

`include/ilic/Compiler.h` MUSS ergänzen:

```cpp
#include "Editor.h"

class CompilerSession {
public:
    EditorSnapshot editorSnapshot(const std::string& uri);
};
```

### 21.1 Synchronisation

Die Methode MUSS das nach P2 definierte Session-Synchronisationsmodell verwenden.

Sie DARF NICHT:

- eine globale Mutex verwenden;
- unabhängige Sessions global serialisieren;
- einen global aktiven Parser setzen.

### 21.2 Sourcezugriff

Die Methode verwendet ausschließlich die Source aus dem Session-`SourceManager`.

### 21.3 Fehlende Source

Eine fehlende URI liefert einen strukturierten Editor-Snapshot mit:

- `success == false`;
- URI;
- Dokumentversion 0;
- Diagnosecode `ILIC-SOURCE-NOT-FOUND`;
- leeren Arrays.

### 21.4 Keine Sourcekopie

Die C++-Methode muss den SourceBuffer nicht dauerhaft kopieren. Sie darf ihn während des synchronisierten Aufrufs referenzieren.

---

## 22. C-ABI

`include/ilic/capi.h` MUSS additiv ergänzen:

```c
ILIC_CAPI_EXPORT uint32_t ilic_editor_snapshot(
    uint32_t session,
    const char *request_json,
    size_t request_length
);
```

### 22.1 Request

Schema:

```json
{
  "schemaVersion": 1,
  "uri": "memory:///Example.ili"
}
```

### 22.2 Response

Beispiel:

```json
{
  "schemaVersion": 1,
  "abiVersion": 1,
  "compilerVersion": "0.9.9",
  "kind": "editor",
  "success": true,
  "recovered": false,
  "complete": true,
  "uri": "memory:///Example.ili",
  "documentVersion": 7,
  "iliVersion": "2.3",
  "declarations": [],
  "references": [],
  "imports": [],
  "contexts": [],
  "diagnostics": []
}
```

### 22.3 Fehlerresultat

`errorResult()` in `source/abi/Capi.cpp` MUSS `kind == "editor"` unterstützen.

Es MUSS alle strukturell erwarteten Felder liefern.

### 22.4 Serializer

Empfohlene neue Funktion:

```cpp
Value editorResult(const ilic::EditorSnapshot& result);
```

Zusätzliche Hilfen:

```cpp
const char* editorSymbolKind(EditorSymbolKind kind);
const char* editorReferenceKind(EditorReferenceKind kind);
```

### 22.5 ABI-Version

Da die Funktion additiv ist, KANN ABI 1 beibehalten werden.

Wenn ABI erhöht wird, MUSS:

- JavaScript alte und neue Version bewusst behandeln;
- native Consumer dokumentiert migriert werden;
- Paketconsumer-Tests beide Fälle prüfen.

Eine unnötige ABI-Erhöhung ist zu vermeiden.

### 22.6 Thread-Sicherheit

Die Handle-Registry darf wie nach P2 bestehen bleiben. Der Editoraufruf darf keine globale Compilerzustandsabhängigkeit einführen.

---

## 23. Emscripten-Build

`CMakeLists.txt` MUSS die neue Funktion exportieren:

```text
_ilic_editor_snapshot
```

Die Exportliste MUSS weiterhin alle bestehenden Symbole enthalten.

Der Build MUSS weiterhin:

- `MODULARIZE=1`;
- `EXPORT_ES6=1`;
- `ENVIRONMENT=web,worker,node`;
- Memory Growth;
- WASM BigInt;
- Exceptions

unterstützen.

P3 DARF keine dynamischen HTTP- oder Dateisystemabhängigkeiten in das WASM-Modul einführen.

---

## 24. JavaScript-Wrapper

### 24.1 `CompilerSession`

Nach P3:

```js
export class CompilerSession {
  #module;
  #handle;
  #allowLegacyEditorProjection;

  editorSnapshot(uri) {
    this.#active();
    if (typeof this.#module._ilic_editor_snapshot === "function") {
      return callJson(
        this.#module,
        this.#module._ilic_editor_snapshot,
        this.#handle,
        { schemaVersion: 1, uri },
      );
    }
    if (!this.#allowLegacyEditorProjection)
      throw new Error("native ilic editor snapshot API is unavailable");
    return projectLegacyEditorSnapshot(this.parse(uri));
  }
}
```

### 24.2 Entfernung der Source-Schattenkopie

Zu entfernen:

```js
#sources = new Map();
```

soweit keine andere echte Wrapperanforderung sie benötigt.

`putSource()` darf nach erfolgreichem C-ABI-Aufruf keine vollständige Textkopie speichern.

`removeSource()` muss keine JS-Map mehr pflegen.

`dispose()` muss keine JS-Source-Map mehr leeren.

### 24.3 Zu löschende Funktionen

Produktiver Code MUSS entfernen:

```text
editorPosition
editorRange
editorTokens
editorTextProjection
```

sowie alle nur dafür benötigten Konstanten und Helper.

### 24.4 Erlaubte Legacy-Projektion

Die bestehende reine Projektion:

```text
SyntaxSnapshot -> EditorSnapshot
```

KANN als ausdrücklich aktivierter Legacy-Fallback erhalten bleiben.

Sie MUSS umbenannt werden, beispielsweise:

```js
projectLegacyEditorSnapshot(syntax)
```

Sie DARF:

- `syntax.tokens`;
- `syntax.nodes`;
- `syntax.importReferences`;
- `syntax.contexts`;
- `syntax.diagnostics`

auswerten.

Sie DARF NICHT:

- Rohtext erhalten;
- Rohtext tokenisieren;
- eine Source-Schattenkopie verwenden;
- Strings oder Kommentare lexen;
- eigene Versionserkennung betreiben.

### 24.5 Fallback standardmässig aus

Empfohlene Option:

```js
const compiler = await createCompiler({
  compatibility: {
    allowLegacyEditorProjection: false,
  },
});
```

Standard:

```text
false
```

Eine alternative flache Option ist zulässig, wenn klar dokumentiert.

### 24.6 Capability

`Compiler` SOLL exponieren:

```js
compiler.capabilities.nativeEditorSnapshot === true
```

oder eine gleichwertige read-only Information.

### 24.7 Kein stiller Fallback

Fehlt `_ilic_editor_snapshot`, darf der Wrapper nicht still in einen anderen Parserpfad wechseln.

### 24.8 CompilerVersion

Ein nativer Editor-Snapshot MUSS die echte CompilerVersion liefern.

Der künstliche Wert:

```text
ilic-editor-snapshot
```

darf im normalen Pfad nicht mehr erscheinen.

---

## 25. TypeScript-Typen

`packages/compiler-wasm/index.d.ts` MUSS aktualisiert werden.

### 25.1 Emscripten-Modul

Ergänzen:

```ts
_ilic_editor_snapshot(
  session: number,
  request: number,
  requestLength: number
): number;
```

Falls Legacy-Module unterstützt werden:

```ts
_ilic_editor_snapshot?: (...args: ...) => number;
```

### 25.2 EditorSnapshot

Die P3-Laufzeit MUSS `recovered` und `complete` immer als Boolean serialisieren.

Für die erste P3-Veröffentlichung müssen die TypeScript-Felder aus Gründen der Quellkompatibilität zu bestehenden Mocks, Fixtures und persistierten Snapshots optional sein:

```ts
recovered?: boolean;
complete?: boolean;
```

Die Kommentare in `index.d.ts` müssen klarstellen:

- P3-Runtimeergebnisse enthalten beide Properties immer;
- optional ist nur die statische Typdeklaration für Alt-Consumer;
- neue Consumer dürfen mit `snapshot.recovered ?? !snapshot.success` und `snapshot.complete ?? snapshot.success` normalisieren.

`iliVersion` muss den unbekannten Zustand abbilden, wenn die C++-API ihn liefert:

```ts
iliVersion: "1.0" | "2.3" | "2.4" | "unknown";
```

Bestehende Werte dürfen nicht umbenannt werden.

### 25.3 Compileroptionen

Die explizite Legacy-Fallbackoption muss typisiert sein.

### 25.4 Capabilities

Falls exponiert, muss ein Interface vorhanden sein:

```ts
export interface CompilerCapabilities {
  nativeEditorSnapshot: boolean;
}
```

### 25.5 Keine Phantomtypen

Die `.d.ts`-Datei muss exakt die Laufzeit-API beschreiben.

### 25.6 API-Diff-Test

Ein Test oder Skript MUSS die vor P3 dokumentierte High-Level-TypeScript-API mit der P3-API vergleichen.

Mindestens unverändert:

```text
createCompiler
Compiler
Compiler.createSession
CompilerSession.putSource
CompilerSession.putWorkspace
CompilerSession.removeSource
CompilerSession.compile
CompilerSession.parse
CompilerSession.editorSnapshot
CompilerSession.analyze
CompilerSession.compileAndAnalyze
CompilerSession.format
CompilerSession.dispose
```

Der Test darf additive Interfaces, Properties und Low-Level-Emscripten-Funktionen erlauben.

### 25.7 Compile-Time-Consumer-Fixture

Unter den Pakettests MUSS eine TypeScript-Fixture existieren, die einen pre-P3-artigen Consumer repräsentiert.

Sie muss insbesondere ein EditorSnapshot-Testdouble ohne `recovered` und `complete` typisieren können:

```ts
const legacyFixture: EditorSnapshot = {
  schemaVersion: 1,
  abiVersion: 1,
  compilerVersion: "fixture",
  kind: "editor",
  success: true,
  uri: "memory:///Example.ili",
  documentVersion: 1,
  iliVersion: "2.3",
  declarations: [],
  references: [],
  imports: [],
  contexts: [],
  diagnostics: [],
};
```

Dies beweist die gewünschte Quellkompatibilität der ersten P3-Version.

### 25.8 Kein Rückgabetypwechsel

`editorSnapshot()` bleibt synchron.

Die `.d.ts` darf nicht versehentlich `Promise<EditorSnapshot>` deklarieren.

### 25.9 Low-Level-Mocks

Testmodule, die `EmscriptenIlicModule` vollständig implementieren, müssen `_ilic_editor_snapshot` ergänzen.

Wenn ein Mock bewusst ein Legacy-Modul simuliert, darf die Methode optional fehlen. Der Test muss dann den expliziten Fallback- oder Fehlerpfad wählen.

### 25.10 Exportstabilität

Ein Pakettest MUSS die exportierten High-Level-Namen prüfen. Additive Exporte sind zulässig; entfernte oder umbenannte bestehende Exporte sind nicht zulässig.

---

## 26. Worker

`packages/compiler-wasm/worker.js` unterstützt bereits `editorSnapshot`.

P3 MUSS verifizieren:

- Worker ruft die neue native Wrappermethode auf;
- keine Sourcekopie im Worker;
- Sessiontrennung bleibt bestehen;
- Fehler werden als Worker-Response zurückgegeben;
- Dokumentversion bleibt erhalten;
- mehrere Worker-Sessions vermischen keine Daten.

Das Worker-Protokoll benötigt für P3 keine neue Methode, sofern `editorSnapshot` bereits vorhanden ist.

### 26.1 Unverändertes Request-/Response-Protokoll

Der bestehende Methodenname, die Argumentreihenfolge und die Responsehülle bleiben unverändert.

### 26.2 Worker-/WASM-Buildkonsistenz

Der Worker MUSS denselben Paketbuild laden wie die Hauptanwendung.

Ein Worker-Test MUSS ein fehlendes `_ilic_editor_snapshot` als verständlichen Fehler an den Client weiterreichen.

### 26.3 Keine Worker-Sourcekopie

Der Worker darf keine eigene Map mit Source-Rohtext für Editoranalyse führen. Sessionzustand bleibt im Compiler-WASM-Wrapper beziehungsweise nativen `SourceManager`.

### 26.4 Dokumentversion

Ein Worker-Test MUSS folgende Sequenz prüfen:

1. `putSource(uri, textV1, 1)`;
2. `editorSnapshot(uri)` liefert `documentVersion == 1`;
3. `putSource(uri, textV2, 2)`;
4. `editorSnapshot(uri)` liefert `documentVersion == 2`;
5. keine Daten aus Version 1 im zweiten Snapshot.

### 26.5 Mehrere Sessions

Zwei Worker-Sessions mit gleicher URI und unterschiedlichem Text müssen vollständig isoliert bleiben.

---

## 27. Native SyntaxSnapshot

P3 darf den bestehenden `SyntaxSnapshot` nicht verschlechtern.

### 27.1 Pflichtfelder

Weiterhin:

- URI;
- Dokumentversion;
- Sprache;
- Tokens;
- Nodes;
- Contexts;
- Imports;
- Import References;
- Diagnostics.

### 27.2 Fehlertoleranz

`SyntaxSnapshot` MUSS auch bei Parserfehlern:

- alle lexbaren Tokens liefern;
- den partiellen Parse-Tree serialisieren;
- bereits erkannte Imports liefern;
- Diagnostics liefern;
- nicht crashen.

### 27.3 Deterministische Node-IDs

Node-IDs müssen bei identischer Source stabil sein.

### 27.4 Parentkonsistenz

Für jeden Node mit Parent gilt:

```text
parent < node.id
```

und Parent existiert.

### 27.5 Rangeinvarianten

Für jeden Token und Node:

```text
start.byteOffset <= end.byteOffset <= source.size()
```

---

## 28. JavaScript-Doppelparser-Architecture-Guard

Neue CTest- oder Node-Prüfung, beispielsweise:

```text
test/architecture/NoJavaScriptParserTest.mjs
```

oder:

```text
cmake/check_no_javascript_parser.cmake
```

### 28.1 Zu prüfender Produktivbereich

Mindestens:

```text
packages/compiler-wasm/index.js
packages/compiler-wasm/worker.js
```

### 28.2 Verbotene Symbole

Der Guard MUSS mindestens erkennen:

```text
editorTokens
editorTextProjection
codePointAt
isNameStart
isNamePart
ILIC-LIVE-END-NAME
ILIC-LIVE-DUPLICATE
compilerVersion: "ilic-editor-snapshot"
```

Die Diagnosecodes dürfen in JavaScript-Testfixtures vorkommen, aber nicht als produktive Diagnoseimplementierung.

### 28.3 Verbotene Struktur

Der Guard MUSS zusätzlich erkennen:

- eine private Source-Map mit vollständigem Source-Text;
- Keywordtabellen, die eine INTERLIS-Grammatik nachbilden;
- eine Schleife über einzelne Quelltextzeichen im Wrapper;
- Regex-basierte Erkennung von Deklarationen aus Rohtext.

### 28.4 Kleine Allowlist

Eine Allowlist darf nur:

- Testdateien;
- Dokumentation;
- expliziten Legacy-Syntax-Projektionscode ohne Rohtextzugriff

umfassen.

### 28.5 Negativer Canary

Der Agent MUSS den Guard einmal mit einer temporär eingefügten verbotenen Funktion fehlschlagen lassen und danach die Änderung vollständig entfernen.

---

## 29. Native Unit-Tests

### 29.1 `SyntaxSnapshotTest`

Der bestehende Test MUSS erweitert oder in fokussierte Tests aufgeteilt werden.

Prüfen:

- Ili1;
- Ili2.3;
- Ili2.4;
- unbekannte Version;
- Imports;
- `UNQUALIFIED`;
- Unicode vor Token;
- astrale Zeichen;
- CRLF;
- LF;
- leere Source;
- fehlende Source;
- unvollständige Source;
- Lexerfehler;
- Parserfehler;
- stabile Node-IDs;
- Parentkonsistenz;
- Tokenkanäle;
- Kommentar- und Stringtokens;
- wiederholte deterministische Ausgabe.

### 29.2 `EditorSnapshotTest`

Neue Datei:

```text
test/core/EditorSnapshotTest.cpp
```

Mindestens folgende Gruppen:

#### Deklarationen

- Model;
- Topic;
- Class;
- Structure;
- Association;
- View;
- Graphic;
- Domain;
- Unit;
- Attribute;
- Ili1 Model/Topic/Table/Domain/Attribute.

#### Scopes

- Model → Topic → Class → Attribute;
- Model-Level Domain;
- Topic-Level Domain;
- gleiche Namen in unterschiedlichen Scopes;
- `containerId`;
- `qualifiedName`.

#### Ranges

- gesamter Deklarationsrange;
- Selection Range;
- End Range;
- kein End Range bei unvollständiger Deklaration;
- Unicode;
- CRLF.

#### Referenzen

- Extends;
- Type;
- Collection;
- Reference;
- Unit;
- qualifizierter Pfad;
- unqualifizierter Pfad;
- kein Import als normale Referenz;
- kein String-/Kommentarfalsepositive.

#### Imports

- mehrere Imports;
- `UNQUALIFIED`;
- fehlendes Semikolon;
- Import Range.

#### Diagnosen

- falscher Endname;
- doppelte Deklaration;
- unvollständiger Header;
- unvollständiger String;
- fehlender Abschluss;
- mehrere Syntaxfehler.

#### Robustheit

- leere Datei;
- nur Kommentar;
- nur Header;
- Source endet nach jedem relevanten Token;
- tiefe Verschachtelung;
- grosse Datei;
- wiederholter Lauf.

### 29.3 Testhelper

Empfohlene Helper:

```cpp
EditorSnapshot snapshot(
    std::string source,
    std::uint64_t version = 1
);

const EditorDeclaration& requireDeclaration(
    const EditorSnapshot& snapshot,
    EditorSymbolKind kind,
    std::string_view qualifiedName
);

const EditorReference& requireReference(
    const EditorSnapshot& snapshot,
    EditorReferenceKind kind,
    std::string_view text
);
```

Tests müssen `ilic::test-support` verwenden.

---

## 30. C-ABI-Tests

`test/abi/CapiTest.cpp` erweitern oder neue Datei:

```text
test/abi/EditorCapiTest.cpp
```

Prüfen:

- gültiger Request;
- invalid session;
- null request;
- invalid JSON;
- falsche schemaVersion;
- fehlende URI;
- unbekannte URI;
- Dokumentversion;
- JSON-Kind `editor`;
- alle Arrays vorhanden;
- nullbare Container-/Endfelder;
- Diagnosefelder;
- wiederholte Result-Handle-Zerstörung korrekt;
- keine Leaks;
- zwei parallele Sessions nach P2.

---

## 31. JavaScript-Unit-Tests

Der Agent MUSS die tatsächlichen vorhandenen Testdateien unter:

```text
packages/compiler-wasm/test
```

inventarisieren und erweitern.

### 31.1 Spy-Modul

Ein Fake-Emscripten-Modul SOLL aufzeichnen:

- `_ilic_session_put_source`;
- `_ilic_parse`;
- `_ilic_editor_snapshot`;
- `_ilic_result_json`;
- `_ilic_result_destroy`.

### 31.2 Standardpfad

Test:

- `_ilic_editor_snapshot` existiert;
- `session.editorSnapshot(uri)` ruft genau diese Funktion auf;
- `_ilic_parse` wird nicht aufgerufen;
- keine JS-Textprojektion findet statt.

### 31.3 Legacy-Fallback aus

Test:

- Modul ohne `_ilic_editor_snapshot`;
- Defaultoption;
- `editorSnapshot()` wirft klaren Fehler;
- kein stiller Fallback.

### 31.4 Legacy-Fallback an

Test:

- explizite Option;
- Modul ohne `_ilic_editor_snapshot`;
- `_ilic_parse` wird einmal aufgerufen;
- reine Syntaxprojektion liefert EditorSnapshot;
- kein Zugriff auf Source-Rohtext.

### 31.5 Source-Schattenkopie

Ein Test MUSS indirekt beweisen, dass EditorSnapshot nach `putSource()` nicht aus einer JS-Kopie erzeugt wird.

Geeigneter Spy-Nachweis:

- Native Fake-Funktion liefert bewusst ein Ergebnis, das nicht aus dem Source ableitbar ist.
- Wrapper muss exakt dieses Ergebnis zurückgeben.

### 31.6 Dispose

- Editoraufruf nach `dispose()` wirft;
- Dispose bleibt idempotent.

### 31.7 Worker

Worker-Test prüft `editorSnapshot`-Forwarding.

### 31.8 High-Level-API-Kompatibilität

Ein Test MUSS den unveränderten Consumerablauf ausführen:

```js
const compiler = await createCompiler({ moduleFactory });
const session = compiler.createSession();
session.putSource("memory:///Example.ili", source, 17);
const snapshot = session.editorSnapshot("memory:///Example.ili");
```

Prüfen:

- kein zusätzlicher Parameter nötig;
- kein Promise als Resultat;
- `kind == "editor"`;
- `documentVersion == 17`;
- bestehende Arrays vorhanden;
- neue Runtimefelder vorhanden.

### 31.9 Legacy-TypeScript-Fixture

Der Paketbuild muss eine TypeScript-Consumer-Fixture kompilieren, die einen alten Snapshot ohne neue Properties konstruiert.

### 31.10 Fehlerhafter, aber nutzbarer Snapshot

Ein Test MUSS einen offenen Container oder falschen Endnamen verwenden und prüfen:

- `success == false`;
- `recovered == true`;
- `complete == false`;
- bereits geschriebene Deklarationen vorhanden;
- Diagnosen vorhanden;
- keine Exception nur wegen Syntaxfehler;
- kein leerer Snapshot.

### 31.11 Gemischte Assetversion

Fake-Modul ohne `_ilic_editor_snapshot`:

- Standardkonfiguration wirft verständlichen Kompatibilitätsfehler;
- kein Aufruf eines JS-Textparsers;
- kein Zugriff auf eine Source-Schattenkopie;
- expliziter Legacy-Fallback wird separat getestet.

### 31.12 Exportoberfläche

Ein Test MUSS bestehende High-Level-Exporte gegen eine feste Allowlist prüfen.

### 31.13 Golden-Diff

Mindestens ein Golden-Test MUSS den pre-P3- und P3-EditorSnapshot für repräsentative gültige Modelle vergleichen.

Differenzen sind in Kategorien auszugeben:

```text
added-field
removed-field
type-change
value-change
range-change
added-declaration
removed-declaration
added-reference
removed-reference
diagnostic-change
order-change
```

Nicht zulässig:

- entfernte Pflichtfelder;
- Typänderung bestehender Felder;
- Enum-Umbenennung;
- Änderung von `kind` oder `schemaVersion`.

### 31.14 Consumer-Paket-Smoke-Test

Wenn ein lokaler Checkout oder ein installierbares Paket für Node-LSP, Language Service oder Web-IDE verfügbar ist, SOLL der Agent:

1. das P3-Snapshotpaket lokal erzeugen;
2. den Consumer ohne Produktionscodeänderung dagegen bauen;
3. dessen vorhandene Tests ausführen;
4. den exakten Consumer-Commit dokumentieren.

Fehlt ein Consumer-Checkout, MUSS mindestens die repräsentative High-Level-Consumer-Fixture im `ilic-fork`-Repository laufen.

---

## 32. Native/WASM-Parität

### 32.1 Ziel

Für dieselbe Source muss gelten:

```text
normalize(native syntax) == normalize(wasm syntax)
normalize(native editor) == normalize(wasm editor)
```

### 32.2 Native Snapshot Driver

Neue test-only ausführbare Datei, beispielsweise:

```text
test/snapshot/SnapshotJsonDriver.cpp
```

Funktionen:

```text
syntax
editor
```

Eingabe über stdin:

```json
{
  "operation": "editor",
  "uri": "memory:///Example.ili",
  "documentVersion": 7,
  "source": "INTERLIS 2.3; ..."
}
```

Ausgabe:

- exakt das gleiche C-ABI-JSON-Schema;
- ein JSON-Objekt;
- keine zusätzlichen Logs auf stdout;
- Fehler auf stderr;
- nicht null Exitcode nur bei Driver-/Protokollfehler.

### 32.3 Warum C-ABI-Serializer

Der native Driver SOLL denselben Serializer wie die C-ABI verwenden, damit geprüft wird:

- Core-Snapshot;
- C-ABI;
- JSON;
- WASM;
- JavaScript-Wrapper.

### 32.4 Node-Paritätsrunner

Neue Datei, beispielsweise:

```text
test/snapshot/SnapshotParityTest.mjs
```

Ablauf:

1. WASM laden.
2. Native Driver starten.
3. identische Source registrieren.
4. Native Syntax abrufen.
5. WASM Syntax abrufen.
6. Native Editor abrufen.
7. WASM Editor abrufen.
8. normalisieren.
9. tief vergleichen.
10. bei Differenz JSON-Diff ausgeben.

### 32.5 Normalisierung

Normalisiert werden dürfen nur:

- absolute Binärpfade außerhalb des Snapshots;
- plattformspezifische Zeilenenden in Testprotokollen;
- explizit nicht deterministische Buildmetadaten, falls vorhanden.

Nicht normalisiert werden dürfen:

- Token;
- Node-IDs;
- Parent-IDs;
- Kinds;
- Ranges;
- Byteoffsets;
- UTF-16-Spalten;
- Diagnosen;
- Deklarationen;
- Referenzen;
- Imports;
- `recovered`;
- `complete`;
- Dokumentversion;
- Sprache.

### 32.6 CompilerVersion

Da Native und WASM aus demselben Commit gebaut werden, SOLL CompilerVersion identisch sein.

---

## 33. Snapshot-Corpus aus der externen Testsuite

### 33.1 Verwendung des realen Korpus

Die externe `interlis-compiler-testsuite` enthält sehr viele Modelle und MUSS zusätzlich zur Compile-Conformance als Snapshot-Corpus verwendet werden.

### 33.2 Keine Änderung der Suite

Der Agent DARF das Nachbarrepository nicht verändern, zurücksetzen oder formatieren.

### 33.3 Commit

Verwende den in `.github/workflows/ci.yml` gepinnten Commit.

Weicht `../interlis-compiler-testsuite` ab, ist ein separater Checkout oder Worktree zu verwenden.

### 33.4 Corpus-Discovery

Der Snapshot-Corpus-Runner MUSS die reale Suite-Struktur lesen.

Bevorzugt:

- `suite/test-cases.json` parsen;
- alle referenzierten `.ili`-Ressourcen sammeln;
- Duplikate über kanonischen Pfad oder SHA-256 entfernen;
- keine nicht referenzierten Buildartefakte einbeziehen.

Falls das Manifest Testdaten indirekt referenziert, MUSS der Agent dessen Schema tatsächlich analysieren.

### 33.5 Pro Datei

Für jede eindeutige `.ili`-Datei:

- Source als Bytes lesen;
- stabile virtuelle URI erzeugen;
- Native SyntaxSnapshot;
- WASM SyntaxSnapshot;
- Native EditorSnapshot;
- WASM EditorSnapshot;
- Parität prüfen;
- Crash/Timeout verhindern;
- Resultatstatistik erfassen.

### 33.6 Gültige und ungültige Modelle

Auch absichtlich ungültige Modelle müssen Snapshot-Parität besitzen.

Ein Syntaxfehler ist kein Testfehler, solange:

- beide Plattformen gleich reagieren;
- strukturierte Resultate zurückkommen;
- kein Crash entsteht.

### 33.7 Report

Maschinenlesbar:

```text
build/snapshot-corpus/report.json
```

Menschenlesbar:

```text
build/snapshot-corpus/summary.md
```

Felder:

- Suite-Commit;
- Anzahl Dateien;
- Bytes;
- Ili1;
- Ili2.3;
- Ili2.4;
- unknown;
- valide Syntax;
- recoverte Syntax;
- Native/WASM-Differenzen;
- Crashes;
- Timeouts;
- langsamste Dateien.

---

## 34. Deterministische Truncation-Tests

### 34.1 Ziel

Editorzustände entstehen typischerweise durch abgeschnittene oder halb geschriebene Dateien.

### 34.2 Auswahl

Für jede ausgewählte Corpusdatei werden deterministisch höchstens acht Varianten erzeugt:

1. 0 Bytes;
2. nach dem Header;
3. nach 10 Prozent;
4. nach 25 Prozent;
5. nach 50 Prozent;
6. nach 75 Prozent;
7. unmittelbar vor letztem `END`;
8. ein Token vor EOF.

### 34.3 UTF-8-Sicherheit

Truncation darf nicht mitten in einer UTF-8-Sequenz schneiden, außer ein separater Test prüft bewusst ungültiges UTF-8.

### 34.4 Begrenzung

Um CI-Zeit zu kontrollieren:

- deterministische Stichprobe;
- stabile Auswahl anhand SHA-256;
- mindestens alle Sprachversionen;
- mindestens 100 unterschiedliche Ausgangsmodelle, sofern der Korpus dies bietet;
- keine zufällige, nicht reproduzierbare Auswahl.

### 34.5 Prüfungen

Für jede Variante:

- kein Crash;
- kein Hang;
- Snapshot innerhalb Timeout;
- Native/WASM-Parität;
- Rangeinvarianten;
- deterministische Wiederholung.

---

## 35. Fuzz-nahe Mutationstests

Ohne ein separates Fuzzing-Framework einzuführen, SOLL P3 folgende deterministische Mutationen prüfen:

- einzelnes Token löschen;
- Semikolon löschen;
- `END`-Name ändern;
- Schlusszeichen löschen;
- Stringabschluss löschen;
- Kommentarabschluss löschen, soweit Blockkommentare existieren;
- Punkt in Pfad löschen;
- Doppelpunkt löschen;
- `=` löschen;
- Containerkeyword austauschen.

Die Mutationen müssen aus festen Seeds erzeugt werden.

Ein libFuzzer-Harness KANN ergänzt werden, ist aber kein Ersatz für die deterministischen Tests.

---

## 36. Externe Compiler-Conformance

### 36.1 Ausgangslauf

Vor produktiven Änderungen:

- aktuellen Release-Compiler bauen;
- vollständige externe Suite ausführen;
- Report sichern;
- aktuelle Known-Failure-Anzahl ermitteln.

### 36.2 Zwischenläufe

Vollständiger Lauf mindestens nach:

1. Range-/Syntax-Pipeline;
2. nativer Editor-Snapshot;
3. C-ABI;
4. JavaScript-Wrapperentfernung;
5. finalem Mergezustand.

### 36.3 Abschlusslauf

Release MUSS vollständig grün gemäß Baseline sein.

### 36.4 Baseline

- keine neuen Einträge;
- keine breiteren Wildcards;
- kein automatisches Regenerieren;
- behobene Fälle müssen entfernt werden;
- neue interne Fehler sind zwingend zu beheben.

### 36.5 Conformance ist kein Snapshot-Paritätstest

Die Compiler-Conformance prüft Semantik. Sie ersetzt nicht:

- Editor-Snapshot-Tests;
- Native/WASM-Parität;
- Truncation-Corpus.

Umgekehrt ersetzen Snapshot-Tests nicht die Conformance-Suite.

---

## 37. Performance und Speicher

### 37.1 Ziele

P3 soll:

- JavaScript-Source-Duplikation entfernen;
- nur einen Parserpfad pflegen;
- pro EditorSnapshot nur einen WASM-Aufruf benötigen;
- pro Aufruf nur ein Resultat-JSON übertragen.

### 37.2 Kein flakiger Zeit-Gate

Absolute Laufzeiten dürfen nicht als enges CI-Gate verwendet werden.

### 37.3 Messbericht

Ein Benchmarkskript SOLL messen:

- `putSource` für 1 KB, 100 KB, 1 MB;
- `parse`;
- `editorSnapshot`;
- JSON-Grösse;
- WASM-Heapwachstum;
- wiederholte 100 Snapshotaufrufe.

### 37.4 Akzeptanz

Keine unbegründete Verschlechterung um mehr als Faktor 2 auf dem Referenzrechner.

Eine Überschreitung muss analysiert und dokumentiert werden.

### 37.5 Paketgrösse

`ilic.wasm`- und npm-Tarball-Grösse vor/nach erfassen.

Ein Wachstum muss durch native Editorfunktionalität erklärbar sein.

---

## 38. CMake

### 38.1 Core Sources

Neue Dateien in `ILIC_CORE_SOURCES`, beispielsweise:

```text
source/core/Editor.cpp
source/core/SnapshotPipeline.cpp
source/core/SourceRangeMapper.cpp
```

Dateiaufteilung darf kompakter sein, wenn Verantwortlichkeiten klar bleiben.

### 38.2 Tests

Neue Targets:

```text
ilic_editor_snapshot_test
ilic_snapshot_json_driver
ilic_no_javascript_parser
```

### 38.3 Node-Verfügbarkeit

Paritätstests, die Node benötigen, sind nur zu registrieren, wenn Node gefunden wurde.

Native Kernprüfungen dürfen nicht von Node abhängen.

### 38.4 Emscripten

Der WASM-Build muss den neuen Export erhalten.

### 38.5 Keine produktive Testabhängigkeit

Produktiver Core darf keine Node-, Test- oder Corpus-Abhängigkeit erhalten.

---

## 39. CI

### 39.1 Native Release-Matrix

Bestehende Linux/macOS/Windows-Tests bleiben.

EditorSnapshotTest muss auf allen Plattformen laufen.

### 39.2 Debug

Vollständige Syntax- und Editor-Tests im Debug-Build.

### 39.3 ASan/UBSan/LSan

Mindestens:

- SyntaxSnapshotTest;
- EditorSnapshotTest;
- CapiTest;
- Truncation-Kerntests;
- wiederholte Snapshot-Lebenszyklen.

Keine Leaks.

### 39.4 TSan nach P2

Wenn P2 TSan eingeführt hat:

- parallele `parse()`- und `editorSnapshot()`-Aufrufe auf unterschiedlichen Sessions;
- keine Datenrennen;
- kein globaler Parserzustand.

### 39.5 WASM-Job

MUSS ausführen:

1. gepinntes Emscripten installieren;
2. WASM bauen;
3. npm-Tests;
4. TypeScript-API-Diff-Test;
5. Legacy-Consumer-Compile-Fixture;
6. High-Level-Consumer-Smoke-Test;
7. Worker-Protokolltest;
8. Mixed-Asset-Fehlertest;
9. Native Snapshot Driver bauen;
10. fokussierte Native/WASM-Parität;
11. externer Snapshot-Corpus;
12. Paketconsumer-Tests.

### 39.6 Conformance-Job

Bestehender vollständiger `interlis-compiler-testsuite`-Job bleibt verbindlich.

### 39.7 Reports

Bei Fehlschlag hochladen:

- Snapshot-Paritätsdiffs;
- Corpus-Report;
- Conformance-Report;
- Benchmarkreport, falls erzeugt.

---

## 40. Dokumentation

Aktualisieren:

```text
packages/compiler-wasm/README.md
docs/wasm.md
docs/native-api.md
docs/diagnostik-und-logging.md
```

### 40.1 WASM README

Die Aussage:

> kompakter linearer Editor-Scan

muss entfernt oder korrigiert werden.

Neu dokumentieren:

- EditorSnapshot stammt aus dem nativen C++-/ANTLR-Parser;
- fehlertolerant;
- keine JS-Grammatik;
- Felder `recovered` und `complete`;
- expliziter Legacy-Fallback;
- Workerempfehlung.

### 40.2 Native API

Dokumentiere:

```cpp
CompilerSession::editorSnapshot()
```

### 40.3 C-ABI

Dokumentiere `ilic_editor_snapshot`.

### 40.4 Architektur

Ein Diagramm SOLL zeigen, dass Native und WASM denselben Core verwenden.

### 40.5 Grenzen

Dokumentiere ehrlich:

- noch kein inkrementelles Parsing;
- synchroner Snapshotaufruf;
- Worker für UI;
- P5 später für Caching/Inkrementalität.

### 40.6 Consumer-Migration

Dokumentiere ausdrücklich:

- High-Level-Anwender müssen `editorSnapshot(uri)` nicht umstellen;
- Node-LSP und Web-IDE sollen keine fachliche API-Migration benötigen;
- `recovered` und `complete` sind zur Laufzeit immer vorhanden;
- die TypeScript-Felder bleiben in der ersten P3-Version optional für alte Fixtures;
- `success == false` kann dennoch partielle nutzbare Daten enthalten;
- rohe Emscripten-Mocks müssen `_ilic_editor_snapshot` ergänzen;
- Wrapper, Worker und WASM müssen gemeinsam aktualisiert werden;
- PWA-/Service-Worker-Caches müssen Paketversionen nicht vermischen.

### 40.7 Upgrade-Hinweis

Die Release Notes MUSS einen kurzen Abschnitt enthalten:

```text
No application API migration is required for users of CompilerSession.editorSnapshot().
Consumers that implement a complete EmscriptenIlicModule mock must add
_ilic_editor_snapshot. Snapshot golden files may change because the native parser
provides more precise ranges, recovery information and diagnostics.
```

---

## 41. Dateiplan

### 41.1 Neue Dateien

Empfohlen:

```text
include/ilic/Editor.h
source/core/Editor.cpp
source/core/SnapshotPipeline.h
source/core/SnapshotPipeline.cpp
source/core/SourceRangeMapper.h
source/core/SourceRangeMapper.cpp
test/core/EditorSnapshotTest.cpp
test/snapshot/SnapshotJsonDriver.cpp
test/snapshot/SnapshotParityTest.mjs
test/snapshot/SnapshotCorpusTest.mjs
test/architecture/NoJavaScriptParserTest.mjs
scripts/run-snapshot-corpus.mjs
docs/editor-snapshot.md
```

### 41.2 Zu ändernde Dateien

Mindestens:

```text
include/ilic/Compiler.h
include/ilic/Syntax.h
include/ilic/capi.h
source/core/Compiler.cpp
source/core/Syntax.cpp
source/abi/Capi.cpp
packages/compiler-wasm/index.js
packages/compiler-wasm/index.d.ts
packages/compiler-wasm/worker.js
packages/compiler-wasm/README.md
docs/wasm.md
docs/native-api.md
CMakeLists.txt
.github/workflows/ci.yml
```

### 41.3 Zu löschender Produktivcode

Aus `packages/compiler-wasm/index.js`:

```text
editorPosition
editorRange
editorTokens
editorTextProjection
#sources
```

Weitere ausschließlich dafür vorhandene Helper sind ebenfalls zu entfernen.

---

## 42. Methoden-Migrationsmatrix

| Bisher | Ziel |
|---|---|
| `parseSyntax(sources, uri)` | `SnapshotPipeline::syntax(uri)` oder gleichwertig |
| internes `parseIli1()` | `SnapshotPipeline::buildIli1()` |
| internes `parseIli2()` | `SnapshotPipeline::buildIli2()` |
| `rangeAt()` | `SourceRangeMapper` |
| `utf8ByteOffset()` | zentraler Source-/Rangeindex |
| `SnapshotErrorListener` | `SnapshotErrorCollector` |
| `appendTokens()` | `SyntaxTokenBuilder::append()` |
| `appendNodes()` | `SyntaxTreeBuilder::append()` |
| JS `editorProjection()` | `projectLegacyEditorSnapshot()` nur expliziter Fallback |
| JS `editorTokens()` | löschen |
| JS `editorTextProjection()` | löschen |
| JS `#sources` | löschen |
| JS `CompilerSession.editorSnapshot()` | C-ABI `_ilic_editor_snapshot` aufrufen |
| kein C++ EditorSnapshot | `ilic::EditorSnapshot` |
| keine C++-Methode | `CompilerSession::editorSnapshot()` |
| keine C-ABI-Methode | `ilic_editor_snapshot()` |
| kein C-ABI-Serializer | `editorResult()` |
| kein WASM-Export | `_ilic_editor_snapshot` exportieren |

---

## 43. Klassen- und Methodenabnahme

### 43.1 `SourceRangeMapper`

MUSS getestet und dokumentiert sein.

### 43.2 `SnapshotPipeline`

MUSS garantieren, dass EditorSnapshot pro Aufruf nur einmal parst.

### 43.3 `EditorSnapshotAccumulator`

MUSS Ausgabereihenfolge, IDs, Deduplizierung und Diagnosen finalisieren.

### 43.4 `Ili2EditorCollector`

MUSS alle vereinbarten Ili2-Deklarationen und Referenzarten erfassen.

### 43.5 `Ili1EditorCollector`

MUSS mindestens die bestehenden Ili1-Editoranforderungen erfüllen und darf nicht leer bleiben.

### 43.6 `CompilerSession`

MUSS native EditorSnapshot-API besitzen.

### 43.7 `Capi.cpp`

MUSS JSON und Fehlerresultate vollständig serialisieren.

### 43.8 JavaScript

MUSS dünner ABI-Wrapper sein.

---

## 44. Migrationsphasen

### P3.0 – Baseline und Inventar

- Git-Status;
- aktuellen Commit;
- P2-Zustand;
- CI;
- aktuelle Conformance-Baseline;
- externen Suite-Commit;
- aktuelle Syntax-/WASM-Tests;
- aktuelles Editorverhalten;
- Ausgangs-Conformance;
- Ausgangs-npm-Tests;
- Paketgrösse.

### P3.1 – Range- und Syntaxhärtung

- gemeinsamen RangeMapper;
- Sprachdetektion;
- Diagnostics;
- SyntaxSnapshotTests;
- keine API-Änderung nötig.

Gate:

- native Tests;
- Conformance-Suite.

### P3.2 – C++-Editor-Datenmodell

- `Editor.h`;
- Serializer-Utilities;
- Accumulator;
- Unit-Tests für Value Semantics.

### P3.3 – Ili2-Collector

- Deklarationen;
- Imports;
- Referenzen;
- Ranges;
- Live-Diagnosen;
- Recovery.

Gate:

- EditorSnapshotTest Ili2;
- Conformance-Suite.

### P3.4 – Ili1-Collector

- Model;
- Topic;
- Table;
- Domain;
- Attribute;
- Referenzen;
- Recovery.

Gate:

- EditorSnapshotTest Ili1;
- Conformance-Suite.

### P3.5 – CompilerSession und C-ABI

- native Methode;
- C-ABI;
- JSON;
- Tests;
- kein globaler Zustand.

### P3.6 – WASM-Export

- CMake Export;
- WASM-Build;
- direkter C-ABI-Test.

### P3.7 – JavaScript-Bereinigung

- nativer Standardpfad;
- expliziter Legacy-Fallback;
- Source-Map entfernen;
- Textparser löschen;
- TypeScript;
- Worker.

Gate:

- npm-Tests;
- Architecture Guard.

### P3.8 – Parität

- Native Driver;
- Node-Paritätsrunner;
- fokussierte Fixtures;
- Unicode;
- Fehlerfälle;
- Truncations.

### P3.9 – Externer Corpus

- Suite-Manifest lesen;
- Modelle sammeln;
- Snapshot-Corpus;
- Reports;
- CI.

### P3.10 – Abschluss

- alle Builds;
- Sanitizer;
- TSan;
- WASM;
- npm;
- Conformance;
- Dokumentation;
- Diff-Check.

---

## 45. Verbotene Abkürzungen

Ausdrücklich verboten:

- JavaScript-Parser lediglich umzubenennen;
- `editorTextProjection()` in eine andere Datei verschieben;
- Textscanner als „Recovery Scanner“ beibehalten;
- JavaScript-Keywordtabellen weiterführen;
- JS-Source-Map nur zu verstecken;
- native API einzuführen, aber im Normalfall weiter JS zu verwenden;
- stiller Legacy-Fallback;
- Native/WASM-Unterschiede wegzunormalisieren;
- Ranges nur ungefähr zu vergleichen;
- EditorSnapshot bei Syntaxfehlern leer zurückgeben;
- Parser beim ersten Fehler abbrechen;
- `source.text.find("2.4")`;
- neuen globalen Parserzustand;
- `thread_local` aktiven Snapshotkontext;
- globale Mutex für Parseraufrufe;
- zweite ANTLR-Instanz innerhalb eines EditorSnapshot-Aufrufs;
- neue P3-Conformancefehler in die Baseline aufnehmen;
- externe Testsuite verändern;
- P0-/P2-Tests entfernen;
- C++17 anheben;
- C-ABI ohne Not brechen;
- Worker-API ohne Migration brechen;
- Tests durch gelockerte Erwartungen grün machen;
- flakige Zufallstests ohne Seed;
- ungemessene Behauptung von Parität;
- Erfolg behaupten, ohne Befehle auszuführen.

---

## 46. Verifikationsbefehle

Der Agent MUSS die aktuellen Projektbefehle lesen und entsprechend anpassen.

### 46.1 Release

```bash
cmake -S . -B build/release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/release --parallel
ctest --test-dir build/release --output-on-failure
```

### 46.2 Debug

```bash
cmake -S . -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/debug --parallel
ctest --test-dir build/debug --output-on-failure
```

### 46.3 Sanitizer

```bash
cmake -S . -B build/sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/sanitizers --parallel

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:strict_string_checks=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build/sanitizers --output-on-failure -j2
```

### 46.4 WASM

```bash
source ../emsdk/emsdk_env.sh
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

Der tatsächliche EMSDK-Pfad ist aus Projektkonfiguration oder Umgebung zu ermitteln.

### 46.5 Parität

Beispiel:

```bash
node test/snapshot/SnapshotParityTest.mjs \
  --native build/release/ilic_snapshot_json_driver \
  --package packages/compiler-wasm
```

### 46.6 Snapshot-Corpus

Beispiel:

```bash
node scripts/run-snapshot-corpus.mjs \
  --suite ../interlis-compiler-testsuite/suite/test-cases.json \
  --native build/release/ilic_snapshot_json_driver \
  --package packages/compiler-wasm \
  --out build/snapshot-corpus
```

### 46.7 Compiler-Conformance

Der exakte Befehl ist aus `.github/workflows/ci.yml` zu übernehmen.

---

## 47. Pflicht-Testmatrix

| Dimension | Werte |
|---|---|
| Plattform | Linux, macOS, Windows, WASM |
| Build | Release, Debug, ASan/UBSan/LSan, TSan soweit vorhanden |
| Sprache | 1.0, 2.3, 2.4, unknown |
| Syntax | gültig, lexerfehlerhaft, parserfehlerhaft, unvollständig |
| Zeilenende | LF, CRLF |
| Unicode | ASCII, BMP, astral, kombinierend |
| Sourcegrösse | leer, klein, mittel, gross |
| API | C++, C-ABI, JS, Worker |
| Consumer | direkter WASM-Nutzer, Node-LSP, Language Service, Web-IDE, Mock |
| Kompatibilität | bestehender Aufruf, altes Fixture, Mixed Assets, Worker-Protokoll |
| Snapshot | Syntax, Editor |
| Lauf | einmalig, wiederholt, parallel |
| Corpus | lokale Fixtures, externe Suite, Truncations |

---

## 48. Definition of Done

P3 ist nur abgeschlossen, wenn alle folgenden Punkte erfüllt sind:

- [ ] P2-Voraussetzungen verifiziert.
- [ ] Ausgangs-Conformance dokumentiert.
- [ ] `include/ilic/Editor.h` oder gleichwertige öffentliche API vorhanden.
- [ ] `CompilerSession::editorSnapshot()` vorhanden.
- [ ] C++-EditorSnapshot wird aus ANTLR-Parse-Ergebnis erzeugt.
- [ ] Pro Editoraufruf nur ein Lexer-/Parserlauf.
- [ ] Ili1-Collector vorhanden.
- [ ] Ili2-Collector vorhanden.
- [ ] Deklarationen vollständig getestet.
- [ ] Referenzarten vollständig getestet.
- [ ] Imports vollständig getestet.
- [ ] Ranges Unicode-sicher.
- [ ] CRLF getestet.
- [ ] unvollständige Dokumente liefern partielle Snapshots.
- [ ] Endnamenfehler nativ.
- [ ] Duplikatdiagnose nativ.
- [ ] C-ABI `ilic_editor_snapshot`.
- [ ] C-ABI-Fehlerresultat vollständig.
- [ ] Emscripten-Export vorhanden.
- [ ] JS-Standardpfad ruft native Funktion.
- [ ] `editorTokens` gelöscht.
- [ ] `editorTextProjection` gelöscht.
- [ ] JS-Source-Schattenkopie gelöscht.
- [ ] kein produktiver JS-Textparser.
- [ ] Legacy-Fallback explizit und standardmässig aus.
- [ ] Legacy-Fallback verwendet nur SyntaxSnapshot.
- [ ] TypeScript aktuell.
- [ ] `editorSnapshot(uri)`-Signatur unverändert.
- [ ] `editorSnapshot()` bleibt synchron.
- [ ] bestehende High-Level-Exporte unverändert.
- [ ] bestehende EditorSnapshot-Felder unverändert.
- [ ] `schemaVersion == 1` bleibt erhalten.
- [ ] `recovered` und `complete` werden zur Laufzeit immer geliefert.
- [ ] `recovered` und `complete` sind in der ersten P3-TypeScript-API optional.
- [ ] Legacy-TypeScript-Fixture kompiliert.
- [ ] fehlerhafter Snapshot liefert partielle Daten.
- [ ] Worker-Protokoll unverändert.
- [ ] Worker aktuell.
- [ ] Mixed-Wrapper/WASM-Fehler verständlich getestet.
- [ ] Node-LSP-Consumer-Smoke-Test oder repräsentative Fixture grün.
- [ ] Language-Service-Consumer-Smoke-Test oder repräsentative Fixture grün.
- [ ] Web-IDE-/Worker-Consumer-Smoke-Test oder repräsentative Fixture grün.
- [ ] keine fachliche Consumer-Produktionscodeänderung erforderlich.
- [ ] Architecture Guard vorhanden.
- [ ] negativer Guard-Canary ausgeführt.
- [ ] Native/WASM-Parität für Syntax.
- [ ] Native/WASM-Parität für Editor.
- [ ] externe Suite als Snapshot-Corpus verwendet.
- [ ] Truncation-Corpus ausgeführt.
- [ ] vollständige Compiler-Conformance ohne neue Abweichung.
- [ ] Release grün.
- [ ] Debug grün.
- [ ] ASan/UBSan/LSan grün.
- [ ] TSan grün, sofern P2-Job vorhanden.
- [ ] WASM grün.
- [ ] npm-Pakettests grün.
- [ ] Dokumentation aktualisiert.
- [ ] `git diff --check` grün.
- [ ] keine fremden Änderungen beschädigt.

---

## 49. Akzeptanzkriterien im Detail

### 49.1 Architektur

Eine Quellcodeinspektion darf im produktiven JavaScript keinen zweiten INTERLIS-Parser finden.

### 49.2 Funktionalität

Für alle bisherigen EditorSnapshot-Beispiele müssen mindestens dieselben Deklarationen, Referenzen und Imports verfügbar sein.

### 49.3 Recovery

Eine definierte Matrix unvollständiger Quellen muss partielle Ergebnisse liefern.

### 49.4 Parität

Alle fokussierten und Corpus-Paritätsfälle müssen ohne Differenz bestehen.

### 49.5 Semantik

Die externe Compiler-Conformance darf keine neue Abweichung zeigen.

### 49.6 Speicher

Keine neue Leak- oder Use-after-free-Meldung.

### 49.7 API

Bestehende `compile`, `parse`, `editorSnapshot`, `analyze`, `compileAndAnalyze`, `format` bleiben funktionsfähig.

### 49.8 Consumer-Kompatibilität

Ein bestehender High-Level-Consumer muss ohne Änderung seiner Produktionsaufrufe bauen und laufen.

### 49.9 Node-LSP

Der dokumentierte Node-LSP-Aufruf mit `putSource()` und `editorSnapshot(uri)` bleibt gültig.

### 49.10 Web-IDE

Der Worker- beziehungsweise Language-Service-Pfad bleibt ohne Protokolländerung nutzbar.

### 49.11 Typkompatibilität

Ein altes `EditorSnapshot`-Fixture ohne `recovered` und `complete` kompiliert gegen die erste P3-TypeScript-Definition.

### 49.12 Runtime-Vollständigkeit

Jeder P3-Runtime-EditorSnapshot enthält `recovered` und `complete` als Boolean.

### 49.13 Assetkonsistenz

Ein neuer Wrapper mit altem WASM scheitert kontrolliert und verständlich; ein stiller JS-Parserfallback findet nicht statt.

---

## 50. Abschlussbericht

Der Coding-Agent MUSS am Ende exakt diese Abschnitte liefern:

1. **Ausgangszustand**
2. **Verwendeter ilic-Commit**
3. **Verwendete externe Testsuite**
4. **Ausgangs-Conformance**
5. **Entfernter JavaScript-Parser**
6. **Native Snapshot-Pipeline**
7. **SourceRangeMapper**
8. **Sprachversionsdetektion**
9. **SyntaxSnapshot-Änderungen**
10. **EditorSnapshot-Datenmodell**
11. **Ili2-Collector**
12. **Ili1-Collector**
13. **Recovery-Verhalten**
14. **Diagnosen**
15. **C++-API**
16. **C-ABI**
17. **WASM-Export**
18. **JavaScript-Wrapper**
19. **Legacy-Fallback**
20. **Öffentlicher API-Diff**
21. **Consumer-Kompatibilitätsvertrag**
22. **Node-LSP-Kompatibilität**
23. **Language-Service-Kompatibilität**
24. **Web-IDE- und PWA-Kompatibilität**
25. **Worker und TypeScript**
26. **Gemischte Wrapper-/WASM-Versionen**
27. **Neue und geänderte Dateien**
28. **Native Unit-Tests**
29. **C-ABI-Tests**
30. **JavaScript-Tests**
31. **Consumer-Smoke-Tests**
32. **Native/WASM-Parität**
33. **Snapshot-Corpus**
34. **Truncation-Tests**
35. **Finale Compiler-Conformance**
36. **Release-Ergebnisse**
37. **Debug-Ergebnisse**
38. **Sanitizer-Ergebnisse**
39. **TSan-Ergebnisse**
40. **WASM- und npm-Ergebnisse**
41. **Performance- und Grössenmessung**
42. **Architecture Guard**
43. **Ausgeführte Befehle**
44. **Abweichungen von der Spezifikation**
45. **Externe Blocker**
46. **Verbleibende Risiken**
47. **Abschliessender Git-Status**

Für jeden Testlauf:

- exakter Befehl;
- Buildtyp;
- Exitcode;
- Testanzahl;
- Fehleranzahl;
- Reportpfad;
- Suite-Commit, falls relevant.

---

## Anhang A – Beispiel eines vollständigen Editor-Snapshots

```json
{
  "schemaVersion": 1,
  "abiVersion": 1,
  "compilerVersion": "0.9.9",
  "kind": "editor",
  "success": true,
  "recovered": false,
  "complete": true,
  "uri": "memory:///Example.ili",
  "documentVersion": 7,
  "iliVersion": "2.3",
  "declarations": [
    {
      "id": "model:21:Example",
      "name": "Example",
      "qualifiedName": "Example",
      "kind": "model",
      "containerId": null,
      "range": {
        "uri": "memory:///Example.ili",
        "start": { "line": 1, "character": 0, "byteOffset": 15 },
        "end": { "line": 8, "character": 12, "byteOffset": 180 }
      },
      "selectionRange": {
        "uri": "memory:///Example.ili",
        "start": { "line": 1, "character": 6, "byteOffset": 21 },
        "end": { "line": 1, "character": 13, "byteOffset": 28 }
      },
      "endRange": {
        "uri": "memory:///Example.ili",
        "start": { "line": 8, "character": 4, "byteOffset": 171 },
        "end": { "line": 8, "character": 11, "byteOffset": 178 }
      }
    }
  ],
  "references": [],
  "imports": [],
  "contexts": [],
  "diagnostics": []
}
```

---

## Anhang B – Startreihenfolge für den Coding-Agenten

Der Agent beginnt zwingend mit:

```bash
git status --short
git branch --show-current
git rev-parse HEAD
```

Danach vollständig lesen:

```text
ilic-p3-single-parser-spec.md
.github/workflows/ci.yml
include/ilic/Syntax.h
include/ilic/Compiler.h
include/ilic/capi.h
source/core/Syntax.cpp
source/core/Compiler.cpp
source/abi/Capi.cpp
packages/compiler-wasm/index.js
packages/compiler-wasm/index.d.ts
packages/compiler-wasm/worker.js
packages/compiler-wasm/README.md
docs/wasm.md
test/core/SyntaxSnapshotTest.cpp
```

Danach:

1. tatsächliche Pakettests inventarisieren;
2. externen Suite-Checkout prüfen;
3. Ausgangs-Conformance ausführen;
4. Ausgangs-WASM/npm-Tests ausführen;
5. JS-Doppelparserinventar erstellen;
6. schrittweise implementieren;
7. nach jeder grösseren Phase testen;
8. keine Rückfrage bei normalen Implementierungsentscheidungen;
9. nicht nach einem Teilgerüst stoppen;
10. Abschluss nur nach vollständiger Verifikation.

---

## Anhang C – Prüffragen für Code Review

### C.1 Single Parser

- Gibt es irgendeinen produktiven JS-Code, der Sourcezeichen interpretiert?
- Gibt es irgendeine Keywordliste für INTERLIS im Wrapper?
- Gibt es eine JS-Sourcekopie?
- Wird `_ilic_editor_snapshot` immer im Normalpfad verwendet?

### C.2 Recovery

- Liefert ein offenes Modell eine Deklaration?
- Liefert ein offenes Topic seine bereits geschriebenen Klassen?
- Werden Strings und Kommentare korrekt ignoriert?
- Sind Endnamenfehler nativ?

### C.3 Parität

- Sind Byteoffsets identisch?
- Sind UTF-16-Spalten identisch?
- Sind IDs identisch?
- Sind Diagnosen identisch?
- Sind Reihenfolgen identisch?

### C.4 Regression

- Ist die externe Conformance unverändert?
- Ist die Known-Failure-Baseline unverändert oder kleiner?
- Sind npm-Consumer weiterhin kompatibel?
- Ist der Workerpfad getestet?

### C.5 Consumer-Kompatibilität

- Bleibt `editorSnapshot(uri)` synchron und signaturgleich?
- Bleiben alle bisherigen High-Level-Exporte vorhanden?
- Bleiben bestehende JSON-Felder und Enumwerte erhalten?
- Kompiliert ein altes EditorSnapshot-Fixture ohne neue Properties?
- Liefert die Runtime trotzdem immer `recovered` und `complete`?
- Nutzt ein Consumer partielle Daten auch bei `success == false`?
- Bleibt das Worker-Protokoll unverändert?
- Scheitern gemischte Wrapper-/WASM-Versionen verständlich?
- Müssen Node-LSP oder Web-IDE Produktionscode ändern? Falls ja: warum ist das kein P3-Fehler?

---

## Anhang D – Mindestfälle für fokussierte Snapshot-Parität

1. gültiges Ili2.3-Minimalmodell;
2. gültiges Ili2.4-Modell;
3. gültiges Ili1-Modell;
4. Unicode vor Deklarationsname;
5. CRLF-Datei;
6. mehrere Imports;
7. `UNQUALIFIED`;
8. nested Topic/Class/Attribute;
9. Association;
10. View;
11. Graphic;
12. Domain;
13. Unit;
14. Extends;
15. Collection;
16. Reference;
17. falscher Endname;
18. doppelte Deklaration;
19. fehlender Abschluss;
20. unvollständiger String;
21. Kommentar mit Keywords;
22. leere Datei;
23. unbekannte URI;
24. grosse Datei;
25. jede bekannte Syntaxdiagnoseklasse.

---

## Anhang E – Explizite Erfolgsaussage

Der Agent darf P3 nur als abgeschlossen bezeichnen, wenn folgende Aussage wahr und durch ausgeführte Tests belegt ist:

> `@ilic/compiler-wasm` interpretiert INTERLIS-Quelltext nicht mehr selbst. Syntax- und Editorinformationen stammen im Normalbetrieb vollständig aus demselben C++-/ANTLR-Kern wie die native API. Native und WASM liefern für den geprüften Korpus identische Snapshots, die bestehende High-Level-API von Node-LSP, Language Service, Worker und Web-IDE bleibt ohne fachliche Consumer-Migration nutzbar, und die vollständige externe Compiler-Conformance weist keine neue Abweichung auf.
