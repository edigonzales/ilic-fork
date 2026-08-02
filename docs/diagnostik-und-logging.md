# Diagnostik und Logging

[Dokumentationsindex](README.md) · [Native APIs](native-api.md) · [WASM](wasm.md)

Fehlerdiagnosen und Laufzeitlogs sind getrennte Datenströme:

- Eine **Diagnostic** beschreibt einen Fehler, eine Warnung oder einen Hinweis,
  den ein Editor einer Source-Position zuordnen kann.
- Ein **LogEvent** beschreibt den Ablauf, beispielsweise Parser-, Semantic-,
  Repository- oder Cacheaktivität.

## CLI-Ausgabe

Die Präfixe sind:

| Präfix | Bedeutung |
| --- | --- |
| `inf:` | Information |
| `dbg:` | Debugmeldung |
| `wrn:` | Warnung |
| `err:` | Fehler |
| `int:` | interner Abbruch |

Ein lokalisierter Fehler sieht beispielsweise so aus:

```text
err:    docs/examples/models/Invalid.ili:6:1: type MissingDomain not found.
```

```sh
build/macos/ilic docs/examples/models/Invalid.ili
echo "exit=$?"                 # 1
```

Eine Logdatei kann trotz stiller Konsole geschrieben werden:

```sh
build/macos/ilic -silent -debug \
  -log /tmp/ilic.log \
  docs/examples/models/Legacy.ili
```

`-no_info`, `-no_warnings` und `-no_errors` sind reine Anzeigefilter.
Diagnostics und Zähler werden dadurch nicht aus dem Compilation Result
entfernt. `-werror` markiert Warnungen mit `treatedAsError=true` und erhöht
zusätzlich den Fehlerzähler.

## Struktur einer Diagnostic

```json
{
  "severity": "error",
  "code": "ILIC-NAME-TYPE-NOT-FOUND",
  "message": "type MissingDomain not found.",
  "range": {
    "uri": "memory:///Invalid.ili",
    "start": { "line": 5, "character": 0, "byteOffset": 0 },
    "end": { "line": 5, "character": 1, "byteOffset": 0 }
  },
  "relatedInformation": [],
  "notes": [],
  "treatedAsError": false
}
```

`range` kann `null` sein, wenn ein Fehler keinem exakten Token zugeordnet
werden kann. Zeile und Spalte sind nullbasiert:

Zusätzlich können Diagnosen `phase`, `tags`, `helpId` und einen stabilen
`fingerprint` enthalten. Diese Felder sind additiv und dürfen von älteren
Consumern ignoriert werden.

- `line`: Zeile;
- `character`: derzeit die vom Parser beziehungsweise Semantic Checker
  gelieferte Spalte;
- `byteOffset`: Feld für einen UTF-8-Byteoffset im Source-Buffer.

### Positionen und Unicode

`SourceManager::position(uri,byteOffset)` berechnet für einen bekannten
UTF-8-Byteoffset zuverlässig die nullbasierte Zeile und eine UTF-16-Spalte. Das
ist die für LSP benötigte Umrechnung.

`DiagnosticRangeResolver` ist die gemeinsame Stelle für Byte-zu-UTF-16-
Umrechnung, EOF- und Einfügepositionen sowie das Begrenzen ungültiger Byte-
Bereiche. Unicode-, CRLF- und EOF-Fälle werden mit demselben Rangevertrag
veröffentlicht.

## Severities

- `error`
- `warning`
- `information`
- `hint`

Der aktuelle Compiler erzeugt hauptsächlich Errors und Warnings. Die weiteren
Werte sind Teil des stabilen öffentlichen Typs und können direkt auf
`DiagnosticSeverity` des Language Server Protocol abgebildet werden.

## Diagnostic-Codes

Codes sind stabile, regelbezogene Bezeichner. Die Fachprüfung wählt intern
zuerst eine getypte `DiagnosticId`; erst daraus wird der öffentliche
`ILIC-<AREA>-<REASON>`-String erzeugt. Der Meldungstext wird nicht analysiert
und darf verbessert werden, ohne dass sich der Code ändert.

Wichtige Codefamilien und Beispiele:

