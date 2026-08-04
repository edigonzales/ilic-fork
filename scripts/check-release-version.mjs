#!/usr/bin/env node

import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

export function readCMakeProjectVersion(cmakeText) {
  const matches = [
    ...cmakeText.matchAll(
      /project\s*\(\s*ilic\s+VERSION\s+(\d+\.\d+\.\d+)(?=\s|\))/gi,
    ),
  ];
  if (matches.length !== 1) {
    throw new Error(
      `Expected exactly one project(ilic VERSION X.Y.Z ...) declaration, found ${matches.length}`,
    );
  }
  return matches[0][1];
}

export function normalizeReleaseTag(tag) {
  if (!/^v\d+\.\d+\.\d+$/.test(tag ?? "")) {
    throw new Error(`Release tag must use vX.Y.Z, received ${String(tag)}`);
  }
  return tag.slice(1);
}

async function readJson(path) {
  return JSON.parse(await readFile(path, "utf8"));
}

export async function collectReleaseVersionState(projectRoot) {
  projectRoot = resolve(projectRoot);
  const [cmakeText, repositoryCore, tools, compilerWasm] = await Promise.all([
    readFile(resolve(projectRoot, "CMakeLists.txt"), "utf8"),
    readJson(resolve(projectRoot, "packages/repository-core/package.json")),
    readJson(resolve(projectRoot, "packages/tools/package.json")),
    readJson(resolve(projectRoot, "packages/compiler-wasm/package.json")),
  ]);
  return {
    cmakeVersion: readCMakeProjectVersion(cmakeText),
    repositoryCoreVersion: repositoryCore.version,
    toolsVersion: tools.version,
    compilerWasmVersion: compilerWasm.version,
    toolsRepositoryCoreDependency:
      tools.dependencies?.["@ilic/repository-core"],
  };
}

export function validateReleaseVersionState(state, expectedVersion, expectedTag) {
  if (!/^\d+\.\d+\.\d+$/.test(expectedVersion ?? "")) {
    throw new Error(
      `Expected stable release version X.Y.Z, received ${String(expectedVersion)}`,
    );
  }
  const tagVersion = normalizeReleaseTag(expectedTag);
  if (tagVersion !== expectedVersion) {
    throw new Error(
      `Release tag ${expectedTag} does not match expected version ${expectedVersion}`,
    );
  }
  const fields = [
    ["CMake project version", state.cmakeVersion],
    ["@ilic/repository-core version", state.repositoryCoreVersion],
    ["@ilic/tools version", state.toolsVersion],
    ["@ilic/compiler-wasm version", state.compilerWasmVersion],
    [
      "@ilic/tools dependency @ilic/repository-core",
      state.toolsRepositoryCoreDependency,
    ],
  ];
  for (const [field, actual] of fields) {
    if (actual !== expectedVersion) {
      throw new Error(
        `${field} is ${String(actual)}, expected ${expectedVersion}`,
      );
    }
  }
  return { ...state, expectedVersion, expectedTag };
}

function parseArguments(argv) {
  const result = { projectRoot: resolve(import.meta.dirname, "..") };
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    const value = argv[index + 1];
    if (
      ["--project-root", "--expected-version", "--expected-tag"].includes(
        argument,
      )
    ) {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") result.projectRoot = resolve(value);
      else if (argument === "--expected-version") {
        result.expectedVersion = value;
      } else result.expectedTag = value;
    } else {
      throw new Error(`Unknown argument ${argument}`);
    }
  }
  if (!result.expectedVersion) throw new Error("--expected-version is required");
  if (!result.expectedTag) throw new Error("--expected-tag is required");
  return result;
}

async function main() {
  const options = parseArguments(process.argv.slice(2));
  const result = validateReleaseVersionState(
    await collectReleaseVersionState(options.projectRoot),
    options.expectedVersion,
    options.expectedTag,
  );
  process.stdout.write(`${JSON.stringify(result, null, 2)}\n`);
}

const invokedPath = process.argv[1]
  ? pathToFileURL(resolve(process.argv[1])).href
  : "";
if (invokedPath === import.meta.url) {
  main().catch((error) => {
    console.error(error instanceof Error ? error.message : String(error));
    process.exitCode = 1;
  });
}
