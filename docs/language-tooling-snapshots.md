# Language-Tooling-Snapshots

[Dokumentationsindex](README.md) · [Native APIs](native-api.md) · [WASM](wasm.md)

`ilic` 0.10 stellt zusätzlich zur rückwärtskompatiblen Compile-/Format-API
versionierte Projektionen für Editoren bereit. Sie sind die stabile Grenze zum
separaten Repository `interlis-language-tools`; UI-, LSP- und Monaco-Code gehört
nicht in den Compiler.

## SyntaxSnapshot

`CompilerSession.parse(uri)` analysiert immer den registrierten Source-Buffer
und liefert:

- Parsebaum, Tokenstrom und Syntaxdiagnostik;
- strukturierte Cursor-Kontexte für Completion und On-Type-Verhalten;
- Imports und UTF-16-/UTF-8-genaue Quellpositionen;
- URI, Dokumentversion, Compiler- und Schema-Version.

Ungespeicherte Texte bleiben damit die primäre Quelle. Es werden keine
temporären `.ili`- oder Logdateien erzeugt.

## SemanticSnapshot

`CompilerSession.analyze(request)` liefert nach erfolgreicher oder teilweise
erfolgreicher Analyse:

- stabile Symbole, Definitionen, Referenzen und Typinformationen;
- Import- und Reverse-Dependency-Graph;
- normalisierte Diagramm- und Dokumentationsprojektionen;
- alle Quellbereiche mit URI, Dokumentversion, UTF-16-Zeile/-Spalte und
  UTF-8-Byteoffset.

Die Projektionen beschreiben Fachsemantik, nicht Darstellung. Layout, SVG und
DOCX werden deshalb in TypeScript erzeugt. Ein Language Service kann den letzten
erfolgreichen Snapshot als sichtbar veralteten Navigations-/Diagrammstand
weiterverwenden.

## Versionierung und Kompatibilität

Die Snapshot-Schemas beginnen bei Version `1`. Konsumenten müssen unbekannte
Schema-Versionen ablehnen und dürfen Ergebnisse nur veröffentlichen, wenn die
Dokumentversion noch aktuell ist. Die bestehenden `compile()`, `format()` und
Worker-Aufrufe bleiben unverändert nutzbar.

Ausführbare Beispiele und Typen liegen in `packages/compiler-wasm`; native und
WASM-Golden-Tests prüfen Unicode-Positionen, ungültige Syntax, Imports,
INTERLIS 1/2.3/2.4 sowie ABI-Rückwärtskompatibilität.
