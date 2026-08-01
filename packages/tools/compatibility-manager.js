import {
  Md5RepositoryChecksum,
  RepositoryError,
  RepositoryManagerCore,
  SafeRepositoryMetadataDecoder,
  normalizeRepositoryPolicy
} from "@ilic/repository-core";

const textEncoder = new TextEncoder();

export class SystemRepositoryClock {
  now() { return Date.now(); }
}

export class MemoryCache {
  #values = new Map();
  #clock;
  #clone;
  #faultInjection;

  constructor({ clock = new SystemRepositoryClock(), cloneBytes = true, faultInjection } = {}) {
    this.#clock = clock;
    this.#clone = cloneBytes;
    this.#faultInjection = faultInjection;
  }

  #fault(operation, key) {
    const fault = this.#faultInjection?.[operation];
    if (typeof fault === "function") fault(key);
    else if (fault instanceof Error) throw fault;
  }

  async get(key) {
    this.#fault("get", key);
    const entry = this.#values.get(key);
    if (!entry) return undefined;
    return { value: this.#clone ? new Uint8Array(entry.value) : entry.value, storedAt: entry.storedAt };
  }

  async putEntry(key, entry) {
    this.#fault("put", key);
    this.#values.set(key, {
      value: this.#clone ? new Uint8Array(entry.value) : entry.value,
      storedAt: entry.storedAt
    });
  }

  async put(key, value) {
    await this.putEntry(key, { value, storedAt: this.#clock.now() });
  }

  async delete(key) {
    this.#fault("delete", key);
    this.#values.delete(key);
  }

  async clear() {
    this.#fault("clear");
    this.#values.clear();
  }
}

async function readResponseBytes(response, maxBytes) {
  if (response.body?.getReader) {
    const reader = response.body.getReader();
    const chunks = [];
    let length = 0;
    try {
      while (true) {
        const part = await reader.read();
        if (part.done) break;
        length += part.value.byteLength;
        if (length > maxBytes) {
          await reader.cancel();
          throw new Error(`response exceeds ${maxBytes} byte limit`);
        }
        chunks.push(part.value);
      }
    } finally { reader.releaseLock?.(); }
    const result = new Uint8Array(length);
    let offset = 0;
    for (const chunk of chunks) { result.set(chunk, offset); offset += chunk.byteLength; }
    return result;
  }
  const bytes = new Uint8Array(await response.arrayBuffer());
  if (bytes.byteLength > maxBytes) throw new Error(`response exceeds ${maxBytes} byte limit`);
  return bytes;
}

export class FetchRepositoryTransport {
  #fetch;
  constructor({ fetch: fetchFunction } = {}) {
    const defaultFetch = fetchFunction === undefined ? globalThis.fetch : fetchFunction;
    this.#fetch = fetchFunction === undefined && typeof defaultFetch === "function"
      ? defaultFetch.bind(globalThis)
      : defaultFetch;
  }

  async get(request) {
    if (typeof this.#fetch !== "function") return {
      success: false, statusCode: 0, body: new Uint8Array(), finalUri: request.uri,
      error: "No repository transport is available. Provide `load`, `transport`, or a platform factory.",
      retryable: false
    };
    try {
      let currentUri = request.uri;
      let response;
      for (let redirect = 0; ; redirect += 1) {
        response = await this.#fetch(currentUri, { signal: request.signal, redirect: "manual" });
        const statusCode = response.status ?? (response.ok ? 200 : 0);
        if (statusCode < 300 || statusCode >= 400) break;
        if (redirect >= request.maxRedirects) return {
          success: false, statusCode, body: new Uint8Array(), finalUri: currentUri,
          error: `repository redirect limit (${request.maxRedirects}) exceeded`, retryable: false
        };
        const location = response.headers?.get?.("location");
        if (!location) return {
          success: false, statusCode, body: new Uint8Array(), finalUri: currentUri,
          error: "repository redirect has no Location header", retryable: false
        };
        try { currentUri = new URL(location, currentUri).href; }
        catch (error) { return {
          success: false, statusCode, body: new Uint8Array(), finalUri: currentUri,
          error: `invalid repository redirect: ${error.message}`, retryable: false
        }; }
      }
      const statusCode = response.status ?? (response.ok ? 200 : 0);
      if (!response.ok) return {
        success: false, statusCode, body: new Uint8Array(),
        finalUri: response.url || currentUri,
        error: `${statusCode} ${response.statusText || "HTTP error"}`,
        notFound: statusCode === 404, retryable: statusCode >= 500 || statusCode === 429
      };
      const body = await readResponseBytes(response, request.maxBytes);
      return { success: true, statusCode, body,
        finalUri: response.url || currentUri, error: "", notFound: false, retryable: false };
    } catch (error) {
      if (request.signal?.aborted || error?.name === "AbortError") return {
        success: false, statusCode: 0, body: new Uint8Array(), finalUri: request.uri,
        error: "repository request was cancelled", errorCode: "ILIC-REPO-CANCELLED", retryable: false
      };
      return { success: false, statusCode: 0, body: new Uint8Array(), finalUri: request.uri,
        error: error?.message ?? String(error), retryable: true };
    }
  }
}

export class LegacyLoadTransport {
  #load;
  constructor(load) { this.#load = load; }
  async get(request) {
    try {
      if (request.signal?.aborted) return { success: false, error: "repository request was cancelled", errorCode: "ILIC-REPO-CANCELLED" };
      const value = await this.#load(request.uri);
      const body = typeof value === "string" ? textEncoder.encode(value) : new Uint8Array(value);
      if (body.byteLength > request.maxBytes) return {
        success: false, statusCode: 0, body: new Uint8Array(), finalUri: request.uri,
        error: `response exceeds ${request.maxBytes} byte limit`, retryable: false
      };
      return { success: true, statusCode: 200, body, finalUri: request.uri, error: "", retryable: false };
    } catch (error) {
      return { success: false, statusCode: error?.status ?? 0, body: new Uint8Array(), finalUri: request.uri,
        error: error?.message ?? String(error), notFound: error?.status === 404, retryable: error?.retryable ?? true };
    }
  }
}

export function createCompatibilityDependencies(options = {}) {
  const clock = options.clock ?? new SystemRepositoryClock();
  const cache = options.cache ?? new MemoryCache({ clock });
  const warning = options.onWarning;
  const observer = diagnostic => {
    warning?.({
      severity: diagnostic.severity, code: diagnostic.code, uri: diagnostic.uri ?? "",
      operation: diagnostic.operation ?? "cache", message: diagnostic.message
    });
    options.observer?.(diagnostic);
  };
  let transport = options.transport;
  if (transport && options.load) observer({ severity: "warning", code: "ILIC-REPO-CONFIG",
    operation: "config", message: "both transport and load were provided; transport takes precedence" });
  if (!transport) transport = options.load ? new LegacyLoadTransport(options.load)
    : new FetchRepositoryTransport({ fetch: options.fetch });
  return { clock, cache, transport, observer };
}

export class RepositoryManager {
  #core;

  constructor(options = {}) {
    const dependencies = createCompatibilityDependencies(options);
    const policy = normalizeRepositoryPolicy({
      offline: options.offline, metadataTtlMs: options.metadataTtlMs,
      modelTtlMs: options.modelTtlMs, allowStaleOnError: options.allowStaleOnError,
      followSiteLinks: options.followSiteLinks, validateChecksums: options.validateChecksums,
      ...(options.limits ?? {})
    });
    this.repositories = [...(options.repositories ?? [])];
    this.cache = dependencies.cache;
    this.load = options.load ?? (async uri => {
      const response = await dependencies.transport.get({
        uri, kind: "model", maxBytes: policy.maxModelBytes,
        maxRedirects: policy.maxRedirects, signal: options.signal
      });
      if (!response.success) throw new Error(response.error ?? "repository request failed");
      return response.body;
    });
    this.offline = policy.offline;
    this.metadataTtlMs = policy.metadataTtlMs;
    this.modelTtlMs = policy.modelTtlMs;
    this.allowStaleOnError = policy.allowStaleOnError;
    this.followSiteLinks = policy.followSiteLinks;
    this.validateChecksums = policy.validateChecksums;
    this.onWarning = options.onWarning;
    this.#core = new RepositoryManagerCore({
      repositories: this.repositories,
      transport: dependencies.transport,
      cache: dependencies.cache,
      clock: dependencies.clock,
      metadataDecoder: options.metadataDecoder ?? new SafeRepositoryMetadataDecoder({ maxBytes: policy.maxMetadataBytes }),
      checksum: options.checksum ?? new Md5RepositoryChecksum(),
      policy,
      observer: dependencies.observer,
      signal: options.signal
    });
  }

  listModels() { return this.#core.listModels(); }
  resolveWorkspace(models, schemaLanguage = "", signal) { return this.#core.resolveWorkspace(models, schemaLanguage, signal); }
  resolveModel(model, schemaLanguage = "", signal) { return this.#core.resolveModel(model, schemaLanguage, signal); }
}

export { RepositoryError };
