export type DiagnosticSeverity = "error" | "warning" | "information" | "hint";
export interface Position { line: number; character: number; byteOffset: number; }
export interface SourceRange { uri: string; start: Position; end: Position; }
export interface Diagnostic {
  severity: DiagnosticSeverity;
  code: string;
  message: string;
  range: SourceRange | null;
  relatedInformation: Array<{ range: SourceRange | null; message: string }>;
  notes: string[];
  treatedAsError: boolean;
}
export interface LogEvent {
  level: "trace" | "debug" | "information" | "warning" | "error";
  category: string;
  message: string;
  context: Record<string, string>;
}
export interface CompilationRequest {
  roots: string[];
  options?: {
    autoSearch?: boolean;
    warningsAsErrors?: boolean;
    modelDirectories?: string[];
  };
  externalMetaAttributes?: Array<{ element: string; name: string; value: string }>;
}
export interface CompilationResult {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "compilation";
  success: boolean;
  cancelled: boolean;
  errorCount: number;
  warningCount: number;
  missingModels: string[];
  models: Array<{
    name: string;
    iliVersion: string;
    uri: string;
    metaAttributes: Array<{ name: string; value: string }>;
  }>;
  diagnostics: Diagnostic[];
  logs: LogEvent[];
  /** CLI-compatible human-readable compiler transcript, including the final status line. */
  transcript?: string[];
}
export interface SyntaxToken { kind: string; text: string; channel: number; range: SourceRange; }
export interface SyntaxNode { id: number; parent: number | null; kind: string; range: SourceRange; }
export interface SyntaxContext { kind: string; range: SourceRange; }
export interface SyntaxImportReference {
  model: string; unqualified: boolean; range: SourceRange;
}
export interface SyntaxSnapshot {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "syntax";
  success: boolean;
  uri: string;
  documentVersion: number;
  iliVersion: "1.0" | "2.3" | "2.4";
  tokens: SyntaxToken[];
  nodes: SyntaxNode[];
  contexts: SyntaxContext[];
  imports: string[];
  importReferences?: SyntaxImportReference[];
  diagnostics: Diagnostic[];
}
export interface SemanticSymbol {
  id: string; name: string; qualifiedName: string; kind: string;
  containerId: string; range: SourceRange | null; abstract: boolean;
  selectionRange?: SourceRange | null;
}
export interface SemanticReference {
  sourceId: string; targetId: string; kind: string; range: SourceRange | null;
}
export interface SemanticDependency {
  sourceUri: string; targetUri: string; model: string; range?: SourceRange | null;
}
export interface DiagramMember { name: string; type: string; inherited: boolean; }
export interface DiagramNode {
  id: string; containerId: string; label: string; kind: string; abstract: boolean;
  range: SourceRange | null; members: DiagramMember[];
}
export interface DiagramEdge {
  id: string; sourceId: string; targetId: string; kind: string;
  label: string; cardinality: string;
}
export interface DocumentationSection {
  id: string; title: string; kind: string; text: string; level: number;
}
export interface SemanticSnapshot {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "semantic";
  success: boolean;
  cancelled: boolean;
  roots: string[];
  documentVersions: Record<string, number>;
  missingModels?: string[];
  symbols: SemanticSymbol[];
  references: SemanticReference[];
  dependencies: SemanticDependency[];
  diagram: { nodes: DiagramNode[]; edges: DiagramEdge[] };
  documentation: { title: string; sections: DocumentationSection[] };
  diagnostics: Diagnostic[];
  logs: LogEvent[];
}
export interface CompilationAnalysisResult {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "compilation-analysis";
  compilation: CompilationResult;
  semantic: SemanticSnapshot;
  syntax: SyntaxSnapshot[];
}
export interface FormatResult {
  schemaVersion: 1;
  abiVersion: 1;
  compilerVersion: string;
  kind: "formatting";
  success: boolean;
  applicable: boolean;
  changed: boolean;
  text: string;
  diagnostics: Diagnostic[];
}
export interface ResolvedWorkspace {
  models: Array<{ uri: string; source: string }>;
}
export interface EmscriptenIlicModule {
  HEAPU8: Uint8Array;
  HEAPU32: Uint32Array;
  _ilic_abi_version(): number;
  _ilic_alloc(length: number): number;
  _ilic_free(pointer: number): void;
  _ilic_session_create(): number;
  _ilic_session_destroy(session: number): void;
  _ilic_session_put_source(session: number, uri: number, uriLength: number,
    source: number, sourceLength: number, version: bigint): number;
  _ilic_session_remove_source(session: number, uri: number, uriLength: number): number;
  _ilic_compile(session: number, request: number, requestLength: number): number;
  _ilic_parse(session: number, request: number, requestLength: number): number;
  _ilic_analyze(session: number, request: number, requestLength: number): number;
  _ilic_compile_and_analyze(session: number, request: number, requestLength: number): number;
  _ilic_format(session: number, request: number, requestLength: number): number;
  _ilic_result_json(result: number, resultLength: number): number;
  _ilic_result_destroy(result: number): void;
}
export class CompilerSession {
  constructor(module: EmscriptenIlicModule);
  putSource(uri: string, source: string | Uint8Array, version?: number): void;
  putWorkspace(workspace: ResolvedWorkspace): void;
  removeSource(uri: string): boolean;
  compile(request: CompilationRequest): CompilationResult;
  parse(uri: string): SyntaxSnapshot;
  analyze(request: CompilationRequest): SemanticSnapshot;
  compileAndAnalyze(request: CompilationRequest): CompilationAnalysisResult;
  format(uri: string, options?: { indentSize?: number; requireValidSyntax?: boolean }): FormatResult;
  dispose(): void;
}
export class Compiler {
  constructor(module: EmscriptenIlicModule);
  readonly module: EmscriptenIlicModule;
  readonly abiVersion: number;
  createSession(): CompilerSession;
}
export function createCompiler(options?: {
  moduleFactory?: (options?: Record<string, unknown>) => Promise<EmscriptenIlicModule> | EmscriptenIlicModule;
  moduleOptions?: Record<string, unknown>;
}): Promise<Compiler>;
