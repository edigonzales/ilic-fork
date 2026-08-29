# Versionierungsvertrag

Die numerische Produkt- und API-Basis steht in `project(ilic VERSION …)` in
`CMakeLists.txt`. Source-Manifeste der Entwicklungszeile melden weiterhin
`0.10.0-SNAPSHOT`; dies ist keine veröffentlichte Paketversion.

Neue unveränderliche Artefakte verwenden in npm und vcpkg dieselbe Identität:

```text
stabil:    X.Y.Z
Snapshot:  X.Y.Z-snapshot.g<12 Zeichen des Source-SHA>
Beispiel:  0.10.0-snapshot.ge901af642470
```

Der vollständige 40-stellige Git-SHA, GitHub-Run-ID, Publikationszeit und die
Toolchain stehen in `interlis-release.json`. Datum und Run-ID sind absichtlich
kein Teil der Version. Derselbe Commit ergibt dadurch deterministisch dieselbe
Snapshot-Version.

Die drei ilic-npm-Pakete besitzen immer dieselbe Version und exakte interne
Abhängigkeiten. `snapshot` zeigt auf den jüngsten freigegebenen Vorabstand;
`latest` wird ausschließlich von einem stabilen `vX.Y.Z`-Tag bewegt.

Snapshots werden nur über einen expliziten koordinierten Workflow-Dispatch
publiziert. Ein erfolgreicher `main`-Build veröffentlicht nichts automatisch.
Bestehende Zeitstempel- und kurze vcpkg-Snapshot-Versionen bleiben unverändert
verfügbar, werden aber nicht mehr neu erzeugt. Reine Änderungen am vcpkg-Port
verwenden `port-version`, nicht eine neue Upstream-Version.

`scripts/release_metadata.py` prüft und erzeugt Version und Provenance.
`prepare-npm-snapshot.mjs` und `prepare-npm-release.mjs` stagen nur explizit
erlaubte Dateien und verändern keine Quellmanifeste.
