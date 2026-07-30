# Compiler-Conformance

[Dokumentationsindex](README.md) · [Funktionsumfang](funktionsumfang.md)

Die unabhängige Suite liegt im Schwesterprojekt
[`interlis-compiler-conformance`](https://codeberg.org/edigonzales/interlis-compiler-conformance).
Dieses Compiler-Repository enthält keine Conformance-Fixtures, keinen Manifest-Importer
und keinen Conformance-Runner mehr.

## Grundmodell

Das Schwesterprojekt versioniert ein gemeinsames Referenzset. `ili2c` und `ilic`
werden unabhängig daran gemessen. Das Referenzset enthält mindestens die erwartete
Compilerentscheidung `true` oder `false`; geprüfte semantische Diagnoseerwartungen
können später ergänzt werden. Der aktuelle Datensatz umfasst 1049 Inventarfälle
und 620 ausführbare Referenzset-Fälle. Solange die fachliche Prüfung noch nicht
abgeschlossen ist, trägt das Set seine Herkunft (`ili2c`) und `reviewed: false`
sichtbar in `reference/reference-set.json`.

`reviewed: false` bedeutet, dass die Erwartungen noch nicht für jeden Fall manuell
bestätigt sind. Ein aktueller `ili2c`-Lauf kann daher `REFERENCE_MISMATCH` melden.
Das sind offene Fragen zum Referenzset und keine automatisch akzeptierten oder
als XFAIL klassierten `ilic`-Fehler. Die historischen Inventarwerte
`needs-adjudication` steuern den Lauf nicht.

Die wichtigsten Ergebnisgruppen sind:

- `CONFORMANT`: Compilerentscheidung entspricht dem Referenzset.
- `ACCEPTS_INVALID`: Referenz erwartet Ablehnung, Compiler akzeptiert.
- `REJECTS_VALID`: Referenz erwartet Annahme, Compiler lehnt ab.
- `DIAGNOSTIC_MISMATCH`: Entscheidung stimmt, aber eine geprüfte Diagnosesemantik weicht ab.
- `REFERENCE_MISMATCH`: `ili2c` selbst weicht vom noch offenen Referenzset ab.
- `INFRASTRUCTURE_ERROR`: Timeout, Absturz, fehlendes Programm oder Adapterfehler.

Reine Unterschiede im Meldungstext werden nicht automatisch als semantische Abweichung
gewertet. Ohne geprüfte Diagnoseerwartung zählt nur die Annahme-/Ablehnungsentscheidung.

## Expected Deviations, XFAIL und XPASS

Die sechs bekannten aktuellen `ilic`-Abweichungen stehen im Schwesterprojekt in
`reference/expected-deviations.json`. Dort wird kein einzelner `XFAIL`-Status
gespeichert. Das Gate berechnet ihn beim Lauf:

- `XFAIL`: Die dokumentierte Abweichung tritt weiterhin auf; die Pipeline bleibt grün.
- `XPASS`: Die Abweichung ist unerwartet verschwunden; die Pipeline wird rot, bis
  der Eintrag geprüft und entfernt wurde.

## Reproduzierbarer Lauf

Im Schwesterprojekt werden `ili2c`, ein `ilic`-Checkout und das Referenzset festgelegt:

```sh
cd /pfad/zu/interlis-compiler-conformance
export ILI2C_JAR=/pfad/zu/ili2c.jar
export ILIC_EXECUTABLE=/pfad/zu/ilic/build/ilic
export ILIC_REPO=/pfad/zu/ilic

./gradlew ciConformance --no-configuration-cache
```

Der Lauf erzeugt unter `reports/latest/` `results.json`, `summary.json`,
`summary.md`, `report.html`, `gate.json` und `gate.md`. Die Pipeline blockiert bei
neuen Abweichungen, Infrastrukturfehlern und unerwarteten Verbesserungen einer
ausdrücklich erwarteten Abweichung. Bekannte aktuelle Abweichungen bleiben im
Report sichtbar und werden über `baselines/ilic/current.json` nachvollziehbar gehalten.

## Compiler-CI

Die normale Compiler-CI führt die Unit-, Integrations- und Regressionstests von `ilic`
aus. Der Conformance-Lauf wird zentral aus dem Schwesterprojekt gegen den jeweiligen
`ilic`-Commit gestartet. So bleibt der Compiler frei von einer zweiten Test-Suite und
die Vergleichslogik befindet sich an einer einzigen Stelle.

Wenn ein neuer `ilic`-Commit geprüft werden soll, wird die Suite mit diesem
Commit ausgeführt. Der Compiler selbst muss dafür keine Conformance-Dateien oder
Runner übernehmen.
