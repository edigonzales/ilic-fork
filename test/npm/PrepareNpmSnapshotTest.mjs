import assert from "node:assert/strict";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { join } from "node:path";
import { tmpdir } from "node:os";
import test from "node:test";
import {
  formatUtcTimestamp,
  prepareNpmSnapshot
} from "../../scripts/prepare-npm-snapshot.mjs";

const fixedTimestamp = "20260718143152";

async function writeJson(path, value) {
  await writeFile(path, `${JSON.stringify(value, null, 2)}\n`);
}

async function createFixture(t) {
  const root = await mkdtemp(join(tmpdir(), "ilic-npm-snapshot-"));
  t.after(() => rm(root, { recursive: true, force: true }));
  await mkdir(join(root, "packages/tools"), { recursive: true });
  await mkdir(join(root, "packages/compiler-wasm"), { recursive: true });
  await writeFile(join(root, "CMakeLists.txt"),
    "project(ilic VERSION 0.9.9 LANGUAGES C CXX)\n");
  await writeJson(join(root, "packages/tools/package.json"), {
    name: "@ilic/tools",
    version: "0.9.9",
    files: ["README.md", "index.js"]
  });
  await writeFile(join(root, "packages/tools/README.md"), "tools\n");
  await writeFile(join(root, "packages/tools/index.js"), "export {};\n");
  await writeJson(join(root, "packages/compiler-wasm/package.json"), {
    name: "@ilic/compiler-wasm",
    version: "0.9.9",
    files: ["README.md", "index.js", "ilic.mjs", "ilic.wasm"]
  });
  await writeFile(join(root, "packages/compiler-wasm/README.md"), "compiler\n");
  await writeFile(join(root, "packages/compiler-wasm/index.js"), "export {};\n");
  await writeFile(join(root, "packages/compiler-wasm/ilic.mjs"), "export default {};\n");
  await writeFile(join(root, "packages/compiler-wasm/ilic.wasm"), new Uint8Array([0, 97, 115, 109]));
  return root;
}

test("formats UTC timestamps", () => {
  assert.equal(formatUtcTimestamp(new Date("2026-07-18T14:31:52Z")), fixedTimestamp);
});

test("stages both packages with one snapshot version without mutating sources", async t => {
  const root = await createFixture(t);
  const toolsManifest = join(root, "packages/tools/package.json");
  const compilerManifest = join(root, "packages/compiler-wasm/package.json");
  const before = await Promise.all([readFile(toolsManifest, "utf8"), readFile(compilerManifest, "utf8")]);

  const result = await prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    timestamp: fixedTimestamp
  });

  assert.equal(result.baseVersion, "0.9.9");
  assert.equal(result.snapshotVersion, `0.9.9-SNAPSHOT.${fixedTimestamp}`);
  const staged = await Promise.all([
    readFile(join(result.directories.tools, "package.json"), "utf8"),
    readFile(join(result.directories.compiler_wasm, "package.json"), "utf8")
  ]);
  assert.equal(JSON.parse(staged[0]).version, result.snapshotVersion);
  assert.equal(JSON.parse(staged[1]).version, result.snapshotVersion);
  assert.deepEqual(await Promise.all([
    readFile(toolsManifest, "utf8"), readFile(compilerManifest, "utf8")
  ]), before);
});

test("adds a numeric build ID to both compiler package versions", async t => {
  const root = await createFixture(t);
  const result = await prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    timestamp: fixedTimestamp,
    buildId: "12345"
  });

  assert.equal(result.snapshotVersion, `0.9.9-SNAPSHOT.${fixedTimestamp}.12345`);
  const manifest = JSON.parse(await readFile(join(result.directories.tools, "package.json"), "utf8"));
  assert.equal(manifest.version, result.snapshotVersion);
});

test("rejects non-numeric build IDs", async t => {
  const root = await createFixture(t);
  await assert.rejects(() => prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    timestamp: fixedTimestamp,
    buildId: "run-123"
  }), /build ID/i);
});

test("rejects a package version that differs from CMake", async t => {
  const root = await createFixture(t);
  const path = join(root, "packages/tools/package.json");
  const manifest = JSON.parse(await readFile(path, "utf8"));
  await writeJson(path, { ...manifest, version: "0.9.8" });
  await assert.rejects(() => prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    timestamp: fixedTimestamp
  }), /does not match project version/);
});

test("rejects malformed and impossible timestamps", async t => {
  const root = await createFixture(t);
  for (const timestamp of ["2026-07-18", "20260230120000"]) {
    await assert.rejects(() => prepareNpmSnapshot({
      projectRoot: root,
      outputRoot: join(root, "build/npm"),
      timestamp
    }), /timestamp/i);
  }
});

test("rejects missing WASM package artifacts", async t => {
  for (const file of ["ilic.mjs", "ilic.wasm"]) {
    await t.test(file, async subtest => {
      const root = await createFixture(subtest);
      await rm(join(root, `packages/compiler-wasm/${file}`));
      await assert.rejects(() => prepareNpmSnapshot({
        projectRoot: root,
        outputRoot: join(root, "build/npm"),
        timestamp: fixedTimestamp
      }), new RegExp(`Missing .*${file.replace(".", "\\.")}`));
    });
  }
});
