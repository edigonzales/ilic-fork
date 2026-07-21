export class BrowserCache {
  constructor(database = "ilic-repository-v1") { this.database = database; }
  async #db() {
    return await new Promise((resolve, reject) => {
      const request = indexedDB.open(this.database, 1);
      request.onupgradeneeded = () => request.result.createObjectStore("resources");
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error);
    });
  }
  async #transaction(mode, operation) {
    const db = await this.#db();
    return await new Promise((resolve, reject) => {
      const transaction = db.transaction("resources", mode);
      const request = operation(transaction.objectStore("resources"));
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error);
      transaction.oncomplete = () => db.close();
    });
  }
  get(key) { return this.#transaction("readonly", store => store.get(key)); }
  put(key, value) {
    return this.#transaction("readwrite", store => store.put({ value, storedAt: Date.now() }, key));
  }
  delete(key) { return this.#transaction("readwrite", store => store.delete(key)); }
  clear() { return this.#transaction("readwrite", store => store.clear()); }
}
