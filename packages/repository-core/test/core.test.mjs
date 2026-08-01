import assert from "node:assert/strict";
import test from "node:test";
import {
  ManualRepositoryClock,
  Md5RepositoryChecksum,
  RepositoryManagerCore,
  SafeRepositoryMetadataDecoder,
  classifyCacheEntry,
  md5
} from "../index.js";

const encoder = new TextEncoder();
const repository = "https://core.invalid/repository";
const modelSource = "INTERLIS 2.4; MODEL CoreModel = END CoreModel.";
const indexSource = `<?xml version="1.0"?><TRANSFER><DATASECTION><IliRepository20.RepositoryIndex.ModelMetadata><Name>CoreModel</Name><File>CoreModel.ili</File><SchemaLanguage>ili2_4</SchemaLanguage><Version>1</Version><md5>${md5(encoder.encode(modelSource))}</md5></IliRepository20.RepositoryIndex.ModelMetadata></DATASECTION></TRANSFER>`;

test("uses the injected clock and treats the TTL boundary as stale", () => {
  const clock = new ManualRepositoryClock(1000);
  const entry = { value: encoder.encode("value"), storedAt: clock.now() };
  assert.equal(classifyCacheEntry(entry, 1999, 1000), "fresh");
  assert.equal(classifyCacheEntry(entry, 2000, 1000), "stale");
  clock.advance(1000);
  assert.equal(clock.now(), 2000);
});

test("decodes safe metadata and rejects DTD input", () => {
  const decoder = new SafeRepositoryMetadataDecoder();
  assert.equal(decoder.decodeModelIndex(indexSource, repository).success, true);
  const rejected = decoder.decodeModelIndex("<!DOCTYPE x [<!ENTITY x SYSTEM 'file:///secret'>]><x/>", repository);
  assert.equal(rejected.success, false);
  assert.match(rejected.diagnostics[0].message, /DTD|entity/i);
});

test("resolves a workspace through explicit ports", async () => {
  const values = new Map([
    [`${repository}/ilimodels.xml`, encoder.encode(indexSource)],
    [`${repository}/CoreModel.ili`, encoder.encode(modelSource)]
  ]);
  const calls = [];
  const cache = {
    async get() { return undefined; },
    async put() {},
    async clear() {}
  };
  const transport = {
    async get(request) {
      calls.push(request);
      const body = values.get(request.uri);
      return body ? { success: true, statusCode: 200, body, finalUri: request.uri }
        : { success: false, statusCode: 404, error: "not found", notFound: true };
    }
  };
  const clock = { now: () => 0 };
  const decoder = new SafeRepositoryMetadataDecoder();
  const manager = new RepositoryManagerCore({
    repositories: [repository], transport, cache, clock, decoder,
    metadataDecoder: decoder, checksum: new Md5RepositoryChecksum(),
    policy: { followSiteLinks: false }
  });
  const workspace = await manager.resolveModel("CoreModel", "ili2_4");
  assert.equal(workspace.models[0].uri, `${repository}/CoreModel.ili`);
  assert.equal(workspace.models[0].source, modelSource);
  assert.deepEqual(calls.map(value => value.uri), [
    `${repository}/ilimodels.xml`, `${repository}/CoreModel.ili`
  ]);
});
