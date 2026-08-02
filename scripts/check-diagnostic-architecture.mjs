#!/usr/bin/env node

import { readFile, readdir } from "node:fs/promises";
import { resolve } from "node:path";

const CODE_RE = /\bILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+\b/g;

function codes(text) {
  return text.match(CODE_RE) ?? [];
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

async function main() {
  const root = resolve(process.argv[2] ?? process.cwd());
  const files = [];
  for (const directory of ["include", "source", "packages"]) {
    files.push(...(await filesUnder(resolve(root, directory))));
  }
  const contents = new Map();
  for (const file of files) contents.set(file, await readFile(file, "utf8"));
  const catalogFiles = files.filter((file) => file.endsWith("source/util/DiagnosticCode.cpp") || file.endsWith("source/diagnostics/DiagnosticCatalog.cpp"));
  const catalogCodes = catalogFiles.flatMap((file) => {
    const text = contents.get(file);
    return [
      ...(text.match(/DiagnosticDefinition\{[^\n]+,"(ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+)"\}/g) ?? []).map((value) => value.match(/"(ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+)"/)?.[1]),
      ...(text.match(/\{"(ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+)",DiagnosticPhase/g) ?? []).map((value) => value.match(/"(ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)+)"/)?.[1]),
    ].filter(Boolean);
  });
  const catalogSet = new Set(catalogCodes);
  const errors = [];
  if (new Set(catalogCodes).size !== catalogCodes.length) errors.push("duplicate diagnostic code in catalog");
  for (const [file, text] of contents) {
    for (const code of new Set(codes(text))) {
      if (!catalogSet.has(code)) errors.push(`unregistered code ${code} in ${file}`);
    }
    if (/T__\d+/.test(text) && /diagnostic|message|error/i.test(text)) errors.push(`ANTLR token name leak in ${file}`);
    if (/typeid\([^)]*\)\.name\(\)/.test(text) && /diagnostic|message/i.test(text)) errors.push(`C++ type-name leak in ${file}`);
    if (!file.endsWith("source/repository/Md5.cpp") && !file.endsWith("packages/compiler-wasm/ilic.mjs") && /(?:^|["' ])0x[0-9a-f]{6,}/i.test(text) && /diagnostic|message/i.test(text)) errors.push(`pointer-like diagnostic text in ${file}`);
  }
  const workbench = [...contents.entries()].find(([file]) => file.endsWith("src/workbench/workbench.ts"));
  if (workbench && /textContent\.split\(|innerText\.split\(|rendered.*diagnostic/i.test(workbench[1])) errors.push("Web-IDE derives Problems from rendered text");
  if (process.argv.includes("--canary")) errors.push("canary: unregistered diagnostic code ILIC-P6-CANARY-UNREGISTERED");
  if (errors.length) {
    for (const error of errors) console.error(error);
    process.exitCode = 1;
  } else process.stdout.write(`diagnostic architecture guard: ${catalogSet.size} registered codes, ${files.length} files checked\n`);
}

main().catch((error) => {
  console.error(error instanceof Error ? error.message : String(error));
  process.exitCode = 2;
});
