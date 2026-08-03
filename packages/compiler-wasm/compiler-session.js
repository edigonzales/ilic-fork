import { projectLegacyEditorSnapshot } from "./legacy-editor-projection.js";
import { callJson, copyIn, readResultJson } from "./wasm-memory.js";

export class CompilerSession {
  #module;
  #handle;
  #allowLegacyEditorProjection;

  constructor(module, { allowLegacyEditorProjection = false } = {}) {
    this.#module = module;
    this.#allowLegacyEditorProjection = allowLegacyEditorProjection;
    this.#handle = module._ilic_session_create();
    if (!this.#handle) throw new Error("ilic could not create a compiler session");
  }

  #active() {
    if (!this.#handle) throw new Error("compiler session has been disposed");
  }

  putSource(uri, source, version = 0) {
    this.#active();
    const encodedUri = copyIn(this.#module, uri);
    const encodedSource = copyIn(this.#module, source);
    try {
      const status = this.#module._ilic_session_put_source(this.#handle, encodedUri.pointer, encodedUri.length,
        encodedSource.pointer, encodedSource.length, BigInt(version));
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
      return this.#module._ilic_session_remove_source(this.#handle, encodedUri.pointer, encodedUri.length) === 0;
    } finally {
      this.#module._ilic_free(encodedUri.pointer);
    }
  }

  #read(operation, unavailable) {
    this.#active();
    if (typeof this.#module[operation] !== "function") throw new Error(unavailable);
    return readResultJson(this.#module, this.#module[operation](this.#handle));
  }

  incrementalStats() { return this.#read("_ilic_incremental_stats", "native incremental statistics API is unavailable"); }
  incrementalTrace() { return this.#read("_ilic_incremental_trace", "native incremental trace API is unavailable"); }
  incrementalCacheSnapshot() { return this.#read("_ilic_incremental_cache_snapshot", "native incremental cache API is unavailable"); }

  resetIncrementalStats() {
    this.#active();
    if (typeof this.#module._ilic_reset_incremental_stats !== "function") throw new Error("native incremental statistics reset API is unavailable");
    const status = this.#module._ilic_reset_incremental_stats(this.#handle);
    if (status !== 0) throw new Error(`ilic could not reset incremental statistics (${status})`);
  }

  clearIncrementalCaches() {
    this.#active();
    if (typeof this.#module._ilic_clear_incremental_caches !== "function") throw new Error("native incremental cache API is unavailable");
    const status = this.#module._ilic_clear_incremental_caches(this.#handle);
    if (status !== 0) throw new Error(`ilic could not clear incremental caches (${status})`);
  }

  compile(request) { this.#active(); return callJson(this.#module, this.#module._ilic_compile, this.#handle, { schemaVersion: 1, ...request }); }
  parse(uri) { this.#active(); return callJson(this.#module, this.#module._ilic_parse, this.#handle, { schemaVersion: 1, uri }); }

  editorSnapshot(uri) {
    this.#active();
    if (typeof this.#module._ilic_editor_snapshot !== "function") {
      if (!this.#allowLegacyEditorProjection) throw new Error("native ilic editor snapshot API is unavailable");
      return projectLegacyEditorSnapshot(this.parse(uri));
    }
    return callJson(this.#module, this.#module._ilic_editor_snapshot, this.#handle, { schemaVersion: 1, uri });
  }

  analyze(request) { this.#active(); return callJson(this.#module, this.#module._ilic_analyze, this.#handle, { schemaVersion: 1, ...request }); }
  compileAndAnalyze(request) { this.#active(); return callJson(this.#module, this.#module._ilic_compile_and_analyze, this.#handle, { schemaVersion: 1, ...request }); }
  format(uri, options = {}) { this.#active(); return callJson(this.#module, this.#module._ilic_format, this.#handle, { schemaVersion: 1, uri, options }); }

  dispose() {
    if (!this.#handle) return;
    this.#module._ilic_session_destroy(this.#handle);
    this.#handle = 0;
  }
}
