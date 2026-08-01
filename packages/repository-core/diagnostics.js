export function makeDiagnostic(code, message, {
  severity = "error", uri, operation, relatedInformation
} = {}) {
  const value = { severity, code, message };
  if (uri !== undefined) value.uri = uri;
  if (operation !== undefined) value.operation = operation;
  if (relatedInformation !== undefined) value.relatedInformation = relatedInformation;
  return value;
}

export class RepositoryError extends Error {
  constructor(message, diagnostics = [], partialWorkspace) {
    super(message);
    this.name = "RepositoryError";
    this.diagnostics = diagnostics;
    if (partialWorkspace !== undefined) this.partialWorkspace = partialWorkspace;
  }
}

export function errorFromDiagnostics(message, diagnostics, partialWorkspace) {
  return new RepositoryError(message, diagnostics, partialWorkspace);
}
