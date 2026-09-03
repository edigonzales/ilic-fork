# Language-Tooling-Snapshots

[Dokumentationsindex](README.md) · [Native APIs](native-api.md) · [WASM](wasm.md)

`ilic` stellt zusätzlich zur rückwärtskompatiblen Compile-/Format-API
versionierte Snapshots für Editoren bereit. Sie sind die stabile Grenze zum
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

## EditorSnapshot

`CompilerSession.editorSnapshot(uri)` ist der native, synchrone Editorpfad.
Syntax- und Editor-Snapshot verwenden denselben ANTLR-Lexer, Parser und
`SourceRangeMapper`; der WASM-/JavaScript-Wrapper reicht nur den C-ABI-Request
weiter. `recovered` erlaubt Teilresultate nach Syntaxfehlern, während
`complete` nur bei einem vollständigen, nicht wiederhergestellten Parse gesetzt
wird. `iliVersion` kann bei unvollständigem Text zusätzlich `unknown` sein.

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

### Dokumentationsprojektion v1

Die Dokumentationsprojektion ist bewusst additiv und stellt die für mehrere
Darstellungen benötigte Semantik bereit. Die vollständige Struktur ist im
[versionierten JSON-Schema](../schemas/semantic-snapshot-v1.schema.json)
beschrieben. Die folgenden Felder sind für DOCX- und HTML-Export relevant:

| Objekt | Feld | Bedeutung |
| --- | --- | --- |
| `documentation` | `title`, `sections`, `models` | Projektion der Dokumentationsabschnitte und Modelle |
| `DocumentationModel` | `title`, `shortDescription` | Modell-Metadaten aus den dokumentierten Metaattributen |
| `DocumentationViewable` | `kind` | `class`, `structure`, `view` oder `association`; Associations sind eigene Viewables |
| `DocumentationRow` | `range` | normalisierte Darstellung von Text- und Numeric-Wertebereichen; optional |
| `DocumentationEnumerationEntry` | `displayName` | Anzeigename aus `ili2db.dispName`; optional, `value` bleibt der Enumerationswert |
| `DocumentationViewable` | `roles` | Rollen einer Association mit Name, Kardinalität, Zieltyp und Dokumentation |
| `DocumentationViewable` | `uniqueness` | stabile Liste direkter und geerbter UNIQUE-Definitionen |

Eine `DocumentationUnique` enthält `scope`
(`global` oder `local`), `perBasket`,
den optionalen lokalen `prefix`, die geordneten
Schlüsselpfade `elements`, die optionale
`where`-Bedingung sowie `origin`
(`direct` oder `inherited`). Bei geerbten
Definitionen bezeichnet `inheritedFrom` die Basisklasse.
UNIQUE-Definitionen werden von der ältesten Basisklasse zur abgeleiteten
Klasse gesammelt; eine zyklische Vererbung darf die Projektion nicht endlos
laufen lassen.

Pfade und Bedingungen werden deterministisch als INTERLIS-Text dargestellt.
Wenn ein Compilerobjekt nicht verlustfrei darstellbar ist, liefert die
Projektion einen stabilen Fallback-Text. Ein solcher Fallback ist ein
fachlicher Wert und kein Grund, den gesamten Snapshot oder Export abzubrechen.
Die bestehenden Navigationszeilen an Klassen bleiben neben den eigenständigen
Association-Viewables erhalten.

Die neuen Felder ändern weder `schemaVersion` noch
`abiVersion`: Beide bleiben bei `1`. Konsumenten
müssen die neuen Felder ignorieren können und optionale Felder weiterhin als
fehlend behandeln; Produzenten dürfen die Felder in Snapshot-v1-Ergebnissen
ergänzen. Die JSON-Schema-Version ist unabhängig von der nativen ABI-Version.
`@ilic/docx` konsumiert diese Projektion und ist für Seitenformat,
Tabellenbreiten und die konkrete Darstellung zuständig.

Ein minimales Beispiel der neuen Dokumentationsfelder:

~~~json
{
  "schemaVersion": 1,
  "abiVersion": 1,
  "kind": "semantic",
  "documentation": {
    "title": "Beispiel",
    "sections": [],
    "models": [{
      "name": "Example",
      "uri": "memory:///Example.ili",
      "title": "Beispielmodell",
      "shortDescription": "Kurzbeschreibung",
      "topics": [{
        "name": "Data",
        "documentation": "Fachdaten",
        "viewables": [{
          "name": "Link",
          "kind": "association",
          "isAbstract": false,
          "documentation": "Verknüpfung",
          "rows": [],
          "roles": [{
            "name": "source",
            "cardinality": "1",
            "type": "Source",
            "description": "Quellobjekt"
          }],
          "uniqueness": [{
            "scope": "global",
            "perBasket": false,
            "prefix": "",
            "elements": ["source", "target"],
            "where": "",
            "origin": "direct",
            "inheritedFrom": ""
          }]
        }],
        "enumerations": []
      }],
      "viewables": [],
      "enumerations": []
    }]
  }
}
~~~

## Versionierung und Kompatibilität

Die Snapshot-Schemas beginnen bei Version `1`. Konsumenten müssen unbekannte
Schema-Versionen ablehnen und dürfen Ergebnisse nur veröffentlichen, wenn die
Dokumentversion noch aktuell ist. Die bestehenden `compile()`, `format()` und
Worker-Aufrufe bleiben unverändert nutzbar.

Ausführbare Beispiele und Typen liegen in `packages/compiler-wasm`; native und
WASM-Golden-Tests prüfen Unicode-Positionen, ungültige Syntax, Imports,
INTERLIS 1/2.3/2.4 sowie ABI-Rückwärtskompatibilität.
