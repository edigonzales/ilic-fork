function containsRange(outer, inner) {
  return (
    outer.start.byteOffset <= inner.start.byteOffset &&
    outer.end.byteOffset >= inner.end.byteOffset
  );
}

function tokensInRange(tokens, range) {
  return tokens.filter((token) => containsRange(range, token.range));
}

function pathKind(node, nodesById) {
  let parent = node.parent;
  while (parent !== null && parent !== undefined) {
    const kind = nodesById.get(parent)?.kind ?? "";
    if (kind === "referenceAttr" || kind === "restrictedRef") return "reference";
    if (kind === "bagOrListType") return "collection";
    if (/^(?:class|structure|association|topic|view|graphic)Def$/u.test(kind)) return "extends";
    if (/unit/iu.test(kind)) return "unit";
    parent = nodesById.get(parent)?.parent;
  }
  return "type";
}

export function projectLegacyEditorSnapshot(syntax) {
  const declarationKinds = new Map([
    ["modelDef", "model"], ["topicDef", "topic"], ["classDef", "class"],
    ["structureDef", "structure"], ["associationDef", "association"],
    ["viewDef", "view"], ["graphicDef", "graphic"], ["domainDef", "domain"],
    ["unitDef", "unit"], ["attributeDef", "attribute"],
  ]);
  const containers = new Set(["model", "topic", "class", "structure", "association", "view", "graphic"]);
  const significantTokens = syntax.tokens.filter((token) => token.channel === 0);
  const nodesById = new Map(syntax.nodes.map((node) => [node.id, node]));
  const declarations = [];
  const declarationByNode = new Map();
  for (const node of syntax.nodes) {
    const kind = declarationKinds.get(node.kind);
    if (!kind) continue;
    const tokens = tokensInRange(significantTokens, node.range);
    const nameToken = tokens.find((token, index) => token.kind === "NAME" &&
      (kind === "attribute" || tokens.slice(0, index).some((candidate) =>
        ["MODEL", "TOPIC", "CLASS", "STRUCTURE", "ASSOCIATION", "VIEW", "GRAPHIC", "ILIDOMAIN", "UNIT"].includes(candidate.kind))));
    if (!nameToken) continue;
    let parentNode = node.parent;
    while (parentNode !== null && parentNode !== undefined && !declarationByNode.has(parentNode))
      parentNode = nodesById.get(parentNode)?.parent;
    const parent = parentNode === null || parentNode === undefined ? undefined : declarationByNode.get(parentNode);
    const container = parent && containers.has(parent.kind) ? parent : undefined;
    const declaration = {
      id: `${kind}:${nameToken.range.start.byteOffset}:${nameToken.text}`,
      name: nameToken.text,
      qualifiedName: container ? `${container.qualifiedName}.${nameToken.text}` : nameToken.text,
      kind, containerId: container?.id ?? null, range: node.range,
      selectionRange: nameToken.range, endRange: null,
    };
    declarations.push(declaration);
    declarationByNode.set(node.id, declaration);
  }
  const references = [];
  for (const node of syntax.nodes) {
    if (node.kind !== "path") continue;
    const tokens = tokensInRange(significantTokens, node.range).filter((token) => token.kind === "NAME" || token.kind === "DOT");
    if (!tokens.length || tokens[0].kind !== "NAME") continue;
    const range = { uri: node.range.uri, start: tokens[0].range.start, end: tokens[tokens.length - 1].range.end };
    if (syntax.importReferences?.some((reference) => containsRange(reference.range, range))) continue;
    let parentNode = node.parent;
    while (parentNode !== null && parentNode !== undefined && !declarationByNode.has(parentNode))
      parentNode = nodesById.get(parentNode)?.parent;
    const owner = parentNode === null || parentNode === undefined ? undefined : declarationByNode.get(parentNode);
    references.push({ text: tokens.map((token) => token.text).join(""), kind: pathKind(node, nodesById), sourceId: owner?.id ?? null, range });
  }
  return {
    schemaVersion: 1, abiVersion: syntax.abiVersion, compilerVersion: syntax.compilerVersion,
    kind: "editor", success: syntax.success, recovered: !syntax.success, complete: syntax.success,
    uri: syntax.uri, documentVersion: syntax.documentVersion, iliVersion: syntax.iliVersion,
    declarations, references, imports: syntax.importReferences ?? [], contexts: syntax.contexts,
    diagnostics: syntax.diagnostics,
  };
}
