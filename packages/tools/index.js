const textDecoder = new TextDecoder();

function decodeXml(value = "") {
  return value.replaceAll("&amp;", "&").replaceAll("&lt;", "<")
    .replaceAll("&gt;", ">").replaceAll("&quot;", '"').trim();
}

function tag(body, name) {
  const match = body.match(new RegExp(`<${name}>([\\s\\S]*?)</${name}>`));
  return decodeXml(match?.[1]);
}

export function parseIliModelsXml(xml, repository) {
  const models = [];
  const pattern = /<[^>]*ModelMetadata(?:\s[^>]*)?>([\s\S]*?)<\/[^>]*ModelMetadata>/g;
  for (const match of xml.matchAll(pattern)) {
    const body = match[1];
    const name = tag(body, "Name");
    const file = tag(body, "File");
    if (!name || !file) continue;
    const dependencyBody = body.match(/<dependsOnModel>([\s\S]*?)<\/dependsOnModel>/)?.[1] ?? "";
    const dependencies = [...dependencyBody.matchAll(/<value>([\s\S]*?)<\/value>/g)]
      .map(value => decodeXml(value[1]));
    models.push({
      name,
      file,
      repository,
      dependencies,
      schemaLanguage: tag(body, "SchemaLanguage"),
      version: tag(body, "Version"),
      publishingDate: tag(body, "publishingDate"),
      precursorVersion: tag(body, "precursorVersion"),
      md5: tag(body, "md5"),
      browseOnly: tag(body, "browseOnly") === "true"
    });
  }
  return models;
}

function parseSiteLinks(xml) {
  const links = [];
  const pattern = /<(?:parentSite|subsidiarySite)>([\s\S]*?)<\/(?:parentSite|subsidiarySite)>/g;
  for (const section of xml.matchAll(pattern)) {
    for (const value of section[1].matchAll(/<value>([\s\S]*?)<\/value>/g)) links.push(decodeXml(value[1]));
  }
  return links;
}

