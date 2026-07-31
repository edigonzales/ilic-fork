import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import test from "node:test";
import { Compiler, createCompiler } from "../index.js";

const modulePath = resolve(import.meta.dirname, "../ilic.mjs");

test("rejects mixed wrapper/WASM versions and only allows explicit legacy projection", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built",
}, async () => {
  const moduleFactory = (await import("../ilic.mjs")).default;
  const native = await moduleFactory();
  let parseCalls = 0;
  const legacyModule = new Proxy(native, {
    get(target, property, receiver) {
      if (property === "_ilic_editor_snapshot") return undefined;
      if (property === "_ilic_parse") {
        const parse = Reflect.get(target, property, receiver);
        return (...args) => {
          parseCalls += 1;
          return parse(...args);
        };
      }
      return Reflect.get(target, property, receiver);
    },
  });

  assert.throws(
    () => new Compiler(legacyModule),
    /ilic native editor snapshot export is missing/,
  );
  const compiler = await createCompiler({
    moduleFactory: () => legacyModule,
    compatibility: { allowLegacyEditorProjection: true },
  });
  assert.equal(compiler.capabilities.nativeEditorSnapshot, false);
  const session = compiler.createSession();
  const uri = "memory:///legacy.ili";
  session.putSource(uri, `INTERLIS 2.3;
MODEL Legacy AT "https://example.invalid" VERSION "1" =
END Legacy.
`);
  const snapshot = session.editorSnapshot(uri);
  assert.equal(parseCalls, 1);
  assert.equal(snapshot.kind, "editor");
  assert.equal(snapshot.declarations[0].name, "Legacy");
  session.dispose();
});
