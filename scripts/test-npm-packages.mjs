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
    stagingRoot: undefined
  };
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    const value = argv[index + 1];
    if (argument === "--project-root" || argument === "--staging-root") {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") result.projectRoot = resolve(value);
      else result.stagingRoot = resolve(value);
    } else {
      throw new Error(`Unknown argument ${argument}`);
    }
  }
  result.stagingRoot ??= resolve(result.projectRoot, "build/npm");
  return result;
}

function npmPack(directory, args = []) {
  return JSON.parse(run("npm", ["pack", "--json", ...args, directory], { capture: true }))[0];
}

function expectedFiles(manifest) {
  return ["package.json", ...manifest.files].sort();
}

async function verifyPackList(directory, expectedName) {
  const manifest = JSON.parse(await readFile(resolve(directory, "package.json"), "utf8"));
  assert.equal(manifest.name, expectedName);
  assert.match(manifest.version, /^\d+\.\d+\.\d+-SNAPSHOT\.\d{14}$/);
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
  const { projectRoot, stagingRoot } = parseArguments(process.argv.slice(2));
  const packages = [
    { id: "tools", name: "@ilic/tools", directory: resolve(stagingRoot, "tools") },
    { id: "compiler", name: "@ilic/compiler-wasm", directory: resolve(stagingRoot, "compiler-wasm") }
  ];
  const manifests = [];
  for (const value of packages) manifests.push(await verifyPackList(value.directory, value.name));
  assert.equal(manifests[0].version, manifests[1].version,
    "both packages must use the same snapshot version");

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
import * as compilerPackage from "@ilic/compiler-wasm";
import { BrowserCache } from "@ilic/tools/browser";
import * as toolsPackage from "@ilic/tools";
import { NodeFileCache } from "@ilic/tools/node";

const { createCompiler } = compilerPackage;
const { MemoryCache, RepositoryManager } = toolsPackage;
assert.deepEqual(Object.keys(compilerPackage).sort(),
  ["Compiler", "CompilerSession", "createCompiler"]);
assert.deepEqual(Object.keys(toolsPackage).sort(),
  ["MemoryCache", "RepositoryManager", "parseIliModelsXml"]);
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
const session = compiler.createSession();
try {
  session.putWorkspace(workspace);
  const root = workspace.models.find(model => model.metadata.name === "RepositoryRoot");
  const compilation = session.compile({ roots: [root.uri] });
  assert.equal(compilation.success, true, JSON.stringify(compilation.diagnostics));

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
    env: { ILIC_REPOSITORY_FIXTURE: resolve(projectRoot, "test/repository/fixture") }
  });
  process.stdout.write(`verified ${manifests[0].version}: ${tarballs.map(path => basename(path)).join(", ")}\n`);
}

main().catch(error => {
  console.error(error instanceof Error ? error.stack : String(error));
  process.exitCode = 1;
});
