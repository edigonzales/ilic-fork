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
export interface RepositoryClock { now(): number; }
export interface RepositoryChecksum { md5(bytes: Uint8Array): string | Promise<string>; }
export type RepositoryDiagnosticSeverity = "error" | "warning" | "information";
export interface RepositoryDiagnostic {
  severity: RepositoryDiagnosticSeverity;
  code: string;
  message: string;
  uri?: string;
  operation?: "config" | "metadata" | "site" | "version" | "model" | "cache" | "transport" | "checksum" | "install";
  relatedInformation?: unknown[];
}
export class RepositoryError extends Error {
  readonly diagnostics: RepositoryDiagnostic[];
  readonly partialWorkspace?: ResolvedWorkspace;
}
export class MemoryCache implements RepositoryCache {
  constructor(options?: { clock?: RepositoryClock; cloneBytes?: boolean; faultInjection?: Record<string, unknown> });
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
  transport?: RepositoryTransport;
  clock?: RepositoryClock;
  checksum?: RepositoryChecksum;
  metadataDecoder?: RepositoryMetadataDecoder;
  limits?: Partial<RepositoryPolicy>;
  signal?: AbortSignal;
  fetch?: typeof globalThis.fetch;
}
export interface RepositoryPolicy {
  metadataTtlMs: number;
  modelTtlMs: number;
  offline: boolean;
  allowStaleOnError: boolean;
  followSiteLinks: boolean;
  validateChecksums: boolean;
  maxMetadataBytes: number;
  maxModelBytes: number;
  maxRepositoriesVisited: number;
  maxDependencyDepth: number;
  maxModelsResolved: number;
  maxRedirects: number;
}
export interface RepositoryTransportRequest {
  uri: string;
  kind: "model-index" | "site-index" | "model";
  maxBytes: number;
  maxRedirects: number;
  signal?: AbortSignal;
}
export interface RepositoryTransportResponse {
  success: boolean;
  statusCode?: number;
  body?: Uint8Array;
  finalUri?: string;
  error?: string;
  notFound?: boolean;
  retryable?: boolean;
}
export interface RepositoryTransport { get(request: RepositoryTransportRequest): Promise<RepositoryTransportResponse>; }
export interface RepositoryMetadataDecoder {
  decodeModelIndex(bytes: Uint8Array | string, repository: string): {
    success: boolean; index: { repository: string; models: ModelMetadata[] }; diagnostics: RepositoryDiagnostic[];
  };
  decodeSite(bytes: Uint8Array | string, repository: string): {
    success: boolean; site: { parentSites: string[]; subsidiarySites: string[] }; diagnostics: RepositoryDiagnostic[];
  };
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
