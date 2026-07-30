# Implementierungsspezifikation P0: Native Tests wirksam machen und CI härten

**Repository:** `https://github.com/edigonzales/ilic-fork`  
**Zielbranch:** aktueller Default-Branch (`main`)  
**Zielgruppe:** autonomer LLM-Coding-Agent mit Zugriff auf Git, CMake, C/C++-Toolchains und GitHub Actions  
**Priorität:** P0 / vor allen weiteren Architektur- und Featurearbeiten  
**Sprache der Implementierung:** C++17, CMake 3.20+, GitHub Actions YAML  
**Status dieser Spezifikation:** umsetzungsfertig

---

## 1. Auftrag an den Coding-Agenten

Du sollst die native Testinfrastruktur des INTERLIS-Compilers `ilic-fork` so korrigieren und härten, dass sämtliche Testprüfungen in **Release-, Debug- und Sanitizer-Builds garantiert ausgeführt werden** und Fehler zuverlässig zu einem von null verschiedenen Prozessstatus führen.

Das Hauptproblem ist, dass zahlreiche native Testprogramme ihre Erwartungen über das Standardmakro `assert(...)` ausdrücken. Bei üblichen CMake-Release-Builds ist `NDEBUG` definiert; dadurch werden diese Prüfungen vom Präprozessor vollständig entfernt. Die Testprogramme können dann mit Exit-Code `0` enden, obwohl keine fachliche Erwartung geprüft wurde.

Die Lösung darf dieses Problem **nicht durch Compiler-Flags oder Präprozessortricks kaschieren**. Stattdessen ist eine kleine, projektinterne, immer aktive Test-Assertion-Schicht einzuführen. Alle betroffenen nativen Testprogramme sind darauf zu migrieren. Zusätzlich sind ein Release-Canary, ein statischer Quellscan sowie separate Debug- und ASan/UBSan-CI-Jobs einzubauen.

Der Auftrag umfasst ausdrücklich:

1. Einführung eines kleinen C++17-kompatiblen, immer aktiven Test-Harness.
2. Migration aller nativen Testprüfungen unter `test/` von `assert(...)` auf dieses Harness.
3. Schutz vor künftiger Wiedereinführung von `<cassert>` beziehungsweise `assert(...)` in nativen Tests.
4. Einen Canary, der beweist, dass eine absichtlich falsche Erwartung auch bei definiertem `NDEBUG` fehlschlägt.
5. Einen nativen Linux-Debug-CI-Job.
6. Einen nativen Linux-Sanitizer-CI-Job mit AddressSanitizer und UndefinedBehaviorSanitizer.
7. Beibehaltung und erfolgreiche Ausführung der bestehenden Release-, Plattform- und WASM-Jobs.

Der Auftrag umfasst **keine Änderungen an der fachlichen INTERLIS-Semantik**, keine Compilerarchitektur-Refaktorierung, keine Repository-Abstraktion, keinen Wechsel des Parsergenerators und keine inkrementelle Kompilation.

---

## 2. Ausgangslage und technische Begründung

### 2.1 Betroffener Buildpfad

Der aktuelle GitHub-Actions-Workflow konfiguriert die nativen Builds auf Linux, macOS und Windows mit:

```text
-DCMAKE_BUILD_TYPE=Release
-DBUILD_TESTING=ON
```

Die nativen Testprogramme werden über CTest gestartet. Viele dieser Programme verwenden jedoch `<cassert>` und `assert(...)` als einzige oder überwiegende Prüfmechanik.

Bei aktiviertem `NDEBUG` expandiert das Standardmakro konzeptionell zu einer Operation ohne Prüfung. Insbesondere dürfen folgende vermeintliche Tests dann erfolgreich enden, obwohl alle Erwartungen aus dem Binärprogramm entfernt wurden:

- C-ABI-Prüfungen
- CompilerSession-Prüfungen
- Diagnostic-Code- und Diagnostic-Quality-Prüfungen
- Syntax- und Semantic-Snapshot-Prüfungen
- Formatter-Prüfungen
- Repository-URI-, XML-, Versions-, Cache-, Traversierungs-, Integrations- und CLI-Prüfungen

Die modellbasierten CMake-Regressionstests, die Exit-Codes und reguläre Ausdrücke über CMake-Skripte prüfen, sind von diesem speziellen Problem nicht betroffen. Sie bleiben unverändert bestehen.

### 2.2 Warum `-UNDEBUG` keine akzeptable Lösung ist

Folgende scheinbar einfache Lösungen sind ausdrücklich verboten:

- `-UNDEBUG` oder `/UNDEBUG` nur für Testtargets
- Entfernen von `NDEBUG` aus `CMAKE_CXX_FLAGS_RELEASE`
- `#undef NDEBUG` vor `#include <cassert>`
- eigenes Umdefinieren des Namens `assert`
- erzwungenes Kompilieren aller Tests als Debug, während die Release-Binärdateien ungetestet bleiben
- globale Änderung der CMake-Release-Flags

Diese Varianten würden die technische Schuld nur verstecken:

- Die Testwirksamkeit bliebe von Buildflags abhängig.
- Ein späterer Toolchain- oder Buildsystemwechsel könnte das Problem zurückbringen.
- Release-spezifische Fehler würden weiterhin schlechter abgesichert.
- Das Testverhalten wäre nicht explizit im Testcode erkennbar.

Die Tests müssen ihre fachlichen Erwartungen über eine projektdefinierte, in jedem Buildmodus aktive Primitive ausdrücken.

---

## 3. Verbindliche Designentscheidung

### 3.1 Kein neues Drittanbieter-Testframework in P0

Für diesen P0-Auftrag ist **kein externes Framework** wie Catch2, GoogleTest oder doctest einzuführen.

Begründung:

- Die bestehenden Tests sind eigenständige Programme mit jeweils einem `main()`.
- Das P0-Ziel ist die Wiederherstellung der tatsächlichen Testwirksamkeit, nicht die vollständige Neuorganisation der Testsuite.
- Ein neues Dependency-Management, Vendoring oder `FetchContent` würde zusätzliche Fehlerquellen und Offline-/Supply-Chain-Fragen erzeugen.
- Die Migration soll klein, kontrollierbar und auf allen bestehenden Plattformen unmittelbar baubar bleiben.

Stattdessen ist ein sehr kleines internes Test-Harness zu erstellen.

### 3.2 Ziel-API des Test-Harness

Erstelle die Datei:

```text
test/support/ilic/test/TestHarness.h
```

Sie muss mindestens folgende öffentliche Makros bereitstellen:

```cpp
ILIC_REQUIRE(expression)
ILIC_REQUIRE_MSG(expression, message)
ILIC_FAIL(message)
```

Die Implementierung liegt im Namespace:

```cpp
namespace ilic::test
```

Die zentrale Funktion soll konzeptionell folgende Signatur besitzen:

```cpp
[[noreturn]] inline void fail(
    const char* expression,
    const char* file,
    int line,
    std::string_view message = {}) noexcept;
```

Die exakte interne Zerlegung darf angepasst werden, die beobachtbaren Eigenschaften sind aber verbindlich.

### 3.3 Verbindliche Eigenschaften des Harness

Das Harness muss:

1. mit C++17 kompilierbar sein;
2. vollständig unabhängig von `NDEBUG` sein;
3. einen Ausdruck exakt einmal auswerten;
4. bei Fehlschlag immer den gesamten Prozess mit einem von null verschiedenen Status beenden;
5. auf Linux, macOS und Windows funktionieren;
6. auch aus einem Worker-Thread heraus zuverlässig den gesamten Testprozess fehlschlagen lassen;
7. vor dem Prozessende eine klar erkennbare Meldung auf `stderr` schreiben;
8. `stderr` explizit flushen;
9. keine Ausnahme aus `fail(...)` propagieren;
10. keine externen Bibliotheken benötigen;
11. keine Abhängigkeit zu `ilic-core` oder anderen Produktivbibliotheken besitzen;
12. keine Prüfung in Release-Builds herausoptimieren lassen.

### 3.4 Warum Prozessbeendigung statt Exception

Mindestens ein bestehender Test (`RepositoryCacheTest.cpp`) führt Prüfungen innerhalb mehrerer `std::thread`-Worker aus. Ein Test-Harness, das bei Fehlschlag lediglich eine Exception wirft, würde in einem solchen Worker ohne lokalen Catch-Block zu `std::terminate()` führen und die gewünschte strukturierte Fehlermeldung möglicherweise verlieren.

