function windowsDrive(value) {
  return /^[A-Za-z]:/.test(value);
}

function hasScheme(value) {
  return /^[A-Za-z][A-Za-z0-9+.-]*:/.test(value) && !windowsDrive(value);
}

function normalizePath(value) {
  value = value.replaceAll("\\", "/");
  const unc = value.startsWith("//");
  const absolute = !unc && value.startsWith("/");
  const drive = windowsDrive(value) ? value.slice(0, 2) : "";
  let rest = value.slice(unc ? 2 : absolute ? 1 : drive ? 2 : 0);
  if (drive && rest.startsWith("/")) rest = rest.slice(1);
  const parts = [];
  for (const part of rest.split("/")) {
    if (!part || part === ".") continue;
    if (part === ".." && parts.length && parts.at(-1) !== "..") parts.pop();
    else if (part !== ".." || (!unc && !absolute && !drive)) parts.push(part);
  }
  const prefix = unc ? "//" : absolute ? "/" : drive ? `${drive}/` : "";
  return `${prefix}${parts.join("/")}`.replace(/\/$/, "") || ".";
}

function normalizeRemote(value) {
  const suffixAt = value.search(/[?#]/);
  let base = suffixAt < 0 ? value : value.slice(0, suffixAt);
  const suffix = suffixAt < 0 ? "" : value.slice(suffixAt);
  const schemeEnd = base.indexOf("://");
  const pathStart = base.indexOf("/", schemeEnd + 3);
  if (pathStart < 0) return base + suffix;
  const authority = base.slice(0, pathStart);
  const parts = [];
  for (const part of base.slice(pathStart + 1).split("/")) {
    if (!part || part === ".") continue;
    if (part === "..") parts.pop();
    else parts.push(part);
  }
  return `${authority}${parts.length ? `/${parts.join("/")}` : ""}${suffix}`;
}

export function normalizeRepositoryUri(value) {
  if (typeof value !== "string" || value.length === 0) throw new Error("repository URI is empty");
  if (/^https?:\/\//i.test(value)) return normalizeRemote(value);
  if (/^file:\/\//i.test(value)) {
    const raw = decodeURIComponent(value.slice(value.indexOf("://") + 3));
    const path = normalizePath(raw.startsWith("/") ? raw : `//${raw}`);
    return path.startsWith("//") ? `file:${path}` : `file://${path}`;
  }
  if (hasScheme(value)) throw new Error(`unsupported repository URI scheme in ${value}`);
  return normalizePath(value);
}

export function resolveRepositoryUri(base, relative) {
  if (hasScheme(relative) || windowsDrive(relative) || relative.startsWith("\\\\"))
    return normalizeRepositoryUri(relative);
  const normalizedBase = normalizeRepositoryUri(base);
  const clean = relative.replaceAll("\\", "/");
  if (/^https?:\/\//i.test(normalizedBase)) {
    const suffixAt = normalizedBase.search(/[?#]/);
    const root = suffixAt < 0 ? normalizedBase : normalizedBase.slice(0, suffixAt);
    const suffix = suffixAt < 0 ? "" : normalizedBase.slice(suffixAt);
    if (clean.startsWith("//"))
      return normalizeRepositoryUri(`${root.slice(0, root.indexOf(":"))}:${clean}`);
    const pathAt = root.indexOf("/", root.indexOf("://") + 3);
    const authority = pathAt < 0 ? root : root.slice(0, pathAt);
    if (clean.startsWith("?") || clean.startsWith("#"))
      return normalizeRepositoryUri(`${root}${clean}`);
    if (clean.startsWith("/")) return normalizeRepositoryUri(`${authority}${clean}`);
    return normalizeRepositoryUri(`${root}/${clean}${suffix}`);
  }
  if (relative.startsWith("/")) return normalizeRepositoryUri(relative);
  if (/^file:\/\//i.test(normalizedBase)) {
    const raw = normalizedBase.slice(normalizedBase.indexOf("://") + 3);
    return normalizeRepositoryUri(`file://${normalizePath(`${raw}/${clean}`)}`);
  }
  return normalizePath(`${normalizedBase}/${clean}`);
}

export function validateRepositoryRelativePath(value) {
  if (typeof value !== "string" || value.length === 0)
    return { valid: false, normalized: "", error: "repository path is empty" };
  const normalized = value.replaceAll("\\", "/");
  if (normalized.startsWith("/") || normalized.startsWith("//")
      || windowsDrive(normalized) || hasScheme(normalized))
    return { valid: false, normalized: "", error: "repository path must be relative" };
  const parts = [];
  for (const part of normalized.split("/")) {
    if (part === "..") return {
      valid: false, normalized: "", error: "repository path must not contain parent traversal"
    };
    if (part && part !== ".") parts.push(part);
  }
  if (!parts.length) return { valid: false, normalized: "", error: "repository path is empty" };
  return { valid: true, normalized: parts.join("/"), error: "" };
}
