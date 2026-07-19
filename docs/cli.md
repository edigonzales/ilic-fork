# CLI-Referenz

[Dokumentationsindex](README.md) · [Formatter](formatter.md) · [Repositories](repositories.md)

## Aufruf

```text
ilic [Optionen] file1.ili file2.ili ...
ilic --compile-json
ilic-format [--check] model.ili ...
```

Optionen akzeptieren grundsätzlich einen oder zwei Bindestriche. Namen sind
case-sensitive. Eingabedateien müssen auf `.ili` enden.

`--compile-json` ist ein separater, maschinenlesbarer Modus. Er liest genau
einen Compilation-Request des stabilen C-API-Schemas Version 1 von `stdin` und
schreibt genau ein Resultat-JSON nach `stdout`:

```sh
printf '%s' '{"schemaVersion":1,"roots":["docs/examples/models/Legacy.ili"],"options":{"autoSearch":true},"externalMetaAttributes":[]}' \
  | build/macos/ilic --compile-json
```

Eine fachliche Compilerablehnung wird mit `success: false` im JSON und Exitcode
0 ausgegeben. Exitcode 2 ist Request-, Ein-/Ausgabe- und Laufzeitfehlern
vorbehalten. Weitere Felder sind in der [Native-API](native-api.md) beschrieben.

```sh
build/macos/ilic -silent docs/examples/models/Legacy.ili
build/macos/ilic --silent docs/examples/models/Legacy.ili
```

## Eingabeoptionen

| Option | Wert | Funktion |
| --- | --- | --- |
| `-ilidirs` | Verzeichnisliste | Lokale Suchverzeichnisse, vorzugsweise mit Semikolon getrennt; Kommas werden ebenfalls verarbeitet. |
| `-no_auto` | – | Unterbindet die automatische Suche nach fehlenden Imports. Bereits explizit geladene Dateien bleiben verfügbar. |
| `-repositories` | URI-Liste | Geordnete lokale, `file://`- oder HTTP(S)-Repositories; Semikolon oder Komma als Trenner. |
| `-models` | Modellnamen | Kommagetrennte Root-Modelle, die über `-repositories` aufgelöst werden. |

Lokaler Import:

```sh
build/macos/ilic -silent \
  -ilidirs docs/examples/models \
  docs/examples/models/Example.ili
```

Ohne automatische Suche müssen alle Dateien explizit genannt werden:

```sh
build/macos/ilic -silent -no_auto \
  docs/examples/models/Base.ili \
  docs/examples/models/Example.ili
```

Repository-Modell ohne manuelles Herunterladen:

```sh
build/macos/ilic -silent \
  -repositories https://models.interlis.ch \
  -models DatasetIdx16
```

Ein lokales Repository kann genauso verwendet werden:

```sh
build/macos/ilic -silent \
  -repositories test/repository/fixture \
  -models RepositoryRoot
```

Wenn Eingabedateien Imports enthalten, sucht die CLI zuerst in bereits
geladenen Dateien und `-ilidirs`. Nur wenn dort nichts gefunden wird, greift sie
auf den konfigurierten `RepositoryManager` zurück.

## Ausgabeoptionen

| Option | Standarddatei | Funktion |
| --- | --- | --- |
| `-o1 [datei]` | `interlis1.ili` | INTERLIS-1-Ausgabe |
| `-o23 [datei]`, `-o2 [datei]` | `interlis23.ili` | INTERLIS-2.3-Ausgabe |
| `-o24 [datei]` | `interlis24.ili` | INTERLIS-2.4-Ausgabe |
| `-oIMD [datei]` | `output.imd` | IlisMeta07/IMD |
| `-oXSD [datei]` | `output.xsd` | XML Schema für XTF |
| `-oGML [verzeichnis]` | `output.gml` | GML-Schemas; eine `.xsd`-Datei pro Modell |
| `-output datei`, `-out datei` | – | ili2c-Kompatibilitätswert für IMD-, XSD- und GML-Ausgaben |
| `-model_filter name` | letztes Modell | Modellfilter für `-o1` |

**Wichtig:** Bei `-o1`, `-o23` und `-o24` muss eine auf `.ili` endende
Ausgabedatei nach der Eingabedatei stehen. Andernfalls bindet der historische
Argumentparser sie als Optionswert, ohne eine Eingabe zu registrieren.

```sh
# Richtig
build/macos/ilic -silent docs/examples/models/Legacy.ili \
  -o1 /tmp/legacy-output.ili

build/macos/ilic -silent -ilidirs docs/examples/models \
  docs/examples/models/Example.ili \
  -o23 /tmp/example-23.ili \
  -o24 /tmp/example-24.ili

# IMD, XSD und GML in einem Lauf; das GML-Zielverzeichnis muss existieren
mkdir -p /tmp/example-gml
build/macos/ilic -silent -ilidirs docs/examples/models \
  docs/examples/models/Example.ili \
  -oIMD /tmp/example.imd \
  -oXSD /tmp/example.xsd \
  -oGML /tmp/example-gml
```