Darum soll `ilic::test::fail(...)` nach Ausgabe der Fehlermeldung `std::exit(EXIT_FAILURE)` verwenden. Die Ausgabe muss vor `std::exit` abgeschlossen und geflusht sein.

Automatische lokale Destruktoren müssen im Fehlerfall nicht abgearbeitet werden. Der Testprozess wird ohnehin verworfen. Im Erfolgsfall bleibt das normale Return-Verhalten unverändert.

### 3.5 Verbindliches Ausgabeformat

Die Fehlermeldung muss mindestens folgende Informationen enthalten:

- stabilen Marker `ILIC_TEST_FAILURE`
- Quelldatei
- Quellzeile
- ursprünglichen Ausdruck
- optionale Zusatznachricht

Empfohlenes Format:

```text
ILIC_TEST_FAILURE
  file: /path/to/test.cpp
  line: 123
  expression: result.success
  message: compilation was expected to succeed
```

Bei `ILIC_FAIL(message)` darf `expression` beispielsweise als `<explicit failure>` ausgegeben werden.

Die Ausgabe muss für den Canary maschinenlesbar genug sein. Der Canary wird mindestens nach folgenden Fragmenten suchen:

```text
ILIC_TEST_FAILURE
intentional canary failure
```

### 3.6 Empfohlene Referenzimplementierung

Die folgende Form ist als Orientierung gedacht. Der Agent darf sie geringfügig verbessern, darf aber die oben definierten Eigenschaften nicht verändern:

```cpp
#pragma once

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string_view>

namespace ilic::test {
namespace detail {

inline std::mutex& failureOutputMutex()
{
    static std::mutex mutex;
    return mutex;
}

} // namespace detail

[[noreturn]] inline void fail(
    const char* expression,
    const char* file,
    int line,
    std::string_view message = {}) noexcept
{
    {
        std::lock_guard<std::mutex> lock(detail::failureOutputMutex());
        std::fprintf(stderr, "ILIC_TEST_FAILURE\n");
        std::fprintf(stderr, "  file: %s\n", file != nullptr ? file : "<unknown>");
        std::fprintf(stderr, "  line: %d\n", line);
        std::fprintf(stderr, "  expression: %s\n",
            expression != nullptr ? expression : "<explicit failure>");
        if (!message.empty()) {
            std::fprintf(stderr, "  message: %.*s\n",
                static_cast<int>(message.size()), message.data());
        }
        std::fflush(stderr);
    }
    std::exit(EXIT_FAILURE);
}

} // namespace ilic::test

#define ILIC_REQUIRE(expression) \
    do { \
        if (!(expression)) { \
            ::ilic::test::fail(#expression, __FILE__, __LINE__); \
        } \
    } while (false)

#define ILIC_REQUIRE_MSG(expression, message) \
    do { \
        if (!(expression)) { \
            ::ilic::test::fail(#expression, __FILE__, __LINE__, (message)); \
        } \
    } while (false)

#define ILIC_FAIL(message) \
    do { \
        ::ilic::test::fail(nullptr, __FILE__, __LINE__, (message)); \
    } while (false)
```

Wichtige Reviewpunkte dieser Implementierung:

- Der Ausdruck steht nur einmal in einer ausgewerteten Position.
- Die Zeichenkette `#expression` ist reine Präprozessor-Stringifizierung.
- Die Nachricht wird nur im Fehlerpfad ausgewertet.
- Der Mutex schützt die zusammengehörige Fehlermeldung bei konkurrierenden Worker-Threads.
- Der Lock wird vor `std::exit` wieder freigegeben, da der `lock_guard` in einem eigenen Block liegt.
- Es gibt keine Abhängigkeit von `<cassert>`.

---

## 4. CMake-Integration des Test-Harness

### 4.1 Interface-Target

Innerhalb des bestehenden `if(BUILD_TESTING)`-Blocks in `CMakeLists.txt` ist ein Interface-Target anzulegen:

```cmake
add_library(ilic-test-support INTERFACE)
add_library(ilic::test-support ALIAS ilic-test-support)

target_include_directories(ilic-test-support
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/test/support"
)
```

Optional, aber empfohlen:

```cmake
target_compile_features(ilic-test-support INTERFACE cxx_std_17)
```

Dieses Target enthält keine Produktivquellen und darf ausserhalb von `BUILD_TESTING` nicht erzeugt werden.

### 4.2 Verlinkung der Testtargets

Jedes migrierte C++-Testtarget muss `ilic-test-support` als private Abhängigkeit erhalten.

Beispiel vorher:

```cmake
target_link_libraries(ilic_formatter_test PRIVATE ilic-core)
```

Beispiel nachher:

```cmake
target_link_libraries(ilic_formatter_test
    PRIVATE
        ilic-core
        ilic::test-support
)
```

Tests, die gegen `ilic-capi`, `ilic-repository` oder weitere Targets linken, behalten diese Abhängigkeiten unverändert bei und ergänzen nur `ilic::test-support`.

### 4.3 Keine Produktivabhängigkeit

Es ist verboten:

- `ilic-core` gegen `ilic-test-support` zu linken;
- den Header unter `include/ilic/` zu installieren;
- das Harness in C-ABI-, WASM- oder npm-Artefakte aufzunehmen;
- das Harness ausserhalb von Testtargets als Include-Pfad zu exportieren.

---

## 5. Vollständige Migrationsstrategie für native Tests

### 5.1 Vor der Änderung: verbindlicher Repository-Scan

Der Agent muss vor Änderungen im aktuellen Checkout ausführen:

```bash
rg -n '#include\s*[<"]cassert[>"]|\bassert\s*\(' test docs/examples source include
```

Falls `rg` nicht vorhanden ist, ist eine äquivalente Suche mit `grep` oder einem kurzen Python-Skript zu verwenden.

Der unten angegebene Dateibestand ist die bekannte Ausgangslage. Der Agent muss neue oder inzwischen umbenannte Dateien zusätzlich berücksichtigen.

### 5.2 Bekannte zu migrierende Dateien

Mindestens folgende Dateien sind zu bearbeiten, sofern sie im aktuellen Checkout weiterhin `assert(...)` verwenden:

#### ABI

```text
test/abi/CapiTest.cpp
```

#### Compilerkern

```text
test/core/CompilerSessionTest.cpp
test/core/DiagnosticCodeTest.cpp
test/core/DiagnosticQualityTest.cpp
test/core/SyntaxSnapshotTest.cpp
test/core/SemanticSnapshotTest.cpp
test/core/FormatterTest.cpp
```

#### Repository

```text
test/repository/RepositoryTest.cpp
test/repository/RepositoryUriTest.cpp
test/repository/RepositoryXmlTest.cpp
test/repository/RepositoryVersionTest.cpp
test/repository/RepositoryCacheTest.cpp
test/repository/RepositoryTraversalTest.cpp
test/repository/RepositoryIntegrationTest.cpp
test/repository/RepositoryCliTest.cpp
```

Der Agent muss nach der Migration erneut repositoryweit prüfen. Das Ziel ist:

```text
Kein #include <cassert> in test/**/*.cpp, test/**/*.h oder test/**/*.hpp
Kein Aufruf des Standardmakros assert(...) in diesen Dateien
```

### 5.3 Mechanische Grundregel

Jede fachliche Erwartung:

```cpp
assert(expression);
```

wird mindestens zu:

```cpp
ILIC_REQUIRE(expression);
```

Jede betroffene Datei erhält:

```cpp
#include "ilic/test/TestHarness.h"
```

und entfernt:

```cpp
#include <cassert>
```

### 5.4 Kein blindes Search-and-Replace

Die Migration darf nicht ausschliesslich über ein globales textuelles Ersetzen erfolgen. Für jede Prüfung sind insbesondere folgende Risiken zu beurteilen:

- Dereferenzierung eines Nullpointers
- Zugriff auf `optional::operator->`
- Zugriff über `.front()`, `.back()` oder `operator[]` ohne vorgängige Grössenprüfung
- Iterator-Dereferenzierung nach `std::find_if`
- Zugriff über `.at()` auf einen möglicherweise fehlenden Schlüssel
- mehrere fachlich unterschiedliche Erwartungen in einem einzigen `&&`-Ausdruck
- temporäre Resultate, deren Pointer sofort dereferenziert werden
- Assertions innerhalb von Worker-Threads
- Assertions mit Initializer-Listen und Kommas

