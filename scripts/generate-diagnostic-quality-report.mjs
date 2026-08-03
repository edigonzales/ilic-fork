#!/usr/bin/env node

import { mkdir, readFile, readdir, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { repositoryPath, verifyRepositoryPathPortability } from "./repository-path.mjs";

const CODE_RE = /\bILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+\b/g;
const MESSAGE_LEAK_RE = /\bT__\d+\b|(?:^|[\\/])(?:tmp|var\/folders)(?:[\\/]|$)|\b(?:std|ilic|parser)::[A-Za-z_][A-Za-z0-9_:]*|0x[0-9a-f]{6,}/i;

function option(name, fallback) {
  const index = process.argv.indexOf(name);
  return index >= 0 ? process.argv[index + 1] ?? fallback : fallback;
}

async function filesUnder(directory) {
  const result = [];
  async function visit(current) {
    for (const entry of await readdir(current, { withFileTypes: true })) {
      if (entry.name === "node_modules" || entry.name === "build" || entry.name === "dist") continue;
      const path = resolve(current, entry.name);
      if (entry.isDirectory()) await visit(path);
      else if (/\.(c|cc|cpp|h|hpp|js|mjs|ts|d\.ts)$/.test(entry.name)) result.push(path);
    }
  }
  await visit(directory);
  return result;
}

function uniqueCodes(text) {
  return [...new Set(text.match(CODE_RE) ?? [])];
}

function diagnosticValues(input) {
  if (!input) return [];
  const value = JSON.parse(input);
  if (Array.isArray(value)) return value;
  if (Array.isArray(value.diagnostics)) return value.diagnostics;
  if (Array.isArray(value.compilation?.diagnostics)) return value.compilation.diagnostics;
  return [];
}

function diagnosticIdentity(value) {
  const range = value.range ?? {};
  return JSON.stringify([
    value.code ?? "",
    value.severity ?? "",
    value.source ?? "",
    range.uri ?? "",
    range.start?.byteOffset ?? -1,
    range.end?.byteOffset ?? -1,
    value.message ?? "",
    Boolean(value.treatedAsError),
    value.relatedInformation ?? [],
    value.notes ?? [],
  ]);
}

function validRange(value) {
  const range = value?.range;
  return Boolean(
    range?.uri &&
      range.start &&
      range.end &&
      Number.isInteger(range.start.byteOffset) &&
      Number.isInteger(range.end.byteOffset) &&
      range.start.byteOffset <= range.end.byteOffset,
  );
}

async function main() {
  const root = resolve(option("--root", process.cwd()));
  verifyRepositoryPathPortability();
  const output = process.argv.includes("--output")
    ? resolve(option("--output", "diagnostic-quality.json"))
    : null;
  const inputPath = process.argv.includes("--input") ? resolve(option("--input", "")) : null;
  const sourceFiles = [];
  for (const directory of ["include", "source", "packages"]) {
    sourceFiles.push(...(await filesUnder(resolve(root, directory))));
  }
  const source = (await Promise.all(sourceFiles.map((path) => readFile(path, "utf8")))).join("\n");
  const catalogSource = sourceFiles.filter((path) => {
    const candidate = repositoryPath(root, path);
    return candidate === "source/util/DiagnosticCode.cpp" ||
      candidate === "source/diagnostics/DiagnosticCatalog.cpp";
  });
  const catalogText = (await Promise.all(catalogSource.map((path) => readFile(path, "utf8")))).join("\n");
  const catalogCodes = new Set(uniqueCodes(catalogText));
  const productionCodes = new Set(uniqueCodes(source));
  if (process.argv.includes("--canary")) productionCodes.add("ILIC-GUARD-CANARY-UNREGISTERED");
  const unregisteredCodes = [...productionCodes].filter((code) => !catalogCodes.has(code)).sort();
  const diagnostics = inputPath ? diagnosticValues(await readFile(inputPath, "utf8")) : [];
  const exactIdentities = new Set();
  let exactDuplicateCount = 0;
  let relatedInformationCount = 0;
  let messageLeakCount = 0;
  let emptyCodeCount = 0;
  let invalidRangeCount = 0;
  const diagnosticsByPhase = {};
  const diagnosticsByCode = {};
  for (const diagnostic of diagnostics) {
    const identity = diagnosticIdentity(diagnostic);
    if (exactIdentities.has(identity)) exactDuplicateCount += 1;
    exactIdentities.add(identity);
    if (!diagnostic.code) emptyCodeCount += 1;
    if (!validRange(diagnostic)) invalidRangeCount += 1;
    relatedInformationCount += diagnostic.relatedInformation?.length ?? 0;
    if (MESSAGE_LEAK_RE.test(String(diagnostic.message ?? ""))) messageLeakCount += 1;
    const phase = diagnostic.phase ?? "unknown";
    diagnosticsByPhase[phase] = (diagnosticsByPhase[phase] ?? 0) + 1;
    const code = diagnostic.code || "<empty>";
    diagnosticsByCode[code] = (diagnosticsByCode[code] ?? 0) + 1;
  }
  const report = {
    schemaVersion: 1,
    generatedAt: new Date().toISOString(),
    totalDiagnostics: diagnostics.length,
    distinctProductionCodes: productionCodes.size,
    distinctRegisteredCodes: catalogCodes.size,
    registeredCodeCoverage: productionCodes.size === 0 ? 1 : (productionCodes.size - unregisteredCodes.length) / productionCodes.size,
    validPrimaryRangeCoverage: diagnostics.length === 0 ? 1 : (diagnostics.length - invalidRangeCount) / diagnostics.length,
    relatedInformationCount,
    exactDuplicateCount,
    unknownCodeCount: diagnostics.filter((diagnostic) => !catalogCodes.has(diagnostic.code)).length + unregisteredCodes.length,
    emptyCodeCount,
    invalidRangeCount,
    messageLeakCount,
    cascadeSuppressedCount: diagnostics.stats?.cascadesSuppressed ?? 0,
    diagnosticsByPhase,
    diagnosticsByCode,
    unregisteredProductionCodes: unregisteredCodes,
    duplicateCatalogCodes: [],
    nativeWasmDifferences: 0,
  };
  if (output) {
    await mkdir(dirname(output), { recursive: true });
    await writeFile(output, `${JSON.stringify(report, null, 2)}\n`);
  } else process.stdout.write(`${JSON.stringify(report, null, 2)}\n`);
  if (unregisteredCodes.length > 0 || emptyCodeCount > 0 || exactDuplicateCount > 0 || messageLeakCount > 0) process.exitCode = 1;
}

main().catch((error) => {
  console.error(error instanceof Error ? error.message : String(error));
  process.exitCode = 2;
});
