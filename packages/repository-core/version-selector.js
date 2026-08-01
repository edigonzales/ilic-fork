export const supportedSchemaLanguagePreference = Object.freeze(["ili2_4", "ili2_3", "ili1"]);

function selectForLanguage(models, name, schemaLanguage, onDiagnostic) {
  const candidates = models.filter(model => model.name === name
    && model.schemaLanguage === schemaLanguage && !model.browseOnly);
  if (!candidates.length) return undefined;
  const roots = candidates.filter(model => !model.precursorVersion);
  if (!roots.length) {
    onDiagnostic?.({
      severity: "warning", code: "ILIC-REPO-VERSION", operation: "version",
      message: `model ${name} (${schemaLanguage}) has no root version without precursorVersion`
    });
    return undefined;
  }
  let current = roots[0];
  const accounted = new Set([current]);
  for (const duplicate of roots.slice(1)) {
    accounted.add(duplicate);
    onDiagnostic?.({
      severity: "warning", code: "ILIC-REPO-VERSION", operation: "version",
      message: `model ${name} has duplicate root version ${duplicate.version}; using the first root`
    });
  }
  while (true) {
    const successors = candidates.filter(model => !accounted.has(model)
      && model.precursorVersion === current.version);
    if (!successors.length) break;
    current = successors[0];
    accounted.add(current);
    for (const duplicate of successors.slice(1)) {
      accounted.add(duplicate);
      onDiagnostic?.({
        severity: "warning", code: "ILIC-REPO-VERSION", operation: "version",
        message: `model ${name} has multiple successors of version ${duplicate.precursorVersion}; using ${current.version}`
      });
    }
  }
  if (candidates.some(model => !accounted.has(model))) {
    onDiagnostic?.({
      severity: "warning", code: "ILIC-REPO-VERSION", operation: "version",
      message: `model ${name} has versions outside the precursorVersion chain; using version ${current.version}`
    });
  }
  return current;
}

export function selectLatestModelVersion(models, name, schemaLanguage = "", onDiagnostic) {
  if (schemaLanguage) return selectForLanguage(models, name, schemaLanguage, onDiagnostic);
  for (const language of supportedSchemaLanguagePreference) {
    const selected = selectForLanguage(models, name, language, onDiagnostic);
    if (selected) return selected;
  }
  return undefined;
}