Wo eine einzige bestehende Assertion mehrere Vorbedingungen und eine anschliessende Dereferenzierung verbindet, ist sie in logisch getrennte `ILIC_REQUIRE`-Prüfungen aufzuteilen.

Beispiel vorher:

```cpp
assert(result.model->version == "3");
```

Beispiel nachher:

```cpp
ILIC_REQUIRE(result.model != nullptr);
ILIC_REQUIRE(result.model->version == "3");
```

Beispiel vorher:

```cpp
assert(result.success && result.models.size() == 2);
assert(result.models[1].metadata.version == "2.10");
```

Beispiel nachher:

```cpp
ILIC_REQUIRE(result.success);
ILIC_REQUIRE(result.models.size() == 2);
ILIC_REQUIRE(result.models[1].metadata.version == "2.10");
```

Diese Aufteilung verbessert die Fehlerlokalisierung und verhindert, dass der Test selbst mit einem Segmentation Fault statt einer verständlichen Testmeldung endet.

### 5.5 Komma-Ausdrücke und Initializer-Listen

Makros interpretieren nicht geklammerte Kommas als Argumenttrenner. Bestehende doppelte Klammerungen sind deshalb beizubehalten.

Beispiel:

```cpp
ILIC_REQUIRE((site.parentSites ==
    std::vector<std::string>{"https://parent-1", "https://parent-2"}));
```

Es ist verboten, die äussere Klammerung in solchen Fällen zu entfernen.

### 5.6 Assertions in Schleifen und Threads

Prüfungen wie:

```cpp
for (const char character : cached.content)
    assert(character == cached.content.front());
```

werden zu:

```cpp
ILIC_REQUIRE(!cached.content.empty());
for (const char character : cached.content) {
    ILIC_REQUIRE(character == cached.content.front());
}
```

Prüfungen innerhalb eines Worker-Threads werden direkt mit `ILIC_REQUIRE` ersetzt. Es darf kein lokaler Catch-Block erforderlich sein.

Beispiel:

```cpp
writers.emplace_back([&, index] {
    const std::string content(10000, static_cast<char>('A' + index));
    ILIC_REQUIRE(cache.store(uri, content).success);
});
```

### 5.7 Argumentprüfungen in `main`

Prüfungen wie:

```cpp
assert(argc == 2);
```

sollen eine Zusatzmeldung erhalten:

```cpp
ILIC_REQUIRE_MSG(argc == 2,
    "expected exactly one fixture-directory argument");
```

Die Nachricht ist an die jeweilige Datei anzupassen.

### 5.8 Umbenennung von Helpern mit `assert_` im Namen

Helpernamen, die weiterhin `assert` suggerieren, sind semantisch umzubenennen, damit der statische Quellscan eindeutig bleiben kann.

Verbindliche bekannte Umbenennungen:

```text
assert_explicit_semantic_diagnostic_ids
    -> require_explicit_semantic_diagnostic_ids

assert_single_diagnostic
    -> require_single_diagnostic
```

Aufrufstellen sind entsprechend anzupassen.

Es ist kein allgemeines Style-Refactoring vorzunehmen. Snake Case kann beibehalten werden.

---

## 6. Dateispezifische Anforderungen

Dieser Abschnitt beschreibt die Mindestanforderungen pro bekannter Testdatei. Der Agent soll die aktuelle Datei vollständig lesen und die tatsächliche Struktur berücksichtigen.

### 6.1 `test/abi/CapiTest.cpp`

#### Ziel

Alle Lebenszyklus-, JSON- und C-ABI-Prüfungen müssen auch in Release wirksam sein.

#### Konkrete Arbeiten

- `<cassert>` entfernen.
- TestHarness einbinden.
- Alle `assert(...)` migrieren.
- In `resultJson(std::uint32_t result)` vor Zugriff auf das Ergebnis prüfen:
  - Result-Handle ist ungleich null, sofern die API dies garantiert.
  - `ilic_result_json(...)` liefert keinen Nullpointer.
- Sicherstellen, dass `ilic_result_destroy(result)` weiterhin genau einmal aufgerufen wird.
- Session-Lebenszyklus beibehalten:
  - ABI-Version
  - Version-String
  - Session-Erzeugung
  - Source-Upload
  - Compile
  - Parse
  - Analyze
  - Compile-and-Analyze
  - Format
  - ungültiges JSON
  - Cross-File-Diagnostics
  - Source-Entfernung
  - Verhalten nach `session_destroy`
- JSON-Stringprüfungen dürfen nicht abgeschwächt oder entfernt werden.

#### Nicht tun

- C-ABI ändern.
- JSON vollständig mit einer neuen Library parsen.
- Test in mehrere Framework-Testfälle zerlegen.

### 6.2 `test/core/CompilerSessionTest.cpp`

#### Ziel

Der grosse CompilerSession-Regressionsblock muss vollständig wirksam bleiben.

#### Konkrete Arbeiten

- Jede bestehende fachliche Assertion migrieren.
- Vor `.front()`, Indexzugriffen und `relatedInformation[...]` explizite Grössenprüfungen ergänzen.
- Bei Diagnostics zuerst `result.success`, `errorCount` und `diagnostics.size()` prüfen, danach einzelne Felder.
- Bestehende Debugausgaben auf `std::cerr` beibehalten.
- Regex-Prüfungen des Compiler-Transkripts beibehalten.
- Stable Diagnostic Codes, Quellbereiche und Related Information unverändert prüfen.
- Die private Testzugriffsklasse `CompilerSessionTestAccess` unverändert lassen, sofern keine Änderung für die Migration nötig ist.

#### Besondere Reviewfrage

Stelle sicher, dass kein Test nach der Migration zuerst `diagnostics.front()` aufruft und erst danach die Nichtleere des Containers prüft.

### 6.3 `test/core/DiagnosticCodeTest.cpp`

#### Ziel

Die Prüfung stabiler und expliziter Diagnostic IDs muss wirksam sein.

#### Konkrete Arbeiten

- Helper umbenennen:

```text
assert_explicit_semantic_diagnostic_ids
-> require_explicit_semantic_diagnostic_ids
```

- Vor dem Lesen jeder Quelldatei prüfen, dass der `std::ifstream` geöffnet wurde:

```cpp
ILIC_REQUIRE_MSG(input.is_open(),
    "unable to open source file while checking diagnostic IDs: " + file.string());
```

Da `ILIC_REQUIRE_MSG` eine Nachricht erwartet, ist sicherzustellen, dass die temporäre `std::string` während des Funktionsaufrufs gültig bleibt. Das ist bei direkter Übergabe in den Fehlerpfad gegeben.

- Prüfung entfernen, dass `translation_diagnostic_id` nicht mehr vorkommt: **nein**. Diese Prüfung bleibt bestehen und wird migriert.
- Jede gefundene `Log.error(`-Stelle muss weiterhin auf einen expliziten Diagnostic-Code geprüft werden.
- Public-Code-Regex, Eindeutigkeit, Mapping und Range-/Related-Information-Prüfungen beibehalten.

### 6.4 `test/core/DiagnosticQualityTest.cpp`

#### Ziel

Die detaillierten Qualitätsprüfungen der Diagnostics dürfen nicht nur kompilieren, sondern müssen in jedem Buildmodus greifen.

#### Konkrete Arbeiten

- Helper umbenennen:

```text
assert_single_diagnostic
-> require_single_diagnostic
```

- Bestehende Diagnoseausgabe bei unerwarteter Anzahl beibehalten.
- Im Helper die Prüfungen in dieser Reihenfolge ausführen:
  1. `!result.success`
  2. `result.errorCount == 1`
  3. `result.diagnostics.size() == 1`
  4. Zugriff auf `front()`
  5. Code
  6. gültiger Range
  7. erwartete Zeile
  8. Mindestanzahl `relatedInformation`
  9. erwartete Meldungsfragmente
  10. keine Platzhalterfragmente wie `TOP` oder `nullptr`
