# npm-Snapshot-Publikation

[Dokumentationsindex](README.md) · [Build](build-und-installation.md) · [WASM](wasm.md)

Die JavaScript-Pakete werden als öffentliche Vorabversionen unter dem
npm-Organisations-Scope `@ilic` vorbereitet:

- `@ilic/compiler-wasm`: Compiler, Formatter, Worker und WASM-Artefakt;
- `@ilic/tools`: Repository-Auflösung und Caches für Node und Browser.

Native Programme oder Bibliotheken werden durch diesen Ablauf weder paketiert
noch als GitHub Release veröffentlicht.

## Version und Dist-Tag

Die Basisversion ist die CMake-Projektversion aus `CMakeLists.txt`, aktuell
`0.9.9`. Ein Publish erhält einen UTC-Zeitstempel:

```text
0.9.9-SNAPSHOT.20260718143152
```

Der Zeitstempel hat das Format `YYYYMMDDHHmmss`. Beide Pakete erhalten in einem
Lauf dieselbe Version. Die eingecheckten `package.json` bleiben dagegen auf der
Basisversion.

npm erlaubt nicht, dieselbe Kombination aus Paketname und Version erneut zu
publizieren. Ein erneuter Lauf verwendet deshalb dieselbe Basisversion und
einen neuen Zeitstempel. Vorabversionen werden immer mit dem Dist-Tag
`snapshot` publiziert:

```sh
npm install @ilic/compiler-wasm@snapshot
npm install @ilic/tools@snapshot
```

