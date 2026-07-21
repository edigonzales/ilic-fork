#!/usr/bin/env node

import { appendFile, cp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { dirname, isAbsolute, normalize, relative, resolve, sep } from "node:path";
import { pathToFileURL } from "node:url";

const PACKAGE_SPECS = [
  {
    id: "tools",
    name: "@ilic/tools",
    source: "packages/tools",
    destination: "tools"
  },
  {
    id: "compiler_wasm",
    name: "@ilic/compiler-wasm",
    source: "packages/compiler-wasm",
    destination: "compiler-wasm"
  }
];

function twoDigits(value) {
  return String(value).padStart(2, "0");
}

export function formatUtcTimestamp(date = new Date()) {
  return `${date.getUTCFullYear()}${twoDigits(date.getUTCMonth() + 1)}`
    + `${twoDigits(date.getUTCDate())}${twoDigits(date.getUTCHours())}`
    + `${twoDigits(date.getUTCMinutes())}${twoDigits(date.getUTCSeconds())}`;
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
    Number(timestamp.slice(12, 14))
  ];
  const date = new Date(Date.UTC(parts[0], parts[1] - 1, parts[2], parts[3], parts[4], parts[5]));
  const normalized = formatUtcTimestamp(date);
  if (parts[0] < 2000 || normalized !== timestamp) {
    throw new Error(`Invalid UTC snapshot timestamp ${timestamp}`);
  }
}

export function validateBuildId(buildId) {
  if (buildId === undefined || buildId === null || buildId === "") return undefined;
  const normalized = String(buildId);
  if (!/^\d+$/.test(normalized)) {
    throw new Error("Snapshot build ID must contain only digits");
  }
  return normalized;
}

function isSameOrParent(parent, child) {
  const path = relative(parent, child);
  return path === "" || (!path.startsWith(`..${sep}`) && path !== "..");
}

function validateOutputRoot(projectRoot, outputRoot) {
  if (outputRoot === resolve(outputRoot, sep)) {
    throw new Error("Refusing to use a filesystem root as npm staging directory");
  }
  if (isSameOrParent(outputRoot, projectRoot) || isSameOrParent(projectRoot, outputRoot)
      && relative(projectRoot, outputRoot).split(sep)[0] !== "build") {
    throw new Error("npm staging directory must be build/npm or a directory outside the project");
  }
  for (const spec of PACKAGE_SPECS) {
    const source = resolve(projectRoot, spec.source);
    if (isSameOrParent(source, outputRoot) || isSameOrParent(outputRoot, source)) {
      throw new Error(`Refusing to stage inside or above ${spec.source}`);
    }
  }
}

function validatePublishFile(file, packageName) {
  if (typeof file !== "string" || !file || isAbsolute(file) || file.includes("*")
      || file.includes("?") || normalize(file).split(sep).includes("..")) {
    throw new Error(`${packageName} contains unsupported publish path ${String(file)}`);
  }
}

async function readJson(path) {
  return JSON.parse(await readFile(path, "utf8"));
}

async function assertRegularFile(path, description) {
  try {
    if (!(await stat(path)).isFile()) throw new Error();
  } catch {
    throw new Error(`Missing ${description}: ${path}`);
  }
}

function projectVersion(cmake) {
  const match = cmake.match(/project\s*\(\s*ilic\s+VERSION\s+(\d+\.\d+\.\d+)\b/i);
  if (!match) throw new Error("Could not read ilic project version from CMakeLists.txt");
  return match[1];
}

export async function prepareNpmSnapshot({
  projectRoot = resolve(import.meta.dirname, ".."),
  outputRoot = resolve(projectRoot, "build/npm"),
  timestamp = formatUtcTimestamp(),
  buildId
} = {}) {
  projectRoot = resolve(projectRoot);
  outputRoot = resolve(outputRoot);
  validateTimestamp(timestamp);
  const normalizedBuildId = validateBuildId(buildId);
  validateOutputRoot(projectRoot, outputRoot);

  const baseVersion = projectVersion(await readFile(resolve(projectRoot, "CMakeLists.txt"), "utf8"));
  const snapshotVersion = `${baseVersion}-SNAPSHOT.${timestamp}${normalizedBuildId ? `.${normalizedBuildId}` : ""}`;
  const packages = [];

  for (const spec of PACKAGE_SPECS) {
    const source = resolve(projectRoot, spec.source);
    const manifestPath = resolve(source, "package.json");
    const manifest = await readJson(manifestPath);
    if (manifest.name !== spec.name) {
      throw new Error(`${spec.source}/package.json must be named ${spec.name}`);
    }
    if (manifest.version !== baseVersion) {
      throw new Error(`${spec.name} version ${manifest.version} does not match project version ${baseVersion}`);
    }
    if (!Array.isArray(manifest.files) || manifest.files.length === 0) {
      throw new Error(`${spec.name} must declare an explicit non-empty files list`);
    }
    for (const file of manifest.files) {
      validatePublishFile(file, spec.name);
      await assertRegularFile(resolve(source, file), `${spec.name} publish file ${file}`);
    }
    packages.push({ spec, source, manifest });
  }

  await rm(outputRoot, { recursive: true, force: true });
  await mkdir(outputRoot, { recursive: true });

  const directories = {};
  for (const { spec, source, manifest } of packages) {
    const destination = resolve(outputRoot, spec.destination);
    await mkdir(destination, { recursive: true });
    for (const file of manifest.files) {
      const target = resolve(destination, file);
      await mkdir(dirname(target), { recursive: true });
      await cp(resolve(source, file), target);
    }
    await writeFile(resolve(destination, "package.json"),
      `${JSON.stringify({ ...manifest, version: snapshotVersion }, null, 2)}\n`);
    directories[spec.id] = destination;
  }

  return { baseVersion, timestamp, snapshotVersion, outputRoot, directories };
}

function parseArguments(argv) {
  const options = {};
  let githubOutput;
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    const value = argv[index + 1];
    if (["--project-root", "--output", "--timestamp", "--build-id", "--github-output"].includes(argument)) {
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
    await appendFile(githubOutput, [
      `base_version=${result.baseVersion}`,
      `snapshot_version=${result.snapshotVersion}`,
      `tools_directory=${result.directories.tools}`,
      `compiler_wasm_directory=${result.directories.compiler_wasm}`,
      ""
    ].join("\n"));
  }
  process.stdout.write(`${JSON.stringify(result, null, 2)}\n`);
}

const invokedPath = process.argv[1] ? pathToFileURL(resolve(process.argv[1])).href : "";
if (invokedPath === import.meta.url) {
  main().catch(error => {
    console.error(error instanceof Error ? error.message : String(error));
    process.exitCode = 1;
  });
}
