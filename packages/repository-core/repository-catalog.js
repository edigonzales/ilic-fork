import { resolveRepositoryUri } from "./repository-uri.js";

export class RepositoryCatalog {
  constructor(resources, decoder) {
    this.resources = resources;
    this.decoder = decoder;
    this.indexes = new Map();
    this.sites = new Map();
    this.indexInFlight = new Map();
    this.siteInFlight = new Map();
  }

  async index(repository, diagnostics) {
    const known = this.indexes.get(repository);
    if (known?.status === "available") return known.value;
    if (known?.status === "unavailable") return undefined;
    if (!this.indexInFlight.has(repository)) {
      const promise = this.#loadIndex(repository, diagnostics).finally(() => this.indexInFlight.delete(repository));
      this.indexInFlight.set(repository, promise);
    }
    return this.indexInFlight.get(repository);
  }

  async #loadIndex(repository, diagnostics) {
    const uri = resolveRepositoryUri(repository, "ilimodels.xml");
    const resource = await this.resources.load({ uri, kind: "model-index", ttl: this.resources.policy.metadataTtlMs });
    diagnostics.push(...resource.diagnostics);
    if (!resource.success) {
      this.indexes.set(repository, { status: "unavailable" });
      return undefined;
    }
    const decoded = this.decoder.decodeModelIndex(resource.content, repository);
    diagnostics.push(...decoded.diagnostics);
    if (!decoded.success) {
      this.indexes.set(repository, { status: "unavailable" });
      return undefined;
    }
    this.indexes.set(repository, { status: "available", value: decoded.index });
    return decoded.index;
  }

  async site(repository, diagnostics) {
    if (!this.resources.policy.followSiteLinks) return undefined;
    const known = this.sites.get(repository);
    if (known?.status === "available") return known.value;
    if (known?.status === "unavailable") return undefined;
    if (!this.siteInFlight.has(repository)) {
      const promise = this.#loadSite(repository, diagnostics).finally(() => this.siteInFlight.delete(repository));
      this.siteInFlight.set(repository, promise);
    }
    return this.siteInFlight.get(repository);
  }

  async #loadSite(repository, diagnostics) {
    const uri = resolveRepositoryUri(repository, "ilisite.xml");
    const resource = await this.resources.load({ uri, kind: "site-index", optional: true, ttl: this.resources.policy.metadataTtlMs });
    diagnostics.push(...resource.diagnostics);
    if (!resource.success) {
      this.sites.set(repository, { status: "unavailable" });
      return undefined;
    }
    const decoded = this.decoder.decodeSite(resource.content, repository);
    diagnostics.push(...decoded.diagnostics);
    if (!decoded.success) {
      this.sites.set(repository, { status: "unavailable" });
      return undefined;
    }
    this.sites.set(repository, { status: "available", value: decoded.site });
    return decoded.site;
  }

  invalidate(repository) {
    this.indexes.delete(repository);
    this.sites.delete(repository);
  }

  clearMemoryState() {
    this.indexes.clear();
    this.sites.clear();
  }
}
