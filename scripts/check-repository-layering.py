#!/usr/bin/env python3
"""Keep the repository core independent from platform adapters and the compiler."""

from __future__ import annotations

import argparse
from pathlib import Path
import tempfile


CORE_FORBIDDEN = (
    "node:",
    "globalThis.fetch",
    "Date.now(",
    "new Date(",
    "indexedDB",
    "process.",
)


def forbidden_tokens(text: str) -> list[str]:
    return [token for token in CORE_FORBIDDEN if token in text]


def scan(root: Path) -> list[str]:
    failures: list[str] = []
    js_core = root / "packages" / "repository-core"
    for path in sorted(js_core.glob("*.js")):
        text = path.read_text(encoding="utf-8")
        for token in forbidden_tokens(text):
            failures.append(f"{path.relative_to(root)} contains forbidden {token!r}")

    native_core = root / "source" / "repository" / "core"
    for path in sorted(native_core.glob("*.cpp")) + sorted(native_core.glob("*.h")):
        text = path.read_text(encoding="utf-8")
        for token in ("curl/", "pugixml", "std::filesystem", "system_clock::now("):
            if token in text:
                failures.append(f"{path.relative_to(root)} contains native adapter dependency {token!r}")

    compiler_core = root / "source" / "core"
    for path in sorted(compiler_core.glob("*.cpp")) + sorted(compiler_core.glob("*.h")):
        text = path.read_text(encoding="utf-8")
        if "repository/" in text or "Repository.h" in text:
            failures.append(f"{path.relative_to(root)} imports repository code")

    cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")
    if "add_library(ilic-repository-core STATIC" not in cmake:
        failures.append("CMakeLists.txt does not define ilic-repository-core")
    if "target_link_libraries(ilic-core" in cmake and "ilic-repository" in cmake.split("target_link_libraries(ilic-core", 1)[1].split(")", 1)[0]:
        failures.append("ilic-core links the repository layer")
    return failures


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("root", type=Path)
    parser.add_argument("--negative-canary", action="store_true")
    args = parser.parse_args()
    failures = scan(args.root.resolve())
    if args.negative_canary:
        with tempfile.TemporaryDirectory(prefix="ilic-repository-guard-") as directory:
            canary = Path(directory) / "canary.js"
            canary.write_text("globalThis.fetch('https://example.invalid');\n", encoding="utf-8")
            if "globalThis.fetch('https://example.invalid');" not in canary.read_text(encoding="utf-8"):
                failures.append("negative canary could not be created")
            else:
                canary_text = canary.read_text(encoding="utf-8")
                if not forbidden_tokens(canary_text):
                    failures.append("negative canary did not trigger a forbidden dependency")
    if failures:
        for failure in failures:
            print(failure)
        return 1
    print("repository layering guard passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
