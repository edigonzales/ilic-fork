import {
  normalizeRepositoryUri,
  resolveRepositoryUri,
  validateRepositoryRelativePath
} from "./repository-uri.js";
import { selectLatestModelVersion } from "./repository-version.js";

const textDecoder = new TextDecoder();
const textEncoder = new TextEncoder();

function decodeXml(value = "") {
  return value.replaceAll("&amp;", "&").replaceAll("&lt;", "<")
    .replaceAll("&gt;", ">").replaceAll("&quot;", '"').replaceAll("&apos;", "'").trim();
}

function tag(body, name) {
  const pattern = new RegExp(`<(?:(?:[\\w.-]+):)?${name}(?:\\s[^>]*)?>([\\s\\S]*?)<\\/(?:(?:[\\w.-]+):)?${name}>`);
  return decodeXml(body.match(pattern)?.[1]);
}

function values(body) {
  return [...body.matchAll(/<(?:[\w.-]+:)?value(?:\s[^>]*)?>([\s\S]*?)<\/(?:[\w.-]+:)?value>/g)]
    .map(value => decodeXml(value[1])).filter(Boolean);
}

export function parseIliModelsXml(xml, repository) {
  const models = [];
  const pattern = /<([\w:.-]*ModelMetadata)(?:\s[^>]*)?>([\s\S]*?)<\/\1>/g;
  for (const match of xml.matchAll(pattern)) {
    const body = match[2];
    const name = tag(body, "Name");
    const schemaLanguage = tag(body, "SchemaLanguage");
    const file = tag(body, "File");
    if (!name || !schemaLanguage || !file) continue;
    const dependency = body.match(/<(?:[\w.-]+:)?dependsOnModel(?:\s[^>]*)?>([\s\S]*?)<\/(?:[\w.-]+:)?dependsOnModel>/)?.[1] ?? "";
    models.push({
      name, schemaLanguage, file, repository, dependencies: values(dependency),
      version: tag(body, "Version"), publishingDate: tag(body, "publishingDate"),
      precursorVersion: tag(body, "precursorVersion"), md5: tag(body, "md5"),
      browseOnly: ["true", "1"].includes(tag(body, "browseOnly").toLowerCase())
    });
  }
  return models;
}

export function parseIliSiteXml(xml) {
  const site = { parentSites: [], subsidiarySites: [] };
  const pattern = /<(?:[\w.-]+:)?(parentSite|subsidiarySite)(?:\s[^>]*)?>([\s\S]*?)<\/(?:[\w.-]+:)?\1>/g;
  for (const section of xml.matchAll(pattern)) {
    site[section[1] === "parentSite" ? "parentSites" : "subsidiarySites"].push(...values(section[2]));
  }
  return site;
}

