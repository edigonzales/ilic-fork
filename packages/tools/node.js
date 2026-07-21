import { createHash, randomUUID } from "node:crypto";
import { mkdir, readFile, rename, rm, writeFile } from "node:fs/promises";
import { basename, join } from "node:path";

async function publishMetadata(temporary, target) {
  let lastError;
  for (let attempt = 0; attempt < 64; attempt++) {
    try { await rename(temporary, target); return; }
    catch (error) {
      lastError = error;
      if (!["EEXIST", "EPERM", "EACCES"].includes(error?.code)) throw error;
      // Windows rename() cannot replace an existing file. Concurrent writers
      // can recreate it after this removal, so retry the complete operation.
      await rm(target, { force: true });
    }
  }
  throw lastError;
}

export class NodeFileCache {
  constructor(directory) { this.directory = directory; }
  #path(key) { return join(this.directory, createHash("md5").update(key).digest("hex")); }
  async get(key) {
    try {
      const path = this.#path(key);
      const parsed = JSON.parse(await readFile(`${path}.json`, "utf8"));
      if (parsed.file && basename(parsed.file) !== parsed.file) return undefined;
      const value = await readFile(parsed.file ? join(this.directory, parsed.file) : path);
      if (parsed.size !== value.byteLength) return undefined;
      if (parsed.md5 !== createHash("md5").update(value).digest("hex")) return undefined;
      return { value: new Uint8Array(value), storedAt: parsed.storedAt };
    } catch { return undefined; }
  }
  async put(key, value) {
    await mkdir(this.directory, { recursive: true });
    const path = this.#path(key);
    const suffix = `${process.pid}.${randomUUID()}.tmp`;
    const generationPath = `${path}.${suffix.slice(0, -4)}.data`;
    const contentTemporary = `${generationPath}.tmp`;
    const metadataPath = `${path}.json`;
    const metadataTemporary = `${metadataPath}.${suffix}`;
    const metadata = JSON.stringify({
      storedAt: Date.now(), size: value.byteLength,
      md5: createHash("md5").update(value).digest("hex"), file: basename(generationPath)
    });
    let publishedContent = false;
    let publishedMetadata = false;
    try {
      await Promise.all([
        writeFile(contentTemporary, value),
        writeFile(metadataTemporary, metadata)
      ]);
      await rename(contentTemporary, generationPath);
      publishedContent = true;
      // A brief metadata gap on Windows is a cache miss; immutable content is
      // never paired with metadata from a different concurrent writer.
      await publishMetadata(metadataTemporary, metadataPath);
      publishedMetadata = true;
      await rm(path, { force: true }); // legacy content layout
    } finally {
      await Promise.all([
        rm(contentTemporary, { force: true }), rm(metadataTemporary, { force: true }),
        ...(publishedContent && !publishedMetadata ? [rm(generationPath, { force: true })] : [])
      ]);
    }
  }
  async delete(key) {
    const path = this.#path(key);
    let generation;
    try {
      const parsed = JSON.parse(await readFile(`${path}.json`, "utf8"));
      if (parsed.file && basename(parsed.file) === parsed.file)
        generation = join(this.directory, parsed.file);
    } catch {}
    await Promise.all([
      rm(path, { force: true }), rm(`${path}.json`, { force: true }),
      ...(generation ? [rm(generation, { force: true })] : [])
    ]);
  }
  async clear() { await rm(this.directory, { recursive: true, force: true }); }
}
