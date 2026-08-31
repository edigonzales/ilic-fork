# Versionierung und Release

Die numerische Basis steht in `project(ilic VERSION …)` in `CMakeLists.txt`.
Die eingecheckten npm-Manifeste tragen zusätzlich den Source-Qualifier
`SNAPSHOT`; publizierte Versionen werden daraus erzeugt und nicht in den
Quellmanifesten gespeichert.

```text
stabil:    X.Y.Z
Snapshot:  X.Y.Z-snapshot.g<erste 12 Zeichen des vollständigen Source-SHA>
```

`scripts/release_metadata.py` prüft die Quellen, erzeugt die Version und
schreibt `interlis-release.json`. Die drei npm-Pakete besitzen stets dieselbe
Version und exakte interne Abhängigkeiten. npm-`snapshot` zeigt auf den
jüngsten freigegebenen Snapshot; `latest` wird nur durch ein stabiles Tag
bewegt. Bestehende Zeitstempel-Snapshots bleiben installierbar, sind aber kein
Format für neue Releases.

## Vorprüfung

Vor jeder Publikation muss der beabsichtigte Commit auf `main` liegen und CI
grün sein. Lokal mindestens ausführen:

```sh
python3 scripts/release_metadata.py check
node --test test/release/CheckReleaseVersionTest.mjs
node --test test/npm/PrepareNpmSnapshotTest.mjs
node test/docs/DocumentationTest.mjs .
```

Native Änderungen zusätzlich mit dem Build aus
[Build und Installation](build-und-installation.md) prüfen. npm-Pakete werden
nach dem WASM-Build mit `scripts/test-npm-packages.mjs` in einem leeren
Consumer-Projekt getestet.

## Snapshot von `main`

Für denselben ausgewählten Main-Commit werden zwei Workflows manuell gestartet:

1. **Release native compiler** baut die drei CLI-Archive, aktualisiert den
   beweglichen GitHub-Prerelease `snapshot` und fordert die ilic-vcpkg-Version
   samt Binary-Cache an.
2. **Publish ilic npm packages** erzeugt die drei Pakete, publiziert die
   deterministische Version mit Dist-Tag `snapshot` und lädt die Provenienz als
   Workflow-Artefakt hoch.

Die native Snapshot-Release-Referenz ist beweglich; die npm- und
vcpkg-Versionen sind unveränderlich. Keiner der Workflows startet automatisch
einen Downstream. Dieser übernimmt den neuen Stand durch seinen eigenen
Lock-Commit.

## Stabiler Release

1. Basisversion, Qualifier, Changelog und Paketmanifeste vorbereiten und alle
   Vorprüfungen ausführen.
2. Das geprüfte Commit unveränderlich als `vX.Y.Z` taggen und pushen. Der Tag
   muss exakt zur Basisversion passen.
3. Der Tag startet **Release native compiler** und **Publish ilic npm
   packages**. Der erste Workflow erstellt GitHub Release und vcpkg-Anforderung,
   der zweite publiziert die drei npm-Pakete unter `latest`.
4. GitHub-Archive und `SHA256SUMS`, npm-Versionen/Provenienz sowie den neuen
   Registry-Baseline und alle vier Binary-Cache-Triplets prüfen.
5. Downstream-Locks in separaten Commits aktualisieren.

Stabile Tags und Paketversionen werden nie verschoben, überschrieben oder
gelöscht. Eine reine Portkorrektur verwendet eine neue vcpkg-`port-version`
statt einer neuen ilic-Version.

## vcpkg-Publikation

`Publish shared vcpkg registry version` ist der serialisierte Schreiber für
`ilic` und `iox-cpp`. Er prüft Version, Source-SHA und Archiv-SHA512, schreibt
den Port in den Branch `vcpkg-registry`, aktualisiert dessen Baseline und
dispatcht danach die Binary-Cache-Workflows. Details zu Varianten und
Konsumenten stehen in [Ökosystem](ecosystem.md#vcpkg-registry-und-binary-cache).

Ein manueller Lauf der Cache-Workflows ist nur eine Reparatur für eine bereits
bekannte unveränderliche Version und ihren Registry-Baseline; er erfindet
keine neue Version.

## npm-Authentisierung und Kontrolle

Alle drei Pakete verwenden npm Trusted Publishing mit GitHub OIDC. Der Trusted
Publisher ist auf Repository `edigonzales/ilic-fork` und Workflow
`publish-npm.yml` eingeschränkt; ein dauerhaftes `NPM_TOKEN` ist nicht nötig.

```sh
npm view @ilic/repository-core@snapshot version
npm view @ilic/tools@snapshot version
npm view @ilic/compiler-wasm@snapshot version
npm dist-tag ls @ilic/compiler-wasm
```

Die drei Snapshot-Versionen müssen übereinstimmen. Für einen stabilen Release
wird zusätzlich dieselbe Version unter `latest` erwartet.

## Fehler und Wiederholung

- Vor dem ersten Publish beenden Build- oder Testfehler den Lauf ohne
  Publikation.
- npm ist über mehrere Pakete nicht transaktional. Ein erneuter Lauf desselben
  Commits akzeptiert nur eine bereits vorhandene Version mit passendem
  `gitHead` und ergänzt fehlende Pakete; nichts wird unpubliziert.
- Ein stabiler GitHub Release und ein stabiles npm-Artefakt werden nicht
  ersetzt. Ursache korrigieren und bei nötiger Codeänderung eine neue Version
  vorbereiten.
- Nach einer vcpkg-Störung denselben Source-SHA, dieselbe Version und den
  bereits erzeugten Registry-Baseline verwenden. Keinen beweglichen
  Branchkopf als Lock übernehmen.