Ohne `--tag snapshot` würde `npm publish` standardmässig den Tag `latest`
setzen. Das ist für diese Vorabversionen nicht erwünscht. Siehe
[npm-Dist-Tags](https://docs.npmjs.com/cli/v11/commands/npm-dist-tag/).

## Lokales Erzeugen und Prüfen

Zuerst muss der WASM-Build mit der in `.emscripten-version` festgelegten
Emscripten-Version erstellt werden. Die vollständige Erstinstallation ist unter
[Emscripten SDK einmalig installieren](build-und-installation.md#emscripten-sdk-einmalig-installieren)
dokumentiert:

```sh
source /pfad/zu/emsdk/emsdk_env.sh
./scripts/build-wasm.sh
```

Danach erzeugt das Staging-Skript saubere Paketverzeichnisse unter
`build/npm/`:

```sh
node scripts/prepare-npm-snapshot.mjs
```

Für einen reproduzierbaren Test kann der Zeitstempel vorgegeben werden:

```sh
node scripts/prepare-npm-snapshot.mjs \
  --timestamp 20260718143152 \
  --output build/npm
```

Das Skript bricht ab, wenn:

- die Version eines Quellpakets nicht zur CMake-Projektversion passt;
- ein Zeitstempel nicht dem UTC-Format entspricht oder kein gültiges Datum ist;
- eine in `files` deklarierte Datei fehlt;
- insbesondere `ilic.mjs` oder `ilic.wasm` noch nicht gebaut wurden;
- ein Paket keine explizite Dateiliste besitzt.

Die vollständige lokale Paketprüfung erzeugt echte Tarballs, installiert beide
in ein leeres temporäres Projekt und kompiliert und formatiert über das daraus
importierte WASM-Paket:

```sh
node scripts/test-npm-packages.mjs
```

Die Quell-Manifeste werden bei diesen Schritten nicht verändert. Alle
generierten Dateien bleiben unter dem ignorierten Verzeichnis `build/npm/`.

## Einmaliger Bootstrap auf npm

### 1. Organisation und Konto

1. Auf [npmjs.com](https://www.npmjs.com/) als Benutzer `edigonzales` anmelden.
2. Zwei-Faktor-Authentisierung oder einen Passkey aktivieren.
3. Die npm-Organisation `ilic` erstellen oder prüfen, dass `edigonzales` dort
   Schreibrechte besitzt.

Der Scope `@ilic` gehört zur npm-Organisation `ilic`, nicht automatisch zum
gleichnamigen GitHub-Repository oder zum Benutzer `edigonzales`. Öffentliche
Pakete in einem Organisations-Scope werden mit `--access public` erzeugt.
Details stehen unter
[Creating and publishing scoped public packages](https://docs.npmjs.com/creating-and-publishing-scoped-public-packages/).

### 2. Erste Paketversionen publizieren

Ein Trusted Publisher wird in den Einstellungen eines bereits vorhandenen
Pakets konfiguriert. Die beiden ersten Snapshot-Versionen werden deshalb
interaktiv gebootstrapped:

```sh
npm login
npm whoami

source /pfad/zu/emsdk/emsdk_env.sh
./scripts/build-wasm.sh
npm test --prefix packages/tools
npm test --prefix packages/compiler-wasm
node --test test/npm/PrepareNpmSnapshotTest.mjs
node scripts/prepare-npm-snapshot.mjs
node scripts/test-npm-packages.mjs

npm publish build/npm/tools --access public --tag snapshot
npm publish build/npm/compiler-wasm --access public --tag snapshot
```

Der Login und ein möglicher 2FA-Dialog finden lokal statt. Weder Passwort noch
Einmalcode werden in Dateien oder GitHub-Secrets geschrieben.

### 3. Trusted Publisher konfigurieren

Nach dem ersten Publish wird für **jedes** der beiden Pakete auf npmjs.com
unter `Package → Settings → Trusted Publisher` eingetragen:

| Feld | Wert |
| --- | --- |
| Provider | GitHub Actions |
| GitHub user/organization | `edigonzales` |
| Repository | `ilic-fork` |
| Workflow filename | `publish-npm-snapshot.yml` |
| Environment | leer |
| Allowed action | `npm publish` |

Der Dateiname muss exakt übereinstimmen; es wird nur der Dateiname und nicht
`.github/workflows/` eingetragen. Die `repository.url` beider Pakete verweist
exakt auf `https://github.com/edigonzales/ilic-fork.git`.

Trusted Publishing tauscht den kurzlebigen GitHub-OIDC-Token während
`npm publish` gegen eine ebenso kurzlebige npm-Berechtigung. Der Workflow
benötigt dafür:

```yaml
permissions:
  contents: read
  id-token: write
```

Es ist **kein** `NPM_TOKEN`-Secret im GitHub-Repository nötig. npm verlangt für
Trusted Publishing mindestens Node 22.14 und npm 11.5.1; der Workflow verwendet
Node 24 und eine festgelegte npm-11-Version. Bei einem öffentlichen Paket aus
einem öffentlichen Repository wird die Provenance automatisch mitpubliziert.
Siehe [npm Trusted Publishing](https://docs.npmjs.com/trusted-publishers/).

Nach einem erfolgreichen OIDC-Publish wird pro Paket unter
`Settings → Publishing access` die Einstellung
`Require two-factor authentication and disallow tokens` aktiviert. Der Trusted
Publisher funktioniert weiterhin, traditionelle Publish-Tokens dagegen nicht.

## GitHub Action starten

Der Workflow liegt unter `.github/workflows/publish-npm-snapshot.yml` und wird
ausschliesslich manuell ausgeführt:

1. GitHub-Repository öffnen.
2. `Actions → Publish npm snapshot → Run workflow` wählen.
3. Über GitHubs native Auswahl den gewünschten Branch oder Tag wählen.
4. Lauf starten und Build, Tests, Paketprüfung und beide Publikationen abwarten.

Der Workflow muss auf dem Default-Branch `main` vorhanden sein, damit GitHub
den manuellen Trigger anbietet. Er muss ebenfalls im ausgewählten Branch oder
Tag enthalten sein. GitHub setzt beim manuellen Trigger `GITHUB_REF` und
`GITHUB_SHA` auf den ausgewählten Branch beziehungsweise Tag; der Checkout und
der npm-Provenance-Nachweis beziehen sich dadurch auf denselben Commit. Siehe
[workflow_dispatch](https://docs.github.com/en/actions/reference/workflows-and-actions/events-that-trigger-workflows#workflow_dispatch).

Ein beliebiger Commit-SHA wird nicht über ein eigenes Workflow-Eingabefeld
ausgecheckt, weil dies den Event-SHA vom gebauten Code trennen würde. Für einen
bestimmten älteren Commit wird zuerst ein Git-Tag erstellt:

```sh
git tag npm-snapshot-test <commit-sha>
git push origin npm-snapshot-test
```

Danach wird dieser Tag in der nativen GitHub-Auswahl verwendet. Das Löschen des
Tags löscht keine bereits publizierte npm-Version.

## Token-Alternative

OIDC ist der dauerhafte Standard. Falls Trusted Publishing vorübergehend nicht
verwendet werden kann, ist ausschliesslich ein kurzlebiger granularer npm-Token
mit minimalen Schreibrechten für die beiden Pakete zu verwenden:

1. Token interaktiv auf npmjs.com erzeugen.
2. Unter `GitHub → Settings → Secrets and variables → Actions` als
   Repository-Secret `NPM_TOKEN` speichern.
3. Nur im Publish-Schritt als `NODE_AUTH_TOKEN` übergeben:

```yaml
- run: npm publish build/npm/tools --access public --tag snapshot
  env:
    NODE_AUTH_TOKEN: ${{ secrets.NPM_TOKEN }}
```

Der echte Tokenwert darf nie in Git, Logs, `package.json` oder `.npmrc`
stehen. Nach der Migration zu OIDC wird das Secret gelöscht und der Token auf
npmjs.com widerrufen. npm schränkt tokenbasierte direkte Publikationen weiter
ein; siehe
[npm token security changes](https://github.blog/changelog/2026-07-08-npm-install-time-security-and-gat-bypass2fa-deprecation/).

## Kontrolle nach einem Publish

```sh
npm view @ilic/tools@snapshot version
npm view @ilic/compiler-wasm@snapshot version
npm dist-tag ls @ilic/tools
npm dist-tag ls @ilic/compiler-wasm
```

Beide `snapshot`-Tags müssen dieselbe Version melden. Vor der ersten stabilen
Publikation darf kein Snapshot den Tag `latest` besitzen. Auf den npm-Seiten
zeigt der Bereich `Provenance` Repository, Workflow und Commit des OIDC-Laufs.

## Teilfehler und Wiederholung

Alle Builds, Tests und Tarball-Prüfungen laufen vor dem ersten Publish. npm
stellt jedoch keine Transaktion über mehrere Pakete bereit. Falls
`@ilic/tools` erfolgreich publiziert wurde und der nachfolgende Publish von
`@ilic/compiler-wasm` fehlschlägt:

1. Ursache anhand des Action-Logs beheben;
2. Workflow erneut starten;
3. der neue Lauf verwendet einen neuen Zeitstempel und publiziert wieder beide
   Pakete;
4. nichts automatisch löschen oder unpublishen.

Die Pakete besitzen keine npm-Laufzeitabhängigkeit voneinander. Eine alte,
nicht mehr getaggte Snapshot-Version von `@ilic/tools` beeinträchtigt deshalb
den Compiler nicht.

## Noch nicht Bestandteil

- stabile Versionen und der Dist-Tag `latest`;
- automatische Publikation anhand von Release-Tags;
- GitHub Releases;
- native Binaries oder native Bibliothekspakete.
