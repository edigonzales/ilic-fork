import { normalizeRepositoryUri, validateRepositoryRelativePath, resolveRepositoryUri } from "./repository-uri.js";
import { RepositoryCatalog } from "./repository-catalog.js";
import { RepositoryGraphWalker } from "./repository-graph-walker.js";
import { DEFAULT_REPOSITORY_POLICY, normalizeRepositoryPolicy, RepositoryResourceCoordinator } from "./resource-coordinator.js";
import { Md5RepositoryChecksum, md5 } from "./md5.js";
import { makeDiagnostic, RepositoryError } from "./diagnostics.js";

const decoder = new TextDecoder("utf-8", { fatal: true });

function cancelled(signal) {
  return signal?.aborted === true;
}

export class RepositoryManagerCore {
  constructor(options) {
    if (!options || !Array.isArray(options.repositories)) throw new TypeError("repositories are required");
    for (const name of ["transport", "cache", "clock", "metadataDecoder", "checksum"])
      if (!options[name]) throw new TypeError(`RepositoryManagerCore requires ${name}`);
    const repositories = [];
    const seen = new Set();
    for (const repository of options.repositories) {
      const normalized = normalizeRepositoryUri(repository);
      if (!seen.has(normalized)) { seen.add(normalized); repositories.push(normalized); }
    }
    this.repositories = repositories;
    this.policy = normalizeRepositoryPolicy({ ...DEFAULT_REPOSITORY_POLICY, ...options.policy });
    this.signal = options.signal;
    this.resources = new RepositoryResourceCoordinator({
      policy: this.policy,
      transport: options.transport,
      cache: options.cache,
      clock: options.clock,
      checksum: options.checksum,
      observer: options.observer
    });
    this.catalog = new RepositoryCatalog(this.resources, options.metadataDecoder);
    this.graph = new RepositoryGraphWalker(this.catalog, this.policy);
  }

  async listModels() {
    const result = await this.graph.listModels(this.repositories);
    if (this.repositories.length && !result.models.length) {
      const failures = result.diagnostics.filter(value => value.severity === "error")
        .map(value => new RepositoryError(value.message, [value]));
      throw new AggregateError(failures, "no configured INTERLIS repository is available");
    }
    return result.models;
  }

  async #resolveOne(name, schemaLanguage, state, diagnostics, signal) {
    if (name === "INTERLIS" || state.resolvedModels.has(name)) return;
    if (cancelled(signal)) throw new RepositoryError("repository request was cancelled", [
      makeDiagnostic("ILIC-REPO-CANCELLED", "repository request was cancelled", { operation: "model" })
    ]);
    if (state.stack.length >= this.policy.maxDependencyDepth)
      throw new RepositoryError("dependency depth limit exceeded", [
        makeDiagnostic("ILIC-REPO-LIMIT", "dependency depth limit exceeded", { operation: "model" })
      ]);
    if (state.stack.includes(name)) {
      const cycle = [...state.stack.slice(state.stack.indexOf(name)), name];
      throw new RepositoryError(`dependency cycle: ${cycle.join(" -> ")}`, [
        makeDiagnostic("ILIC-REPO-CYCLE", `dependency cycle: ${cycle.join(" -> ")}`, { operation: "model" })
      ]);
    }
    state.stack.push(name);
    try {
      const lookup = await this.graph.findModel(this.repositories, name, schemaLanguage);
      diagnostics.push(...lookup.diagnostics);
      if (!lookup.found) throw new RepositoryError(`model ${name} not found in configured repositories`, [
        ...diagnostics, makeDiagnostic("ILIC-REPO-NOT-FOUND", `model ${name} not found in configured repositories`, { operation: "model" })
      ]);
      for (const dependency of lookup.metadata.dependencies)
        await this.#resolveOne(dependency, schemaLanguage, state, diagnostics, signal);
      if (state.output.length >= this.policy.maxModelsResolved)
        throw new RepositoryError("resolved model limit exceeded", [
          makeDiagnostic("ILIC-REPO-LIMIT", "resolved model limit exceeded", { operation: "model" })
        ]);
      const path = validateRepositoryRelativePath(lookup.metadata.file);
      if (!path.valid) throw new RepositoryError(`unsafe repository path ${lookup.metadata.file}: ${path.error}`, [
        ...diagnostics, makeDiagnostic("ILIC-REPO-PATH", `unsafe repository path ${lookup.metadata.file}: ${path.error}`, { operation: "model" })
      ]);
      const uri = resolveRepositoryUri(lookup.metadata.repository, path.normalized);
      const existing = state.files.get(uri);
      if (existing) {
        if (this.policy.validateChecksums && lookup.metadata.md5
            && existing.checksum.toLowerCase() !== lookup.metadata.md5.toLowerCase()) {
          throw new RepositoryError(`MD5 mismatch for ${uri}: expected ${lookup.metadata.md5}, actual ${existing.checksum}`, [
            makeDiagnostic("ILIC-REPO-CHECKSUM", `MD5 mismatch for ${uri}: expected ${lookup.metadata.md5}, actual ${existing.checksum}`, { operation: "checksum", uri })
          ]);
        }
      } else {
        const resource = await this.resources.load({
          uri, kind: "model", ttl: this.policy.modelTtlMs,
          expectedMd5: lookup.metadata.md5, signal
        });
        diagnostics.push(...resource.diagnostics);
        if (!resource.success) {
          if (resource.cancelled) throw new RepositoryError(resource.error, diagnostics);
          throw new RepositoryError(resource.error ?? `unable to fetch ${uri}`, diagnostics);
        }
        let source;
        try { source = decoder.decode(resource.content); }
        catch (error) {
          throw new RepositoryError(`model source is not valid UTF-8: ${error.message}`, [
            ...diagnostics, makeDiagnostic("ILIC-REPO-XML", error.message, { operation: "model", uri })
          ]);
        }
        state.files.set(uri, { checksum: md5(resource.content), source });
        state.output.push({ metadata: lookup.metadata, uri, source,
          fromCache: resource.fromCache, stale: resource.stale });
      }
      state.resolvedModels.add(name);
    } finally { state.stack.pop(); }
  }

  async resolveWorkspace(models, schemaLanguage = "", signal = this.signal) {
    if (!Array.isArray(models)) throw new TypeError("models must be an array");
    const state = { stack: [], resolvedModels: new Set(), files: new Map(), output: [] };
    const diagnostics = [];
    for (const model of models) await this.#resolveOne(model, schemaLanguage, state, diagnostics, signal);
    return { models: state.output };
  }

  resolveModel(model, schemaLanguage = "", signal = this.signal) {
    return this.resolveWorkspace([model], schemaLanguage, signal);
  }
}

export { Md5RepositoryChecksum };
