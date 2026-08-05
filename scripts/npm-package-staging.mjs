import { cp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import {
  dirname,
  isAbsolute,
  normalize,
  relative,
  resolve,
  sep,
} from "node:path";

export const PACKAGE_SPECS = Object.freeze([
  Object.freeze({
    id: "repository_core",
    name: "@ilic/repository-core",
    source: "packages/repository-core",
    destination: "repository-core",
  }),
  Object.freeze({
    id: "tools",
    name: "@ilic/tools",
    source: "packages/tools",
    destination: "tools",
  }),
  Object.freeze({
    id: "compiler_wasm",
    name: "@ilic/compiler-wasm",
    source: "packages/compiler-wasm",
    destination: "compiler-wasm",
  }),
]);

const INTERNAL_PACKAGE_NAMES = new Set(PACKAGE_SPECS.map(({ name }) => name));
const DEPENDENCY_FIELDS = [
  "dependencies",
  "optionalDependencies",
  "peerDependencies",
  "devDependencies",
];

function isSameOrParent(parent, child) {
  const path = relative(parent, child);
  return path === "" || (!path.startsWith(`..${sep}`) && path !== "..");
}

function validatePublishPath(path, packageName) {
  if (
    typeof path !== "string" ||
    !path ||
    isAbsolute(path) ||
    path.includes("*") ||
    path.includes("?") ||
    normalize(path).split(sep).includes("..")
  ) {
    throw new Error(
      `${packageName} contains unsupported publish path ${String(path)}`,
    );
  }
}

async function assertRegularFile(path, description) {
  try {
    if (!(await stat(path)).isFile()) throw new Error();
  } catch {
    throw new Error(`Missing ${description}: ${path}`);
  }
}

export function readCMakeProjectVersions(cmake) {
  const matches = [
    ...cmake.matchAll(
      /project\s*\(\s*ilic\s+VERSION\s+(\d+\.\d+\.\d+)(?=\s|\))/gi,
    ),
  ];
  if (matches.length !== 1) {
    throw new Error(
      `Expected exactly one project(ilic VERSION X.Y.Z ...) declaration, found ${matches.length}`,
    );
  }
  const projectVersion = matches[0][1];
  const qualifier = cmake.match(
    /set\s*\(\s*ILIC_VERSION_QUALIFIER\s+"([A-Za-z0-9-]+)"\s*\)/i,
  )?.[1];
  return {
    projectVersion,
    sourceVersion: qualifier
      ? `${projectVersion}-${qualifier}`
      : projectVersion,
  };
}

export async function readProjectVersions(projectRoot) {
  const cmake = await readFile(resolve(projectRoot, "CMakeLists.txt"), "utf8");
  return readCMakeProjectVersions(cmake);
}

export async function readProjectVersion(projectRoot) {
  return (await readProjectVersions(projectRoot)).projectVersion;
}

export async function readProjectSourceVersion(projectRoot) {
  return (await readProjectVersions(projectRoot)).sourceVersion;
}

export function validateOutputRoot(
  projectRoot,
  outputRoot,
  allowedProjectDirectory,
) {
  projectRoot = resolve(projectRoot);
  outputRoot = resolve(outputRoot);
  if (outputRoot === resolve(outputRoot, sep)) {
    throw new Error("Refusing to use a filesystem root as npm staging directory");
  }
  if (isSameOrParent(outputRoot, projectRoot)) {
    throw new Error("Refusing to use the project or one of its parents for npm staging");
  }
  if (isSameOrParent(projectRoot, outputRoot)) {
    const allowedRoot = resolve(
      projectRoot,
      "build",
      allowedProjectDirectory,
    );
    if (!isSameOrParent(allowedRoot, outputRoot)) {
      throw new Error(
        `npm staging inside the project must use ${relative(projectRoot, allowedRoot)}/`,
      );
    }
  }
  for (const spec of PACKAGE_SPECS) {
    const source = resolve(projectRoot, spec.source);
    if (isSameOrParent(source, outputRoot) || isSameOrParent(outputRoot, source)) {
      throw new Error(`Refusing to stage inside or above ${spec.source}`);
    }
  }
  return outputRoot;
}

function validateInternalDependencies(manifest, baseVersion) {
  for (const field of DEPENDENCY_FIELDS) {
    for (const [name, version] of Object.entries(manifest[field] ?? {})) {
      if (name.startsWith("@ilic/") && !INTERNAL_PACKAGE_NAMES.has(name)) {
        throw new Error(
          `${manifest.name} contains unknown internal dependency ${name}@${version}`,
        );
      }
      if (INTERNAL_PACKAGE_NAMES.has(name) && version !== baseVersion) {
        throw new Error(
          `${manifest.name} dependency ${name}@${version} does not match project version ${baseVersion}`,
        );
      }
    }
  }
}

export async function readAndValidateSourcePackages({
  projectRoot,
  baseVersion,
}) {
  const packages = [];
  for (const spec of PACKAGE_SPECS) {
    const source = resolve(projectRoot, spec.source);
    const manifestPath = resolve(source, "package.json");
    const manifest = JSON.parse(await readFile(manifestPath, "utf8"));
    if (manifest.name !== spec.name) {
      throw new Error(`${spec.source}/package.json must be named ${spec.name}`);
    }
    if (manifest.version !== baseVersion) {
      throw new Error(
        `${spec.name} version ${manifest.version} does not match project version ${baseVersion}`,
      );
    }
    if (!Array.isArray(manifest.files) || manifest.files.length === 0) {
      throw new Error(`${spec.name} must declare an explicit non-empty files list`);
    }
    for (const path of manifest.files) {
      validatePublishPath(path, spec.name);
      await assertRegularFile(
        resolve(source, path),
        `${spec.name} publish file ${path}`,
      );
    }
    if (!manifest.files.includes("LICENSE")) {
      throw new Error(`${spec.name} must publish LICENSE`);
    }
    if (
      spec.name === "@ilic/compiler-wasm" &&
      !manifest.files.includes("THIRD_PARTY_NOTICES.md")
    ) {
      throw new Error(`${spec.name} must publish THIRD_PARTY_NOTICES.md`);
    }
    validateInternalDependencies(manifest, baseVersion);
    packages.push({ spec, source, manifest });
  }
  return packages;
}

export function rewriteInternalDependencies(manifest, targetVersion) {
  const result = JSON.parse(JSON.stringify(manifest));
  for (const field of DEPENDENCY_FIELDS) {
    if (!result[field]) continue;
    result[field] = Object.fromEntries(
      Object.entries(result[field]).map(([name, version]) => [
        name,
        INTERNAL_PACKAGE_NAMES.has(name) ? targetVersion : version,
      ]),
    );
  }
  return result;
}

export async function stageCompilerPackages({
  projectRoot,
  outputRoot,
  targetVersion,
  allowedProjectDirectory,
}) {
  projectRoot = resolve(projectRoot);
  outputRoot = validateOutputRoot(
    projectRoot,
    outputRoot,
    allowedProjectDirectory,
  );
  if (!/^\d+\.\d+\.\d+(?:-SNAPSHOT\.\d{14}(?:\.\d+)?)?$/.test(targetVersion)) {
    throw new Error(`Invalid compiler package target version ${targetVersion}`);
  }
  const projectVersion = await readProjectVersion(projectRoot);
  const sourceVersion = await readProjectSourceVersion(projectRoot);
  const packages = await readAndValidateSourcePackages({
    projectRoot,
    baseVersion: sourceVersion,
  });

  await rm(outputRoot, { recursive: true, force: true });
  await mkdir(outputRoot, { recursive: true });

  const directories = {};
  for (const { spec, source, manifest } of packages) {
    const destination = resolve(outputRoot, spec.destination);
    await mkdir(destination, { recursive: true });
    for (const path of manifest.files) {
      const target = resolve(destination, path);
      await mkdir(dirname(target), { recursive: true });
      await cp(resolve(source, path), target);
    }
    const stagedManifest = rewriteInternalDependencies(manifest, targetVersion);
    stagedManifest.version = targetVersion;
    await writeFile(
      resolve(destination, "package.json"),
      `${JSON.stringify(stagedManifest, null, 2)}\n`,
    );
    directories[spec.id] = destination;
  }
  return {
    baseVersion: sourceVersion,
    projectVersion,
    targetVersion,
    outputRoot,
    directories,
  };
}
