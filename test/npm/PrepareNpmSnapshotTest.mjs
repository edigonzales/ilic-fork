import assert from "node:assert/strict";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { join } from "node:path";
import { tmpdir } from "node:os";
import test from "node:test";
import { prepareNpmSnapshot } from "../../scripts/prepare-npm-snapshot.mjs";

const sourceSha = "0123456789abcdef0123456789abcdef01234567";
const snapshotVersion = "0.10.0-snapshot.g0123456789ab";

async function writeJson(path, value) {
  await writeFile(path, `${JSON.stringify(value, null, 2)}\n`);
}

async function createFixture(t) {
  const root = await mkdtemp(join(tmpdir(), "ilic-npm-snapshot-"));
  t.after(() => rm(root, { recursive: true, force: true }));
  for (const name of ["tools", "compiler-wasm", "repository-core"]) {
    await mkdir(join(root, `packages/${name}`), { recursive: true });
  }
  await writeFile(join(root, "CMakeLists.txt"),
    "project(ilic VERSION 0.10.0 LANGUAGES C CXX)\n" +
    "set(ILIC_VERSION_QUALIFIER \"SNAPSHOT\")\n");
  const license = "MIT License\n";
  await writeJson(join(root, "packages/tools/package.json"), {
    name: "@ilic/tools", version: "0.10.0-SNAPSHOT",
    dependencies: { "@ilic/repository-core": "0.10.0-SNAPSHOT" },
    files: ["LICENSE", "README.md", "index.js"]
  });
  await writeJson(join(root, "packages/compiler-wasm/package.json"), {
    name: "@ilic/compiler-wasm", version: "0.10.0-SNAPSHOT",
    files: ["LICENSE", "THIRD_PARTY_NOTICES.md", "README.md", "index.js", "ilic.mjs", "ilic.wasm"]
  });
  await writeJson(join(root, "packages/repository-core/package.json"), {
    name: "@ilic/repository-core", version: "0.10.0-SNAPSHOT",
    files: ["LICENSE", "README.md", "index.js"]
  });
  for (const name of ["tools", "compiler-wasm", "repository-core"]) {
    await writeFile(join(root, `packages/${name}/LICENSE`), license);
    await writeFile(join(root, `packages/${name}/README.md`), `${name}\n`);
    await writeFile(join(root, `packages/${name}/index.js`), "export {};\n");
  }
  await writeFile(join(root, "packages/compiler-wasm/THIRD_PARTY_NOTICES.md"), "ANTLR 4 C++ Runtime\nThe BSD License\n");
  await writeFile(join(root, "packages/compiler-wasm/ilic.mjs"), "export default {};\n");
  await writeFile(join(root, "packages/compiler-wasm/ilic.wasm"), new Uint8Array([0, 97, 115, 109]));
  const releaseManifestPath = join(root, "interlis-release.json");
  await writeJson(releaseManifestPath, {
    schemaVersion: 1,
    project: "ilic",
    artifactVersion: snapshotVersion,
    sourceSha,
    dependencies: {},
    build: { githubRunId: "123", publishedAt: "2026-08-28T18:00:00Z" }
  });
  return { root, releaseManifestPath };
}

test("stages all packages with one SHA-derived snapshot without mutating sources", async t => {
  const { root, releaseManifestPath } = await createFixture(t);
  const sourcePaths = ["tools", "compiler-wasm", "repository-core"]
    .map(name => join(root, `packages/${name}/package.json`));
  const before = await Promise.all(sourcePaths.map(path => readFile(path, "utf8")));
  const result = await prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    sourceSha,
    releaseManifestPath,
  });
  assert.equal(result.snapshotVersion, snapshotVersion);
  const tools = JSON.parse(await readFile(join(result.directories.tools, "package.json"), "utf8"));
  assert.equal(tools.version, snapshotVersion);
  assert.equal(tools.gitHead, sourceSha);
  assert.equal(tools.dependencies["@ilic/repository-core"], snapshotVersion);
  assert.ok(tools.files.includes("interlis-release.json"));
  assert.equal(
    JSON.parse(await readFile(join(result.directories.tools, "interlis-release.json"), "utf8")).sourceSha,
    sourceSha,
  );
  assert.deepEqual(await Promise.all(sourcePaths.map(path => readFile(path, "utf8"))), before);
});

test("rejects a release manifest for another source", async t => {
  const { root, releaseManifestPath } = await createFixture(t);
  const manifest = JSON.parse(await readFile(releaseManifestPath, "utf8"));
  manifest.sourceSha = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  await writeJson(releaseManifestPath, manifest);
  await assert.rejects(() => prepareNpmSnapshot({
    projectRoot: root,
    outputRoot: join(root, "build/npm"),
    sourceSha,
    releaseManifestPath,
  }), /release manifest/i);
});

test("rejects missing WASM package artifacts", async t => {
  for (const file of ["ilic.mjs", "ilic.wasm"]) {
    await t.test(file, async subtest => {
      const { root, releaseManifestPath } = await createFixture(subtest);
      await rm(join(root, `packages/compiler-wasm/${file}`));
      await assert.rejects(() => prepareNpmSnapshot({
        projectRoot: root,
        outputRoot: join(root, "build/npm"),
        sourceSha,
        releaseManifestPath,
      }), new RegExp(`Missing .*${file.replace(".", "\\.")}`));
    });
  }
});
