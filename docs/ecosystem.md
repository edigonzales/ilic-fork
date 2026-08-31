# INTERLIS-Ökosystem

Dieses Dokument ist die zentrale Übersicht über Abhängigkeiten,
Artefaktverantwortung und Release-Übernahmen. Details eines einzelnen Projekts
stehen in dessen eigenem Release-Runbook.

```text
ilic-fork → iox-cpp → duckdb-interlis
     └────→ interlis-language-tools → interlis-web-ide
```

| Repository | Verantwortung | Konsumierte Artefakte |
| --- | --- | --- |
| `ilic-fork` | Compiler, C/C++-API, WASM, gemeinsames vcpkg-Registry | keine projektinterne Abhängigkeit |
| `iox-cpp` | XTF-Leser/-Schreiber und WASM | gelocktes ilic |
| `duckdb-interlis` | DuckDB-Extension | gelocktes `iox-cpp[ilic]` |
| `interlis-language-tools` | Language Service, Monaco, Diagramm, DOCX und LSP | drei gelockte ilic-npm-Pakete |
| `interlis-web-ide` | Browser-IDE und Pages-Deployment | gelocktes `@ilic/tools` und vier Language-Tools-Pakete |

Ein publiziertes Upstream-Artefakt ändert keinen Downstream automatisch. Jeder
Downstream übernimmt Version und vollständigen Source-SHA durch eine
committete Lock-Datei, lässt seine CI laufen und publiziert danach seinen
eigenen Snapshot oder Release.

## Gemeinsame Versionen

Neue Artefakte verwenden:

```text
stabil:    X.Y.Z        Git-Tag: vX.Y.Z
Snapshot:  X.Y.Z-snapshot.g<erste 12 Zeichen des Source-SHA>
```

Der vollständige SHA und die Build-Provenienz stehen im jeweiligen
`interlis-release.json`. Datum und GitHub-Run-ID sind kein Teil der Version.
Bereits publizierte Zeitstempel- und kurze vcpkg-Snapshots bleiben
unveränderlich verfügbar, werden aber nicht mehr erzeugt.

## vcpkg-Registry und Binary-Cache

Das Git-Registry liegt im Branch `vcpkg-registry` dieses Repositories. Es
enthält die Katalog- und Portversionen von `ilic` und `iox-cpp`. Ein Consumer
pinnt den Branch über einen unveränderlichen Registry-`baseline`; der bewegte
Branchkopf ist kein Dependency-Lock.

Der davon getrennte Binary-Cache ist ein NuGet-Feed unter
`https://nuget.pkg.github.com/edigonzales/index.json`. Das Registry entscheidet,
welcher Port gebaut wird; der Cache speichert bereits gebaute Pakete. Zugriffe
aus GitHub Actions benötigen `packages: read` und eine für das Repository
freigegebene Berechtigung oder einen minimalen `read:packages`-Token.

| Port | Features | Binäre Triplets |
| --- | --- | --- |
| `ilic` | Bibliothek mit `ilic::core` und `ilic::capi`; keine CLI und kein Repository-Transport | `x64-linux`, `arm64-osx`, `x64-windows`, `x64-windows-static` |
| `iox-cpp` | `ilic`, `geos`, `ilic+geos`; kein featureloses Cachepaket | `x64-linux`, `arm64-osx`, `x64-windows`, `x64-windows-static` |

`iox-cpp` stellt seine gelockten nativen Abhängigkeiten im eigenen CI strikt
aus diesem Cache wieder her. `duckdb-interlis` stellt `iox-cpp[ilic]` samt
transitivem ilic für seinen schnellen Projekt-CI ebenfalls binär wieder her.
Externe Forks dürfen auf einen Source-Build zurückfallen.

DuckDB Community Extensions kann den privaten GitHub-Packages-Cache nicht
voraussetzen. Deren CI verwendet deshalb die unter `duckdb-interlis/vcpkg/ports`
eingecheckten öffentlichen Overlay-/Source-Ports. Die JavaScript-Projekte
verwenden kein vcpkg.

## Übernahmefluss

1. Upstream baut und prüft einen exakten Commit.
2. Upstream publiziert dessen unveränderliche Version und Provenienz.
3. Der Downstream aktualisiert Version und vollständigen SHA in seiner
   Lock-Datei und prüft das verschachtelte Dependency-Verhältnis.
4. Erst dieser Downstream-Commit darf dessen eigenen Snapshot oder stabilen
   Release erzeugen.

Es gibt bewusst keinen automatischen Cross-Repository-Deploy. Dadurch bleibt
jede ausgelieferte Kombination aus dem jeweiligen Repository rekonstruierbar.
