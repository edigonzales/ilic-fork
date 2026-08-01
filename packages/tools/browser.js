import {
  FetchRepositoryTransport,
  MemoryCache,
  RepositoryManager,
  SystemRepositoryClock
} from "./compatibility-manager.js";

export class BrowserCache {
  constructor(database = "ilic-repository-v1", { clock = new SystemRepositoryClock(), version = 2 } = {}) {
    this.database = database;
    this.clock = clock;
    this.version = version;
  }

  async #db() {
    if (typeof indexedDB === "undefined") throw new Error("IndexedDB is not available");
    return await new Promise((resolve, reject) => {
      const request = indexedDB.open(this.database, this.version);
      request.onupgradeneeded = () => {
        const database = request.result;
        if (!database.objectStoreNames.contains("resources")) database.createObjectStore("resources");
      };
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error ?? new Error("unable to open repository cache"));
      request.onblocked = () => reject(new Error("repository cache upgrade is blocked by another tab"));
    });
  }

  async #transaction(mode, operation) {
    const db = await this.#db();
    return await new Promise((resolve, reject) => {
      let settled = false;
      const finish = (callback, value) => {
        if (settled) return;
        settled = true;
        try { db.close(); } finally { callback(value); }
      };
      let transaction;
      try {
        transaction = db.transaction("resources", mode);
        const request = operation(transaction.objectStore("resources"));
        request.onsuccess = () => {};
        request.onerror = () => finish(reject, request.error ?? new Error("repository cache request failed"));
        transaction.oncomplete = () => finish(resolve, request.result);
        transaction.onerror = () => finish(reject, transaction.error ?? new Error("repository cache transaction failed"));
        transaction.onabort = () => finish(reject, transaction.error ?? new Error("repository cache transaction aborted"));
      } catch (error) { finish(reject, error); }
    });
  }

  async get(key) {
    const value = await this.#transaction("readonly", store => store.get(key));
    if (!value || !Number.isFinite(value.storedAt)) return undefined;
    return { value: new Uint8Array(value.value), storedAt: value.storedAt };
  }

  async putEntry(key, entry) {
    await this.#transaction("readwrite", store => store.put({
      value: new Uint8Array(entry.value), storedAt: entry.storedAt
    }, key));
  }

  async put(key, value) { return this.putEntry(key, { value, storedAt: this.clock.now() }); }
  delete(key) { return this.#transaction("readwrite", store => store.delete(key)); }
  clear() { return this.#transaction("readwrite", store => store.clear()); }
}
export { FetchRepositoryTransport };

export function createBrowserRepositoryManager(options = {}) {
  const clock = options.clock ?? new SystemRepositoryClock();
  const cache = options.cache ?? new BrowserCache(options.database, { clock });
  const transport = options.transport ?? new FetchRepositoryTransport({ fetch: options.fetch });
  return new RepositoryManager({ ...options, clock, cache, transport });
}
