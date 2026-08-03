import { CompilerSession } from "./compiler-session.js";

export class Compiler {
  #allowLegacyEditorProjection;

  constructor(module, { allowLegacyEditorProjection = false } = {}) {
    this.module = module;
    this.#allowLegacyEditorProjection = allowLegacyEditorProjection;
    this.abiVersion = module._ilic_abi_version();
    if (this.abiVersion !== 1) throw new Error(`unsupported ilic ABI ${this.abiVersion}`);
    this.capabilities = Object.freeze({
      nativeEditorSnapshot: typeof module._ilic_editor_snapshot === "function",
      incrementalSession: typeof module._ilic_incremental_stats === "function",
      incrementalStats: typeof module._ilic_incremental_stats === "function",
      incrementalTrace: typeof module._ilic_incremental_trace === "function",
      incrementalCacheSnapshot: typeof module._ilic_incremental_cache_snapshot === "function",
      strictEditorSeparation: typeof module._ilic_incremental_trace === "function" && typeof module._ilic_editor_snapshot === "function",
    });
    if (!this.capabilities.nativeEditorSnapshot && !allowLegacyEditorProjection)
      throw new Error("ilic native editor snapshot export is missing; the wrapper and WASM module are incompatible");
  }

  createSession() {
    return new CompilerSession(this.module, { allowLegacyEditorProjection: this.#allowLegacyEditorProjection });
  }
}

export async function createCompiler({ moduleFactory, moduleOptions = {}, compatibility = {} } = {}) {
  const factory = moduleFactory ?? (await import("./ilic.mjs")).default;
  return new Compiler(await factory(moduleOptions), compatibility);
}
