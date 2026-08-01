import type {
  RepositoryCache, CacheEntry, RepositoryClock, RepositoryManagerOptions,
  RepositoryTransport, RepositoryTransportRequest, RepositoryTransportResponse,
  RepositoryManager
} from "./index.js";
export class NodeFileCache implements RepositoryCache {
  constructor(directory: string, options?: { clock?: RepositoryClock });
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  delete(key: string): Promise<void>;
  clear(): Promise<void>;
}
export class NodeRepositoryTransport implements RepositoryTransport {
  constructor(options?: { fetch?: typeof globalThis.fetch; fileRoots?: string[]; maxRedirects?: number });
  get(request: RepositoryTransportRequest): Promise<RepositoryTransportResponse>;
}
export function createNodeRepositoryManager(options?: RepositoryManagerOptions & { cacheDirectory?: string }): RepositoryManager;