- Für abgeleitete Prüfungen ausserhalb des Helpers vor `[1]` oder `.front()` die Grösse explizit absichern.
- Die Testfälle und erwarteten Diagnostic Codes nicht verändern.

### 6.5 `test/core/SyntaxSnapshotTest.cpp`

#### Ziel

Snapshot-, Import-, Unicode- und Error-Recovery-Prüfungen müssen in Release aktiv sein.

#### Konkrete Arbeiten

- Vor `imports[0]`, `imports[1]`, `importReferences[0]` und `importReferences[1]` die exakte oder minimale Grösse prüfen.
- UTF-8-/UTF-16- und Byte-Offset-Erwartungen beibehalten.
- Beim kaputten Dokument vor `diagnostics.front()` Nichtleere prüfen.

### 6.6 `test/core/SemanticSnapshotTest.cpp`

#### Ziel

Die umfassenden Semantic-Snapshot-, Dokumentations-, Diagramm-, Symbol-, Reference- und Dependency-Prüfungen müssen in Release aktiv bleiben.

#### Konkrete Arbeiten

- Sämtliche Assertions migrieren.
- Iteratoren aus `std::find_if` immer vor Dereferenzierung gegen `.end()` prüfen.
- `.front()` nur nach Nichtleere beziehungsweise passender Grössenprüfung verwenden.
- `documentVersions.at(uri)` darf verwendet werden, nachdem `count(uri) == 1` oder `find(uri) != end()` geprüft wurde. Alternativ kann die bestehende `at`-Erwartung als Teil des Tests bestehen bleiben, aber eine explizite Vorprüfung liefert die bessere Fehlermeldung.
- Bei Diagramm- und Dokumentationsstrukturen zuerst die Containergrössen absichern.
- `compileInvocationCount`-Erwartung beibehalten.
- Sicherstellen, dass das irrelevante kaputte Dokument weiterhin nicht im analysierten Snapshot erscheint.
- Keine Erwartung zur fachlichen Modellprojektion entfernen.

#### Nicht tun

- Den grossen Test in diesem P0 zwingend in mehrere Dateien zerlegen.
- Semantic-Snapshot-Produktionscode verändern, nur um einen Test leichter zu schreiben.

### 6.7 `test/core/FormatterTest.cpp`

#### Ziel

Formatter-Erfolg, Kommentar-/Metadaten-Erhalt, Einrückung, Idempotenz und anschliessende Kompilierbarkeit müssen geprüft werden.

#### Konkrete Arbeiten

Die kombinierte Prüfung:

```cpp
formatted.success && formatted.applicable
```

in zwei Prüfungen zerlegen:

```cpp
ILIC_REQUIRE(formatted.success);
ILIC_REQUIRE(formatted.applicable);
```

Die übrigen String- und Idempotenzprüfungen beibehalten.

### 6.8 `test/repository/RepositoryTest.cpp`

#### Ziel

Repository-Auflösung und Übergabe der aufgelösten Quellen an `CompilerSession` prüfen.

#### Konkrete Arbeiten

- `argc` mit verständlicher Meldung prüfen.
- `catalog.size()` vor Zugriffen prüfen.
- `resolved.models.size()` vor `.front()` und `.back()` prüfen.
- Sicherstellen, dass `rootUri` nach der Schleife nicht leer ist, bevor es als Root verwendet wird.
- Compilererfolg prüfen.

### 6.9 `test/repository/RepositoryUriTest.cpp`

#### Ziel

URI-Normalisierung, Auflösung, Cache Keys, lokale Pfade und Path-Safety müssen wirksam geprüft werden.

#### Konkrete Arbeiten

- Jede `std::optional<RepositoryUri>` vor `->` explizit prüfen, sofern nicht bereits im gleichen kurzschliessenden Ausdruck garantiert.
- Zur besseren Diagnose kombinierte Ausdrücke aufteilen:

```cpp
ILIC_REQUIRE(http.has_value());
ILIC_REQUIRE(http->kind() == RepositoryUriKind::Https);
```

- Plattformbedingte Windows-Prüfung beibehalten.
- Listenprüfungen für sichere und unsichere Pfade beibehalten.

### 6.10 `test/repository/RepositoryXmlTest.cpp`

#### Ziel

Repository-XML-Parsing und Diagnostik müssen geprüft werden.

#### Konkrete Arbeiten

- Vor `index.models.front()` Grösse prüfen.
- Kombinierte Feldprüfungen vorzugsweise in einzelne Erwartungen zerlegen, damit der Fehlerort eindeutig ist.
- Vor `diagnostics[0]` Grösse prüfen.
- Parent-/Subsidiary-Site-Vektorvergleiche mit doppelter Klammerung beibehalten.
- Invalid-XML-Fall beibehalten.

### 6.11 `test/repository/RepositoryVersionTest.cpp`

#### Ziel

Versionsketten, Mehrdeutigkeiten, Browse-only-Filter und Sprachpräferenz müssen geprüft werden.

#### Konkrete Arbeiten

- Jedes Resultat aus `selectLatestVersion(...)` zunächst einer lokalen Variable zuweisen, wenn anschliessend `model->...` gelesen wird.
- Vor jeder Pointerdereferenzierung:

```cpp
ILIC_REQUIRE(result.model != nullptr);
```

- Im erwarteten Nullfall explizit `result.model == nullptr` prüfen.
- Diagnostikgrössen separat prüfen.
- Den Vektorvergleich der Sprachpräferenz mit doppelter Klammerung beibehalten.

### 6.12 `test/repository/RepositoryCacheTest.cpp`

#### Ziel

Cache-Speicherung, konkurrierende Writer, Cache-Hits, Fehlerfälle, stale fallback, Offline-Verhalten und Materialisierungsfallback müssen in Release und unter Sanitizern geprüft werden.

#### Konkrete Arbeiten

- Thread-Assertions direkt durch `ILIC_REQUIRE` ersetzen.
- Nach dem Join prüfen, dass der gelesene Content nicht leer ist, bevor `.front()` verwendet wird.
- Temporärverzeichnis am Ende weiterhin best-effort entfernen.
- Bei einem Testfehlschlag darf das Harness den Prozess beenden; eine Aufräumgarantie ist dann nicht erforderlich.
- Alle Transport-Request-Zähler beibehalten.
- Keine Sanitizer-Suppression in dieser Testdatei hinzufügen.

### 6.13 `test/repository/RepositoryTraversalTest.cpp`

#### Ziel

Suchreihenfolge, Parent-/Subsidiary-Traversierung und Cycle-Schutz müssen geprüft werden.

#### Konkrete Arbeiten

- Kombinierte `found && metadata...`-Prüfungen aufteilen.
- Vektorvergleiche mit doppelter Klammerung beibehalten.
- Anzahl und Reihenfolge der angefragten Repositories unverändert prüfen.

### 6.14 `test/repository/RepositoryIntegrationTest.cpp`

#### Ziel

End-to-End-Auflösung lokaler Fixture-Repositories inklusive Versionen, Shared Files, Prüfsummen, Zyklen, unsicheren Pfaden und Recoverable Warnings muss aktiv geprüft werden.

#### Konkrete Arbeiten

- `argc` mit Meldung prüfen.
- Vor jedem `models[n]` die passende Grösse prüfen.
- Rückgabewerte von `findDiagnostic(...)` einer lokalen Pointervariable zuweisen und vor Dereferenzierung prüfen.
- Beispiel:

```cpp
const auto* indexDiagnostic = findDiagnostic(recovered, "ILIC-REPO-INDEX");
ILIC_REQUIRE(indexDiagnostic != nullptr);
ILIC_REQUIRE(indexDiagnostic->severity == ilic::DiagnosticSeverity::Warning);
```

- Cycle-Message und alle Codes unverändert prüfen.

### 6.15 `test/repository/RepositoryCliTest.cpp`

#### Ziel

CLI-Konfigurationslogik für Sprachversionen, Defaults, fehlende Inputs und ungültige Werte muss aktiv geprüft werden.

#### Konkrete Arbeiten

- Bei `std::optional<std::string>` vor `->find(...)` explizit `has_value()` prüfen.
- Vor `.front()`/`.back()` die erwartete Grösse prüfen.
- Default-Repository- und Deduplizierungsprüfungen beibehalten.

---

## 7. Schutz gegen künftige `assert`-Regressionen

### 7.1 Neues CMake-Prüfskript

Erstelle:

