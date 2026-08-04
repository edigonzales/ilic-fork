#!/usr/bin/env node

import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { basename, resolve } from "node:path";

function run(command, args, options = {}) {
  const result = spawnSync(command, args, {
    cwd: options.cwd,
    encoding: "utf8",
    env: { ...process.env, ...options.env },
    stdio: options.capture ? "pipe" : "inherit"
  });
  if (result.status !== 0) {
    const details = options.capture ? `\n${result.stdout}\n${result.stderr}` : "";
    throw new Error(`${command} ${args.join(" ")} failed with status ${result.status}${details}`);
  }
  return result.stdout;
}

function parseArguments(argv) {
  const result = {
    projectRoot: resolve(import.meta.dirname, ".."),
    stagingRoot: undefined,
    expectedVersion: undefined,
    versionKind: undefined
  };
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    const value = argv[index + 1];
    if (["--project-root", "--staging-root", "--expected-version", "--version-kind"].includes(argument)) {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") result.projectRoot = resolve(value);
      else if (argument === "--staging-root") result.stagingRoot = resolve(value);
      else if (argument === "--expected-version") result.expectedVersion = value;
      else result.versionKind = value;
    } else {
      throw new Error(`Unknown argument ${argument}`);
    }
  }
  result.stagingRoot ??= resolve(result.projectRoot, "build/npm");
  if (!result.expectedVersion) throw new Error("--expected-version is required");
  if (!["stable", "snapshot"].includes(result.versionKind)) {
    throw new Error("--version-kind must be stable or snapshot");
  }
  return result;
}

function npmPack(directory, args = []) {
  return JSON.parse(run("npm", ["pack", "--json", ...args, directory], { capture: true }))[0];
}

function expectedFiles(manifest) {
  return ["package.json", ...manifest.files].sort();
}

async function verifyPackList(directory, expectedName, expectedVersion, versionKind) {
  const manifest = JSON.parse(await readFile(resolve(directory, "package.json"), "utf8"));
  assert.equal(manifest.name, expectedName);
  assert.equal(manifest.version, expectedVersion);
  if (versionKind === "stable") {
    assert.match(manifest.version, /^\d+\.\d+\.\d+$/);
  } else {
    assert.match(manifest.version, /^\d+\.\d+\.\d+-SNAPSHOT\.\d{14}(?:\.\d+)?$/);
  }
  assert.equal(manifest.author, "edigonzales");
  assert.equal(manifest.license, "MIT");
  assert.equal(manifest.repository?.url, "https://github.com/edigonzales/ilic-fork.git");
  assert.equal(manifest.publishConfig?.access, "public");
  assert.equal(manifest.publishConfig?.registry, "https://registry.npmjs.org/");
  const dryRun = npmPack(directory, ["--dry-run"]);
  assert.deepEqual(dryRun.files.map(file => file.path).sort(), expectedFiles(manifest),
    `${expectedName} npm tarball contains unexpected files`);
  assert.match(await readFile(resolve(directory, "LICENSE"), "utf8"), /MIT License/);
  if (expectedName === "@ilic/compiler-wasm") {
    assert.match(await readFile(resolve(directory, "THIRD_PARTY_NOTICES.md"), "utf8"),
      /ANTLR 4 C\+\+ Runtime[\s\S]*The BSD License/);
  }
  return manifest;
}

