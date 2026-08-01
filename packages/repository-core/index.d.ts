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
export interface RepositoryDiagnostic {
  severity: "error" | "warning" | "information";
  code: string;
  message: string;
  uri?: string;
  operation?: string;
  relatedInformation?: unknown[];
}
export class RepositoryError extends Error {
  readonly diagnostics: RepositoryDiagnostic[];
  readonly partialWorkspace?: ResolvedWorkspace;
}
export interface RepositoryClock { now(): number; }
export class ManualRepositoryClock implements RepositoryClock {
  constructor(initial?: number);
  now(): number;
  advance(milliseconds: number): void;
  set(value: number): void;
}
export interface CacheEntry { value: Uint8Array; storedAt: number; }
export interface RepositoryCache {
  get(key: string): Promise<CacheEntry | undefined>;
  put(key: string, value: Uint8Array): Promise<void>;
  putEntry?(key: string, entry: CacheEntry): Promise<void>;
  delete?(key: string): Promise<void>;
  clear(): Promise<void>;
}
export type RepositoryResourceKind = "model-index" | "site-index" | "model";
export interface RepositoryTransportRequest {
  uri: string;
  kind: RepositoryResourceKind;
  maxBytes: number;
  maxRedirects: number;
  signal?: AbortSignal;
}
export interface RepositoryTransportResponse {
  success: boolean;
  statusCode: number;
  body: Uint8Array;
  finalUri: string;
  error: string;
  notFound: boolean;
  retryable: boolean;
}
export interface RepositoryTransport {
  get(request: RepositoryTransportRequest): Promise<RepositoryTransportResponse>;
}
export interface RepositoryChecksum { md5(bytes: Uint8Array): string | Promise<string>; }
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
export interface RepositoryMetadataDecoder {
  decodeModelIndex(bytes: Uint8Array | string, repository: string): {
    success: boolean;
    index: { repository: string; models: ModelMetadata[] };
    diagnostics: RepositoryDiagnostic[];
  };
  decodeSite(bytes: Uint8Array | string, repository: string): {
    success: boolean;
    site: { parentSites: string[]; subsidiarySites: string[] };
    diagnostics: RepositoryDiagnostic[];
  };
}
export interface RepositoryManagerCoreOptions {
  repositories: string[];
  transport: RepositoryTransport;
  cache: RepositoryCache;
  clock: RepositoryClock;
  metadataDecoder: RepositoryMetadataDecoder;
  checksum: RepositoryChecksum;
  policy?: Partial<RepositoryPolicy>;
  observer?: (diagnostic: RepositoryDiagnostic) => void;
  signal?: AbortSignal;
}
export interface ResolvedWorkspace {
  models: Array<{
    metadata: ModelMetadata;
    uri: string;
    source: string;
    fromCache: boolean;
    stale: boolean;
    localPath?: string;
  }>;
}
export class RepositoryManagerCore {
  constructor(options: RepositoryManagerCoreOptions);
  readonly repositories: string[];
  readonly policy: RepositoryPolicy;
  listModels(): Promise<ModelMetadata[]>;
  resolveModel(model: string, schemaLanguage?: string, signal?: AbortSignal): Promise<ResolvedWorkspace>;
  resolveWorkspace(models: string[], schemaLanguage?: string, signal?: AbortSignal): Promise<ResolvedWorkspace>;
}
export class RepositoryResourceCoordinator {
  constructor(options: {
    policy?: Partial<RepositoryPolicy>;
    transport: RepositoryTransport;
    cache: RepositoryCache;
    clock: RepositoryClock;
    checksum: RepositoryChecksum;
    observer?: (diagnostic: RepositoryDiagnostic) => void;
  });
  load(request: { uri: string; kind?: RepositoryResourceKind; ttl?: number; optional?: boolean; expectedMd5?: string; signal?: AbortSignal }): Promise<{
    success: boolean; uri: string; content?: Uint8Array; fromCache?: boolean; stale?: boolean;
    diagnostics: RepositoryDiagnostic[]; error?: string; unavailable?: boolean; cancelled?: boolean;
  }>;
}
export const DEFAULT_REPOSITORY_POLICY: Readonly<RepositoryPolicy>;
export function normalizeRepositoryPolicy(options?: Partial<RepositoryPolicy>): RepositoryPolicy;
export function classifyCacheEntry(entry: CacheEntry | undefined, now: number, ttl: number): "missing" | "fresh" | "stale";
export class RepositoryCatalog {
  constructor(resources: RepositoryResourceCoordinator, decoder: RepositoryMetadataDecoder);
  invalidate(repository: string): void;
  clearMemoryState(): void;
}
export class RepositoryGraphWalker {
  constructor(catalog: RepositoryCatalog, policy: RepositoryPolicy);
}
export class SafeRepositoryMetadataDecoder implements RepositoryMetadataDecoder {
  constructor(options?: { maxBytes?: number; maxDepth?: number });
  decodeModelIndex(bytes: Uint8Array | string, repository: string): ReturnType<RepositoryMetadataDecoder["decodeModelIndex"]>;
  decodeSite(bytes: Uint8Array | string, repository: string): ReturnType<RepositoryMetadataDecoder["decodeSite"]>;
}
export class Md5RepositoryChecksum implements RepositoryChecksum { md5(bytes: Uint8Array): string; }
export function md5(bytes: Uint8Array): string;
export function makeDiagnostic(code: string, message: string, options?: Partial<RepositoryDiagnostic>): RepositoryDiagnostic;
export function parseIliModelsXml(xml: Uint8Array | string, repository: string): ModelMetadata[];
export function parseIliSiteXml(xml: Uint8Array | string): { parentSites: string[]; subsidiarySites: string[] };
export const supportedSchemaLanguagePreference: readonly ["ili2_4", "ili2_3", "ili1"];
export function selectLatestModelVersion(models: ModelMetadata[], name: string, schemaLanguage?: string, onDiagnostic?: (diagnostic: RepositoryDiagnostic) => void): ModelMetadata | undefined;
export function normalizeRepositoryUri(value: string): string;
export function resolveRepositoryUri(base: string, relative: string): string;
export function validateRepositoryRelativePath(value: string): { valid: boolean; normalized: string; error: string };
