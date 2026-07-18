# Formatter

[Dokumentationsindex](README.md) · [CLI](cli.md) · [WASM](wasm.md)

`ilic-format` erzeugt eine reproduzierbare Standarddarstellung für INTERLIS 2.3
und 2.4. Anders als eine Ausgabe über `ilic -o23` oder `-o24` arbeitet der
Formatter direkt auf dem Quelltext und erhält Kommentare.

## Unterstütztes Verhalten

- INTERLIS 2.3 und 2.4;
- zwei Leerzeichen pro Blockebene in der CLI;
- Erhalt von `!!`-Zeilenkommentaren;
- Erhalt verschachtelter `/* ... */`-Blockkommentare;
- Erhalt des dominanten Zeilenendes (`LF` oder `CRLF`);
- höchstens eine aufeinanderfolgende Leerzeile;
- abschließendes Zeilenende;
- idempotentes Resultat;
- Konvertierung nicht valider UTF-8-Eingabe als ISO-8859-1 nach UTF-8 mit
  Diagnostic `ILIC-FORMAT-ENCODING`.

INTERLIS 1.0 ist nicht anwendbar und liefert `ILIC-FORMAT-VERSION`.

## Beispiel

Eingabe:

```ili
INTERLIS 2.3;
!! Erklärung bleibt erhalten
!!@ displayName = "Beispiel"
MODEL Example AT "https://example.invalid" VERSION "1" =
TOPIC Data =
CLASS Item =
Name : TEXT*80;
END Item;
END Data;
END Example.
```

Ausgabe:

```ili
INTERLIS 2.3;
!! Erklärung bleibt erhalten
!!@ displayName="Beispiel"
MODEL Example AT "https://example.invalid" VERSION "1" =
  TOPIC Data =
    CLASS Item =
      Name : TEXT*80;
    END Item;
  END Data;
END Example.
```

Der Formatter verändert derzeit nicht jede mögliche Token-Leerstelle. Sein
Vertrag ist eine stabile Blockeinrückung, normalisierte Metaattributzeilen und
Trivia-Erhalt – keine vollständige Neuemission des Metamodells.

## CLI

Der formatierte Text wird nach `stdout` geschrieben. Die Quelldatei wird nie
überschrieben.

```sh
build/macos/ilic-format docs/examples/models/Example.ili \
  > /tmp/Example.formatted.ili

# Erst nach erfolgreicher Prüfung bewusst ersetzen
mv /tmp/Example.formatted.ili docs/examples/models/Example.ili
```

Mehrere Dateien lassen sich mit `--check` prüfen:

```sh
build/macos/ilic-format --check \
  docs/examples/models/Base.ili \
  docs/examples/models/Example.ili
```

`--check` führt für jede Datei zwei Formatierläufe aus und verlangt, dass der
zweite Lauf identisch zum ersten ist. Es prüft damit Syntax, Anwendbarkeit und
Idempotenz. Es prüft **nicht**, ob das Original bereits der formatierten Ausgabe
entspricht.

Exitcodes:

- `0`: alle Dateien anwendbar und idempotent;
- `1`: Datei nicht lesbar, Syntaxfehler, nicht unterstützte Version oder nicht
  idempotentes Resultat;
- `2`: ungültige Argumente.

## C++-API

```cpp
#include "ilic/Formatter.h"

ilic::FormatOptions options;
options.indentSize = 4;
options.requireValidSyntax = true;

ilic::FormatResult result = ilic::Formatter().format(
   "memory:///Example.ili", source, options);
if (result.success) {
   std::cout << result.text;
}
```

`requireValidSyntax=false` ist für editornahe Experimente verfügbar. Dann wird
die lineare Einrückung auch ohne vorherigen Parserlauf angewendet. Die CLI setzt
diese Option nicht frei und validiert immer.

## C-ABI und WASM

Eine Quelle muss zuerst in der Session registriert sein:

```js
const session = compiler.createSession();
session.putSource("memory:///Example.ili", source);
const result = session.format("memory:///Example.ili", {
  indentSize: 2,
  requireValidSyntax: true
});
session.dispose();
```

In JSON v1 muss `indentSize` eine ganze Zahl zwischen 1 und 16 sein. Das
vollständige Format-Request-Schema liegt unter
[`../schemas/format-request-v1.schema.json`](../schemas/format-request-v1.schema.json).

## Kommentare und Metaattribute

Normale `!!`-Kommentare werden nicht als Metaattribute interpretiert. Eine
`!!@`-Zeile wird auf die Form `!!@ name=value` normalisiert; ein zitierter Wert
bleibt zitiert. Dokumentations- und Blockkommentare zwischen einem Metaattribut
und einer Deklaration bleiben im Text erhalten.

Das reale Beispielmodell liegt unter
[`examples/models/Example.ili`](examples/models/Example.ili).
