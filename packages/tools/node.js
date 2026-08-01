import { createHash, randomUUID } from "node:crypto";
import { mkdir, readFile, rename, rm, writeFile } from "node:fs/promises";
import { basename, dirname, isAbsolute, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { FetchRepositoryTransport, RepositoryManager, SystemRepositoryClock } from "./compatibility-manager.js";

async function publishMetadata(temporary, target) {
  let lastError;
  for (let attempt = 0; attempt < 64; attempt += 1) {
    try { await rename(temporary, target); return; }
    catch (error) {
      lastError = error;
      if (!["EEXIST", "EPERM", "EACCES"].includes(error?.code)) throw error;
      await rm(target, { force: true });
    }
  }
  throw lastError;
}
function hashKey(key) { return createHash("md5").update(key).digest("hex"); }

export class NodeFileCache {
  constructor(directory, { clock = new SystemRepositoryClock() } = {}) {
    this.directory = directory;
    this.clock = clock;
  }

  #path(key) { return join(this.directory, hashKey(key)); }

  async get(key) {
    try {
      const path = this.#path(key);
      const parsed = JSON.parse(await readFile(`${path}.json`, "utf8"));
      if (parsed.file && basename(parsed.file) !== parsed.file) return undefined;
      const value = await readFile(parsed.file ? join(this.directory, parsed.file) : path);
      if (parsed.size !== value.byteLength || parsed.md5 !== hashKey(value)) return undefined;
      if (!Number.isFinite(parsed.storedAt)) return undefined;
      return { value: new Uint8Array(value), storedAt: parsed.storedAt };
    } catch { return undefined; }
  }

  async putEntry(key, entry) {
    await mkdir(this.directory, { recursive: true });
    const path = this.#path(key);
    const generation = `${path}.${process.pid}.${randomUUID()}.data`;
    const contentTemporary = `${generation}.tmp`;
    const metadataPath = `${path}.json`;
    const metadataTemporary = `${metadataPath}.${process.pid}.${randomUUID()}.tmp`;
    const metadata = JSON.stringify({
      storedAt: entry.storedAt, size: entry.value.byteLength,
      md5: hashKey(entry.value), file: basename(generation)
    });
    let publishedContent = false;
    let publishedMetadata = false;
    try {
      await writeFile(contentTemporary, entry.value);
      await writeFile(metadataTemporary, metadata);
      await rename(contentTemporary, generation);
      publishedContent = true;
      await publishMetadata(metadataTemporary, metadataPath);
      publishedMetadata = true;
      await rm(path, { force: true });
    } finally {
      await Promise.all([
        rm(contentTemporary, { force: true }), rm(metadataTemporary, { force: true }),
        ...(publishedContent && !publishedMetadata ? [rm(generation, { force: true })] : [])
      ]);
    }
  }

  async put(key, value) { return this.putEntry(key, { value, storedAt: this.clock.now() }); }

  async delete(key) {
    const path = this.#path(key);
    let generation;
    try {
      const parsed = JSON.parse(await readFile(`${path}.json`, "utf8"));
      if (parsed.file && basename(parsed.file) === parsed.file) generation = join(this.directory, parsed.file);
    } catch {}
    await Promise.all([
      rm(path, { force: true }), rm(`${path}.json`, { force: true }),
      ...(generation ? [rm(generation, { force: true })] : [])
    ]);
  }

  async clear() { await rm(this.directory, { recursive: true, force: true }); }
}

function localPath(uri) {
  if (uri.startsWith("file://")) return fileURLToPath(uri);
  if (!/^[A-Za-z][A-Za-z0-9+.-]*:/.test(uri)) return uri;
  return undefined;
}

function withinRoots(path, roots) {
  if (!roots.length) return true;
  const target = resolve(path);
  return roots.some(root => {
    const base = resolve(root);
    return target === base || target.startsWith(`${base}${process.platform === "win32" ? "\\" : "/"}`);
  });
}

export class NodeRepositoryTransport {
  #fetchTransport;
  #fileRoots;
  constructor({ fetch, fileRoots = [] } = {}) {
    this.#fetchTransport = new FetchRepositoryTransport({ fetch });
    this.#fileRoots = fileRoots.map(value => resolve(value));
  }

  async get(request) {
    const path = localPath(request.uri);
    if (path === undefined) return this.#fetchTransport.get(request);
    if (!isAbsolute(path) || !withinRoots(path, this.#fileRoots)) return {
      success: false, statusCode: 0, body: new Uint8Array(), finalUri: request.uri,
      error: "local repository path is outside configured roots", retryable: false
    };
    try {
      const body = new Uint8Array(await readFile(path));
      if (body.byteLength > request.maxBytes) throw new Error(`response exceeds ${request.maxBytes} byte limit`);
      return { success: true, statusCode: 200, body, finalUri: request.uri, error: "", retryable: false };
    } catch (error) {
      return { success: false, statusCode: error?.code === "ENOENT" ? 404 : 0,
        body: new Uint8Array(), finalUri: request.uri, error: error?.message ?? String(error),
        notFound: error?.code === "ENOENT", retryable: false };
    }
  }
}

export function createNodeRepositoryManager(options = {}) {
  const clock = options.clock ?? new SystemRepositoryClock();
  const cache = options.cache ?? (options.cacheDirectory ? new NodeFileCache(options.cacheDirectory, { clock }) : undefined);
  const transport = options.transport ?? new NodeRepositoryTransport({ fetch: options.fetch });
  return new RepositoryManager({ ...options, clock, ...(cache ? { cache } : {}), transport });
}
