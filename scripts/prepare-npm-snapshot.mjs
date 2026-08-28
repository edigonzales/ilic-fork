#!/usr/bin/env node

import { appendFile, readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

import {
  readProjectSourceVersion,
  stageCompilerPackages,
} from "./npm-package-staging.mjs";

export async function prepareNpmSnapshot({
  projectRoot = resolve(import.meta.dirname, ".."),
  outputRoot = resolve(projectRoot, "build/npm"),
  sourceSha,
  releaseManifestPath,
} = {}) {
  projectRoot = resolve(projectRoot);
  outputRoot = resolve(outputRoot);
  if (!/^[0-9a-f]{40}$/.test(sourceSha ?? "")) {
    throw new Error("Snapshot packaging requires --source-sha with a full Git SHA");
  }
  if (!releaseManifestPath) {
    throw new Error("Snapshot packaging requires --release-manifest");
  }
  const sourceVersion = await readProjectSourceVersion(projectRoot);
  if (!sourceVersion.endsWith("-SNAPSHOT")) {
    throw new Error(
      `Snapshot packaging requires a -SNAPSHOT source version, got ${sourceVersion}`,
    );
  }
  const baseVersion = sourceVersion.slice(0, -"-SNAPSHOT".length);
  const snapshotVersion = `${baseVersion}-snapshot.g${sourceSha.slice(0, 12)}`;
  const releaseManifest = JSON.parse(
    await readFile(resolve(releaseManifestPath), "utf8"),
  );
  const staged = await stageCompilerPackages({
    projectRoot,
    outputRoot,
    targetVersion: snapshotVersion,
    allowedProjectDirectory: "npm",
    sourceSha,
    releaseManifest,
  });
  return {
    ...staged,
    baseVersion: sourceVersion,
    sourceSha,
    snapshotVersion,
  };
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
        "--source-sha",
        "--release-manifest",
        "--github-output",
      ].includes(argument)
    ) {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") options.projectRoot = resolve(value);
      else if (argument === "--output") options.outputRoot = resolve(value);
      else if (argument === "--source-sha") options.sourceSha = value;
      else if (argument === "--release-manifest") options.releaseManifestPath = resolve(value);
      else githubOutput = value;
    } else {
      throw new Error(`Unknown argument ${argument}`);
    }
  }
  return { options, githubOutput };
}

async function main() {
  const { options, githubOutput } = parseArguments(process.argv.slice(2));
  const result = await prepareNpmSnapshot(options);
  if (githubOutput) {
    await appendFile(
      githubOutput,
      [
        `base_version=${result.baseVersion}`,
        `snapshot_version=${result.snapshotVersion}`,
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
