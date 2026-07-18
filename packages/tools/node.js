import { createHash } from "node:crypto";
import { mkdir, readFile, rename, rm, writeFile } from "node:fs/promises";
import { join } from "node:path";

export class NodeFileCache {
  constructor(directory) { this.directory = directory; }
  #path(key) { return join(this.directory, createHash("md5").update(key).digest("hex")); }
  async get(key) {
    try {
      const path = this.#path(key);
      const [value, metadata] = await Promise.all([readFile(path), readFile(`${path}.json`, "utf8")]);
      return { value: new Uint8Array(value), storedAt: JSON.parse(metadata).storedAt };
    } catch { return undefined; }
  }
  async put(key, value) {
    await mkdir(this.directory, { recursive: true });
    const path = this.#path(key);
    const temporary = `${path}.tmp`;
    await writeFile(temporary, value);
    await rename(temporary, path);
    await writeFile(`${path}.json`, JSON.stringify({ storedAt: Date.now() }));
  }
  async clear() { await rm(this.directory, { recursive: true, force: true }); }
}
