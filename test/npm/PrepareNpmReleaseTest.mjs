import assert from "node:assert/strict";
import {
  mkdtemp,
  mkdir,
  readFile,
  rm,
  writeFile,
} from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";

import { prepareNpmRelease } from "../../scripts/prepare-npm-release.mjs";

async function writeJson(path, value) {
  await writeFile(path, `${JSON.stringify(value, null, 2)}\n`);
}

async function createFixture(t) {
  const root = await mkdtemp(join(tmpdir(), "ilic-npm-release-"));
  t.after(() => rm(root, { recursive: true, force: true }));
  await writeFile(
    join(root, "CMakeLists.txt"),
    "project(ilic VERSION 0.9.10 LANGUAGES C CXX)\n",
  );
  const license = "MIT License\n";
  for (const directory of ["repository-core", "tools", "compiler-wasm"]) {
    await mkdir(join(root, "packages", directory), { recursive: true });
  }
  await writeJson(join(root, "packages/repository-core/package.json"), {
    name: "@ilic/repository-core",
    version: "0.9.10",
    files: ["LICENSE", "README.md", "index.js"],
  });
  await writeJson(join(root, "packages/tools/package.json"), {
    name: "@ilic/tools",
    version: "0.9.10",
    dependencies: { "@ilic/repository-core": "0.9.10" },
    files: ["LICENSE", "README.md", "index.js"],
  });
  await writeJson(join(root, "packages/compiler-wasm/package.json"), {
    name: "@ilic/compiler-wasm",
    version: "0.9.10",
    files: [
      "LICENSE",
      "THIRD_PARTY_NOTICES.md",
      "README.md",
      "index.js",
      "ilic.mjs",
      "ilic.wasm",
    ],
  });
  for (const directory of ["repository-core", "tools", "compiler-wasm"]) {
    const packageRoot = join(root, "packages", directory);
    await writeFile(join(packageRoot, "LICENSE"), license);
    await writeFile(join(packageRoot, "README.md"), `${directory}\n`);
    await writeFile(join(packageRoot, "index.js"), "export {};\n");
  }
  await writeFile(
    join(root, "packages/compiler-wasm/THIRD_PARTY_NOTICES.md"),
    "ANTLR 4 C++ Runtime\nThe BSD License\n",
  );
  await writeFile(
    join(root, "packages/compiler-wasm/ilic.mjs"),
    "export default {};\n",
  );
  await writeFile(
    join(root, "packages/compiler-wasm/ilic.wasm"),
    new Uint8Array([0, 97, 115, 109]),
  );
  return root;
}

test("stages all stable packages without mutating source manifests", async (t) => {
  const root = await createFixture(t);
  const sourceManifests = [
    "repository-core",
    "tools",
    "compiler-wasm",
  ].map((name) => join(root, "packages", name, "package.json"));
  const before = await Promise.all(
    sourceManifests.map((path) => readFile(path, "utf8")),
  );
  const result = await prepareNpmRelease({
    projectRoot: root,
    outputRoot: join(root, "build/npm-release"),
    expectedVersion: "0.9.10",
    expectedTag: "v0.9.10",
  });
  assert.equal(result.releaseVersion, "0.9.10");
  for (const directory of Object.values(result.directories)) {
    assert.equal(
      JSON.parse(await readFile(join(directory, "package.json"), "utf8"))
        .version,
      "0.9.10",
    );
  }
  const tools = JSON.parse(
    await readFile(join(result.directories.tools, "package.json"), "utf8"),
  );
  assert.equal(tools.dependencies["@ilic/repository-core"], "0.9.10");
  assert.deepEqual(
    await Promise.all(sourceManifests.map((path) => readFile(path, "utf8"))),
    before,
  );
});

test("rejects mismatched versions and malformed tags", async (t) => {
  const root = await createFixture(t);
  await assert.rejects(
    () => prepareNpmRelease({ projectRoot: root, expectedVersion: "0.9.9" }),
    /expected release version/i,
  );
  await assert.rejects(
    () => prepareNpmRelease({ projectRoot: root, expectedTag: "v0.9.9" }),
    /does not match/i,
  );
  await assert.rejects(
    () => prepareNpmRelease({ projectRoot: root, expectedTag: "0.9.10" }),
    /vX\.Y\.Z/i,
  );
});

test("rejects missing WASM artifacts and unsafe output roots", async (t) => {
  for (const file of ["ilic.mjs", "ilic.wasm"]) {
    await t.test(file, async (subtest) => {
      const root = await createFixture(subtest);
      await rm(join(root, "packages/compiler-wasm", file));
      await assert.rejects(
        () => prepareNpmRelease({ projectRoot: root }),
        new RegExp(`Missing .*${file.replace(".", "\\.")}`),
      );
    });
  }
  const root = await createFixture(t);
  for (const outputRoot of [root, join(root, "packages/tools")]) {
    await assert.rejects(
      () => prepareNpmRelease({ projectRoot: root, outputRoot }),
      /refusing|must use/i,
    );
  }
});

test("rejects an unapproved internal dependency version", async (t) => {
  const root = await createFixture(t);
  const path = join(root, "packages/tools/package.json");
  const manifest = JSON.parse(await readFile(path, "utf8"));
  manifest.dependencies["@ilic/repository-core"] = "0.9.9";
  await writeJson(path, manifest);
  await assert.rejects(
    () => prepareNpmRelease({ projectRoot: root }),
    /dependency .* does not match project version/i,
  );
});
