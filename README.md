# Gemeinsames INTERLIS-vcpkg-Registry

Dieser Branch enthält unveränderliche vcpkg-Versionen der Ports `ilic` und
`iox-cpp`. `versions/baseline.json` definiert den aktuellen Katalogstand; diese
README dupliziert bewusst keine einzelnen Versionsnummern.

Anwendungen müssen ein vollständiges Registry-Commit als Baseline fixieren.
Historische Einträge bleiben unverändert, wenn die Baseline vorrückt. Neue
Snapshots verwenden `X.Y.Z-snapshot.g<12-stelliger-Source-SHA>`; reine
Paketkorrekturen erhöhen die vcpkg-`port-version`.

Nur der serialisierte Registry-Workflow in `ilic-fork` schreibt diesen Branch.
