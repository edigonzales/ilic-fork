# Compiler-Conformance

[Dokumentationsindex](README.md) · [Funktionsumfang](funktionsumfang.md)

Die einzige massgebliche, compilerübergreifende Referenz ist die externe
[`interlis-compiler-testsuite`](https://codeberg.org/edigonzales/interlis-compiler-testsuite).
Sie enthält den Runner, die kanonischen Testfälle, deren erwartetes Ergebnis und
die dazugehörigen Verweise auf die Testmodelle. Dieses Repository dupliziert
weder die Suite noch den Runner.

## Verantwortungsgrenze

Die externe Suite umfasst aktuell 610 geprüfte Outcome-Fälle für INTERLIS 1.0,
2.3 und 2.4. Für jeden Fall wird als portables Muss-Kriterium geprüft, ob der
Compiler das Modell annimmt oder ablehnt. Neue geeignete Fälle aus `ili2c`
werden zentral in diese Suite aufgenommen. In `ilic-fork` bleiben nur fokussierte
Unit-, Integrations- und Regressionstests für das Verhalten der eigenen
Komponenten.

`ilic-fork` besitzt genau einen compilerabhängigen Teil des Conformance-Vertrags:
[`../conformance/known-failures.json`](../conformance/known-failures.json). Diese
Baseline beschreibt die elf derzeit akzeptierten Abweichungen. Sie ändert weder
die Erwartungen der Suite noch verbirgt sie Ergebnisse:

- `PASS`: Das tatsächliche Ergebnis entspricht der Suite.
- `XFAIL`: Eine in der Baseline dokumentierte Abweichung besteht weiterhin.
- `FAIL`: Eine neue, nicht akzeptierte Abweichung ist aufgetreten.
- `XPASS`: Eine erwartete Abweichung ist verschwunden; die Baseline muss bewusst
  geprüft werden.
- `ERROR`: Runner, Compiler, Timeout oder Testinfrastruktur sind fehlgeschlagen.

Die elf Baseline-Einträge bleiben vorerst unverändert. Änderungen an der
Baseline erfordern eine ausdrückliche fachliche Prüfung und einen sichtbaren
Review; ein fehlgeschlagener Lauf darf nicht durch Umklassifizieren grün gemacht
werden.

## Reproduzierbarer lokaler Lauf

Bei einer Geschwisterstruktur aus `ilic-fork` und
`interlis-compiler-testsuite` wird die identische Suite wie folgt ausgeführt:

```sh
cmake -S . -B build/stabilization -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build/stabilization --parallel

java ../interlis-compiler-testsuite/InterlisConformance.java run \
  --suite ../interlis-compiler-testsuite/suite/test-cases.json \
  --compiler ilic=build/stabilization/ilic \
  --baseline ilic=conformance/known-failures.json \
  --out build/conformance/local
```

Der erfolgreiche Sollzustand ist derzeit `PASS=599`, `XFAIL=11`, `FAIL=0`,
`XPASS=0` und `ERROR=0`. Die Zählung dokumentiert den aktuellen Stand; die
maschinelle Wahrheit bleiben die gepinnte externe Suite und die lokale Baseline.

## Compiler-CI

Die normale Conformance-CI und der TSan-Job laden denselben explizit gepinnten
Commit der `interlis-compiler-testsuite` herunter und führen alle 610 Fälle aus.
Der Pin verhindert, dass Änderungen eines fremden Repositorys einen ansonsten
unveränderten Compiler-Build unbemerkt beeinflussen. Eine Aktualisierung des Pins
ist ein eigener, überprüfbarer Wartungsschritt.

Die übrigen CI-Jobs führen zusätzlich die lokalen nativen und paketbezogenen
Tests aus. Die lokale Baseline wird dabei nicht erzeugt oder automatisch
aktualisiert.
