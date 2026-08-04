const encoder = new TextEncoder();
const decoder = new TextDecoder();

export function readCString(module, pointer) {
  if (!pointer) return "";
  let end = pointer;
  while (end < module.HEAPU8.length && module.HEAPU8[end] !== 0) end += 1;
  if (end >= module.HEAPU8.length) {
    throw new Error("ilic returned an unterminated C string");
  }
  return decoder.decode(module.HEAPU8.subarray(pointer, end));
}

export function copyIn(module, value) {
  const bytes = typeof value === "string" ? encoder.encode(value) : value;
  const pointer = module._ilic_alloc(bytes.byteLength || 1);
  if (!pointer) throw new Error("ilic WASM allocation failed");
  if (bytes.byteLength) module.HEAPU8.set(bytes, pointer);
  return { pointer, length: bytes.byteLength };
}

export function readResultJson(module, resultHandle) {
  const lengthPointer = module._ilic_alloc(4);
  try {
    const resultPointer = module._ilic_result_json(resultHandle, lengthPointer);
    const length = module.HEAPU32[lengthPointer >>> 2];
    if (!resultPointer) throw new Error("ilic returned an invalid result handle");
    return JSON.parse(decoder.decode(module.HEAPU8.subarray(resultPointer, resultPointer + length)));
  } finally {
    if (resultHandle) module._ilic_result_destroy(resultHandle);
    module._ilic_free(lengthPointer);
  }
}

export function callJson(module, operation, session, request) {
  if (typeof operation !== "function")
    throw new Error("ilic native editor snapshot export is missing; the wrapper and WASM module are incompatible");
  const input = copyIn(module, JSON.stringify(request));
  try {
    return readResultJson(module, operation(session, input.pointer, input.length));
  } finally {
    module._ilic_free(input.pointer);
  }
}
