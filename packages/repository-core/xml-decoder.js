const utf8 = new TextDecoder("utf-8", { fatal: true });

function diagnostic(code, message, severity = "error", operation = "metadata") {
  return { severity, code, message, operation };
}

function isNameStart(character) {
  return character !== undefined && /[A-Za-z_:]/u.test(character);
}

function isNamePart(character) {
  return character !== undefined && /[A-Za-z0-9_.:-]/u.test(character);
}

function decodeEntities(value) {
  let output = "";
  for (let index = 0; index < value.length; index += 1) {
    if (value[index] !== "&") { output += value[index]; continue; }
    const end = value.indexOf(";", index + 1);
    if (end < 0) throw new Error("unterminated XML entity");
    const entity = value.slice(index + 1, end);
    if (entity === "amp") output += "&";
    else if (entity === "lt") output += "<";
    else if (entity === "gt") output += ">";
    else if (entity === "quot") output += '"';
    else if (entity === "apos") output += "'";
    else if (entity.startsWith("#x")) output += String.fromCodePoint(Number.parseInt(entity.slice(2), 16));
    else if (entity.startsWith("#")) output += String.fromCodePoint(Number.parseInt(entity.slice(1), 10));
    else throw new Error(`unsupported XML entity &${entity};`);
    index = end;
  }
  return output;
}

function localName(name) {
  const separator = name.lastIndexOf(":");
  return separator < 0 ? name : name.slice(separator + 1);
}

function skipSpace(xml, index) {
  while (index < xml.length && /\s/u.test(xml[index])) index += 1;
  return index;
}

function parseName(xml, index) {
  if (!isNameStart(xml[index])) throw new Error("invalid XML element name");
  const start = index;
  index += 1;
  while (isNamePart(xml[index])) index += 1;
  return { value: xml.slice(start, index), index };
}

function parseXml(xml, { maxDepth = 128 } = {}) {
  const root = { name: "#document", local: "#document", text: "", children: [] };
  const stack = [root];
  let index = 0;
  let sawRoot = false;
  while (index < xml.length) {
    if (xml[index] !== "<") {
      const end = xml.indexOf("<", index);
      const text = decodeEntities(xml.slice(index, end < 0 ? xml.length : end));
      stack.at(-1).text += text;
      index = end < 0 ? xml.length : end;
      continue;
    }
    if (xml.startsWith("<!--", index)) {
      const end = xml.indexOf("-->", index + 4);
      if (end < 0) throw new Error("unterminated XML comment");
      index = end + 3;
      continue;
    }
    if (xml.startsWith("<![CDATA[", index)) {
      const end = xml.indexOf("]]>", index + 9);
      if (end < 0) throw new Error("unterminated CDATA section");
      stack.at(-1).text += xml.slice(index + 9, end);
      index = end + 3;
      continue;
    }
    if (xml.startsWith("<?", index)) {
      const end = xml.indexOf("?>", index + 2);
      if (end < 0) throw new Error("unterminated processing instruction");
      index = end + 2;
      continue;
    }
    if (xml.startsWith("<!DOCTYPE", index) || xml.startsWith("<!ENTITY", index))
      throw new Error("DTD and external entities are disabled");
    if (xml.startsWith("<!", index)) throw new Error("unsupported XML declaration");
    if (xml.startsWith("</", index)) {
      const parsed = parseName(xml, index + 2);
      index = skipSpace(xml, parsed.index);
      if (xml[index] !== ">") throw new Error("invalid XML closing tag");
      const current = stack.pop();
      if (!current || current.name !== parsed.value) throw new Error("mismatched XML closing tag");
      index += 1;
      continue;
    }
    const parsed = parseName(xml, index + 1);
    index = parsed.index;
    const node = { name: parsed.value, local: localName(parsed.value), text: "", children: [] };
    index = skipSpace(xml, index);
    while (index < xml.length && xml[index] !== ">" && !xml.startsWith("/>", index)) {
      const attribute = parseName(xml, index);
      index = skipSpace(xml, attribute.index);
      if (xml[index] !== "=") throw new Error("invalid XML attribute");
      index = skipSpace(xml, index + 1);
      const quote = xml[index];
      if (quote !== '"' && quote !== "'") throw new Error("XML attributes must be quoted");
      const end = xml.indexOf(quote, index + 1);
      if (end < 0) throw new Error("unterminated XML attribute");
      node.attributes ??= new Map();
      node.attributes.set(attribute.value, decodeEntities(xml.slice(index + 1, end)));
      index = skipSpace(xml, end + 1);
    }
    const selfClosing = xml.startsWith("/>", index);
    if (!selfClosing && xml[index] !== ">") throw new Error("unterminated XML start tag");
    stack.at(-1).children.push(node);
    sawRoot = sawRoot || stack.length === 1;
    index += selfClosing ? 2 : 1;
    if (!selfClosing) {
      if (stack.length >= maxDepth) throw new Error("XML nesting limit exceeded");
      stack.push(node);
    }
  }
  if (stack.length !== 1 || !sawRoot) throw new Error("incomplete XML document");
  if (root.children.length !== 1) throw new Error("XML document must have one root element");
  return root.children[0];
}

