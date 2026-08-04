import assert from "node:assert/strict";
import test from "node:test";

import { readCString } from "../wasm-memory.js";

test("readCString decodes a null-terminated UTF-8 string", () => {
  const prefix = new Uint8Array([99, 99]);
  const text = new TextEncoder().encode("ilic 0.9.10");
  const heap = new Uint8Array(prefix.length + text.length + 1);
  heap.set(prefix);
  heap.set(text, prefix.length);
  assert.equal(readCString({ HEAPU8: heap }, prefix.length), "ilic 0.9.10");
  assert.equal(readCString({ HEAPU8: heap }, 0), "");
});

test("readCString rejects a string without a terminator", () => {
  assert.throws(
    () => readCString({ HEAPU8: new Uint8Array([1, 105, 108, 105, 99]) }, 1),
    /unterminated C string/i,
  );
});
