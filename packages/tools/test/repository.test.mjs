import { readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import test from "node:test";
import assert from "node:assert/strict";
import { RepositoryManager } from "../index.js";

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
