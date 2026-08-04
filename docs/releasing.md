# Release `0.9.10`

Der Tag `v0.9.10` startet den koordinierten Workflow
`build-native-release.yml`. Vor der ersten Publikation prüft er die Versionen,
baut und testet die drei nativen Archive, baut WASM und installiert die drei
npm-Tarballs in ein leeres Consumer-Projekt. Die geprüften Tarballs und ihr
`release-manifest.json` werden unverändert an den Publish-Job übergeben.

Der stabile Pfad ist idempotent: vorhandene GitHub-Assets müssen byte-identisch
sein. Eine bereits vorhandene npm-Version wird nur akzeptiert, wenn ihr
`gitHead` dem Release-Commit entspricht. Publiziert wird in der Reihenfolge
`@ilic/repository-core`, `@ilic/tools`, `@ilic/compiler-wasm`; danach wird
`latest` geprüft. Der stabile Compiler-Release dispatcht Consumer bewusst
nicht automatisch.

Vor dem Tag lokal ausführen:

```sh
node scripts/check-release-version.mjs \
  --expected-version 0.9.10 --expected-tag v0.9.10
./scripts/build-wasm.sh
node scripts/prepare-npm-release.mjs \
  --expected-version 0.9.10 --expected-tag v0.9.10
node scripts/test-npm-packages.mjs \
  --staging-root build/npm-release \
  --expected-version 0.9.10 --version-kind stable
```

Danach sind weiterhin manuell und außerhalb dieses vorbereitenden Changes:
Commit/Review, Push, unveränderliches Tag, Workflow-Freigabe, Kontrolle von
GitHub-Release/SHA256/npm-Provenance sowie der kontrollierte Language-Tools-
und Pages-Release-Train.
