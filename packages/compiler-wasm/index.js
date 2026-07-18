const encoder = new TextEncoder();
const decoder = new TextDecoder();

function copyIn(module, value) {
  const bytes = typeof value === "string" ? encoder.encode(value) : value;
  const pointer = module._ilic_alloc(bytes.byteLength || 1);
  if (!pointer) throw new Error("ilic WASM allocation failed");
  if (bytes.byteLength) module.HEAPU8.set(bytes, pointer);
  return { pointer, length: bytes.byteLength };
}

function callJson(module, operation, session, request) {
  const input = copyIn(module, JSON.stringify(request));
  const lengthPointer = module._ilic_alloc(4);
  let resultHandle = 0;
  try {
    resultHandle = operation(session, input.pointer, input.length);
    const resultPointer = module._ilic_result_json(resultHandle, lengthPointer);
    const length = module.HEAPU32[lengthPointer >>> 2];
    if (!resultPointer) throw new Error("ilic returned an invalid result handle");
    return JSON.parse(decoder.decode(module.HEAPU8.subarray(resultPointer, resultPointer + length)));
  } finally {
    if (resultHandle) module._ilic_result_destroy(resultHandle);
    module._ilic_free(lengthPointer);
    module._ilic_free(input.pointer);
  }
}

export class CompilerSession {
  #module;
  #handle;

  constructor(module) {
    this.#module = module;
    this.#handle = module._ilic_session_create();
    if (!this.#handle) throw new Error("ilic could not create a compiler session");
  }

  #active() {
    if (!this.#handle) throw new Error("compiler session has been disposed");
  }

  putSource(uri, source, version = 0) {
    this.#active();
    const encodedUri = copyIn(this.#module, uri);
    const encodedSource = copyIn(this.#module,
      typeof source === "string" ? encoder.encode(source) : source);
    try {
      const status = this.#module._ilic_session_put_source(this.#handle,
        encodedUri.pointer, encodedUri.length, encodedSource.pointer, encodedSource.length,
        BigInt(version));
      if (status !== 0) throw new Error(`ilic rejected source ${uri} (status ${status})`);
    } finally {
      this.#module._ilic_free(encodedSource.pointer);
      this.#module._ilic_free(encodedUri.pointer);
    }
  }

  putWorkspace(workspace) {
    for (const model of workspace.models) this.putSource(model.uri, model.source);
  }

  removeSource(uri) {
    this.#active();
    const encodedUri = copyIn(this.#module, uri);
    try {
      return this.#module._ilic_session_remove_source(this.#handle,
        encodedUri.pointer, encodedUri.length) === 0;
    } finally {
      this.#module._ilic_free(encodedUri.pointer);
    }
  }

  compile(request) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_compile,
      this.#handle, { schemaVersion: 1, ...request });
  }

  parse(uri) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_parse,
      this.#handle, { schemaVersion: 1, uri });
  }

  format(uri, options = {}) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_format,
      this.#handle, { schemaVersion: 1, uri, options });
  }

  dispose() {
    if (!this.#handle) return;
    this.#module._ilic_session_destroy(this.#handle);
    this.#handle = 0;
  }
}

export class Compiler {
  constructor(module) {
    this.module = module;
    this.abiVersion = module._ilic_abi_version();
    if (this.abiVersion !== 1) throw new Error(`unsupported ilic ABI ${this.abiVersion}`);
  }

  createSession() { return new CompilerSession(this.module); }
}

export async function createCompiler({ moduleFactory, moduleOptions = {} } = {}) {
  const factory = moduleFactory ?? (await import("./ilic.mjs")).default;
  return new Compiler(await factory(moduleOptions));
}
