import type { RepositoryCache, CacheEntry } from "./index.js";
export class BrowserCache implements RepositoryCache {
  constructor(database?: string);
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  clear(): Promise<void>;
}