export class MemoryCache {
  #values = new Map();
  async get(key) { return this.#values.get(key); }
  async put(key, value) { this.#values.set(key, { value, storedAt: Date.now() }); }
  async clear() { this.#values.clear(); }
}

function joinUri(base, relative) {
  if (/^https?:\/\//.test(relative)) return relative;
  if (/^https?:\/\//.test(base)) return `${base.replace(/\/$/, "")}/${relative}`;
  return `${base.replace(/\/$/, "")}/${relative}`;
}

function selectModel(models, name, schemaLanguage) {
  let selected;
  for (const model of models) {
    if (model.name !== name || model.browseOnly) continue;
    if (schemaLanguage && model.schemaLanguage !== schemaLanguage) continue;
    if (!selected) selected = model;
    else if (selected.repository === model.repository && selected.version < model.version) selected = model;
  }
  return selected;
}

function md5(bytes) {
  const shifts = [7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21];
  const constants = Array.from({ length: 64 }, (_, i) =>
    Math.floor(Math.abs(Math.sin(i + 1)) * 0x100000000) >>> 0);
  const paddedLength = Math.ceil((bytes.length + 9) / 64) * 64;
  const padded = new Uint8Array(paddedLength);
  padded.set(bytes);
  padded[bytes.length] = 0x80;
  const view = new DataView(padded.buffer);
  const bitLength = bytes.length * 8;
  view.setUint32(paddedLength - 8, bitLength >>> 0, true);
  view.setUint32(paddedLength - 4, Math.floor(bitLength / 0x100000000), true);
  let a0 = 0x67452301, b0 = 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476;
  const rotate = (value, count) => ((value << count) | (value >>> (32 - count))) >>> 0;
  for (let offset = 0; offset < paddedLength; offset += 64) {
    const words = Array.from({ length: 16 }, (_, i) => view.getUint32(offset + i * 4, true));
    let a = a0, b = b0, c = c0, d = d0;
    for (let i = 0; i < 64; i++) {
      let f, g;
      if (i < 16) { f = (b & c) | (~b & d); g = i; }
      else if (i < 32) { f = (d & b) | (~d & c); g = (5 * i + 1) % 16; }
      else if (i < 48) { f = b ^ c ^ d; g = (3 * i + 5) % 16; }
      else { f = c ^ (b | ~d); g = (7 * i) % 16; }
      const sum = (a + f + constants[i] + words[g]) >>> 0;
      a = d; d = c; c = b; b = (b + rotate(sum, shifts[i])) >>> 0;
    }
    a0 = (a0 + a) >>> 0; b0 = (b0 + b) >>> 0;
    c0 = (c0 + c) >>> 0; d0 = (d0 + d) >>> 0;
  }
  return [a0, b0, c0, d0].flatMap(value => [0, 8, 16, 24]
    .map(shift => ((value >>> shift) & 0xff).toString(16).padStart(2, "0"))).join("");
}

export class RepositoryManager {
  constructor({ repositories = [], cache = new MemoryCache(), load, offline = false,
    metadataTtlMs = 86_400_000, modelTtlMs = 604_800_000, allowStaleOnError = true,
    followSiteLinks = true, onWarning } = {}) {
    this.repositories = repositories;
    this.cache = cache;
    this.offline = offline;
    this.metadataTtlMs = metadataTtlMs;
    this.modelTtlMs = modelTtlMs;
    this.allowStaleOnError = allowStaleOnError;
    this.followSiteLinks = followSiteLinks;
    this.onWarning = onWarning;
    this.load = load ?? (async uri => {
      const response = await fetch(uri);
      if (!response.ok) throw new Error(`${response.status} ${response.statusText}`);
      return new Uint8Array(await response.arrayBuffer());
    });
  }

  async #resource(uri, ttl) {
    const cached = await this.cache.get(uri);
    const fresh = cached && Date.now() - cached.storedAt <= ttl;
    if (fresh || (this.offline && cached)) return { value: cached.value, fromCache: true };
    if (this.offline) throw new Error(`offline and no cached copy of ${uri}`);
    try {
      const value = await this.load(uri);
      const bytes = typeof value === "string" ? new TextEncoder().encode(value) : value;
      await this.cache.put(uri, bytes);
      return { value: bytes, fromCache: false };
    } catch (error) {
      if (cached && this.allowStaleOnError) return { value: cached.value, fromCache: true, stale: true };
      throw error;
    }
  }

  async listModels() {
    const result = [];
    const pending = [...this.repositories];
    const visited = new Set();
    const failures = [];
    let loaded = 0;
    while (pending.length) {
      const repository = pending.shift().replace(/\/$/, "");
      if (visited.has(repository)) continue;
      visited.add(repository);
      let resource;
      const metadataUri = joinUri(repository, "ilimodels.xml");
      try {
        resource = await this.#resource(metadataUri, this.metadataTtlMs);
      } catch (error) {
        const message = error instanceof Error ? error.message : String(error);
        failures.push(new Error(`${metadataUri}: ${message}`));
        this.onWarning?.({ uri: metadataUri, operation: "metadata", message });
        continue;
      }
      loaded++;
      result.push(...parseIliModelsXml(textDecoder.decode(resource.value), repository));
      if (this.followSiteLinks) {
        try {
          const site = await this.#resource(joinUri(repository, "ilisite.xml"), this.metadataTtlMs);
          pending.push(...parseSiteLinks(textDecoder.decode(site.value)));
        } catch { /* ilisite.xml is optional */ }
      }
    }
    if (this.repositories.length > 0 && loaded === 0)
      throw new AggregateError(failures, "no configured INTERLIS repository is available");
    return result;
  }

  async resolveWorkspace(requestedModels, schemaLanguage = "") {
    const catalog = await this.listModels();
    const models = [];
    const resolved = new Set();
    const visiting = new Set();
    const files = new Set();
    const resolveOne = async name => {
      if (name === "INTERLIS" || resolved.has(name)) return;
      if (visiting.has(name)) throw new Error(`dependency cycle involving model ${name}`);
      visiting.add(name);
      const metadata = selectModel(catalog, name, schemaLanguage);
      if (!metadata) throw new Error(`model ${name} not found in configured repositories`);
      for (const dependency of metadata.dependencies) await resolveOne(dependency);
      if (metadata.file.split(/[\\/]/).includes("..")) throw new Error(`unsafe repository path ${metadata.file}`);
      const uri = joinUri(metadata.repository, metadata.file);
      if (!files.has(uri)) {
        let resource;
        try {
          resource = await this.#resource(uri, this.modelTtlMs);
        } catch (error) {
          const message = error instanceof Error ? error.message : String(error);
          this.onWarning?.({ uri, operation: "model", message });
          throw error;
        }
        if (metadata.md5 && md5(resource.value) !== metadata.md5.toLowerCase()) {
          throw new Error(`MD5 mismatch for ${uri}`);
        }
        models.push({ metadata, uri, source: textDecoder.decode(resource.value), fromCache: resource.fromCache });
        files.add(uri);
      }
      visiting.delete(name);
      resolved.add(name);
    };
    for (const model of requestedModels) await resolveOne(model);
    return { models };
  }

  resolveModel(name, schemaLanguage = "") {
    return this.resolveWorkspace([name], schemaLanguage);
  }
}
