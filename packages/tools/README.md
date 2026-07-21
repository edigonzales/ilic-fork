# `@ilic/tools`

`@ilic/tools` findet INTERLIS-Modelle in lokalen und entfernten Repositories,
löst ihre Abhängigkeiten auf und verwaltet die geladenen Ressourcen in einem
konfigurierbaren Cache. Das Paket ist die Hostbibliothek für
`@ilic/compiler-wasm` in Node.js und Browsern.

Die derzeitigen Vorabversionen werden unter dem npm-Dist-Tag `snapshot`
veröffentlicht:

```sh
npm install @ilic/tools@snapshot
```

## Node.js

```js
import { RepositoryManager } from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";

const repositories = new RepositoryManager({
  repositories: ["https://models.interlis.ch"],
  cache: new NodeFileCache(".cache/ilic"),
  metadataTtlMs: 24 * 60 * 60 * 1000,
  modelTtlMs: 7 * 24 * 60 * 60 * 1000,
  allowStaleOnError: true,
  followSiteLinks: true
});

const workspace = await repositories.resolveModel("DatasetIdx16", "ili2_3");
console.log(workspace.models.map(model => model.metadata.name));
```

`resolveWorkspace` gibt die Dateien dependency-first zurück. `INTERLIS` gilt
als eingebaut und wird nicht heruntergeladen. Frühere Repositories haben
Vorrang. Innerhalb desselben Repository wird die aktuelle Version über die
`Version`-/`precursorVersion`-Kette gewählt; `browseOnly=true` ist nicht
kompilierbar. Ohne Sprachargument gilt `ili2_4`, dann `ili2_3`, dann `ili1`.

## Ephemerer Cache und eigener Transport

`MemoryCache` eignet sich für Tests oder kurzlebige Prozesse. Mit einer eigenen
`load`-Funktion können lokale Dateien, Authentisierung oder ein Proxy angebunden
werden:

```js
import { readFile } from "node:fs/promises";
import { MemoryCache, RepositoryManager } from "@ilic/tools";

const repositories = new RepositoryManager({
  repositories: ["/srv/interlis-repository"],
  cache: new MemoryCache(),
  load: uri => readFile(uri),
  followSiteLinks: false
});
```

Eine HTTP-Variante mit Authentisierung:

```js
const repositories = new RepositoryManager({
  repositories: ["https://internal.example/models"],
  load: async uri => {
    const response = await fetch(uri, {
      headers: { Authorization: `Bearer ${token}` }
    });
    if (!response.ok) throw new Error(`${response.status} ${response.statusText}`);
    return new Uint8Array(await response.arrayBuffer());
  }
});
```

## Browser und IndexedDB

```js
import { RepositoryManager } from "@ilic/tools";
import { BrowserCache } from "@ilic/tools/browser";

const repositories = new RepositoryManager({
  repositories: ["https://models.interlis.ch"],
  cache: new BrowserCache("my-interlis-models")
});
const workspace = await repositories.resolveModel("DatasetIdx16", "ili2_3");
```

`BrowserCache` verwendet IndexedDB. Entfernte Repositories müssen CORS für den
Ursprung der Anwendung erlauben.

## Offlinebetrieb und Fehler

- `offline: true` erlaubt ausschließlich bereits gecachte Ressourcen.
- `allowStaleOnError: true` verwendet bei Ladefehlern einen abgelaufenen
  Cacheeintrag.
- `ilimodels.xml` ist obligatorisch, `ilisite.xml` optional.
- Abhängigkeitszyklen, fehlende Modelle, unsichere relative Pfade und
  abweichende MD5-Werte brechen die Auflösung mit einer Exception ab.

## Zusammenspiel mit dem WASM-Compiler

```js
import { createCompiler } from "@ilic/compiler-wasm";

const root = workspace.models.find(
  model => model.metadata.name === "DatasetIdx16"
);
const compiler = await createCompiler();
const session = compiler.createSession();
try {
  session.putWorkspace(workspace);
  const result = session.compile({ roots: [root.uri] });
  console.log(result.success, result.diagnostics);
} finally {
  session.dispose();
}
```

Weitere Details stehen in der
[Repository-Dokumentation](https://github.com/edigonzales/ilic-fork/blob/main/docs/repositories.md)
und der
[WASM-Dokumentation](https://github.com/edigonzales/ilic-fork/blob/main/docs/wasm.md).

Lizenz: MIT.
