import type { RepositoryCache, CacheEntry, RepositoryClock, RepositoryManagerOptions, RepositoryTransport, RepositoryTransportRequest, RepositoryTransportResponse, RepositoryManager } from "./index.js";
export class BrowserCache implements RepositoryCache {
  constructor(database?: string, options?: { clock?: RepositoryClock; version?: number });
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  delete(key: string): Promise<void>;
  clear(): Promise<void>;
}
export class FetchRepositoryTransport implements RepositoryTransport {
  constructor(options?: { fetch?: typeof globalThis.fetch });
  get(request: RepositoryTransportRequest): Promise<RepositoryTransportResponse>;
}
export function createBrowserRepositoryManager(options?: RepositoryManagerOptions & { database?: string }): RepositoryManager;
