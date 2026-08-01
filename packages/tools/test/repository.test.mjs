import { readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import test from "node:test";
import assert from "node:assert/strict";
import { FetchRepositoryTransport } from "../browser.js";
import { MemoryCache, RepositoryManager } from "../index.js";

const here = dirname(fileURLToPath(import.meta.url));
const fixture = resolve(here, "../../../test/repository/fixture");

test("resolves repository dependency closure", async () => {
  const manager = new RepositoryManager({
    repositories: [fixture],
    load: uri => readFile(uri),
    followSiteLinks: false
  });
  const workspace = await manager.resolveModel("RepositoryRoot", "ili2_3");
  assert.deepEqual(workspace.models.map(model => model.metadata.name),
    ["RepositoryBase", "RepositoryRoot"]);
});

const catalog = (name, file = `${name}.ili`, extra = "") => `<?xml version="1.0"?>
<TRANSFER><DATASECTION><IliRepository20.RepositoryIndex.ModelMetadata>
  <Name>${name}</Name><File>${file}</File><SchemaLanguage>ili2_4</SchemaLanguage>
  <Version>1</Version>${extra}
</IliRepository20.RepositoryIndex.ModelMetadata></DATASECTION></TRANSFER>`;

test("binds the browser default fetch and preserves injected fetchers", async () => {
  const originalFetch = globalThis.fetch;
  try {
    let defaultCalled = false;
    globalThis.fetch = function (input, init) {
      defaultCalled = true;
      assert.equal(this, globalThis);
      assert.equal(input, "https://default.invalid/model.ili");
      assert.equal(init.redirect, "manual");
      return Promise.resolve(new Response("default", { status: 200 }));
    };
    const defaultTransport = new FetchRepositoryTransport();
    const defaultResult = await defaultTransport.get({
      uri: "https://default.invalid/model.ili",
      maxBytes: 1024,
      maxRedirects: 3
    });
    assert.equal(defaultCalled, true);
    assert.equal(defaultResult.success, true);
    assert.deepEqual([...defaultResult.body], [...new TextEncoder().encode("default")]);

    let injectedCalled = false;
    const injectedTransport = new FetchRepositoryTransport({
      fetch: async () => {
        injectedCalled = true;
        return new Response("injected", { status: 200 });
      }
    });
    const injectedResult = await injectedTransport.get({
      uri: "https://injected.invalid/model.ili",
      maxBytes: 1024,
      maxRedirects: 3
    });
    assert.equal(injectedCalled, true);
    assert.equal(injectedResult.success, true);
    assert.deepEqual([...injectedResult.body], [...new TextEncoder().encode("injected")]);
  } finally {
    globalThis.fetch = originalFetch;
  }
});

test("keeps usable catalogs when another repository is unavailable", async () => {
  const warnings = [];
  const manager = new RepositoryManager({
    repositories: ["https://broken.invalid", "https://usable.invalid"],
    followSiteLinks: false,
    onWarning: warning => warnings.push(warning),
    load: async uri => {
      if (uri.startsWith("https://broken.invalid")) throw new Error("offline");
      return catalog("Usable");
    }
  });
  assert.deepEqual((await manager.listModels()).map(model => model.name), ["Usable"]);
  assert.equal(warnings.length, 1);
  assert.equal(warnings[0].operation, "metadata");
});

test("uses a warm cache without network access", async () => {
  const cache = new MemoryCache();
  await cache.put(
    "https://cached.invalid/ilimodels.xml",
    new TextEncoder().encode(catalog("Cached"))
  );
  await cache.put(
    "https://cached.invalid/Cached.ili",
    new TextEncoder().encode("INTERLIS 2.4; MODEL Cached = END Cached.")
  );
  const manager = new RepositoryManager({
    repositories: ["https://cached.invalid"],
    cache,
    offline: true,
    followSiteLinks: false,
    load: async () => {
      throw new Error("network must not be used");
    }
  });
  const workspace = await manager.resolveModel("Cached", "ili2_4");
  assert.equal(workspace.models[0].fromCache, true);
  assert.match(workspace.models[0].source, /MODEL Cached/);
});

test("fails clearly if no configured repository can be loaded", async () => {
  const manager = new RepositoryManager({
    repositories: ["https://one.invalid", "https://two.invalid"],
    followSiteLinks: false,
    load: async () => {
      throw new Error("unavailable");
    }
  });
  await assert.rejects(
    manager.listModels(),
    error => error instanceof AggregateError && error.errors.length === 2
  );
});