```text
cmake/check_no_cassert_in_tests.cmake
```

Das Skript erhält:

```text
-DILIC_SOURCE_DIR=<repository root>
```

Es muss rekursiv mindestens folgende Dateiendungen unter `test/` prüfen:

```text
*.cpp
*.cc
*.cxx
*.h
*.hpp
```

Es muss fehlschlagen, wenn eine Datei enthält:

1. einen Include von `<cassert>` oder `"cassert"`;
2. einen Aufruf des Standardnamens `assert` gefolgt von optionalem Whitespace und `(`.

Es soll alle gefundenen Verstösse sammeln und gemeinsam ausgeben, nicht nur den ersten.

Empfohlene Fehlermeldung:

```text
Native tests must use ilic/test/TestHarness.h instead of <cassert>/assert().
Violations:
  test/foo/Test.cpp: includes <cassert>
  test/bar/Test.cpp: calls assert(...)
```

### 7.2 Registrierung als CTest

Im `BUILD_TESTING`-Block:

```cmake
add_test(
    NAME ilic_no_cassert_in_tests
    COMMAND "${CMAKE_COMMAND}"
        "-DILIC_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_no_cassert_in_tests.cmake"
)
```

### 7.3 Anforderungen an den Regex-Scan

Der Scan soll Identifier wie `my_assertion_value` nicht fälschlich als Aufruf erkennen. Helper mit exaktem `assert_...`-Präfix werden dennoch wie oben beschrieben umbenannt, um die Intention klar zu halten.

Kommentare im neu geschriebenen Testcode sollen nicht die Zeichenfolge `assert(` enthalten, damit der Scan einfach und deterministisch bleiben kann.

Der Scan darf gebündelte Drittanbieterquellen unter `lib/` nicht untersuchen.

---

## 8. Release-Canary für die Testwirksamkeit

### 8.1 Zweck

Es ist nicht ausreichend, nur alle vorhandenen Aufrufe umzubenennen. Die CI muss aktiv beweisen, dass:

- das neue Harness bei einer falschen Erwartung einen Fehlerstatus erzeugt;
- dies auch bei definiertem `NDEBUG` gilt;
- die Fehlermeldung den erwarteten Marker enthält;
- der Erfolgsfall weiterhin Exit-Code `0` liefert.

### 8.2 Canary-Programm

Erstelle:

```text
test/support/TestHarnessCanary.cpp
```

Verbindliches Verhalten:

```text
TestHarnessCanary --pass
    -> ILIC_REQUIRE(true)
    -> Exit-Code 0

TestHarnessCanary --fail
    -> ILIC_REQUIRE_MSG(false, "intentional canary failure")
    -> Exit-Code ungleich 0
    -> stderr enthält ILIC_TEST_FAILURE
    -> stderr enthält intentional canary failure

anderes Argument oder falsche Argumentzahl
    -> Exit-Code ungleich 0
```

Das Programm muss das neue Harness verwenden und darf kein `<cassert>` enthalten.

### 8.3 Canary-Buildtarget

Im `BUILD_TESTING`-Block:

```cmake
add_executable(ilic_test_harness_canary
    test/support/TestHarnessCanary.cpp
)
target_link_libraries(ilic_test_harness_canary
    PRIVATE
        ilic::test-support
)
```

Zusätzlich muss für dieses Target `NDEBUG` **explizit** definiert werden, selbst wenn der umgebende Build Debug ist:

```cmake
target_compile_definitions(ilic_test_harness_canary PRIVATE NDEBUG=1)
```

Damit beweist auch der Debug-Job, dass das Harness nicht von der Standard-Assertion abhängt.

### 8.4 Canary-Treiberskript

Erstelle:

```text
cmake/run_test_harness_canary.cmake
```

Parameter:

```text
-DILIC_TEST_HARNESS_CANARY=<path to executable>
```

Ablauf:

1. Canary mit `--pass` ausführen.
2. Verlangen, dass Exit-Code `0` ist.
3. Canary mit `--fail` ausführen.
4. Verlangen, dass Exit-Code ungleich `0` ist.
5. Kombinierte Ausgabe aus stdout und stderr auf `ILIC_TEST_FAILURE` prüfen.
6. Ausgabe auf `intentional canary failure` prüfen.
7. Bei jeder Abweichung `message(FATAL_ERROR ...)` verwenden.

### 8.5 Warum nicht nur `WILL_FAIL TRUE`

Der Canary soll nicht lediglich einen erwarteten Fehlerstatus invertieren. Das CMake-Skript muss zusätzlich die charakteristische Harness-Ausgabe prüfen. Dadurch wird verhindert, dass der Canary zufällig durch einen Loaderfehler, fehlende DLL, einen Crash vor `main()` oder eine andere irrelevante Ursache „erfolgreich fehlschlägt“.

### 8.6 CTest-Registrierung

```cmake
add_test(
    NAME ilic_test_harness_canary
    COMMAND "${CMAKE_COMMAND}"
        "-DILIC_TEST_HARNESS_CANARY=$<TARGET_FILE:ilic_test_harness_canary>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_test_harness_canary.cmake"
)
```

---

## 9. Sanitizer-Unterstützung in CMake

### 9.1 Neue Option

Ergänze auf Top-Level nahe den bestehenden Projektoptionen:

```cmake
option(ILIC_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer for project targets"
    OFF
)
```

### 9.2 Neue CMake-Datei

Erstelle:

```text
cmake/IlicSanitizers.cmake
```

Diese Datei soll die Sanitizer-Konfiguration zentral kapseln.

Empfohlene öffentliche Funktion:

```cmake
function(ilic_configure_sanitizers)
```

Sie wird nach Definition der Option und vor Anlage der Projektziele aufgerufen.

### 9.3 Toolchain-Regeln

Wenn `ILIC_ENABLE_SANITIZERS=OFF`, darf die Funktion keine Buildflags verändern.

Wenn `ILIC_ENABLE_SANITIZERS=ON`:

- Unterstützt werden GNU und Clang-kompatible Compiler.
- Für MSVC ist mit klarer Fehlermeldung abzubrechen, solange keine getestete MSVC-Sanitizer-Konfiguration implementiert wird.
- AppleClang muss nicht Teil des P0-CI-Jobs sein. Die Flags sollen aber nicht unnötig blockiert werden, sofern sie unterstützt werden.

Mögliche Prüfung:

```cmake
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR
        "ILIC_ENABLE_SANITIZERS currently requires GCC or Clang")
endif()
```

### 9.4 Verbindliche Compile-Flags

Für C- und C++-Quellen:

```text
-fsanitize=address,undefined
-fno-omit-frame-pointer
-fno-sanitize-recover=all
```

### 9.5 Verbindliche Link-Flags

```text
-fsanitize=address,undefined
-fno-sanitize-recover=all
```

### 9.6 Geltungsbereich

Die Sanitizer-Instrumentierung muss alle im Repository gebauten, projektkontrollierten nativen Quellen einschliessen, insbesondere:

- eingebundene ANTLR-Runtime-Quellen
- `ilic-core`
- `ilic-capi`
- `ilic-repository`
- CLI-Executables
- native Testexecutables

Importierte Systembibliotheken wie libcurl müssen nicht neu instrumentiert werden.

Eine einfache directory-scope-Konfiguration über `add_compile_options(...)` und `add_link_options(...)` ist für P0 zulässig, sofern sie nachweislich nur bei aktivierter Option greift und den normalen Build nicht verändert.

Alternativ ist ein Interface-Target zulässig. In diesem Fall muss der Agent sorgfältig sicherstellen, dass die Linkoptionen bis zu jedem finalen Executable propagiert werden. Eine halb instrumentierte Konfiguration ist nicht akzeptabel.

### 9.7 Nicht zulässig

- `-fsanitize=address` nur auf Testexecutables, während `ilic-core` uninstrumentiert bleibt
- `detect_leaks=0` als Standard
- pauschale Suppressionen ohne konkret nachgewiesenen externen False Positive
- Weiterlaufen nach Undefined Behavior
- Sanitizer nur kompilieren, aber im CI nicht ausführen

---

## 10. Änderungen am GitHub-Actions-Workflow

Bearbeite:

```text
.github/workflows/ci.yml
```

Die bestehende Release-Matrix und der bestehende WASM-/Package-Job bleiben erhalten.

