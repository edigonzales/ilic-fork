# `@ilic/compiler-wasm`

`@ilic/compiler-wasm` stellt den nativen C++-INTERLIS-Compiler und Formatter
als WebAssembly-Modul für Node.js, Browser und Web Worker bereit. Die
JavaScript-API verwendet dieselbe C-ABI und dasselbe JSON-Protokoll wie eine
native Einbettung.

Die derzeitigen Vorabversionen werden unter dem npm-Dist-Tag `snapshot`
veröffentlicht:

```sh
npm install @ilic/compiler-wasm@snapshot
```

## Kompilieren und formatieren

```js
import { createCompiler } from "@ilic/compiler-wasm";

const compiler = await createCompiler();
const session = compiler.createSession();
const uri = "memory:///Example.ili";

try {
  session.putSource(uri, `INTERLIS 2.3;
!! Dieser Kommentar bleibt beim Formatieren erhalten.
MODEL Example AT "https://example.invalid" VERSION "1" =
END Example.
`, 1);

  const compilation = session.compile({ roots: [uri] });
  console.log(compilation.success, compilation.diagnostics);

  const formatting = session.format(uri, {
    indentSize: 2,
    requireValidSyntax: true
  });
  console.log(formatting.text);
} finally {
  session.dispose();
}
```

Eine Session kann mehrere virtuelle Dateien und Imports enthalten.
`putSource` akzeptiert UTF-8 als `string` oder `Uint8Array`. Diagnostics
enthalten stabile Codes sowie, soweit verfügbar, nullbasierte Zeilen und
UTF-16-Spalten.

## Repository-Modelle

Netzwerk, Dateisystem und Cache liegen bewusst außerhalb des WASM-Moduls.
[`@ilic/tools`](https://www.npmjs.com/package/@ilic/tools) löst Modelle und
Abhängigkeiten auf und liefert einen virtuellen Workspace:

```js
import { createCompiler } from "@ilic/compiler-wasm";
import { RepositoryManager } from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";

const repositories = new RepositoryManager({
  repositories: ["https://models.interlis.ch"],
  cache: new NodeFileCache(".cache/ilic")
});
const workspace = await repositories.resolveModel("DatasetIdx16", "ili2_3");
const root = workspace.models.find(
  model => model.metadata.name === "DatasetIdx16"
);

const compiler = await createCompiler();
const session = compiler.createSession();
try {
  session.putWorkspace(workspace);
  const result = session.compile({ roots: [root.uri] });
  console.log(result.diagnostics);
} finally {
  session.dispose();
}
```

## Web Worker

Der Export `@ilic/compiler-wasm/worker` implementiert eine einfache
Request-/Response-Grenze. Beim Deployment müssen `worker.js`, `ilic.mjs` und
`ilic.wasm` so kopiert werden, dass ihre relative Lage erhalten bleibt. Der
folgende Aufruf nimmt an, dass diese drei Dateien nach `/assets/ilic/` kopiert
wurden:

```js
const worker = new Worker("/assets/ilic/worker.js", { type: "module" });

worker.postMessage({ id: 1, method: "createSession", args: [] });
worker.onmessage = ({ data }) => console.log(data);
```

Unterstützt werden `createSession`, `disposeSession`, `putSource`,
`removeSource`, `compile` und `format`. Für produktiven Einsatz sollte der Host
Requests über `id` Promise-basiert korrelieren.

## Browser und Bundler

Der Emscripten-Loader sucht `ilic.wasm` relativ zu `ilic.mjs`. Ein Bundler muss
die WASM-Datei als Asset übernehmen. Falls der Bundler die Dateien an
unterschiedliche Orte legt, kann über `moduleOptions.locateFile` ein Ziel
vorgegeben werden:

```js
const compiler = await createCompiler({
  moduleOptions: {
    locateFile: name => new URL(`/assets/ilic/${name}`, location.href).href
  }
});
```

## Grenzen und weiterführende Dokumentation

- Kompilation und Formatierung laufen nach dem asynchronen Modul-Load synchron.
- Das Modul enthält kein HTTP-, Dateisystem- oder Cache-Backend.
- Eine harte Abbruchgrenze wird derzeit durch Terminieren eines Workers
  umgesetzt.
- Das Paket ist ein Compiler-SDK und noch kein vollständiger LSP.

Ausführliche Dokumentation und weitere Beispiele:

- [WASM, Worker und LSP](https://github.com/edigonzales/ilic-fork/blob/main/docs/wasm.md)
- [Native API und JSON-Protokoll](https://github.com/edigonzales/ilic-fork/blob/main/docs/native-api.md)
- [Diagnostik und Logging](https://github.com/edigonzales/ilic-fork/blob/main/docs/diagnostik-und-logging.md)

Lizenz: MIT. Hinweise zur eingebetteten ANTLR-Laufzeit stehen in
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).
