const encoder = new TextEncoder();
const decoder = new TextDecoder();

const editorDeclarationKinds = new Map([
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

const editorContainerKinds = new Set([
  "model",
  "topic",
  "class",
  "structure",
  "association",
  "view",
  "graphic",
]);

function containsRange(outer, inner) {
  return outer.start.byteOffset <= inner.start.byteOffset
    && outer.end.byteOffset >= inner.end.byteOffset;
}

function tokensInRange(tokens, range) {
  let low = 0;
  let high = tokens.length;
  while (low < high) {
    const middle = (low + high) >>> 1;
    if (tokens[middle].range.start.byteOffset < range.start.byteOffset)
      low = middle + 1;
    else
      high = middle;
  }
  const result = [];
  for (let index = low; index < tokens.length; index += 1) {
    const token = tokens[index];
    if (token.range.start.byteOffset > range.end.byteOffset) break;
    if (containsRange(range, token.range)) result.push(token);
  }
  return result;
}

function editorPathKind(node, nodesById) {
  let parent = node.parent;
  while (parent !== null && parent !== undefined) {
    const kind = nodesById.get(parent)?.kind ?? "";
    if (kind === "referenceAttr" || kind === "restrictedRef") return "reference";
    if (kind === "bagOrListType") return "collection";
    if (/^(?:class|structure|association|topic|view|graphic)Def$/u.test(kind))
      return "extends";
    if (/unit/iu.test(kind)) return "unit";
    parent = nodesById.get(parent)?.parent;
  }
  return "type";
}

function editorProjection(syntax) {
  const significantTokens = syntax.tokens.filter(token => token.channel === 0);
  const nodesById = new Map(syntax.nodes.map(node => [node.id, node]));
  const declarations = [];
  const declarationByNode = new Map();

  for (const node of syntax.nodes) {
    const kind = editorDeclarationKinds.get(node.kind);
    if (!kind) continue;
    const tokens = tokensInRange(significantTokens, node.range);
    const nameToken = kind === "attribute"
      ? tokens.find(token => token.kind === "NAME")
      : tokens.find((token, index) =>
          token.kind === "NAME"
          && tokens.slice(0, index).some(candidate =>
            [
              "MODEL", "TOPIC", "CLASS", "STRUCTURE", "ASSOCIATION",
              "VIEW", "GRAPHIC", "ILIDOMAIN", "UNIT",
            ].includes(candidate.kind)));
    if (!nameToken) continue;

    let parentNode = node.parent;
    while (parentNode !== null && parentNode !== undefined
      && !declarationByNode.has(parentNode))
      parentNode = nodesById.get(parentNode)?.parent;
    const parent = parentNode === null || parentNode === undefined
      ? undefined
      : declarationByNode.get(parentNode);
    const container = parent && editorContainerKinds.has(parent.kind)
      ? parent
      : undefined;
    const id = `${kind}:${nameToken.range.start.byteOffset}:${nameToken.text}`;
    const endIndex = tokens.findIndex((token, index) =>
      token.kind === "END"
      && tokens[index + 1]?.kind === "NAME"
      && tokens[index + 1]?.text.toUpperCase() === nameToken.text.toUpperCase());
    const endRange = endIndex >= 0 ? tokens[endIndex + 1].range : null;
    const declaration = {
      id,
      name: nameToken.text,
      qualifiedName: container
        ? `${container.qualifiedName}.${nameToken.text}`
        : nameToken.text,
      kind,
      containerId: container?.id ?? null,
      range: node.range,
      selectionRange: nameToken.range,
      endRange,
    };
    declarations.push(declaration);
    declarationByNode.set(node.id, declaration);
  }

  const references = [];
  for (const node of syntax.nodes) {
    if (node.kind !== "path") continue;
    const tokens = tokensInRange(significantTokens, node.range)
      .filter(token => token.kind === "NAME" || token.kind === "DOT");
    if (tokens.length === 0 || tokens[0].kind !== "NAME") continue;
    const text = tokens.map(token => token.text).join("");
    let parentNode = node.parent;
    while (parentNode !== null && parentNode !== undefined
      && !declarationByNode.has(parentNode))
      parentNode = nodesById.get(parentNode)?.parent;
    const owner = parentNode === null || parentNode === undefined
      ? undefined
      : declarationByNode.get(parentNode);
    const range = {
      uri: node.range.uri,
      start: tokens[0].range.start,
      end: tokens[tokens.length - 1].range.end,
    };
    if (syntax.importReferences?.some(reference =>
      containsRange(reference.range, range))) continue;
    references.push({
      text,
      kind: editorPathKind(node, nodesById),
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

function editorPosition(line, character, byteOffset) {
  return { line, character, byteOffset };
}

function editorRange(uri, start, end) {
  return { uri, start, end };
}

function editorTokens(uri, text) {
  const tokens = [];
  let index = 0;
  let line = 0;
  let character = 0;
  let byteOffset = 0;
  const position = () => editorPosition(line, character, byteOffset);
  const advance = () => {
    const codePoint = text.codePointAt(index);
    const width = codePoint > 0xffff ? 2 : 1;
    index += width;
    if (codePoint === 0x0a) {
      line += 1;
      character = 0;
    } else {
      character += width;
    }
    byteOffset += codePoint <= 0x7f ? 1
      : codePoint <= 0x7ff ? 2
        : codePoint <= 0xffff ? 3 : 4;
  };
  const isNameStart = value =>
    value !== undefined && /[A-Za-z_]/u.test(value);
  const isNamePart = value =>
    value !== undefined && /[A-Za-z0-9_]/u.test(value);

  while (index < text.length) {
    const value = text[index];
    if (value === "!" && text[index + 1] === "!") {
      while (index < text.length && text[index] !== "\n") advance();
      continue;
    }
    if (value === "\"") {
      advance();
      while (index < text.length) {
        if (text[index] === "\\" && index + 1 < text.length) {
          advance();
          advance();
          continue;
        }
        const closing = text[index] === "\"";
        advance();
        if (closing) break;
      }
      continue;
    }
    if (/\s/u.test(value)) {
      advance();
      continue;
    }
    const startIndex = index;
    const start = position();
    if (isNameStart(value)) {
      advance();
      while (isNamePart(text[index])) advance();
    } else if (/[0-9]/u.test(value)) {
      advance();
      while (/[0-9.]/u.test(text[index] ?? "")) advance();
    } else {
      advance();
    }
    const tokenText = text.slice(startIndex, index);
    tokens.push({
      text: tokenText,
      upper: tokenText.toUpperCase(),
      range: editorRange(uri, start, position()),
    });
  }
  return tokens;
}

function editorTextProjection(uri, text, documentVersion) {
  const tokens = editorTokens(uri, text);
  const tokenIndexByStartOffset = new Map(tokens.map((token, index) => [
    token.range.start.byteOffset,
    index,
  ]));
  const declarations = [];
  const references = [];
  const referenceKeys = new Set();
  const imports = [];
  const diagnostics = [];
  const stack = [];
  const containerKinds = new Set([
    "model", "topic", "class", "structure", "association", "view", "graphic",
  ]);
  const declarationKinds = new Map([
    ["MODEL", "model"],
    ["TOPIC", "topic"],
    ["CLASS", "class"],
    ["STRUCTURE", "structure"],
    ["ASSOCIATION", "association"],
    ["VIEW", "view"],
    ["GRAPHIC", "graphic"],
    ["DOMAIN", "domain"],
    ["UNIT", "unit"],
  ]);
  const declarationKeywords = new Set(declarationKinds.keys());
  const textEnd = (() => {
    let line = 0;
    let character = 0;
    for (const value of text) {
      if (value === "\n") {
        line += 1;
        character = 0;
      } else {
        character += value.length;
      }
    }
    return editorPosition(line, character, encoder.encode(text).length);
  })();
  const pathAt = start => {
    if (!tokens[start] || !/^[A-Za-z_]/u.test(tokens[start].text)) return null;
    let end = start + 1;
    while (tokens[end]?.text === "."
      && /^[A-Za-z_]/u.test(tokens[end + 1]?.text ?? ""))
      end += 2;
    return {
      text: tokens.slice(start, end).map(token => token.text).join(""),
      range: editorRange(
        uri,
        tokens[start].range.start,
        tokens[end - 1].range.end,
      ),
      next: end,
    };
  };
  const statementEnd = (start, stopAtDeclaration = false) => {
    for (let index = start; index < tokens.length; index += 1) {
      if (tokens[index].text === ";") return index;
      if (tokens[index].upper === "END") return Math.max(start, index - 1);
      if (stopAtDeclaration && index > start
        && declarationKeywords.has(tokens[index].upper))
        return Math.max(start, index - 1);
    }
    return tokens.length - 1;
  };
  const headerEquals = start => {
    for (let index = start; index < Math.min(tokens.length, start + 192); index += 1) {
      if (tokens[index].text === "=") return index;
      if (tokens[index].text === ";" || tokens[index].upper === "END")
        return -1;
      if (index > start && declarationKeywords.has(tokens[index].upper))
        return -1;
    }
    return -1;
  };
  const ownerForOffset = offset => declarations
    .filter(declaration =>
      declaration.range.start.byteOffset <= offset
      && declaration.range.end.byteOffset >= offset)
    .sort((left, right) =>
      right.range.start.byteOffset - left.range.start.byteOffset
      || left.range.end.byteOffset - right.range.end.byteOffset)[0];
  const addReference = (path, kind, sourceId) => {
    if (!path) return;
    if (imports.some(entry =>
      entry.range.start.byteOffset === path.range.start.byteOffset))
      return;
    const key = `${kind}:${path.range.start.byteOffset}:${path.range.end.byteOffset}`;
    if (referenceKeys.has(key)) return;
    referenceKeys.add(key);
    references.push({ text: path.text, kind, sourceId: sourceId ?? null, range: path.range });
  };

  for (let index = 0; index < tokens.length; index += 1) {
    const token = tokens[index];
    if (token.upper === "IMPORTS") {
      let unqualified = false;
      for (let cursor = index + 1; cursor < tokens.length; cursor += 1) {
        const candidate = tokens[cursor];
        if (candidate.text === ";") {
          index = cursor;
          break;
        }
        if (candidate.upper === "UNQUALIFIED") {
          unqualified = true;
          continue;
        }
        if (/^[A-Za-z_][A-Za-z0-9_]*$/u.test(candidate.text))
          imports.push({
            model: candidate.text,
            unqualified,
            range: candidate.range,
          });
      }
      continue;
    }
    if (token.upper === "END") {
      const name = tokens[index + 1];
      if (!name || !/^[A-Za-z_]/u.test(name.text)) continue;
      const declaration = stack.pop();
      if (declaration) {
        if (declaration.name.toUpperCase() !== name.text.toUpperCase())
          diagnostics.push({
            severity: "error",
            code: "ILIC-LIVE-END-NAME",
            message: `Expected END ${declaration.name}`,
            range: name.range,
            relatedInformation: [],
            notes: [],
            treatedAsError: false,
            source: "live",
          });
        const punctuation = [".", ";"].includes(tokens[index + 2]?.text)
          ? tokens[index + 2]
          : name;
        declaration.range = editorRange(
          uri,
          declaration.range.start,
          punctuation.range.end,
        );
        declaration.endRange = name.range;
      }
      continue;
    }

    const keywordIndex = index;
    let nameIndex = index + 1;
    let kind = declarationKinds.get(token.upper);
    if ([":", ".", "OF", "TO", "EXTENDS", "REFERENCE", "LIST", "BAG"]
      .includes(tokens[index - 1]?.upper))
      kind = undefined;
    if (token.upper === "VIEW" && tokens[index + 1]?.upper === "TOPIC") {
      nameIndex = index + 2;
      kind = "topic";
    } else if (
      token.upper === "TOPIC" && tokens[index - 1]?.upper === "VIEW"
    ) {
      continue;
    }
    const name = tokens[nameIndex];
    if (kind && name && /^[A-Za-z_][A-Za-z0-9_]*$/u.test(name.text)) {
      const parent = stack.at(-1);
      const endIndex = statementEnd(nameIndex + 1, containerKinds.has(kind));
      const endToken = tokens[endIndex] ?? name;
      const id = `${kind}:${name.range.start.byteOffset}:${name.text}`;
      const declaration = {
        id,
        name: name.text,
        qualifiedName: parent
          ? `${parent.qualifiedName}.${name.text}`
          : name.text,
        kind,
        containerId: parent?.id ?? null,
        range: editorRange(
          uri,
          tokens[keywordIndex].range.start,
          containerKinds.has(kind) ? textEnd : endToken.range.end,
        ),
        selectionRange: name.range,
        endRange: null,
      };
      declarations.push(declaration);
      if (containerKinds.has(kind)
        && headerEquals(nameIndex + 1) >= 0)
        stack.push(declaration);
      continue;
    }

    if (
      /^[A-Za-z_][A-Za-z0-9_]*$/u.test(token.text)
      && tokens[index + 1]?.text === ":"
      && ["class", "structure", "association", "view"].includes(stack.at(-1)?.kind)
    ) {
      const parent = stack.at(-1);
      const endIndex = statementEnd(index + 2);
      const endToken = tokens[endIndex] ?? tokens[index + 1];
      declarations.push({
        id: `attribute:${token.range.start.byteOffset}:${token.text}`,
        name: token.text,
        qualifiedName: `${parent.qualifiedName}.${token.text}`,
        kind: "attribute",
        containerId: parent.id,
        range: editorRange(uri, token.range.start, endToken.range.end),
        selectionRange: token.range,
        endRange: null,
      });
    }
  }

  for (let index = 0; index < tokens.length; index += 1) {
    const token = tokens[index];
    if (token.upper === "EXTENDS") {
      const path = pathAt(index + 1);
      const owner = ownerForOffset(token.range.start.byteOffset);
      addReference(path, "extends", owner?.id);
      continue;
    }
    if (token.upper === "REFERENCE") {
      const start = tokens[index + 1]?.upper === "TO" ? index + 2 : index + 1;
      const path = pathAt(start);
      const owner = ownerForOffset(token.range.start.byteOffset);
      addReference(path, "reference", owner?.id);
      continue;
    }
    if (token.upper === "LIST" || token.upper === "BAG") {
      let cursor = index + 1;
      while (cursor < Math.min(tokens.length, index + 24)
        && tokens[cursor].upper !== "OF"
        && tokens[cursor].text !== ";")
        cursor += 1;
      const path = tokens[cursor]?.upper === "OF" ? pathAt(cursor + 1) : null;
      const owner = ownerForOffset(token.range.start.byteOffset);
      addReference(path, "collection", owner?.id);
    }
  }

  for (const declaration of declarations) {
    if (declaration.kind !== "attribute"
      && declaration.kind !== "domain"
      && declaration.kind !== "unit")
      continue;
    const startIndex = tokenIndexByStartOffset.get(
      declaration.selectionRange.start.byteOffset,
    ) ?? -1;
    if (startIndex < 0) continue;
    const separator = declaration.kind === "attribute" ? ":" : "=";
    let cursor = startIndex + 1;
    while (cursor < tokens.length
      && tokens[cursor].text !== separator
      && tokens[cursor].text !== ";")
      cursor += 1;
    if (tokens[cursor]?.text !== separator) continue;
    cursor += 1;
    while (tokens[cursor]?.upper === "MANDATORY") cursor += 1;
    if (["LIST", "BAG", "REFERENCE"].includes(tokens[cursor]?.upper))
      continue;
    const skipped = new Set(["OF", "TO", "FORMAT"]);
    while (cursor < tokens.length && skipped.has(tokens[cursor].upper))
      cursor += 1;
    const path = pathAt(cursor);
    if (!path) continue;
    const kind = declaration.kind === "unit" ? "unit" : "type";
    addReference(path, kind, declaration.id);
  }

  const first = tokens[0]?.upper;
  const interlisIndex = tokens.findIndex(token => token.upper === "INTERLIS");
  const version = first === "TRANSFER"
    ? "1.0"
    : interlisIndex >= 0
      ? (tokens[interlisIndex + 1]?.text === "2.4" ? "2.4" : "2.3")
      : "2.3";
  const duplicateNames = new Map();
  for (const declaration of declarations) {
    const key = `${declaration.containerId ?? ""}:${declaration.name.toUpperCase()}`;
    const previous = duplicateNames.get(key);
    if (previous)
      diagnostics.push({
        severity: "error",
        code: "ILIC-LIVE-DUPLICATE",
        message: `Duplicate declaration '${declaration.name}'`,
        range: declaration.selectionRange,
        relatedInformation: [{
          range: previous.selectionRange,
          message: "First declaration",
        }],
        notes: [],
        treatedAsError: false,
        source: "live",
      });
    else
      duplicateNames.set(key, declaration);
  }
  return {
    schemaVersion: 1,
    abiVersion: 1,
    compilerVersion: "ilic-editor-snapshot",
    kind: "editor",
    success: true,
    uri,
    documentVersion,
    iliVersion: version,
    declarations,
    references,
    imports,
    contexts: declarations
      .filter(declaration => containerKinds.has(declaration.kind))
      .map(declaration => ({
        kind: `${declaration.kind}Def`,
        range: declaration.range,
      })),
    diagnostics,
  };
}

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
  #sources = new Map();

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
      this.#sources.set(uri, {
        text: typeof source === "string" ? source : decoder.decode(source),
        version,
      });
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
      const removed = this.#module._ilic_session_remove_source(this.#handle,
        encodedUri.pointer, encodedUri.length) === 0;
      if (removed) this.#sources.delete(uri);
      return removed;
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

  editorSnapshot(uri) {
    this.#active();
    const source = this.#sources.get(uri);
    return source
      ? editorTextProjection(uri, source.text, source.version)
      : editorProjection(this.parse(uri));
  }

  analyze(request) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_analyze,
      this.#handle, { schemaVersion: 1, ...request });
  }

  compileAndAnalyze(request) {
    this.#active();
    return callJson(this.#module, this.#module._ilic_compile_and_analyze,
      this.#handle, { schemaVersion: 1, ...request });
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
    this.#sources.clear();
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