### 10.1 Bestehenden nativen Job klar benennen

Der bestehende Job kann intern `native` heissen, sein sichtbarer Name sollte aber Release klar ausdrücken, beispielsweise:

```yaml
name: Native Release (${{ matrix.name }})
```

Keine bestehende Plattform darf entfernt werden:

- Linux x86_64
- macOS ARM64
- Windows x86_64

Diese Jobs müssen weiterhin `BUILD_TESTING=ON` konfigurieren und `ctest --output-on-failure` ausführen. Dadurch läuft der neue Canary auch in allen Release-Builds.

### 10.2 Neuer Job: Linux Debug

Füge einen separaten Job hinzu, beispielsweise:

```yaml
native-debug-linux:
  name: Native Debug (Linux x86_64)
```

Verbindliche Eigenschaften:

- `runs-on: ubuntu-latest`
- Timeout mindestens 45 Minuten
- Checkout wie in den übrigen Jobs
- Installation von `libcurl4-openssl-dev` und `ninja-build`
- CMake Generator Ninja
- `CMAKE_BUILD_TYPE=Debug`
- `BUILD_TESTING=ON`
- `ILIC_STATIC_DISTRIBUTION=OFF`
- vollständiger Build
- vollständiger CTest-Lauf mit `--output-on-failure`

Empfohlene Befehle:

```yaml
- name: Configure native debug build
  run: >-
    cmake -S . -B build/debug -G Ninja
    -DCMAKE_BUILD_TYPE=Debug
    -DBUILD_TESTING=ON
    -DILIC_STATIC_DISTRIBUTION=OFF

- name: Build native debug compiler and tests
  run: cmake --build build/debug --parallel

- name: Run native debug tests
  run: ctest --test-dir build/debug --output-on-failure
```

### 10.3 Neuer Job: Linux ASan/UBSan

Füge einen separaten Job hinzu, beispielsweise:

```yaml
native-sanitizers-linux:
  name: Native ASan+UBSan (Linux x86_64)
```

Verbindliche Eigenschaften:

- `runs-on: ubuntu-latest`
- Timeout mindestens 45, vorzugsweise 60 Minuten
- Installation von:
  - `clang`
  - `libcurl4-openssl-dev`
  - `ninja-build`
- C-Compiler `clang`
- C++-Compiler `clang++`
- Debug-Build
- `BUILD_TESTING=ON`
- `ILIC_ENABLE_SANITIZERS=ON`
- `ILIC_STATIC_DISTRIBUTION=OFF`
- vollständiger Build
- vollständiger CTest-Lauf

Empfohlene Konfiguration:

```yaml
- name: Configure sanitizer build
  run: >-
    cmake -S . -B build/sanitizers -G Ninja
    -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_C_COMPILER=clang
    -DCMAKE_CXX_COMPILER=clang++
    -DBUILD_TESTING=ON
    -DILIC_ENABLE_SANITIZERS=ON
    -DILIC_STATIC_DISTRIBUTION=OFF
```

### 10.4 Sanitizer-Umgebungsvariablen

Der CTest-Schritt muss mindestens verwenden:

```yaml
env:
  ASAN_OPTIONS: >-
    detect_leaks=1:halt_on_error=1:strict_string_checks=1
  UBSAN_OPTIONS: >-
    halt_on_error=1:print_stacktrace=1
```

Optional und empfohlen:

```yaml
  LSAN_OPTIONS: >-
    exitcode=23
```

Der Agent darf Optionsstrings ohne Whitespace schreiben, falls YAML-Folding Probleme verursacht.

### 10.5 Parallelität des Sanitizer-Testlaufs

Der Sanitizer-Testlauf darf parallelisiert werden, sofern er stabil ist. Bei instabilen Ressourcenverbräuchen ist `ctest -j2` zulässig. Eine Reduktion der getesteten Testmenge ist nicht zulässig.

Empfehlung:

```yaml
run: ctest --test-dir build/sanitizers --output-on-failure -j2
```

### 10.6 WASM-Job

Der bestehende WASM-/npm-Job bleibt funktional unverändert. Insbesondere:

- `ILIC_ENABLE_SANITIZERS` bleibt standardmässig `OFF`.
- Das Test-Harness wird nicht in den WASM-Build aufgenommen, da `BUILD_TESTING=OFF` verwendet wird.
- Der gepinnte Emscripten-Build bleibt bestehen.

---

## 11. Lokale Verifikation durch den Coding-Agenten

Der Agent muss vor Abschluss mindestens die folgenden Prüfungen durchführen. Falls eine Plattform im Agenten-Environment nicht verfügbar ist, ist dies im Abschlussbericht explizit zu nennen; Linux-Release, Linux-Debug und Linux-Sanitizer sind jedoch Pflicht, sofern eine normale Linux-Toolchain verfügbar ist.

### 11.1 Quellscan

```bash
rg -n '#include\s*[<"]cassert[>"]|\bassert\s*\(' test
```

Erwartung:

```text
keine Treffer
```

Zusätzlich:

```bash
rg -n 'ILIC_REQUIRE|ILIC_REQUIRE_MSG|ILIC_FAIL' test
```

Erwartung: Treffer in allen migrierten nativen Testdateien.

### 11.2 Release-Build

```bash
cmake -S . -B build/p0-release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/p0-release --parallel
ctest --test-dir build/p0-release --output-on-failure
```

Erwartung:

- alle Tests erfolgreich;
- `ilic_test_harness_canary` erfolgreich;
- `ilic_no_cassert_in_tests` erfolgreich;
- keine Regression der bestehenden CMake-Modelltests.

### 11.3 Debug-Build

```bash
cmake -S . -B build/p0-debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/p0-debug --parallel
ctest --test-dir build/p0-debug --output-on-failure
```

### 11.4 Sanitizer-Build

```bash
CC=clang CXX=clang++ \
cmake -S . -B build/p0-sanitizers -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON \
  -DILIC_ENABLE_SANITIZERS=ON \
  -DILIC_STATIC_DISTRIBUTION=OFF

cmake --build build/p0-sanitizers --parallel

ASAN_OPTIONS='detect_leaks=1:halt_on_error=1:strict_string_checks=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
LSAN_OPTIONS='exitcode=23' \
ctest --test-dir build/p0-sanitizers --output-on-failure -j2
```

### 11.5 Direkter Canary-Nachweis

Zusätzlich zum registrierten CTest ist der Canary manuell auszuführen:

```bash
build/p0-release/ilic_test_harness_canary --pass
```

Erwartung: Exit-Code `0`.

```bash
build/p0-release/ilic_test_harness_canary --fail
```

Erwartung:

- Exit-Code ungleich `0`;
- Ausgabe enthält `ILIC_TEST_FAILURE`;
- Ausgabe enthält `intentional canary failure`.

Unter Multi-Config-Generatoren ist der tatsächliche Konfigurationsunterordner zu berücksichtigen.

### 11.6 Negativnachweis des Quellscans

Der Agent soll lokal kurz verifizieren, dass der Scan tatsächlich anschlägt:

1. Temporär in einer Testdatei einen `<cassert>`-Include oder `assert(false)` einfügen.
2. Nur `ctest -R ilic_no_cassert_in_tests --output-on-failure` ausführen.
3. Verifizieren, dass der Test fehlschlägt und die Datei nennt.
4. Temporäre Änderung vollständig zurücknehmen.

Diese temporäre Änderung darf nicht committed werden.

---

## 12. Akzeptanzkriterien

Die Implementierung gilt nur dann als abgeschlossen, wenn **alle** folgenden Kriterien erfüllt sind.

### 12.1 Funktionale Kriterien

- [ ] Jede native Testprüfung unter `test/` ist unabhängig von `NDEBUG`.
- [ ] Kein betroffenes Testprogramm enthält `<cassert>`.
- [ ] Kein betroffenes Testprogramm ruft das Standardmakro `assert(...)` auf.
- [ ] Alle bisherigen fachlichen Erwartungen sind erhalten.
- [ ] Pointer, Optionals, Iteratoren und Container werden vor potenziell unsicherer Dereferenzierung geprüft.
- [ ] Assertions in Worker-Threads führen zuverlässig zu einem Prozessfehler.
- [ ] Der absichtlich fehlschlagende Canary liefert auch mit `NDEBUG=1` einen Fehlerstatus.
- [ ] Der Canary-Treiber validiert den Harness-Marker und die Intentional-Failure-Nachricht.
- [ ] Der statische Quellscan verhindert künftige Wiedereinführung von `<cassert>`/`assert(...)`.

