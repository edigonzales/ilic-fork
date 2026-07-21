import { createHash } from "node:crypto";
import { mkdtemp, readFile, readdir, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import test from "node:test";
import assert from "node:assert/strict";

import {
  MemoryCache,
  RepositoryManager,
  normalizeRepositoryUri,
  parseIliModelsXml,
  parseIliSiteXml,
  resolveRepositoryUri,
  selectLatestModelVersion,
  validateRepositoryRelativePath
} from "../index.js";
import { NodeFileCache } from "../node.js";

const here = dirname(fileURLToPath(import.meta.url));
const sharedFixture = resolve(here, "../../../test/repository/fixtures/integration");

function metadata(version, precursorVersion = "", schemaLanguage = "ili2_4", browseOnly = false) {
  return { name: "Example", version, precursorVersion, schemaLanguage, browseOnly,
    file: "Example.ili", repository: "fixture", dependencies: [], publishingDate: "", md5: "" };
}

function catalog(models) {
  return `<?xml version="1.0"?><TRANSFER><DATASECTION>${models.map(model => `
    <IliRepository20.RepositoryIndex.ModelMetadata>
      <Name>${model.name}</Name><SchemaLanguage>${model.schemaLanguage ?? "ili2_4"}</SchemaLanguage>
      <File>${model.file ?? `${model.name}.ili`}</File><Version>${model.version ?? "1"}</Version>
      ${model.precursorVersion ? `<precursorVersion>${model.precursorVersion}</precursorVersion>` : ""}
      ${model.browseOnly ? "<browseOnly>true</browseOnly>" : ""}
      ${model.md5 ? `<md5>${model.md5}</md5>` : ""}
    </IliRepository20.RepositoryIndex.ModelMetadata>`).join("")}</DATASECTION></TRANSFER>`;
}

function site(parents = [], subsidiaries = []) {
  return `<TRANSFER><DATASECTION><Site><parentSite>${parents.map(value => `<value>${value}</value>`).join("")}</parentSite>
    <subsidiarySite>${subsidiaries.map(value => `<value>${value}</value>`).join("")}</subsidiarySite></Site></DATASECTION></TRANSFER>`;
}

test("uses precursorVersion chains, browseOnly filtering and schema preference", () => {
  assert.equal(selectLatestModelVersion([metadata("2.9"), metadata("2.10", "2.9")],
    "Example", "ili2_4").version, "2.10");
  assert.equal(selectLatestModelVersion([metadata("1"), metadata("2", "1", "ili2_4", true)],
    "Example", "ili2_4").version, "1");
  assert.equal(selectLatestModelVersion([
    metadata("23", "", "ili2_3"), metadata("24", "", "ili2_4")
  ], "Example").schemaLanguage, "ili2_4");
  const warnings = [];
  assert.equal(selectLatestModelVersion([metadata("A"), metadata("B"), metadata("X", "UNKNOWN")],
    "Example", "ili2_4", warning => warnings.push(warning)).version, "A");
  assert.equal(warnings.length, 2);
});

test("normalizes repository URIs and rejects unsafe model paths", () => {
  assert.equal(normalizeRepositoryUri("https://example.org/models/"), "https://example.org/models");
  assert.equal(normalizeRepositoryUri("file:///tmp/my%20models"), "file:///tmp/my models");
  assert.equal(normalizeRepositoryUri("C:\\models"), "C:/models");
  assert.equal(resolveRepositoryUri("https://example.org/models", "../shared"),
    "https://example.org/shared");
  assert.equal(resolveRepositoryUri("https://example.org/models", "/root"),
    "https://example.org/root");
  assert.equal(resolveRepositoryUri("https://example.org/models", "//cdn.example.org/models"),
    "https://cdn.example.org/models");
  for (const path of ["Model.ili", "subdir\\nested\\Model.ili"])
    assert.equal(validateRepositoryRelativePath(path).valid, true);
  for (const path of ["../Model.ili", "/Model.ili", "C:\\Model.ili", "https://x/Model.ili"])
    assert.equal(validateRepositoryRelativePath(path).valid, false);
});

test("parses namespaced IliRepository XML and keeps site link kinds separate", () => {
  const xml = `<TRANSFER xmlns:r="urn:test"><r:IliRepository09.RepositoryIndex.ModelMetadata>
    <r:Name>Namespaced</r:Name><r:SchemaLanguage>ili2_4</r:SchemaLanguage><r:File>N.ili</r:File>
  </r:IliRepository09.RepositoryIndex.ModelMetadata></TRANSFER>`;
  assert.equal(parseIliModelsXml(xml, "fixture")[0].name, "Namespaced");
  assert.deepEqual(parseIliSiteXml(site(["P1", "P2"], ["C1"])),
    { parentSites: ["P1", "P2"], subsidiarySites: ["C1"] });
});

test("searches seeds lazily and parents breadth-first", async () => {
  const indexOrder = [];
  const resources = new Map([
    ["https://a/ilimodels.xml", catalog([])], ["https://b/ilimodels.xml", catalog([])],
    ["https://a/ilisite.xml", site(["https://p1", "https://p2"])],
    ["https://b/ilisite.xml", site(["https://p3"])],
    ["https://p1/ilimodels.xml", catalog([])], ["https://p1/ilisite.xml", site()],
    ["https://p2/ilimodels.xml", catalog([])], ["https://p2/ilisite.xml", site()],
    ["https://p3/ilimodels.xml", catalog([{ name: "Wanted" }])],
    ["https://p3/Wanted.ili", "INTERLIS 2.4; MODEL Wanted = END Wanted."]
  ]);
  const manager = new RepositoryManager({ repositories: ["https://a", "https://b"],
    load: async uri => {
      if (uri.endsWith("ilimodels.xml")) indexOrder.push(uri.split("/")[2]);
      if (!resources.has(uri)) throw new Error("missing");
      return resources.get(uri);
    } });
  assert.equal((await manager.resolveModel("Wanted", "ili2_4")).models[0].metadata.repository,
    "https://p3");
  assert.deepEqual(indexOrder, ["a", "b", "p1", "p2", "p3"]);
});

test("does not load a later seed after an earlier seed matches", async () => {
  const requests = [];
  const manager = new RepositoryManager({ repositories: ["https://a", "https://b"],
    followSiteLinks: false, load: async uri => {
      requests.push(uri);
      if (uri.endsWith("ilimodels.xml")) return catalog([{ name: "Wanted" }]);
      return "INTERLIS 2.4; MODEL Wanted = END Wanted.";
    } });
  await manager.resolveModel("Wanted", "ili2_4");
  assert.equal(requests.some(uri => uri.startsWith("https://b")), false);
});

test("shares native conformance fixtures for versions, dependencies, files and cycles", async () => {
  const manager = new RepositoryManager({ repositories: [sharedFixture],
    followSiteLinks: false, load: uri => readFile(uri) });
  const root = await manager.resolveModel("Root");
  assert.deepEqual(root.models.map(model => model.metadata.name), ["Base", "Root"]);
  assert.equal(root.models[1].metadata.version, "2.10");
  assert.equal((await manager.resolveWorkspace(["SharedA", "SharedB"], "ili2_4")).models.length, 1);
  await assert.rejects(
    manager.resolveWorkspace(["SharedA", "SharedBadChecksum"], "ili2_4"), /MD5 mismatch/);
  assert.equal((await manager.resolveModel("Browsable", "ili2_4")).models[0].metadata.version, "1");
  assert.equal((await manager.resolveModel("LanguageChoice")).models[0].metadata.schemaLanguage, "ili2_4");
  await assert.rejects(manager.resolveModel("CycleA", "ili2_4"),
    /CycleA -> CycleB -> CycleA/);
  await assert.rejects(manager.resolveModel("Unsafe", "ili2_4"), /unsafe repository path/);
});

test("recovers once from a corrupt cache using case-insensitive MD5", async () => {
  const cache = new MemoryCache();
  const repository = "https://checksum";
  const modelUri = `${repository}/Checked.ili`;
  const correct = "correct-content";
  const checksum = createHash("md5").update(correct).digest("hex").toUpperCase();
  await cache.put(`${repository}/ilimodels.xml`, new TextEncoder().encode(catalog([
    { name: "Checked", md5: checksum }
  ])));
  await cache.put(modelUri, new TextEncoder().encode("corrupt"));
  let requests = 0;
  const manager = new RepositoryManager({ repositories: [repository], cache,
    followSiteLinks: false, load: async uri => {
      assert.equal(uri, modelUri); requests++; return correct;
    } });
  assert.equal((await manager.resolveModel("Checked", "ili2_4")).models[0].source, correct);
  assert.equal(requests, 1);
});

test("NodeFileCache uses unique temporary paths under concurrent writes", async () => {
  const directory = await mkdtemp(join(tmpdir(), "ilic-node-cache-"));
  try {
    const cache = new NodeFileCache(directory);
    await Promise.all(Array.from({ length: 12 }, (_, index) =>
      cache.put("key", new Uint8Array(1024).fill(index))));
    const entry = await cache.get("key");
    assert.equal(entry.value.byteLength, 1024);
    assert.equal(entry.value.every(value => value === entry.value[0]), true);
    assert.equal((await readdir(directory)).some(name => name.endsWith(".tmp")), false);
  } finally { await rm(directory, { recursive: true, force: true }); }
});
