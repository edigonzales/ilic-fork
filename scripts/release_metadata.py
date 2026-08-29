#!/usr/bin/env python3
"""Create deterministic ilic artifact versions and provenance manifests."""

from __future__ import annotations

import argparse
import datetime as dt
import json
import pathlib
import re
import subprocess
import sys


SEMVER_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+$")
SHA_RE = re.compile(r"^[0-9a-f]{40}$")
SNAPSHOT_RE = re.compile(r"^(?P<base>[0-9]+\.[0-9]+\.[0-9]+)-snapshot\.g(?P<sha>[0-9a-f]{12})$")


def require_semver(value: str) -> str:
    if not SEMVER_RE.fullmatch(value):
        raise ValueError(f"version must be X.Y.Z, got {value!r}")
    return value


def require_sha(value: str) -> str:
    if not SHA_RE.fullmatch(value):
        raise ValueError("source SHA must be a lowercase 40-character Git SHA")
    return value


def snapshot_version(base_version: str, source_sha: str) -> str:
    return f"{require_semver(base_version)}-snapshot.g{require_sha(source_sha)[:12]}"


def validate_artifact_version(version: str, source_sha: str) -> str:
    require_sha(source_sha)
    if SEMVER_RE.fullmatch(version):
        return "stable"
    match = SNAPSHOT_RE.fullmatch(version)
    if not match:
        raise ValueError(
            "artifact version must be X.Y.Z or X.Y.Z-snapshot.g<12-character-source-sha>"
        )
    if match.group("sha") != source_sha[:12]:
        raise ValueError("snapshot suffix does not match the source SHA")
    return "snapshot"


def project_version(project_root: pathlib.Path) -> str:
    text = (project_root / "CMakeLists.txt").read_text(encoding="utf-8")
    matches = re.findall(
        r"project\s*\(\s*ilic\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)(?=\s|\))",
        text,
        flags=re.IGNORECASE,
    )
    if len(matches) != 1:
        raise ValueError(f"expected one project(ilic VERSION X.Y.Z), found {len(matches)}")
    return require_semver(matches[0])


def project_source_version(project_root: pathlib.Path) -> str:
    text = (project_root / "CMakeLists.txt").read_text(encoding="utf-8")
    base = project_version(project_root)
    qualifier = re.search(
        r'set\s*\(\s*ILIC_VERSION_QUALIFIER\s+"([A-Za-z0-9-]+)"\s*\)',
        text,
        flags=re.IGNORECASE,
    )
    return f"{base}-{qualifier.group(1)}" if qualifier else base


def git_sha(project_root: pathlib.Path) -> str:
    value = subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=project_root, text=True
    ).strip()
    return require_sha(value)


def package_versions(project_root: pathlib.Path) -> list[str]:
    result = []
    for package in ("repository-core", "tools", "compiler-wasm"):
        manifest = json.loads(
            (project_root / "packages" / package / "package.json").read_text(
                encoding="utf-8"
            )
        )
        result.append(manifest["version"])
    return result


def check(project_root: pathlib.Path) -> None:
    base = project_version(project_root)
    expected_source = project_source_version(project_root)
    versions = package_versions(project_root)
    if versions != [expected_source] * 3:
        raise ValueError(
            f"source package versions must all be {expected_source}, got {versions}"
        )
    sha = git_sha(project_root)
    validate_artifact_version(snapshot_version(base, sha), sha)


def release_manifest(
    *,
    project_root: pathlib.Path,
    artifact_version: str,
    source_sha: str,
    run_id: str | None,
    published_at: str | None,
    toolchain: str | None,
) -> dict[str, object]:
    kind = validate_artifact_version(artifact_version, source_sha)
    if published_at:
        parsed = dt.datetime.fromisoformat(published_at.replace("Z", "+00:00"))
        if parsed.tzinfo is None:
            raise ValueError("published-at must include a timezone")
    return {
        "schemaVersion": 1,
        "project": "ilic",
        "artifactVersion": artifact_version,
        "versionKind": kind,
        "sourceSha": source_sha,
        "dependencies": {},
        "build": {
            "githubRunId": run_id or None,
            "publishedAt": published_at or None,
            "toolchain": toolchain or None,
        },
        "sourceVersion": project_source_version(project_root),
    }


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser()
    result.add_argument("--project-root", type=pathlib.Path, default=pathlib.Path.cwd())
    sub = result.add_subparsers(dest="command", required=True)
    sub.add_parser("check")
    sub.add_parser("sync")
    version = sub.add_parser("version")
    version.add_argument("--source-sha")
    version.add_argument("--kind", choices=("snapshot", "stable"), default="snapshot")
    version.add_argument("--tag")
    manifest = sub.add_parser("manifest")
    manifest.add_argument("--artifact-version", required=True)
    manifest.add_argument("--source-sha", required=True)
    manifest.add_argument("--run-id")
    manifest.add_argument("--published-at")
    manifest.add_argument("--toolchain")
    manifest.add_argument("--output", type=pathlib.Path, required=True)
    return result


def main(argv: list[str] | None = None) -> int:
    args = parser().parse_args(argv)
    root = args.project_root.resolve()
    try:
        if args.command in {"check", "sync"}:
            check(root)
            print("release metadata is consistent")
        elif args.command == "version":
            base = project_version(root)
            sha = require_sha(args.source_sha or git_sha(root))
            if args.kind == "stable":
                expected_tag = f"v{base}"
                if args.tag != expected_tag:
                    raise ValueError(f"stable release requires tag {expected_tag}")
                print(base)
            else:
                print(snapshot_version(base, sha))
        else:
            data = release_manifest(
                project_root=root,
                artifact_version=args.artifact_version,
                source_sha=require_sha(args.source_sha),
                run_id=args.run_id,
                published_at=args.published_at,
                toolchain=args.toolchain,
            )
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
    except (ValueError, subprocess.CalledProcessError) as error:
        print(str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