### 12.2 Buildkriterien

- [ ] Linux Release kompiliert und alle CTests bestehen.
- [ ] Linux Debug kompiliert und alle CTests bestehen.
- [ ] Linux ASan+UBSan kompiliert und alle CTests bestehen.
- [ ] Bestehende macOS- und Windows-Release-Jobs bleiben konfiguriert.
- [ ] Bestehender WASM-/npm-Job bleibt konfiguriert und wird durch die Änderung nicht unnötig beeinflusst.
- [ ] Normaler Build mit `ILIC_ENABLE_SANITIZERS=OFF` erhält keine Sanitizer-Flags.
- [ ] Build mit ungeeigneter Toolchain und `ILIC_ENABLE_SANITIZERS=ON` bricht mit verständlicher Meldung ab.

### 12.3 Qualitätskriterien

- [ ] Keine externe Testframework-Abhängigkeit wurde hinzugefügt.
- [ ] `CMAKE_CXX_STANDARD` bleibt 17.
- [ ] Keine Produktiv-API wurde verändert.
- [ ] Keine fachliche Compilerregel wurde verändert.
- [ ] Keine Diagnostic Codes oder erwarteten Meldungsinhalte wurden abgeschwächt.
- [ ] Keine Sanitizer-Warnung wurde ohne dokumentierten Grund unterdrückt.
- [ ] Kein globaler Release-Flag wurde manipuliert, um Standard-Assertions zu reaktivieren.
- [ ] CMake- und YAML-Änderungen sind auf den P0-Zweck begrenzt.

---

## 13. Verbotene Implementierungsvarianten

Ein Review muss die Änderung ablehnen, wenn eine der folgenden Varianten verwendet wird:

1. Nur `NDEBUG` für Testtargets entfernen.
2. `assert` über ein eigenes Makro gleichen Namens ersetzen.
3. `#define assert ILIC_REQUIRE` verwenden.
4. Tests nur in Debug ausführen und Release-Tests entfernen.
5. Bestehende Assertions löschen, ohne äquivalente Erwartungen einzuführen.
6. Tests durch reine Smoke-Tests ersetzen.
7. `WILL_FAIL TRUE` als einzigen Canary-Nachweis verwenden.
8. ASan ohne Instrumentierung des Compilerkerns verwenden.
9. Leak Detection pauschal deaktivieren.
10. Sanitizer-Job mit einer reduzierten oder gefilterten Testsuite ausführen.
11. `BUILD_TESTING=OFF` in einem nativen P0-CI-Job verwenden.
12. Produktivcode nur für die Testmigration semantisch verändern.
13. Test-Harness unter `include/ilic` als öffentliche API installieren.
14. Ein neues Paketmanagement oder Online-`FetchContent` nur für das Harness einführen.

---

## 14. Empfohlene Arbeitsreihenfolge

Der Agent soll in folgender Reihenfolge arbeiten:

### Schritt 1: Bestandsaufnahme

- aktuellen Branch und Commit notieren;
- `assert`-/`cassert`-Scan durchführen;
- alle betroffenen Targets aus `CMakeLists.txt` zuordnen;
- aktuellen CI-Workflow vollständig lesen.

### Schritt 2: Harness implementieren

- Header erstellen;
- Interface-Target hinzufügen;
- Canary-Programm und Canary-CMake-Skript erstellen;
- Canary als CTest registrieren;
- Canary in Release lokal verifizieren.

### Schritt 3: Quellscan implementieren

- CMake-Scan-Skript erstellen;
- als CTest registrieren;
- positiven und temporär negativen Fall prüfen.

### Schritt 4: Kleine Testdateien migrieren

Zuerst die übersichtlichen Dateien:

- FormatterTest
- SyntaxSnapshotTest
- RepositoryUriTest
- RepositoryXmlTest
- RepositoryVersionTest
- RepositoryTraversalTest
- RepositoryCliTest

Nach jedem sinnvollen Block bauen und testen.

### Schritt 5: Integrations- und Threadtests migrieren

- RepositoryTest
- RepositoryCacheTest
- RepositoryIntegrationTest

Besonders auf sichere Containerzugriffe und Threadverhalten achten.

### Schritt 6: Grosse Kern- und ABI-Tests migrieren

- CapiTest
- DiagnosticCodeTest
- DiagnosticQualityTest
- CompilerSessionTest
- SemanticSnapshotTest

Keine Erwartung auslassen. Diff sorgfältig gegen die Ausgangsversion vergleichen.

### Schritt 7: Sanitizer-CMake-Unterstützung

- Option und Modul hinzufügen;
- Release/Debug ohne Option prüfen;
- Sanitizer-Build lokal prüfen;
- tatsächliche ASan/UBSan-Befehlszeilen bei Bedarf mit verbose build kontrollieren.

### Schritt 8: CI erweitern

- bestehenden Release-Job erhalten;
- Debug-Job hinzufügen;
- Sanitizer-Job hinzufügen;
- WASM-Job unverändert lassen;
- YAML-Syntax validieren.

### Schritt 9: Gesamtabnahme

- vollständiger Release-CTest;
- vollständiger Debug-CTest;
- vollständiger Sanitizer-CTest;
- finaler Quellscan;
- `git diff --check`;
- sicherstellen, dass keine Buildartefakte committed werden.

---

## 15. Erwartete Dateien im finalen Diff

Mindestens folgende neue Dateien werden erwartet:

```text
test/support/ilic/test/TestHarness.h
test/support/TestHarnessCanary.cpp
cmake/run_test_harness_canary.cmake
cmake/check_no_cassert_in_tests.cmake
cmake/IlicSanitizers.cmake
```

Mindestens folgende bestehende Dateien werden voraussichtlich geändert:

```text
CMakeLists.txt
.github/workflows/ci.yml

test/abi/CapiTest.cpp

test/core/CompilerSessionTest.cpp
test/core/DiagnosticCodeTest.cpp
test/core/DiagnosticQualityTest.cpp
test/core/SyntaxSnapshotTest.cpp
test/core/SemanticSnapshotTest.cpp
test/core/FormatterTest.cpp

test/repository/RepositoryTest.cpp
test/repository/RepositoryUriTest.cpp
test/repository/RepositoryXmlTest.cpp
test/repository/RepositoryVersionTest.cpp
test/repository/RepositoryCacheTest.cpp
test/repository/RepositoryTraversalTest.cpp
test/repository/RepositoryIntegrationTest.cpp
test/repository/RepositoryCliTest.cpp
```

Falls der aktuelle Repositoryzustand weitere native Testdateien mit `assert(...)` enthält, müssen sie ebenfalls im Diff erscheinen.

---

## 16. Review-Checkliste auf Methoden- und Ausdrucksebene

Der Agent muss vor Abschluss jede migrierte Datei mit folgenden Fragen prüfen:

1. Wird jeder frühere `assert`-Ausdruck durch mindestens eine immer aktive Prüfung ersetzt?
2. Wurde ein zusammengesetzter Ausdruck versehentlich abgeschwächt?
3. Wird eine Funktion mit Seiteneffekt nur einmal ausgewertet?
4. Wird ein temporäres Ergebnis mehrfach neu berechnet, obwohl es lokal gespeichert werden sollte?
5. Erfolgt `.front()` erst nach einer Nichtleere-/Grössenprüfung?
6. Erfolgt `.back()` erst nach einer Nichtleere-/Grössenprüfung?
7. Erfolgt `[n]` erst nach `size() > n`?
8. Erfolgt Pointerdereferenzierung erst nach Nullprüfung?
9. Erfolgt Optional-Dereferenzierung erst nach `has_value()`?
10. Erfolgt Iterator-Dereferenzierung erst nach Vergleich mit `.end()`?
11. Bleibt Short-Circuit-Semantik erhalten?
12. Sind Initializer-List-Ausdrücke ausreichend geklammert?
13. Funktioniert die Prüfung in einem Worker-Thread?
14. Enthält die Datei weder `<cassert>` noch Standard-`assert`?
15. Sind vorhandene hilfreiche `std::cerr`-Diagnosen erhalten?
16. Sind erwartete Diagnostic Codes unverändert?
17. Sind erwartete Range-Zeilen und -Spalten unverändert?
18. Sind Related-Information-Prüfungen unverändert oder stärker?
19. Ist der erfolgreiche Rückgabestatus des Testprogramms weiterhin `0`?
20. Entsteht bei Fehler immer ein von null verschiedener Status?