Generatoren laufen nur, wenn keine Compilerfehler vorliegen.
`-oGML` legt das Zielverzeichnis nicht selbst an; im Beispiel entsteht unter
anderem `/tmp/example-gml/Example.xsd`.

## Logging-Optionen

| Option | Funktion |
| --- | --- |
| `-log datei` | Schreibt Meldungen zusätzlich in eine Datei. |
| `-silent`, `-quiet` | Unterdrückt Konsolenausgabe; eine mit `-log` konfigurierte Datei wird weiter geschrieben. |
| `-no_info` | Unterdrückt Informationsmeldungen. |
| `-no_warnings` | Unterdrückt die Anzeige von Warnungen. |
| `-no_errors` | Unterdrückt die Anzeige von Fehlern; der Exitcode bleibt fehlerhaft. |
| `-debug` | Aktiviert Debugmeldungen. |
| `-werror` | Zählt Warnungen zusätzlich als Fehler. |
| `-ident n` | Anzahl Leerzeichen pro Einrückungsstufe der Textausgabe. |

```sh
build/macos/ilic -quiet \
  -log /tmp/ilic.log \
  -debug -werror \
  -ilidirs docs/examples/models \
  docs/examples/models/Example.ili
```

`-no_warnings` und `-no_errors` ändern nur die Anzeige. Die internen Zähler und
der Exitcode bleiben erhalten. Mehr Details stehen unter
[Diagnostik und Logging](diagnostik-und-logging.md).

## Allgemeine Optionen

| Option | Alias | Funktion |
| --- | --- | --- |
| `-help` | `-h` | Vollständige eingebaute Hilfe |
| `-usage` | `-u` | Kurzsyntax und Beispiele |
| `-version` | `-v` | Version, Builddatum und Plattform |
| `-success n` | – | Verwendet bei fehlerfreier Kompilation den Exitcode `n` statt `0`. |

`-gui` wird vom Parser aus historischen Gründen akzeptiert, beendet das
Programm aber ohne GUI. Es ist keine öffentliche Funktion.

### Exitcodes

| Programm | Code | Bedeutung |
| --- | ---: | --- |
| `ilic` | `0` | Erfolg, sofern `-success` nicht gesetzt ist |
| `ilic` | Wert von `-success` | fehlerfreie Kompilation mit konfiguriertem Erfolgsstatus |
| `ilic` | `1` | Compiler-, Argument- oder Repositoryfehler |
| `ilic --compile-json` | `0` | Request ausgeführt; `success` im JSON enthält das Compilerergebnis |
| `ilic --compile-json` | `2` | ungültiger Request oder Transport-/Laufzeitfehler |
| `ilic-format` | `0` | alle Dateien erfolgreich geprüft/formatiert |
| `ilic-format` | `1` | mindestens eine Datei nicht lesbar, ungültig oder nicht anwendbar |
| `ilic-format` | `2` | ungültiger Aufruf |

Beispiel für ein Buildsystem, das Erfolg als `7` erwartet:

```sh
build/macos/ilic -silent -success 7 docs/examples/models/Legacy.ili
test $? -eq 7
```

## Bekannte Abweichungen

Die ältere Hilfe beschreibt einige Aliase, die der aktuelle Argumentparser
nicht akzeptiert. Die funktionierenden Namen sind:

| Nicht verwenden | Tatsächlich verwenden |
| --- | --- |
| `-no-auto` | `-no_auto` |
| `-without-warnings` | `-no_warnings` |
| `-indent` | `-ident` |
| `-model` | `-model_filter` |

Diese Dokumentation bildet bewusst den Ist-Zustand ab. Die Abweichungen werden
nicht durch versteckte Aliasannahmen in Beispielen kaschiert.

## `ilic-format`

```sh
# Formatierten Text nach stdout schreiben
build/macos/ilic-format docs/examples/models/Example.ili \
  > /tmp/Example.formatted.ili

# Syntax und Idempotenz mehrerer Dateien prüfen
build/macos/ilic-format --check \
  docs/examples/models/Base.ili \
  docs/examples/models/Example.ili
```

`--check` vergleicht nicht Original und formatiertes Resultat. Eine anders
formatierte, aber gültige Datei kann daher mit Exitcode 0 enden. Die genaue
Semantik steht unter [Formatter](formatter.md).

Das vollständig ausgeführte Shell-Beispiel liegt in
[`examples/cli.sh`](examples/cli.sh).
