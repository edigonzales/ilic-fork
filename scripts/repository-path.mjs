import path from "node:path";

/** Return a stable, slash-separated path below a repository root. */
export function repositoryPath(root, file, pathApi = path) {
  return pathApi.relative(root, file).split(pathApi.sep).join("/");
}

/** Exercise both path implementations even when the host only provides one style. */
export function verifyRepositoryPathPortability() {
  const posix = repositoryPath("/work/ilic", "/work/ilic/source/core/Compiler.cpp", path.posix);
  const windows = repositoryPath(
    String.raw`C:\work\ilic`,
    String.raw`C:\work\ilic\source\core\Compiler.cpp`,
    path.win32,
  );
  if (posix !== "source/core/Compiler.cpp" || windows !== posix)
    throw new Error(`repository path normalization failed: posix=${posix}, windows=${windows}`);
}
