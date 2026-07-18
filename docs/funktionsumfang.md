# Funktionsumfang

[Dokumentationsindex](README.md) · [CLI](cli.md) · [APIs](native-api.md) · [WASM](wasm.md)

## Eingabesprachen und Validierung

`ilic` liest und validiert Datenmodelle in:

- INTERLIS 1.0;
- INTERLIS 2.3;
- INTERLIS 2.4.

Der Compiler prüft Syntax, Namensauflösung, Imports, Typen, Klassen,
Assoziationen, Rollen, Sichten, Objektpfade, Constraints, generische Domains,
Kontexte sowie `TRANSLATION OF`. Die externe
[Compiler-Conformance-Suite](conformance.md) deckt 571 aus den ili2c-JUnit-Tests
importierte reine Compilerfälle ab.

Ein einzelner CLI-Aufruf darf keine unterschiedlichen INTERLIS-Versionen
mischen. Die C++-/C-/WASM-APIs akzeptieren virtuelle UTF-8-Quellen; eine Datei
muss daher weder lokal existieren noch gespeichert sein.

```sh
# INTERLIS 1.0 validieren
build/macos/ilic -silent docs/examples/models/Legacy.ili

# INTERLIS 2.3 mit lokalem Import validieren
build/macos/ilic -silent \
  -ilidirs docs/examples/models \
  docs/examples/models/Example.ili
```

## Ausgabeformate

Wenn keine Fehler vorliegen, kann die CLI folgende Formate erzeugen:

| Option | Ausgabe |
| --- | --- |
| `-o1` | INTERLIS 1.0 |
| `-o23`, `-o2` | INTERLIS 2.3 |
| `-o24` | INTERLIS 2.4 |
| `-oIMD` | IlisMeta07/IMD |
| `-oXSD` | XML Schema für XTF |
| `-oGML` | GML-Schema |

Mehrere Generatoren dürfen in einem Aufruf aktiviert werden. Bei
Compilerfehlern wird keine Generatorausgabe geschrieben. Die korrekte
Argumentreihenfolge und vollständige Befehle stehen in der
[CLI-Referenz](cli.md#ausgabeoptionen).

## Diagnostik und Logging

- Die CLI schreibt menschenlesbare Meldungen und kann sie gleichzeitig in eine
  Logdatei schreiben.
- Die Bibliotheks-APIs liefern strukturierte Diagnostics mit Severity, stabilem
  Code, Nachricht und – soweit verfügbar – URI, Zeile und Spalte.
- Strukturierte Logevents enthalten Level, Kategorie, Nachricht und Kontext.
- Der SourceManager kann bekannte UTF-8-Byteoffsets in UTF-16-Spalten für LSP
  umrechnen. Die derzeitige Abdeckung strukturierter Diagnostic-Offsets ist
  unter [Diagnostik und Logging](diagnostik-und-logging.md#positionen-und-unicode)
  beschrieben.

Details und JSON-Beispiele: [Diagnostik und Logging](diagnostik-und-logging.md).

## Metaattribute

Quelltext-Metaattribute mit `!!@` werden eingelesen, validiert und im
Metamodell gespeichert. Modell-Metaattribute erscheinen in den strukturierten
Compilation Results.

```ili
!!@ displayName = "Dokumentationsmodell"
MODEL Example AT "https://example.invalid" VERSION "1" =
END Example.
```

Die API kann zusätzlich externe Metaattribute entgegennehmen. Der derzeit
semantisch ausgewertete externe Name ist `ili2c.translationOf` auf einem
Modell. Er ermöglicht eine Übersetzungsverknüpfung, ohne den Quelltext zu
ändern. Andere externe Namen werden noch nicht als allgemeine Metaattribute in
das Metamodell übertragen.

## Formatter

Der separate Formatter unterstützt INTERLIS 2.3 und 2.4. Er bewahrt
`!!`-Kommentare und Blockkommentare, normalisiert Einrückung und Metaattribute
und ist idempotent. INTERLIS 1.0 kann kompiliert, aber derzeit nicht mit
`ilic-format` formatiert werden. Siehe [Formatter](formatter.md).

## Modell-Repositories

Das native Binary kann Modelle aus lokalen und HTTP(S)-Repositories anhand von
`ilimodels.xml` suchen, Abhängigkeiten auflösen, Prüfsummen kontrollieren und
Downloads lokal cachen. Node und Browser erhalten dieselbe Host-Funktion über
`@ilic/tools`. Siehe [Repositories](repositories.md).

## Einbettung

| Oberfläche | Geeignet für |
| --- | --- |
| C++ `CompilerSession` | Native C++-Anwendungen und Tests |
| C-ABI mit JSON v1 | Sprachneutrale native Einbettung und WASM-Grenze |
| `@ilic/compiler-wasm` | Node, Browser, Theia und VS Code Web Extensions |
| Worker-Adapter | Compilerarbeit außerhalb des UI-Threads |

Compilation und Formatierung sind synchron. Kompilationen werden im aktuellen
Core pro Prozess serialisiert, weil Teile des historischen Metamodells globalen
Zustand verwenden. Für eine harte Abbruchgrenze im Browser wird ein Worker
terminiert und neu erzeugt; `CompilationResult.cancelled` wird derzeit nicht
aktiv durch eine Cancel-API gesetzt.

## Bewusste Grenzen

- Die erreichte Conformance-Parität betrifft den eingefrorenen 571-Fälle-Korpus,
  nicht identische Fehlermeldungstexte oder sämtliche ili2c-Ausgabefunktionen.
- Das WASM-Modul enthält kein HTTP-, Dateisystem- oder Cache-Backend.
- `@ilic/compiler-wasm` und `@ilic/tools` sind für versionierte npm-Snapshots
  vorbereitet. Ihre tatsächliche Verfügbarkeit beginnt nach dem einmaligen
  [npm-Bootstrap](npm-publikation.md#einmaliger-bootstrap-auf-npm); stabile
  Releases und der Dist-Tag `latest` sind noch nicht eingerichtet.
- Die CLI enthält einige historische Alias- und Argumentreihenfolge-Eigenheiten,
  die in [Bekannte CLI-Abweichungen](cli.md#bekannte-abweichungen) aufgeführt sind.
- Parser- und Semantic-Diagnostics enthalten Zeile und Spalte, aber noch nicht
  für jeden Fehler einen verwertbaren UTF-8-`byteOffset`; LSP-Hosts müssen
  Unicode-Positionen deshalb derzeit defensiv behandeln.
