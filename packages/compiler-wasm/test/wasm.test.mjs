import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import test from "node:test";
import { createCompiler } from "../index.js";

const modulePath = resolve(import.meta.dirname, "../ilic.mjs");

test("compiles and formats through the real WASM ABI", {
  skip: existsSync(modulePath) ? false : "build/wasm/ilic.mjs has not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///WasmModel.ili";
  session.putSource(uri, `INTERLIS 2.3;
!! kept
MODEL WasmModel AT "https://example.invalid/ilic/tests" VERSION "1" =
END WasmModel.
`);
  const result = session.compile({ roots: [uri] });
  assert.equal(result.success, true, JSON.stringify(result.diagnostics));
  assert.ok(result.models.some(model => model.name === "WasmModel"));
  assert.equal(result.schemaVersion, 1);
  assert.equal(result.abiVersion, 1);

  const formatted = session.format(uri);
  assert.equal(formatted.success, true);
  assert.match(formatted.text, /!! kept/);

  const invalidUri = "memory:///WäsmInvalid.ili";
  session.putSource(invalidUri, `INTERLIS 2.3;
MODEL WaesmInvalid AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      Bezeichnung : UnbekannterTyp;
    END Item;
  END Topic;
END WaesmInvalid.
`);
  const invalid = session.compile({ roots: [invalidUri] });
  assert.equal(invalid.success, false);
  assert.ok(invalid.diagnostics.some(diagnostic =>
    diagnostic.range?.uri === invalidUri && diagnostic.range.start.line === 4));
  session.dispose();
});
