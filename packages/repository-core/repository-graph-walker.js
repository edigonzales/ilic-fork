import { normalizeRepositoryUri, resolveRepositoryUri } from "./repository-uri.js";
import { selectLatestModelVersion } from "./version-selector.js";
import { makeDiagnostic } from "./diagnostics.js";

export class RepositoryGraphWalker {
  constructor(catalog, policy) {
    this.catalog = catalog;
    this.policy = policy;
  }

  #seeds(seeds, diagnostics) {
    const result = [];
    const seen = new Set();
    for (const seed of seeds) {
      try {
        const normalized = normalizeRepositoryUri(seed);
        if (!seen.has(normalized)) { seen.add(normalized); result.push(normalized); }
      } catch (error) {
        diagnostics.push(makeDiagnostic("ILIC-REPO-URI", error.message, {
          severity: "warning", uri: seed, operation: "config"
        }));
      }
    }
    return result;
  }

  #links(repository, values, queue, diagnostics) {
    for (const value of values) {
      try { queue.push(resolveRepositoryUri(repository, value)); }
      catch (error) {
        diagnostics.push(makeDiagnostic("ILIC-REPO-URI", `invalid site link ${value} in ${repository}: ${error.message}`, {
          severity: "warning", uri: repository, operation: "site"
        }));
      }
    }
  }

  async #visitIndex(repository, visited, diagnostics, modelName, schemaLanguage) {
    if (visited.has(repository)) return undefined;
    if (visited.size >= this.policy.maxRepositoriesVisited) {
      diagnostics.push(makeDiagnostic("ILIC-REPO-LIMIT", "repository visit limit exceeded", {
        uri: repository, operation: "metadata"
      }));
      return undefined;
    }
    visited.add(repository);
    const index = await this.catalog.index(repository, diagnostics);
    if (!index) return undefined;
    return selectLatestModelVersion(index.models, modelName, schemaLanguage,
      value => diagnostics.push(value));
  }

  async findModel(seeds, modelName, schemaLanguage = "") {
    const diagnostics = [];
    const normalizedSeeds = this.#seeds(seeds, diagnostics);
    const visited = new Set();
    for (const seed of normalizedSeeds) {
      const model = await this.#visitIndex(seed, visited, diagnostics, modelName, schemaLanguage);
      if (model) return { found: true, metadata: model, diagnostics };
    }
    if (!this.policy.followSiteLinks) return { found: false, diagnostics };
    const parents = [];
    const subsidiaries = [];
    for (const seed of normalizedSeeds) {
      const site = await this.catalog.site(seed, diagnostics);
      if (!site) continue;
      this.#links(seed, site.parentSites, parents, diagnostics);
      this.#links(seed, site.subsidiarySites, subsidiaries, diagnostics);
    }
    const drainParents = async () => {
      while (parents.length) {
        const repository = parents.shift();
        const model = await this.#visitIndex(repository, visited, diagnostics, modelName, schemaLanguage);
        if (model) return model;
        const site = await this.catalog.site(repository, diagnostics);
        if (site) this.#links(repository, site.parentSites, parents, diagnostics);
      }
      return undefined;
    };
    let found = await drainParents();
    if (found) return { found: true, metadata: found, diagnostics };
    while (subsidiaries.length) {
      const repository = subsidiaries.shift();
      found = await this.#visitIndex(repository, visited, diagnostics, modelName, schemaLanguage);
      if (found) return { found: true, metadata: found, diagnostics };
      const site = await this.catalog.site(repository, diagnostics);
      if (site) {
        this.#links(repository, site.subsidiarySites, subsidiaries, diagnostics);
        this.#links(repository, site.parentSites, parents, diagnostics);
        found = await drainParents();
        if (found) return { found: true, metadata: found, diagnostics };
      }
    }
    return { found: false, diagnostics };
  }

  async listModels(seeds) {
    const diagnostics = [];
    const normalizedSeeds = this.#seeds(seeds, diagnostics);
    const visited = new Set();
    const models = [];
    const visit = async repository => {
      if (visited.has(repository)) return false;
      if (visited.size >= this.policy.maxRepositoriesVisited) {
        diagnostics.push(makeDiagnostic("ILIC-REPO-LIMIT", "repository visit limit exceeded", { operation: "metadata" }));
        return false;
      }
      visited.add(repository);
      const index = await this.catalog.index(repository, diagnostics);
      if (index) models.push(...index.models);
      return true;
    };
    for (const seed of normalizedSeeds) await visit(seed);
    if (this.policy.followSiteLinks) {
      const parents = [], subsidiaries = [];
      for (const seed of normalizedSeeds) {
        const site = await this.catalog.site(seed, diagnostics);
        if (site) {
          this.#links(seed, site.parentSites, parents, diagnostics);
          this.#links(seed, site.subsidiarySites, subsidiaries, diagnostics);
        }
      }
      const drainParents = async () => {
        while (parents.length) {
          const repository = parents.shift();
          if (!await visit(repository)) continue;
          const site = await this.catalog.site(repository, diagnostics);
          if (site) this.#links(repository, site.parentSites, parents, diagnostics);
        }
      };
      await drainParents();
      while (subsidiaries.length) {
        const repository = subsidiaries.shift();
        if (!await visit(repository)) continue;
        const site = await this.catalog.site(repository, diagnostics);
        if (site) {
          this.#links(repository, site.subsidiarySites, subsidiaries, diagnostics);
          this.#links(repository, site.parentSites, parents, diagnostics);
          await drainParents();
        }
      }
    }
    return { models, diagnostics };
  }
}
