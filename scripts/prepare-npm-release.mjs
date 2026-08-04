#!/usr/bin/env node

import { appendFile } from "node:fs/promises";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

import { readProjectVersion, stageCompilerPackages } from "./npm-package-staging.mjs";

export async function prepareNpmRelease({
  projectRoot = resolve(import.meta.dirname, ".."),
  outputRoot = resolve(projectRoot, "build/npm-release"),
  expectedVersion,
  expectedTag,
} = {}) {
  projectRoot = resolve(projectRoot);
  outputRoot = resolve(outputRoot);
  const baseVersion = await readProjectVersion(projectRoot);
  if (expectedVersion !== undefined && expectedVersion !== baseVersion) {
    throw new Error(
      `Expected release version ${expectedVersion}, but CMake project version is ${baseVersion}`,
    );
  }
  if (expectedTag !== undefined) {
    if (!/^v\d+\.\d+\.\d+$/.test(expectedTag)) {
      throw new Error(`Release tag must use vX.Y.Z, received ${expectedTag}`);
    }
    if (expectedTag.slice(1) !== baseVersion) {
      throw new Error(
        `Release tag ${expectedTag} does not match CMake project version ${baseVersion}`,
      );
    }
  }
  const staged = await stageCompilerPackages({
    projectRoot,
    outputRoot,
    targetVersion: baseVersion,
    allowedProjectDirectory: "npm-release",
  });
  return { ...staged, releaseVersion: baseVersion };
}

function parseArguments(argv) {
  const options = {};
  let githubOutput;
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    const value = argv[index + 1];
    if (
      [
        "--project-root",
        "--output",
        "--expected-version",
        "--expected-tag",
        "--github-output",
      ].includes(argument)
    ) {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") options.projectRoot = resolve(value);
      else if (argument === "--output") options.outputRoot = resolve(value);
      else if (argument === "--expected-version") {
        options.expectedVersion = value;
      } else if (argument === "--expected-tag") options.expectedTag = value;
      else githubOutput = value;
    } else {
      throw new Error(`Unknown argument ${argument}`);
    }
  }
  return { options, githubOutput };
}

async function main() {
  const { options, githubOutput } = parseArguments(process.argv.slice(2));
  const result = await prepareNpmRelease(options);
  if (githubOutput) {
    await appendFile(
      githubOutput,
      [
        `base_version=${result.baseVersion}`,
        `release_version=${result.releaseVersion}`,
        `repository_core_directory=${result.directories.repository_core}`,
        `tools_directory=${result.directories.tools}`,
        `compiler_wasm_directory=${result.directories.compiler_wasm}`,
        "",
      ].join("\n"),
    );
  }
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
