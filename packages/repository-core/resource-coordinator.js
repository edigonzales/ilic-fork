import { normalizeRepositoryUri } from "./repository-uri.js";
import { makeDiagnostic, RepositoryError } from "./diagnostics.js";

export const DEFAULT_REPOSITORY_POLICY = Object.freeze({
  metadataTtlMs: 86_400_000,
  modelTtlMs: 604_800_000,
  offline: false,
  allowStaleOnError: true,
  followSiteLinks: true,
  validateChecksums: true,
  maxMetadataBytes: 32 * 1024 * 1024,
  maxModelBytes: 64 * 1024 * 1024,
  maxRepositoriesVisited: 4096,
  maxDependencyDepth: 1024,
  maxModelsResolved: 10_000,
  maxRedirects: 10
});

function positiveInteger(value, fallback) {
  return Number.isSafeInteger(value) && value > 0 ? value : fallback;
}

export function normalizeRepositoryPolicy(options = {}) {
  const values = { ...DEFAULT_REPOSITORY_POLICY, ...options };
  const policy = {
    metadataTtlMs: Number.isFinite(values.metadataTtlMs) && values.metadataTtlMs >= 0
      ? Math.min(values.metadataTtlMs, Number.MAX_SAFE_INTEGER) : DEFAULT_REPOSITORY_POLICY.metadataTtlMs,
    modelTtlMs: Number.isFinite(values.modelTtlMs) && values.modelTtlMs >= 0
      ? Math.min(values.modelTtlMs, Number.MAX_SAFE_INTEGER) : DEFAULT_REPOSITORY_POLICY.modelTtlMs,
    offline: Boolean(values.offline),
    allowStaleOnError: values.allowStaleOnError !== false,
    followSiteLinks: values.followSiteLinks !== false,
    validateChecksums: values.validateChecksums !== false,
    maxMetadataBytes: positiveInteger(values.maxMetadataBytes, DEFAULT_REPOSITORY_POLICY.maxMetadataBytes),
    maxModelBytes: positiveInteger(values.maxModelBytes, DEFAULT_REPOSITORY_POLICY.maxModelBytes),
    maxRepositoriesVisited: positiveInteger(values.maxRepositoriesVisited, DEFAULT_REPOSITORY_POLICY.maxRepositoriesVisited),
    maxDependencyDepth: positiveInteger(values.maxDependencyDepth, DEFAULT_REPOSITORY_POLICY.maxDependencyDepth),
    maxModelsResolved: positiveInteger(values.maxModelsResolved, DEFAULT_REPOSITORY_POLICY.maxModelsResolved),
    maxRedirects: positiveInteger(values.maxRedirects, DEFAULT_REPOSITORY_POLICY.maxRedirects)
  };
  return policy;
}

export function classifyCacheEntry(entry, now, ttl) {
  if (entry === undefined) return "missing";
  if (!Number.isFinite(entry.storedAt) || entry.storedAt > now) return "stale";
  if (ttl <= 0) return "stale";
  const expiresAt = entry.storedAt > Number.MAX_SAFE_INTEGER - ttl
    ? Number.MAX_SAFE_INTEGER : entry.storedAt + ttl;
  return now < expiresAt ? "fresh" : "stale";
}

function transient(response) {
  return response?.retryable === true || response?.statusCode === 408
    || response?.statusCode === 429 || response?.statusCode >= 500;
}

function asBytes(value) {
  if (value instanceof Uint8Array) return new Uint8Array(value);
  return new Uint8Array(value ?? []);
}

export class RepositoryResourceCoordinator {
  constructor({ policy, transport, cache, clock, checksum, observer } = {}) {
    if (!transport || !cache || !clock || !checksum)
      throw new TypeError("RepositoryResourceCoordinator requires transport, cache, clock and checksum ports");
    this.policy = normalizeRepositoryPolicy(policy);
    this.transport = transport;
    this.cache = cache;
    this.clock = clock;
    this.checksum = checksum;
    this.observer = observer;
  }