| Code | Bedeutung |
| --- | --- |
| `ILIC-PARSE-SYNTAX` | Lexer- oder Parserfehler |
| `ILIC-PARSE-ENCODING` | Source konnte nicht als erwartet dekodiert werden |
| `ILIC-NAME-*` | Namensauflösung und Deklarationskonflikte |
| `ILIC-ATTRIBUTE-*` | Attributdefinitionen und -erweiterungen |
| `ILIC-ASSOCIATION-*`, `ILIC-ROLE-*` | Assoziations- und Rollenregeln |
| `ILIC-CONSTRAINT-*` | Constraint-Regeln |
| `ILIC-GENERIC-*` | generische Domains und Context-Zuordnungen |
| `ILIC-TRANSLATION-*` | konkrete Übersetzungsäquivalenz-Regeln |
| `ILIC-META-SYNTAX` | ungültige `!!@`-Syntax |
| `ILIC-META-DANGLING` | Metaattribut ohne nachfolgendes Zielelement |
| `ILIC-META-TARGET` | ungültiges externes Metaattributziel |
| `ILIC-FORMAT-VERSION` | Formatter für diese Version nicht anwendbar |
| `ILIC-FORMAT-ENCODING` | ISO-8859-1 wurde nach UTF-8 konvertiert |
| `ILIC-REPO-*` | Repository-Index, Download, Auflösung oder Integrität |
| `ILIC-ABI-REQUEST` | ungültiger JSON-Request an der C-ABI |
| `ILIC-COMPILER-INTERNAL` | interner Compilerfehler ohne fachliche Regel |

Diagnostic-Texte müssen nicht mit ili2c identisch sein. Anwendungen sollen
Severity, Code und Range verwenden und den Text für Menschen anzeigen, nicht
parsen. Der `DiagnosticCatalog` registriert jeden öffentlichen Code. Neue Codes
werden dort zuerst mit Severity, Phase, Titel und Dokumentations-ID eingetragen.
Rohe ANTLR-Meldungen werden durch `ParserDiagnosticTranslator` klassifiziert
und gelangen nicht unverändert in den öffentlichen Vertrag.

Semantische Primärorte stammen aus der betroffenen Deklaration oder dem
Referenz-Token. `relatedInformation` trägt den unabhängigen URI und Bereich der
Basis-, Ziel- oder Erstdeklaration. Deshalb können Primärdiagnose und Related
Information in unterschiedlichen Dateien liegen; die zuletzt verarbeitete
Datei hat keinen Einfluss auf diese Orte.

## Struktur eines LogEvents

```json
{
  "level": "information",
  "category": "repository",
  "message": "downloaded repository resource",
  "context": {
    "uri": "https://models.interlis.ch/ilimodels.xml"
  }
}
```

Levels sind `trace`, `debug`, `information`, `warning` und `error`. Verwendete
Kategorien umfassen derzeit insbesondere `imports`, `parser`, `semantic`,
`repository` und `cache`.

## C++ und WASM

```cpp
ilic::CompilationResult result = session.compile(request);
for (const auto &diagnostic : result.diagnostics) {
   // severity, code, message, range, relatedInformation, notes, phase, tags
}
for (const auto &event : result.logs) {
   // level, category, message, context
}
```

```js
const result = session.compile({ roots: [uri] });
for (const diagnostic of result.diagnostics) {
  connection.sendDiagnostics({
    uri: diagnostic.range?.uri ?? uri,
    diagnostics: diagnostic.range ? [{
      range: diagnostic.range,
      severity: diagnostic.severity,
      code: diagnostic.code,
      message: diagnostic.message
    }] : []
  });
}
```

Ein LSP-Adapter muss die textuellen Severity-Namen auf die numerischen LSP-Enums
abbilden und `code`, `source`, `range` und `relatedInformation` erhalten.

Der maschinenlesbare Qualitätsbericht entsteht mit:

```sh
node scripts/generate-diagnostic-quality-report.mjs \
  --root . --output build/diagnostic-quality.json
```

Erfasst werden unter anderem registrierte Codes, Rangequalität, Related
Information, Duplikate und Implementierungsleaks.

## Verhalten bei internen Fehlern

Die Bibliotheks-API beendet den Hostprozess nicht. Ein interner historischer
Compilerabbruch wird abgefangen und als erfolgloses `CompilationResult` mit
Diagnostics zurückgegeben. Die native CLI beendet einen fehlerhaften Lauf mit
Exitcode 1. In einem Browser kann ein Worker als zusätzliche Isolation und
harte Abbruchgrenze dienen.
