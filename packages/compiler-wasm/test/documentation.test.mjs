import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import test from "node:test";
import { runWasmSessionExample } from "../../../docs/examples/wasm-session.mjs";
import { runWasmRepositoryExample } from "../../../docs/examples/wasm-repository.mjs";

const wasm = resolve(import.meta.dirname,"../ilic.wasm");

test("runs the documented WASM session example", {
  skip: existsSync(wasm) ? false : "WASM package artifacts have not been built"
}, async () => {
  const result = await runWasmSessionExample();
  assert.equal(result.compilation.success,true);
  assert.match(result.formatting.text,/!! This comment remains/);
});

test("runs the documented offline repository workspace example", {
  skip: existsSync(wasm) ? false : "WASM package artifacts have not been built"
}, async () => {
  const result = await runWasmRepositoryExample();
  assert.equal(result.success,true,JSON.stringify(result.diagnostics));
  assert.ok(result.models.some(model => model.name === "RepositoryRoot"));
});
