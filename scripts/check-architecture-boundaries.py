#!/usr/bin/env python3
"""Check durable ownership boundaries without depending on sibling repositories."""

from __future__ import annotations

import pathlib
import re
import sys


def contains(source: str, patterns: tuple[str, ...]) -> bool:
    return any(re.search(pattern, source, re.MULTILINE) for pattern in patterns)


def main() -> int:
    root = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    rules = (
        (
            "compiler-facade",
            root / "source/core/Compiler.cpp",
            (r"generated[/\\].*Parser\.h", r"#include\s+.*Parser\.h"),
            "Compiler.cpp must not own generated parser integration",
        ),
        (
            "capi-facade",
            root / "source/abi/Capi.cpp",
            (r"Value::Object", r"appendJsonDiagnostics", r"\n(?:Value|std::string)\s+semanticResult\s*\("),
            "Capi.cpp must delegate JSON projection",
        ),
        (
            "wasm-export-facade",
            root / "packages/compiler-wasm/index.js",
            (r"projectLegacyEditorSnapshot", r"legacyContainsRange"),
            "index.js must not own legacy editor projection",
        ),
    )

    # The canary proves the matcher itself catches a representative ownership leak.
    if not contains("const value = Value::Object{};", rules[1][2]):
        print("architecture boundary canary did not detect a C-ABI projector", file=sys.stderr)
        return 2

    failures: list[str] = []
    for name, file, patterns, message in rules:
        source = file.read_text(encoding="utf-8")
        if contains(source, patterns):
            failures.append(f"{name}: {message} ({file})")

    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(f"architecture boundaries: {len(rules)} repository-local rules passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
