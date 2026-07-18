import type { RepositoryCache, CacheEntry } from "./index.js";
export class NodeFileCache implements RepositoryCache {
  constructor(directory: string);
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  clear(): Promise<void>;
}