function directText(node, name) {
  const child = node.children.find(current => current.local === name);
  return child ? child.text.trim() : "";
}

function collectValues(node, values) {
  for (const child of node.children) {
    if (child.local === "value") {
      const value = child.text.trim();
      if (value) values.push(value);
    } else collectValues(child, values);
  }
}

function metadataNodes(node, output) {
  if (node.local.endsWith("ModelMetadata")) output.push(node);
  for (const child of node.children) metadataNodes(child, output);
}

function bytesToXml(bytes, maxBytes) {
  const value = typeof bytes === "string" ? bytes : bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
  if (value.byteLength > maxBytes) throw new Error(`XML resource exceeds ${maxBytes} byte limit`);
  return typeof bytes === "string" ? bytes : utf8.decode(value);
}

export class SafeRepositoryMetadataDecoder {
  constructor({ maxBytes = 32 * 1024 * 1024, maxDepth = 128 } = {}) {
    this.maxBytes = maxBytes;
    this.maxDepth = maxDepth;
  }

  decodeModelIndex(bytes, repository) {
    try {
      const root = parseXml(bytesToXml(bytes, this.maxBytes), { maxDepth: this.maxDepth });
      const nodes = [];
      metadataNodes(root, nodes);
      const models = [];
      for (const node of nodes) {
        const model = {
          name: directText(node, "Name"),
          schemaLanguage: directText(node, "SchemaLanguage"),
          file: directText(node, "File"),
          version: directText(node, "Version"),
          publishingDate: directText(node, "publishingDate"),
          precursorVersion: directText(node, "precursorVersion"),
          md5: directText(node, "md5"),
          repository,
          dependencies: [],
          browseOnly: ["true", "1"].includes(directText(node, "browseOnly").toLowerCase())
        };
        for (const child of node.children)
          if (child.local === "dependsOnModel") collectValues(child, model.dependencies);
        if (model.name && model.schemaLanguage && model.file) models.push(model);
      }
      const diagnostics = [];
      if (!models.length) diagnostics.push(diagnostic(
        "ILIC-REPO-INDEX", `no valid ModelMetadata entries in ilimodels.xml from ${repository}`));
      return { success: models.length > 0, index: { repository, models }, diagnostics };
    } catch (error) {
      return {
        success: false, index: { repository, models: [] },
        diagnostics: [diagnostic("ILIC-REPO-XML", `invalid ilimodels.xml in ${repository}: ${error.message}`)]
      };
    }
  }

  decodeSite(bytes, repository) {
    try {
      const root = parseXml(bytesToXml(bytes, this.maxBytes), { maxDepth: this.maxDepth });
      const site = { parentSites: [], subsidiarySites: [] };
      const visit = node => {
        for (const child of node.children) {
          if (child.local === "parentSite") collectValues(child, site.parentSites);
          else if (child.local === "subsidiarySite") collectValues(child, site.subsidiarySites);
          visit(child);
        }
      };
      visit(root);
      return { success: true, site, diagnostics: [] };
    } catch (error) {
      return {
        success: false, site: { parentSites: [], subsidiarySites: [] },
        diagnostics: [diagnostic("ILIC-REPO-SITE", `invalid ilisite.xml in ${repository}: ${error.message}`, "warning", "site")]
      };
    }
  }
}

export function parseIliModelsXml(xml, repository, decoder = new SafeRepositoryMetadataDecoder()) {
  return decoder.decodeModelIndex(xml, repository).index.models;
}

export function parseIliSiteXml(xml, decoder = new SafeRepositoryMetadataDecoder()) {
  return decoder.decodeSite(xml, "").site;
}
