#!/usr/bin/env node

import { readFile } from "node:fs/promises";
import { resolve } from "node:path";

const root = resolve(process.argv[2] ?? process.cwd());
const header = await readFile(resolve(root, "include/ilic/Diagnostic.h"), "utf8");
const declarations = await readFile(
  resolve(root, "packages/compiler-wasm/index.d.ts"),
  "utf8",
);
const capi = await readFile(resolve(root, "source/abi/Capi.cpp"), "utf8");

const phases = [
  "unknown",
  "lexical",
  "syntax",
  "editor-recovery",
  "model-discovery",
  "resolution",
  "semantic",
  "translation",
  "repository",
  "formatting",
  "request",
  "internal",
];
const tags = ["primary", "cascaded", "recovery", "deprecated", "unnecessary"];
const errors = [];

for (const phase of phases) {
  if (!declarations.includes(`| "${phase}"`)) errors.push(`missing WASM phase ${phase}`);
}
for (const tag of tags) {
  if (!declarations.includes(`| "${tag}"`)) errors.push(`missing WASM tag ${tag}`);
}
for (const required of ["phase", "tags", "helpId", "fingerprint"]) {
  if (!header.includes(required) || !capi.includes(required)) {
    errors.push(`missing native/C-ABI field ${required}`);
  }
}

if (errors.length) {
  for (const error of errors) console.error(error);
  process.exitCode = 1;
} else {
  process.stdout.write(
    `diagnostic contract parity: ${phases.length} phases, ${tags.length} tags, 4 additive fields\n`,
  );
}
