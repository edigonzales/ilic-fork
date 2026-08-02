import assert from "node:assert/strict";
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { execFileSync } from "node:child_process";
import { basename, join, resolve } from "node:path";
import { tmpdir } from "node:os";
import test from "node:test";
import { createCompiler } from "../index.js";

const root = resolve(import.meta.dirname, "../../..");
const modulePath = resolve(import.meta.dirname, "../ilic.mjs");
const driver = process.env.ILIC_SNAPSHOT_DRIVER
  ?? [
    resolve(root, "build/p5.1-baseline-release/ilic_snapshot_json_driver"),
    resolve(root, "build/ci/ilic_snapshot_json_driver"),
    resolve(root, "build/p3-baseline/ilic_snapshot_json_driver"),
  ].find(existsSync);
const fixtures = ["valid.ili", "incomplete.ili", "wrong-end.ili", "crlf.ili"];

function nativeSnapshot(path) {
  return JSON.parse(execFileSync(driver, [path], { encoding: "utf8" }));
}

test("native and WASM editor snapshots are deterministic and structurally identical", {
  skip: !existsSync(modulePath) || !driver
    ? "WASM package or native snapshot driver is not built"
    : false,
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const temporaryRoot = mkdtempSync(join(tmpdir(), "ilic-snapshot-parity-"));
  try {
    const sources = fixtures.map(fixture => resolve(root, "test/snapshots", fixture));
    const valid = readFileSync(sources[0]);
    const truncated = join(temporaryRoot, "truncated.ili");
    writeFileSync(truncated, valid.subarray(0, Math.max(1, valid.length - 12)));
    sources.push(truncated);
    const invalid = join(temporaryRoot, "invalid-utf8.ili");
    writeFileSync(invalid, Buffer.concat([valid.subarray(0, 26), Buffer.from([0xff]), valid.subarray(26)]));
    sources.push(invalid);
    for (const path of sources) {
      const uri = `memory:///snapshot/${basename(path)}`;
      const bytes = readFileSync(path);
      session.putSource(uri, bytes, 1);
      const wasm = session.editorSnapshot(uri);
      const native = nativeSnapshot(path);
      assert.deepEqual(wasm, native, basename(path));
      assert.equal(wasm.complete, wasm.success && wasm.diagnostics.length === 0, basename(path));
    }
  } finally {
    rmSync(temporaryRoot, { recursive: true, force: true });
    session.dispose();
  }
});
