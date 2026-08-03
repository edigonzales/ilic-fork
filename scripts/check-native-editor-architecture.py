#!/usr/bin/env python3
"""Keep the editor snapshot on the native parser path."""

from __future__ import annotations

import argparse
from pathlib import Path


FORBIDDEN = (
    "editorTextProjection",
    "editorTokens",
    "editorProjection",
    "editorPathKind",
    "tokensInRange",
    "#sources",
)


def violations(text: str) -> list[str]:
    return [name for name in FORBIDDEN if name in text]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("source_dir", type=Path)
    parser.add_argument("--negative-canary", action="store_true")
    args = parser.parse_args()

    if args.negative_canary:
        if not violations("const editorTokens = new Map();"):
            raise SystemExit("negative canary did not detect a forbidden parser")
        return 0

    package = (args.source_dir / "packages/compiler-wasm/index.js").read_text()
    session_wrapper = (args.source_dir / "packages/compiler-wasm/compiler-session.js").read_text()
    capi = (args.source_dir / "include/ilic/capi.h").read_text()
    pipeline = (args.source_dir / "source/core/SnapshotPipeline.cpp").read_text()
    snapshot_factory = (args.source_dir / "source/core/ParsedSourceArtifactFactory.cpp").read_text()
    parser_sources = pipeline + snapshot_factory
    cache_key = (args.source_dir / "source/core/incremental/ParsedSourceCache.h").read_text()
    if violations(package):
        raise SystemExit("forbidden JS editor parser symbols: " + ", ".join(violations(package)))
    if "_ilic_editor_snapshot" not in package + session_wrapper or "ilic_editor_snapshot" not in capi:
        raise SystemExit("native editor snapshot export is not wired through JS and C ABI")
    if not ("buildIli2Impl<EditorRecoveryTokenStream>(source,mode,true)" in parser_sources
            or "buildIli2Impl<EditorTokenStream>(source,mode,true)" in parser_sources):
        raise SystemExit("tolerant parser recovery is not isolated in the editor mode")
    if "buildIli2Impl<antlr4::CommonTokenStream>(source,mode,false)" not in parser_sources:
        raise SystemExit("strict parser is not wired to the non-recovering token stream")
    if "ParseMode mode" not in cache_key:
        raise SystemExit("parser cache key does not carry ParseMode")
    if "static_cast<unsigned int>(mode)" not in (args.source_dir / "source/core/incremental/ParsedSourceCache.cpp").read_text():
        raise SystemExit("parser cache canonical key omits ParseMode")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
