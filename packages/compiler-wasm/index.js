const encoder = new TextEncoder();
const decoder = new TextDecoder();

function legacyContainsRange(outer, inner) {
  return (
    outer.start.byteOffset <= inner.start.byteOffset &&
    outer.end.byteOffset >= inner.end.byteOffset
  );
}

function legacyTokensInRange(tokens, range) {
  return tokens.filter((token) => legacyContainsRange(range, token.range));
}

function legacyPathKind(node, nodesById) {
  let parent = node.parent;
  while (parent !== null && parent !== undefined) {
    const kind = nodesById.get(parent)?.kind ?? "";
    if (kind === "referenceAttr" || kind === "restrictedRef")
      return "reference";
    if (kind === "bagOrListType") return "collection";
    if (/^(?:class|structure|association|topic|view|graphic)Def$/u.test(kind))
      return "extends";
    if (/unit/iu.test(kind)) return "unit";
    parent = nodesById.get(parent)?.parent;
  }
  return "type";
}

function projectLegacyEditorSnapshot(syntax) {
  const declarationKinds = new Map([
    ["modelDef", "model"],
    ["topicDef", "topic"],
    ["classDef", "class"],
    ["structureDef", "structure"],
    ["associationDef", "association"],
    ["viewDef", "view"],
    ["graphicDef", "graphic"],
    ["domainDef", "domain"],
    ["unitDef", "unit"],
    ["attributeDef", "attribute"],
  ]);
  const containers = new Set([
    "model",
    "topic",
    "class",
    "structure",
    "association",
    "view",
    "graphic",
  ]);
  const significantTokens = syntax.tokens.filter(
    (token) => token.channel === 0,
  );
  const nodesById = new Map(syntax.nodes.map((node) => [node.id, node]));
  const declarations = [];
  const declarationByNode = new Map();
  for (const node of syntax.nodes) {
    const kind = declarationKinds.get(node.kind);
    if (!kind) continue;
    const tokens = legacyTokensInRange(significantTokens, node.range);
    const nameToken = tokens.find(
      (token, index) =>
        token.kind === "NAME" &&
        (kind === "attribute" ||
          tokens
            .slice(0, index)
            .some((candidate) =>
              [
                "MODEL",
                "TOPIC",
                "CLASS",
                "STRUCTURE",
                "ASSOCIATION",
                "VIEW",
                "GRAPHIC",
                "ILIDOMAIN",
                "UNIT",
              ].includes(candidate.kind),
            )),
    );
    if (!nameToken) continue;
    let parentNode = node.parent;
    while (
      parentNode !== null &&
      parentNode !== undefined &&
      !declarationByNode.has(parentNode)
    )
      parentNode = nodesById.get(parentNode)?.parent;
    const parent =
      parentNode === null || parentNode === undefined
        ? undefined
        : declarationByNode.get(parentNode);
    const container =
      parent && containers.has(parent.kind) ? parent : undefined;
    const declaration = {
      id: `${kind}:${nameToken.range.start.byteOffset}:${nameToken.text}`,
      name: nameToken.text,
      qualifiedName: container
        ? `${container.qualifiedName}.${nameToken.text}`
        : nameToken.text,
      kind,
      containerId: container?.id ?? null,
      range: node.range,
      selectionRange: nameToken.range,
      endRange: null,
    };
    declarations.push(declaration);
    declarationByNode.set(node.id, declaration);
  }
  const references = [];
  for (const node of syntax.nodes) {
    if (node.kind !== "path") continue;
    const tokens = legacyTokensInRange(significantTokens, node.range).filter(
      (token) => token.kind === "NAME" || token.kind === "DOT",
    );
    if (!tokens.length || tokens[0].kind !== "NAME") continue;
    const range = {
      uri: node.range.uri,
      start: tokens[0].range.start,
      end: tokens[tokens.length - 1].range.end,
    };
    if (
      syntax.importReferences?.some((reference) =>
        legacyContainsRange(reference.range, range),
      )
    )
      continue;
    let parentNode = node.parent;
    while (
      parentNode !== null &&
      parentNode !== undefined &&
      !declarationByNode.has(parentNode)
    )
      parentNode = nodesById.get(parentNode)?.parent;
    const owner =
      parentNode === null || parentNode === undefined
        ? undefined
        : declarationByNode.get(parentNode);
    references.push({
      text: tokens.map((token) => token.text).join(""),
      kind: legacyPathKind(node, nodesById),
      sourceId: owner?.id ?? null,
      range,
    });
  }
  return {
    schemaVersion: 1,
    abiVersion: syntax.abiVersion,
    compilerVersion: syntax.compilerVersion,
    kind: "editor",
    success: syntax.success,
    recovered: !syntax.success,
    complete: syntax.success,
    uri: syntax.uri,
    documentVersion: syntax.documentVersion,
    iliVersion: syntax.iliVersion,
    declarations,
    references,
    imports: syntax.importReferences ?? [],
    contexts: syntax.contexts,
    diagnostics: syntax.diagnostics,
  };
}

function copyIn(module, value) {
  const bytes = typeof value === "string" ? encoder.encode(value) : value;
  const pointer = module._ilic_alloc(bytes.byteLength || 1);
  if (!pointer) throw new Error("ilic WASM allocation failed");
  if (bytes.byteLength) module.HEAPU8.set(bytes, pointer);
  return { pointer, length: bytes.byteLength };
}

function readResultJson(module, resultHandle) {
  const lengthPointer = module._ilic_alloc(4);
  try {
    const resultPointer = module._ilic_result_json(resultHandle, lengthPointer);
    const length = module.HEAPU32[lengthPointer >>> 2];
    if (!resultPointer)
      throw new Error("ilic returned an invalid result handle");
    return JSON.parse(
      decoder.decode(
        module.HEAPU8.subarray(resultPointer, resultPointer + length),
      ),
    );
  } finally {
    if (resultHandle) module._ilic_result_destroy(resultHandle);
    module._ilic_free(lengthPointer);
  }
}