export class MemoryCache {
  #values = new Map();
  async get(key) { return this.#values.get(key); }
  async put(key, value) { this.#values.set(key, { value, storedAt: Date.now() }); }
  async delete(key) { this.#values.delete(key); }
  async clear() { this.#values.clear(); }
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
  padded.set(bytes); padded[bytes.length] = 0x80;
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
  #indexes = new Map();
  #sites = new Map();
  #indexFailures = [];

  constructor({ repositories = [], cache = new MemoryCache(), load, offline = false,
    metadataTtlMs = 86_400_000, modelTtlMs = 604_800_000, allowStaleOnError = true,
    followSiteLinks = true, validateChecksums = true, onWarning } = {}) {
    const seen = new Set();
    this.repositories = [];
    for (const value of repositories) {
      const normalized = normalizeRepositoryUri(value);
      if (!seen.has(normalized)) { seen.add(normalized); this.repositories.push(normalized); }
    }
    this.cache = cache; this.offline = offline; this.metadataTtlMs = metadataTtlMs;
    this.modelTtlMs = modelTtlMs; this.allowStaleOnError = allowStaleOnError;
    this.followSiteLinks = followSiteLinks; this.validateChecksums = validateChecksums;
    this.onWarning = onWarning;
    this.load = load ?? (async uri => {
      const response = await fetch(uri);
      if (!response.ok) throw new Error(`${response.status} ${response.statusText}`);
      return new Uint8Array(await response.arrayBuffer());
    });
  }

  #warn(uri, operation, message) { this.onWarning?.({ uri, operation, message }); }

  async #download(uri) {
    const value = await this.load(uri);
    const bytes = typeof value === "string" ? textEncoder.encode(value) : value;
    try { await this.cache.put(uri, bytes); }
    catch (error) { this.#warn(uri, "cache", error instanceof Error ? error.message : String(error)); }
    return { value: bytes, fromCache: false, stale: false };
  }

  async #resource(uri, ttl) {
    const cached = await this.cache.get(uri);
    const fresh = cached && Date.now() - cached.storedAt <= ttl;
    if (fresh || (this.offline && cached))
      return { value: cached.value, fromCache: true, stale: !fresh };
    if (this.offline) throw new Error(`offline and no cached copy of ${uri}`);
    try { return await this.#download(uri); }
    catch (error) {
      if (cached && this.allowStaleOnError) {
        this.#warn(uri, "cache", `using stale cache entry after load error: ${error instanceof Error ? error.message : String(error)}`);
        return { value: cached.value, fromCache: true, stale: true };
      }
      throw error;
    }
  }

  async #loadRepositoryIndex(repository) {
    if (this.#indexes.has(repository)) return this.#indexes.get(repository);
    const uri = resolveRepositoryUri(repository, "ilimodels.xml");
    try {
      const resource = await this.#resource(uri, this.metadataTtlMs);
      const models = parseIliModelsXml(textDecoder.decode(resource.value), repository);
      if (!models.length) throw new Error("no valid ModelMetadata entries");
      const index = { repository, models };
      this.#indexes.set(repository, index);
      return index;
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error);
      this.#indexes.set(repository, undefined);
      const failure = new Error(`${uri}: ${message}`);
      this.#indexFailures.push(failure);
      this.#warn(uri, "metadata", message);
      return undefined;
    }
  }

  async #loadRepositorySite(repository) {
    if (!this.followSiteLinks) return undefined;
    if (this.#sites.has(repository)) return this.#sites.get(repository);
    const uri = resolveRepositoryUri(repository, "ilisite.xml");
    try {
      const resource = await this.#resource(uri, this.metadataTtlMs);
      const site = parseIliSiteXml(textDecoder.decode(resource.value));
      this.#sites.set(repository, site);
      return site;
    } catch (error) {
      this.#sites.set(repository, undefined); // optional, including 404
      return undefined;
    }
  }

  #enqueue(repository, links, queue) {
    for (const link of links) queue.push(resolveRepositoryUri(repository, link));
  }

  async #findModel(name, schemaLanguage) {
    const visited = new Set();
    const visit = async repository => {
      if (visited.has(repository)) return undefined;
      visited.add(repository);
      const index = await this.#loadRepositoryIndex(repository);
      if (!index) return undefined;
      return selectLatestModelVersion(index.models, name, schemaLanguage,
        message => this.#warn(repository, "version", message));
    };
    for (const seed of this.repositories) {
      const model = await visit(seed);
      if (model) return model;
    }
    const parents = [], subsidiaries = [];
    for (const seed of this.repositories) {
      const site = await this.#loadRepositorySite(seed);
      if (site) {
        this.#enqueue(seed, site.parentSites, parents);
        this.#enqueue(seed, site.subsidiarySites, subsidiaries);
      }
    }
    const drainParents = async () => {
      while (parents.length) {
        const repository = parents.shift();
        if (visited.has(repository)) continue;
        const model = await visit(repository);
        if (model) return model;
        const site = await this.#loadRepositorySite(repository);
        if (site) this.#enqueue(repository, site.parentSites, parents);
      }
      return undefined;
    };
    let found = await drainParents();
    if (found) return found;
    while (subsidiaries.length) {
      const repository = subsidiaries.shift();
      if (visited.has(repository)) continue;
      found = await visit(repository);
      if (found) return found;
      const site = await this.#loadRepositorySite(repository);
      if (site) {
        this.#enqueue(repository, site.subsidiarySites, subsidiaries);
        this.#enqueue(repository, site.parentSites, parents);
        found = await drainParents();
        if (found) return found;
      }
    }
    return undefined;
  }

  async listModels() {
    const models = [], visited = new Set(), parents = [], subsidiaries = [];
    const visit = async repository => {
      if (visited.has(repository)) return false;
      visited.add(repository);
      const index = await this.#loadRepositoryIndex(repository);
      if (index) models.push(...index.models);
      return true;
    };
    for (const seed of this.repositories) await visit(seed);
    for (const seed of this.repositories) {
      const site = await this.#loadRepositorySite(seed);
      if (site) {
        this.#enqueue(seed, site.parentSites, parents);
        this.#enqueue(seed, site.subsidiarySites, subsidiaries);
      }
    }
    const drainParents = async () => {
      while (parents.length) {
        const repository = parents.shift();
        if (!await visit(repository)) continue;
        const site = await this.#loadRepositorySite(repository);
        if (site) this.#enqueue(repository, site.parentSites, parents);
      }
    };
    await drainParents();
    while (subsidiaries.length) {
      const repository = subsidiaries.shift();
      if (!await visit(repository)) continue;
      const site = await this.#loadRepositorySite(repository);
      if (site) {
        this.#enqueue(repository, site.subsidiarySites, subsidiaries);
        this.#enqueue(repository, site.parentSites, parents);
        await drainParents();
      }
    }
    if (this.repositories.length && !models.length)
      throw new AggregateError(this.#indexFailures, "no configured INTERLIS repository is available");
    return models;
  }

  async resolveWorkspace(requestedModels, schemaLanguage = "") {
    const models = [], resolved = new Set(), stack = [], files = new Map();
    const resolveOne = async name => {
      if (name === "INTERLIS" || resolved.has(name)) return;
      const cycleAt = stack.indexOf(name);
      if (cycleAt >= 0) throw new Error(`dependency cycle: ${[...stack.slice(cycleAt), name].join(" -> ")}`);
      stack.push(name);
      try {
        const metadata = await this.#findModel(name, schemaLanguage);
        if (!metadata) throw new Error(`model ${name} not found in configured repositories`);
        for (const dependency of metadata.dependencies) await resolveOne(dependency);
        const path = validateRepositoryRelativePath(metadata.file);
        if (!path.valid) throw new Error(`unsafe repository path ${metadata.file}: ${path.error}`);
        const uri = resolveRepositoryUri(metadata.repository, path.normalized);
        if (!files.has(uri)) {
          let resource;
          try { resource = await this.#resource(uri, this.modelTtlMs); }
          catch (error) {
            this.#warn(uri, "model", error instanceof Error ? error.message : String(error));
            throw error;
          }
          if (this.validateChecksums && metadata.md5) {
            let actual = md5(resource.value);
            if (actual.toLowerCase() !== metadata.md5.toLowerCase()
                && resource.fromCache && !this.offline) {
              try { await this.cache.delete?.(uri); }
              catch (error) {
                this.#warn(uri, "cache", error instanceof Error ? error.message : String(error));
              }
              resource = await this.#download(uri);
              actual = md5(resource.value);
            }
            if (actual.toLowerCase() !== metadata.md5.toLowerCase())
              throw new Error(`MD5 mismatch for ${uri}: expected ${metadata.md5}, actual ${actual}`);
          }
          models.push({ metadata, uri, source: textDecoder.decode(resource.value),
            fromCache: resource.fromCache, stale: resource.stale });
          files.set(uri, md5(resource.value));
        }
        else if (this.validateChecksums && metadata.md5
            && files.get(uri).toLowerCase() !== metadata.md5.toLowerCase())
          throw new Error(`MD5 mismatch for ${uri}: expected ${metadata.md5}, actual ${files.get(uri)}`);
        resolved.add(name);
      } finally { stack.pop(); }
    };
    for (const model of requestedModels) await resolveOne(model);
    return { models };
  }

  resolveModel(name, schemaLanguage = "") { return this.resolveWorkspace([name], schemaLanguage); }
}
