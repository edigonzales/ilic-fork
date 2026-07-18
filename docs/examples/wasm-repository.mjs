import { readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";
import { createCompiler } from "../../packages/compiler-wasm/index.js";
import { MemoryCache, RepositoryManager } from "../../packages/tools/index.js";

const here = dirname(fileURLToPath(import.meta.url));

async function load(uri) {
  if (/^https?:\/\//.test(uri)) {
    const response = await fetch(uri);
    if (!response.ok) throw new Error(`${response.status} ${response.statusText}`);
    return new Uint8Array(await response.arrayBuffer());
  }
  return new Uint8Array(await readFile(uri));
}

export async function runWasmRepositoryExample({
  repository = resolve(here,"../../test/repository/fixture"),
  model = "RepositoryRoot",
  schemaLanguage = "ili2_3"
} = {}) {
  const repositories = new RepositoryManager({
    repositories: [repository],
    cache: new MemoryCache(),
    load,
    followSiteLinks: false
  });
  const workspace = await repositories.resolveModel(model,schemaLanguage);
  const root = workspace.models.find(entry => entry.metadata.name === model);
  if (!root) throw new Error(`resolved workspace has no root model ${model}`);

  const compiler = await createCompiler();
  const session = compiler.createSession();
  session.putWorkspace(workspace);
  try {
    return session.compile({ roots: [root.uri] });
  } finally {
    session.dispose();
  }
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const result = await runWasmRepositoryExample();
  if (!result.success) throw new Error(JSON.stringify(result.diagnostics));
  console.log(`compiled ${result.models.length} models from a repository workspace`);
}