  #record(diagnostics, diagnostic) {
    diagnostics.push(diagnostic);
    this.observer?.(diagnostic);
  }

  notify(diagnostic) { this.observer?.(diagnostic); }

  async #cachePut(uri, bytes, diagnostics) {
    try {
      if (typeof this.cache.putEntry === "function") {
        await this.cache.putEntry(uri, { value: new Uint8Array(bytes), storedAt: this.clock.now() });
      } else await this.cache.put(uri, new Uint8Array(bytes));
    } catch (error) {
      this.#record(diagnostics, makeDiagnostic("ILIC-REPO-CACHE", error?.message ?? String(error), {
        severity: "warning", uri, operation: "cache"
      }));
    }
  }

  async #cacheDelete(uri, diagnostics) {
    try {
      if (typeof this.cache.delete === "function") await this.cache.delete(uri);
    } catch (error) {
      this.#record(diagnostics, makeDiagnostic("ILIC-REPO-CACHE", error?.message ?? String(error), {
        severity: "warning", uri, operation: "cache"
      }));
    }
  }

  async #getCache(uri, diagnostics) {
    try {
      const entry = await this.cache.get(uri);
      if (entry === undefined) return undefined;
      if (!(entry.value instanceof Uint8Array) || !Number.isFinite(entry.storedAt)) {
        this.#record(diagnostics, makeDiagnostic("ILIC-REPO-CACHE", "invalid cache entry; treating it as a miss", {
          severity: "warning", uri, operation: "cache"
        }));
        return undefined;
      }
      return { value: new Uint8Array(entry.value), storedAt: entry.storedAt };
    } catch (error) {
      this.#record(diagnostics, makeDiagnostic("ILIC-REPO-CACHE", error?.message ?? String(error), {
        severity: "warning", uri, operation: "cache"
      }));
      return undefined;
    }
  }

  async #checksumMatches(bytes, expected) {
    if (!expected || !this.policy.validateChecksums) return { match: true, actual: "" };
    const actual = String(await this.checksum.md5(bytes)).toLowerCase();
    return { match: actual === expected.toLowerCase(), actual };
  }

  async load({ uri, kind = "model", ttl, optional = false, expectedMd5 = "", signal } = {}) {
    const diagnostics = [];
    let normalized;
    try { normalized = normalizeRepositoryUri(uri); }
    catch (error) {
      const diagnostic = makeDiagnostic("ILIC-REPO-URI", error.message, { uri, operation: "transport" });
      return { success: false, uri, diagnostics: [diagnostic], error: error.message };
    }
    if (signal?.aborted) {
      const diagnostic = makeDiagnostic("ILIC-REPO-CANCELLED", "repository request was cancelled", {
        uri: normalized, operation: "transport"
      });
      return { success: false, uri: normalized, diagnostics: [diagnostic], error: diagnostic.message, cancelled: true };
    }
    const limit = kind === "model" ? this.policy.maxModelBytes : this.policy.maxMetadataBytes;
    const cacheEntry = await this.#getCache(normalized, diagnostics);
    let cacheState = classifyCacheEntry(cacheEntry, this.clock.now(), ttl ?? this.policy.modelTtlMs);
    let cacheMismatch = false;
    if (cacheEntry !== undefined && (cacheState === "fresh" || this.policy.offline)) {
      const check = await this.#checksumMatches(cacheEntry.value, expectedMd5);
      if (check.match) return {
        success: true, uri: normalized, content: cacheEntry.value, fromCache: true,
        stale: cacheState !== "fresh", localPath: undefined, diagnostics
      };
      cacheMismatch = true;
      await this.#cacheDelete(normalized, diagnostics);
      cacheState = "missing";
      if (this.policy.offline) {
        const diagnostic = makeDiagnostic("ILIC-REPO-CHECKSUM",
          `MD5 mismatch for ${normalized}: expected ${expectedMd5}, actual ${check.actual}`, {
            uri: normalized, operation: "checksum"
          });
        return { success: false, uri: normalized, diagnostics: [...diagnostics, diagnostic], error: diagnostic.message };
      }
    } else if (cacheEntry !== undefined && this.policy.offline) {
      return { success: true, uri: normalized, content: cacheEntry.value, fromCache: true,
        stale: true, localPath: undefined, diagnostics };
    }
    if (this.policy.offline) {
      if (optional) return { success: false, unavailable: true, uri: normalized, diagnostics };
      const diagnostic = makeDiagnostic("ILIC-REPO-OFFLINE", `offline and no cached copy of ${normalized}`, {
        uri: normalized, operation: kind === "model" ? "model" : "metadata"
      });
      return { success: false, uri: normalized, diagnostics, error: diagnostic.message };
    }
    const request = {
      uri: normalized,
      kind: kind === "model" ? "model" : kind === "site-index" ? "site-index" : "model-index",
      maxBytes: limit,
      maxRedirects: this.policy.maxRedirects,
      signal
    };
    let response;
    try { response = await this.transport.get(request); }
    catch (error) { response = { success: false, error: error?.message ?? String(error), retryable: true }; }
    if (signal?.aborted || response?.cancelled || response?.errorCode === "ILIC-REPO-CANCELLED") {
      const diagnostic = makeDiagnostic("ILIC-REPO-CANCELLED", "repository request was cancelled", {
        uri: normalized, operation: "transport"
      });
      return { success: false, uri: normalized, diagnostics: [...diagnostics, diagnostic], error: diagnostic.message, cancelled: true };
    }
    if (!response?.success) {
      if (optional && (response?.notFound || response?.statusCode === 404))
        return { success: false, unavailable: true, uri: normalized, diagnostics };
      if (!cacheMismatch && cacheEntry && this.policy.allowStaleOnError && transient(response)) {
        const warning = makeDiagnostic("ILIC-REPO-DOWNLOAD", `using stale cache entry after load error: ${response.error ?? "request failed"}`, {
          severity: "warning", uri: normalized, operation: "transport"
        });
        this.#record(diagnostics, warning);
        return { success: true, uri: normalized, content: cacheEntry.value, fromCache: true,
          stale: true, diagnostics };
      }
      const code = response?.statusCode === 404
        ? (kind === "site-index" ? "ILIC-REPO-SITE" : "ILIC-REPO-NOT-FOUND") : "ILIC-REPO-DOWNLOAD";
      const operation = kind === "model" ? "model" : kind === "site-index" ? "site" : "metadata";
      const diagnostic = makeDiagnostic(code, response?.error ?? "repository request failed", {
        uri: normalized, operation
      });
      this.#record(diagnostics, diagnostic);
      return { success: false, uri: normalized, diagnostics, error: diagnostic.message };
    }
    const bytes = asBytes(response.body);
    if (bytes.byteLength > limit) {
      const diagnostic = makeDiagnostic("ILIC-REPO-LIMIT", `resource exceeds ${limit} byte limit`, {
        uri: normalized, operation: "transport"
      });
      this.#record(diagnostics, diagnostic);
      return { success: false, uri: normalized, diagnostics, error: diagnostic.message };
    }
    const check = await this.#checksumMatches(bytes, expectedMd5);
    if (!check.match) {
      const diagnostic = makeDiagnostic("ILIC-REPO-CHECKSUM",
        `MD5 mismatch for ${normalized}: expected ${expectedMd5}, actual ${check.actual}`, {
          uri: normalized, operation: "checksum"
        });
      this.#record(diagnostics, diagnostic);
      return { success: false, uri: normalized, diagnostics: [...diagnostics, diagnostic], error: diagnostic.message };
    }
    await this.#cachePut(normalized, bytes, diagnostics);
    return { success: true, uri: normalized, content: bytes, fromCache: false,
      stale: false, finalUri: response.finalUri ?? normalized, diagnostics };
  }
}
