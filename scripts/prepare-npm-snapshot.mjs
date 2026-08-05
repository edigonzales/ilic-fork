#!/usr/bin/env node

import { appendFile } from "node:fs/promises";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

import {
  readProjectSourceVersion,
  stageCompilerPackages,
} from "./npm-package-staging.mjs";

function twoDigits(value) {
  return String(value).padStart(2, "0");
}

export function formatUtcTimestamp(date = new Date()) {
  return (
    `${date.getUTCFullYear()}${twoDigits(date.getUTCMonth() + 1)}` +
    `${twoDigits(date.getUTCDate())}${twoDigits(date.getUTCHours())}` +
    `${twoDigits(date.getUTCMinutes())}${twoDigits(date.getUTCSeconds())}`
  );
}

function validateTimestamp(timestamp) {
  if (!/^\d{14}$/.test(timestamp)) {
    throw new Error("Snapshot timestamp must use UTC format YYYYMMDDHHmmss");
  }
  const parts = [
    Number(timestamp.slice(0, 4)),
    Number(timestamp.slice(4, 6)),
    Number(timestamp.slice(6, 8)),
    Number(timestamp.slice(8, 10)),
    Number(timestamp.slice(10, 12)),
    Number(timestamp.slice(12, 14)),
  ];
  const date = new Date(
    Date.UTC(parts[0], parts[1] - 1, parts[2], parts[3], parts[4], parts[5]),
  );
  if (parts[0] < 2000 || formatUtcTimestamp(date) !== timestamp) {
    throw new Error(`Invalid UTC snapshot timestamp ${timestamp}`);
  }
}

export function validateBuildId(buildId) {
  if (buildId === undefined || buildId === null || buildId === "") {
    return undefined;
  }
  const normalized = String(buildId);
  if (!/^\d+$/.test(normalized)) {
    throw new Error("Snapshot build ID must contain only digits");
  }
  return normalized;
}

export async function prepareNpmSnapshot({
  projectRoot = resolve(import.meta.dirname, ".."),
  outputRoot = resolve(projectRoot, "build/npm"),
  timestamp = formatUtcTimestamp(),
  buildId,
} = {}) {
  projectRoot = resolve(projectRoot);
  outputRoot = resolve(outputRoot);
  validateTimestamp(timestamp);
  const normalizedBuildId = validateBuildId(buildId);
  const sourceVersion = await readProjectSourceVersion(projectRoot);
  if (!sourceVersion.endsWith("-SNAPSHOT")) {
    throw new Error(
      `Snapshot packaging requires a -SNAPSHOT source version, got ${sourceVersion}`,
    );
  }
  const baseVersion = sourceVersion.slice(0, -"-SNAPSHOT".length);
  const snapshotVersion =
    `${baseVersion}-SNAPSHOT.${timestamp}` +
    (normalizedBuildId ? `.${normalizedBuildId}` : "");
  const staged = await stageCompilerPackages({
    projectRoot,
    outputRoot,
    targetVersion: snapshotVersion,
    allowedProjectDirectory: "npm",
  });
  return {
    ...staged,
    baseVersion: sourceVersion,
    timestamp,
    snapshotId: `${timestamp}${normalizedBuildId ? `.${normalizedBuildId}` : ""}`,
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
        "--timestamp",
        "--build-id",
        "--github-output",
      ].includes(argument)
    ) {
      if (!value) throw new Error(`${argument} requires a value`);
      index += 1;
      if (argument === "--project-root") options.projectRoot = resolve(value);
      else if (argument === "--output") options.outputRoot = resolve(value);
      else if (argument === "--timestamp") options.timestamp = value;
      else if (argument === "--build-id") options.buildId = value;
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