async function main() {
  const { projectRoot, stagingRoot, expectedVersion, versionKind } =
    parseArguments(process.argv.slice(2));
  const packages = [
    { id: "repository_core", name: "@ilic/repository-core", directory: resolve(stagingRoot, "repository-core") },
    { id: "tools", name: "@ilic/tools", directory: resolve(stagingRoot, "tools") },
    { id: "compiler", name: "@ilic/compiler-wasm", directory: resolve(stagingRoot, "compiler-wasm") }
  ];
  const manifests = [];
  for (const value of packages) {
    manifests.push(
      await verifyPackList(value.directory, value.name, expectedVersion, versionKind),
    );
  }
  assert.equal(manifests[0].version, manifests[1].version,
    "all packages must use the same version");
  assert.equal(manifests[1].version, manifests[2].version,
    "all packages must use the same version");
  assert.equal(
    manifests[1].dependencies?.["@ilic/repository-core"],
    expectedVersion,
    "@ilic/tools must use the exact coordinated repository-core version",
  );

  const tarballDirectory = resolve(stagingRoot, "tarballs");
  const consumerDirectory = resolve(stagingRoot, "consumer");
  await rm(tarballDirectory, { recursive: true, force: true });
  await rm(consumerDirectory, { recursive: true, force: true });
  await mkdir(tarballDirectory, { recursive: true });
  await mkdir(consumerDirectory, { recursive: true });

  const tarballs = packages.map(value => {
    const packed = npmPack(value.directory, ["--pack-destination", tarballDirectory]);
    return resolve(tarballDirectory, packed.filename);
  });
  await writeFile(resolve(consumerDirectory, "package.json"),
    `${JSON.stringify({ name: "ilic-package-smoke", private: true, type: "module" }, null, 2)}\n`);
  await writeFile(resolve(consumerDirectory, "smoke.mjs"), `
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import * as repositoryCore from "@ilic/repository-core";
import * as compilerPackage from "@ilic/compiler-wasm";
import { BrowserCache } from "@ilic/tools/browser";
import * as toolsPackage from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";

const { createCompiler } = compilerPackage;
const { MemoryCache, RepositoryManager } = toolsPackage;
assert.deepEqual(Object.keys(compilerPackage).sort(),
  ["Compiler", "CompilerSession", "createCompiler"]);
assert.deepEqual(Object.keys(toolsPackage).sort(),
  [
    "MemoryCache",
    "RepositoryError",
    "RepositoryManager",
    "normalizeRepositoryUri",
    "parseIliModelsXml",
    "parseIliSiteXml",
    "resolveRepositoryUri",
    "selectLatestModelVersion",
    "supportedSchemaLanguagePreference",
    "validateRepositoryRelativePath"
  ]);
assert.equal(typeof repositoryCore.RepositoryManagerCore, "function");
assert.equal(typeof BrowserCache, "function");
assert.equal(typeof MemoryCache, "function");
assert.equal(typeof NodeFileCache, "function");
assert.match(import.meta.resolve("@ilic/compiler-wasm/worker"), /worker\\.js$/);

const repositories = new RepositoryManager({
  repositories: [process.env.ILIC_REPOSITORY_FIXTURE],
  cache: new MemoryCache(),
  load: uri => readFile(uri),
  followSiteLinks: false
});
const workspace = await repositories.resolveModel("RepositoryRoot", "ili2_3");
assert.deepEqual(workspace.models.map(model => model.metadata.name),
  ["RepositoryBase", "RepositoryRoot"]);

const compiler = await createCompiler();
assert.equal(compiler.compilerVersion, process.env.ILIC_EXPECTED_VERSION);
assert.equal(compiler.abiVersion, 1);
const session = compiler.createSession();
try {
  session.putWorkspace(workspace);
  const root = workspace.models.find(model => model.metadata.name === "RepositoryRoot");
  const compilation = session.compile({ roots: [root.uri] });
  assert.equal(compilation.success, true, JSON.stringify(compilation.diagnostics));
  assert.equal(compilation.compilerVersion, process.env.ILIC_EXPECTED_VERSION);

  const uri = "memory:///PackageSmoke.ili";
  session.putSource(uri, 'INTERLIS 2.3;\\n!! kept\\nMODEL PackageSmoke AT "https://example.invalid" VERSION "1" =\\nEND PackageSmoke.\\n');
  const formatting = session.format(uri);
  assert.equal(formatting.success, true, JSON.stringify(formatting.diagnostics));
  assert.match(formatting.text, /!! kept/);
} finally {
  session.dispose();
}
`);

  run("npm", ["install", "--ignore-scripts", "--no-audit", "--no-fund",
    "--package-lock=false", ...tarballs], { cwd: consumerDirectory });
  run(process.execPath, ["smoke.mjs"], {
    cwd: consumerDirectory,
    env: {
      ILIC_REPOSITORY_FIXTURE: resolve(projectRoot, "test/repository/fixture"),
      ILIC_EXPECTED_VERSION: expectedVersion,
    }
  });
  await writeFile(
    resolve(stagingRoot, "release-manifest.json"),
    `${JSON.stringify(
      {
        schemaVersion: 1,
        version: expectedVersion,
        packages: Object.fromEntries(
          packages.map((value, index) => [
            value.name,
            { version: expectedVersion, tarball: basename(tarballs[index]) },
          ]),
        ),
      },
      null,
      2,
    )}\n`,
  );
  process.stdout.write(`verified ${manifests[0].version}: ${tarballs.map(path => basename(path)).join(", ")}\n`);
}

main().catch(error => {
  console.error(error instanceof Error ? error.stack : String(error));
  process.exitCode = 1;
});