function callJson(module, operation, session, request) {
  if (typeof operation !== "function") {
    throw new Error(
      "ilic native editor snapshot export is missing; the wrapper and WASM module are incompatible",
    );
  }
  const input = copyIn(module, JSON.stringify(request));
  try {
    const resultHandle = operation(session, input.pointer, input.length);
    return readResultJson(module, resultHandle);
  } finally {
    module._ilic_free(input.pointer);
  }
}

export class CompilerSession {
  #module;
  #handle;
  #allowLegacyEditorProjection;

  constructor(module, { allowLegacyEditorProjection = false } = {}) {
    this.#module = module;
    this.#allowLegacyEditorProjection = allowLegacyEditorProjection;
    this.#handle = module._ilic_session_create();
    if (!this.#handle)
      throw new Error("ilic could not create a compiler session");
  }

  #active() {
    if (!this.#handle) throw new Error("compiler session has been disposed");
  }

  putSource(uri, source, version = 0) {
    this.#active();
    const encodedUri = copyIn(this.#module, uri);
    const encodedSource = copyIn(
      this.#module,
      typeof source === "string" ? encoder.encode(source) : source,
    );
    try {
      const status = this.#module._ilic_session_put_source(
        this.#handle,
        encodedUri.pointer,
        encodedUri.length,
        encodedSource.pointer,
        encodedSource.length,
        BigInt(version),
      );
      if (status !== 0)
        throw new Error(`ilic rejected source ${uri} (status ${status})`);
    } finally {
      this.#module._ilic_free(encodedSource.pointer);
      this.#module._ilic_free(encodedUri.pointer);
    }
  }

  putWorkspace(workspace) {
    for (const model of workspace.models)
      this.putSource(model.uri, model.source);
  }

  removeSource(uri) {
    this.#active();
    const encodedUri = copyIn(this.#module, uri);
    try {
      return (
        this.#module._ilic_session_remove_source(
          this.#handle,
          encodedUri.pointer,
          encodedUri.length,
        ) === 0
      );
    } finally {
      this.#module._ilic_free(encodedUri.pointer);
    }
  }

  incrementalStats() {
    this.#active();
    if (typeof this.#module._ilic_incremental_stats !== "function") {
      throw new Error("native incremental statistics API is unavailable");
    }
    const resultHandle = this.#module._ilic_incremental_stats(this.#handle);
    return readResultJson(this.#module, resultHandle);
  }

  clearIncrementalCaches() {
    this.#active();
    if (typeof this.#module._ilic_clear_incremental_caches !== "function") {
      throw new Error("native incremental cache API is unavailable");
    }
    const status = this.#module._ilic_clear_incremental_caches(this.#handle);
    if (status !== 0)
      throw new Error(`ilic could not clear incremental caches (${status})`);
  }

  compile(request) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_compile, this.#handle, {
      schemaVersion: 1,
      ...request,
    });
  }

  parse(uri) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_parse, this.#handle, {
      schemaVersion: 1,
      uri,
    });
  }

  editorSnapshot(uri) {
    this.#active();
    if (typeof this.#module._ilic_editor_snapshot !== "function") {
      if (!this.#allowLegacyEditorProjection)
        throw new Error("native ilic editor snapshot API is unavailable");
      return projectLegacyEditorSnapshot(this.parse(uri));
    }
    return callJson(
      this.#module,
      this.#module._ilic_editor_snapshot,
      this.#handle,
      { schemaVersion: 1, uri },
    );
  }

  analyze(request) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_analyze, this.#handle, {
      schemaVersion: 1,
      ...request,
    });
  }

  compileAndAnalyze(request) {
    this.#active();
    return callJson(
      this.#module,
      this.#module._ilic_compile_and_analyze,
      this.#handle,
      { schemaVersion: 1, ...request },
    );
  }

  format(uri, options = {}) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_format, this.#handle, {
      schemaVersion: 1,
      uri,
      options,
    });
  }

  dispose() {
    if (!this.#handle) return;
    this.#module._ilic_session_destroy(this.#handle);
    this.#handle = 0;
  }
}

export class Compiler {
  constructor(module, { allowLegacyEditorProjection = false } = {}) {
    this.module = module;
    this.#allowLegacyEditorProjection = allowLegacyEditorProjection;
    this.abiVersion = module._ilic_abi_version();
    if (this.abiVersion !== 1)
      throw new Error(`unsupported ilic ABI ${this.abiVersion}`);
    this.capabilities = Object.freeze({
      nativeEditorSnapshot: typeof module._ilic_editor_snapshot === "function",
      incrementalSession: typeof module._ilic_incremental_stats === "function",
      incrementalStats: typeof module._ilic_incremental_stats === "function",
    });
    if (
      !this.capabilities.nativeEditorSnapshot &&
      !allowLegacyEditorProjection
    ) {
      throw new Error(
        "ilic native editor snapshot export is missing; the wrapper and WASM module are incompatible",
      );
    }
  }

  #allowLegacyEditorProjection;

  createSession() {
    return new CompilerSession(this.module, {
      allowLegacyEditorProjection: this.#allowLegacyEditorProjection,
    });
  }
}

export async function createCompiler({
  moduleFactory,
  moduleOptions = {},
  compatibility = {},
} = {}) {
  const factory = moduleFactory ?? (await import("./ilic.mjs")).default;
  return new Compiler(await factory(moduleOptions), compatibility);
}