---

## 17. Umgang mit tatsächlich gefundenen Sanitizer-Fehlern

Der Sanitizer-Job kann reale Speicher- oder Undefined-Behavior-Probleme sichtbar machen. Dafür gilt:

### 17.1 Kleine, eindeutig lokale Testfehler

Beispiele:

- unsicherer Testzugriff auf leeren Container;
- Lebensdauerfehler in neuem Testcode;
- temporäres `string_view` wird ausserhalb des unmittelbaren Aufrufs gespeichert.

Diese Fehler sind im Rahmen dieses P0-Auftrags zu korrigieren.

### 17.2 Eindeutige kleine Produktivfehler

Ein kleiner, klar lokalisierter Produktivfehler darf korrigiert werden, wenn alle folgenden Bedingungen erfüllt sind:

- Er wird deterministisch durch die vollständige bestehende Testsuite ausgelöst.
- Die Korrektur verändert keine beabsichtigte INTERLIS-Semantik.
- Die Änderung ist klein und separat erklärbar.
- Ein Regressionstest wird ergänzt oder eine bestehende Prüfung deckt ihn klar ab.

Der Abschlussbericht muss eine solche Änderung ausdrücklich nennen.

### 17.3 Grössere Altlasten oder Leak-Komplexe

Wenn ASan/LSan grössere Ownership-Probleme im globalen Metamodell sichtbar macht, die nicht risikoarm innerhalb P0 lösbar sind:

- Nicht heimlich `detect_leaks=0` setzen.
- Nicht pauschal suppressen.
- Den Fehler mit vollständigem Stacktrace dokumentieren.
- Die kleinstmögliche, eng begrenzte Suppression ist nur zulässig, wenn nachgewiesen wird, dass der Leak aus einer externen Systembibliothek stammt und nicht aus `ilic`.
- Bei einem Leak im eigenen Compilercode gilt der CI-Job grundsätzlich als fehlgeschlagen. Die P0-Änderung darf dann in getrennte, reviewbare Teilcommits aufgeteilt werden, aber der finale Zielzustand dieser Spezifikation bleibt ein grüner Sanitizer-Job.

Der Agent soll nicht eigenmächtig die P0-Anforderung abschwächen.

---

## 18. Empfohlene Commit-Struktur

Bevorzugt drei logisch getrennte Commits:

### Commit 1

```text
test: add always-on native test harness and canaries
```

Enthält:

- Harness
- Interface-Target
- Canary
- Quellscan

### Commit 2

```text
test: replace release-disabled assertions in native tests
```

Enthält:

- Migration aller betroffenen Testdateien
- sichere Vorbedingungsprüfungen
- Helper-Umbenennungen

### Commit 3

```text
ci: add debug and sanitizer coverage for native tests
```

Enthält:

- Sanitizer-CMake-Modul und Option
- Debug-CI-Job
- ASan/UBSan-CI-Job
- sichtbare Umbenennung des Release-Jobs, sofern vorgenommen

Falls ein zusätzlich entdeckter Sanitizerfehler behoben werden muss, soll dies nach Möglichkeit in einem vierten separaten Commit erfolgen.

---

## 19. Erwarteter Abschlussbericht des Coding-Agenten

Der Agent muss nach der Implementierung einen präzisen Bericht liefern mit:

### 19.1 Zusammenfassung

- Ursache des ursprünglichen Problems;
- gewähltes Harness-Design;
- Anzahl migrierter Dateien und Prüfungen;
- neue CI-Jobs.

### 19.2 Geänderte Dateien

Gruppiert nach:

- Test-Harness/Canary
- migrierte Tests
- CMake
- CI
- allfällige Sanitizer-Fixes

### 19.3 Ausgeführte Befehle und Resultate

Mindestens:

- Release configure/build/ctest
- Debug configure/build/ctest
- Sanitizer configure/build/ctest
- Quellscan
- direkter Canary Pass/Fail

Keine pauschale Aussage wie „tests pass“ ohne Angabe der tatsächlich ausgeführten Kommandos.

### 19.4 Abweichungen

- nicht verfügbare Plattformen;
- gefundene und behobene Sanitizerprobleme;
- verbleibende Risiken;
- Dateien, die gegenüber der bekannten Inventarliste hinzugekommen sind.

---

## 20. Definition of Done

P0 ist abgeschlossen, wenn ein Reviewer anhand des Diffs und der CI-Ergebnisse zweifelsfrei feststellen kann:

> Ein nativer Test kann nicht mehr allein deshalb grün werden, weil `NDEBUG` seine fachlichen Prüfungen entfernt hat.

Konkret bedeutet das:

- Der Release-Build führt dieselben expliziten Testbedingungen aus wie Debug.
- Eine absichtlich falsche Bedingung wird in Release erkannt.
- Neue Standard-Assertions werden automatisch verhindert.
- Debug prüft zusätzliche nicht optimierte Pfade.
- ASan und UBSan untersuchen den vollständigen nativen Compiler und die vollständige native Testsuite.
- Die bestehenden Plattform- und WASM-Builds bleiben erhalten.
- Es wurden keine fachlichen Compilerfunktionen als Nebenwirkung dieses P0-Auftrags umgebaut.

---

## 21. Kompakter Agenten-Prompt

Der folgende Block kann direkt als Arbeitsauftrag an einen Coding-Agenten übergeben werden. Er ersetzt nicht die Details dieser Spezifikation; der Agent muss das gesamte Dokument beachten.

```text
Implementiere P0-Test-Hardening im Repository edigonzales/ilic-fork.

Lies zuerst die vollständige Spezifikation ilic-p0-test-hardening-spec.md und den aktuellen Repositoryzustand. Ersetze alle release-deaktivierbaren C++-Standard-Assertions unter test/ durch ein kleines projektinternes C++17-Test-Harness mit ILIC_REQUIRE, ILIC_REQUIRE_MSG und ILIC_FAIL. Das Harness muss unabhängig von NDEBUG sein, bei Fehler eine strukturierte ILIC_TEST_FAILURE-Meldung auf stderr schreiben und den gesamten Prozess mit EXIT_FAILURE beenden; es muss auch aus Worker-Threads funktionieren.

Füge ein CMake-Interface-Target ilic::test-support hinzu. Migriere alle ABI-, Core- und Repository-Testprogramme, ohne fachliche Erwartungen zu entfernen. Zerlege unsichere kombinierte Prüfungen, sodass Pointer, Optionals, Iteratoren und Container erst nach expliziter Vorbedingungsprüfung dereferenziert werden.

Füge einen mit NDEBUG=1 kompilierten TestHarnessCanary hinzu. Ein CMake-Treiberskript muss --pass und --fail ausführen und sowohl Exit-Codes als auch ILIC_TEST_FAILURE und intentional canary failure validieren. Füge ausserdem einen CTest-basierten Quellscan hinzu, der <cassert> und Standard-assert-Aufrufe unter test/ verbietet.

Ergänze die CMake-Option ILIC_ENABLE_SANITIZERS und ein zentrales CMake-Modul für -fsanitize=address,undefined, -fno-omit-frame-pointer und -fno-sanitize-recover=all bei GCC/Clang. Instrumentiere alle projektkontrollierten nativen Targets, nicht nur die Tests.

Erhalte die bestehende Release-Matrix für Linux, macOS und Windows sowie den WASM/npm-Job. Ergänze einen Linux-Debug-Job und einen Linux-Clang-ASan+UBSan-Job, jeweils mit vollständigem CTest-Lauf. Leak Detection darf nicht pauschal deaktiviert werden.

Führe lokal Release-, Debug- und Sanitizer-Builds, den vollständigen CTest-Lauf, den Quellscan und den direkten Canary-Pass/Fail-Nachweis aus. Liefere einen Abschlussbericht mit allen ausgeführten Befehlen und Resultaten. Verändere keine fachliche INTERLIS-Semantik, keine öffentliche Produktiv-API und nicht den C++17-Standard.
```
