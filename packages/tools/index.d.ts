export interface ModelMetadata {
  name: string;
  schemaLanguage: string;
  file: string;
  version: string;
  publishingDate: string;
  precursorVersion: string;
  md5: string;
  repository: string;
  dependencies: string[];
  browseOnly: boolean;
}
export interface CacheEntry { value: Uint8Array; storedAt: number; }
export interface RepositoryCache {
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  delete?(key: string): Promise<void>;
  clear(): Promise<void>;
}
export class MemoryCache implements RepositoryCache {
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  delete(key: string): Promise<void>;
  clear(): Promise<void>;
}
export interface RepositoryManagerOptions {
  repositories?: string[];
  cache?: RepositoryCache;
  load?: (uri: string) => Promise<Uint8Array | string>;
  offline?: boolean;
  metadataTtlMs?: number;
  modelTtlMs?: number;
  allowStaleOnError?: boolean;
  followSiteLinks?: boolean;
  validateChecksums?: boolean;
  onWarning?: (warning: {
    uri: string;
    operation: "metadata" | "model" | "site" | "version" | "cache";
    message: string;
  }) => void;
}
export interface ResolvedWorkspace {
  models: Array<{
    metadata: ModelMetadata;
    uri: string;
    source: string;
    fromCache: boolean;
    stale: boolean;
  }>;
}
export function parseIliModelsXml(xml: string, repository: string): ModelMetadata[];
export function parseIliSiteXml(xml: string): {
  parentSites: string[];
  subsidiarySites: string[];
};
export const supportedSchemaLanguagePreference: readonly ["ili2_4", "ili2_3", "ili1"];
export function selectLatestModelVersion(
  models: ModelMetadata[],
  name: string,
  schemaLanguage?: string,
  onWarning?: (message: string) => void
): ModelMetadata | undefined;
export function normalizeRepositoryUri(value: string): string;
export function resolveRepositoryUri(base: string, relative: string): string;
export function validateRepositoryRelativePath(value: string): {
  valid: boolean;
  normalized: string;
  error: string;
};
export class RepositoryManager {
  constructor(options?: RepositoryManagerOptions);
  listModels(): Promise<ModelMetadata[]>;
  resolveWorkspace(models: string[], schemaLanguage?: string): Promise<ResolvedWorkspace>;
  resolveModel(model: string, schemaLanguage?: string): Promise<ResolvedWorkspace>;
}
