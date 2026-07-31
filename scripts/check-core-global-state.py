#!/usr/bin/env python3
"""Reject the process-global compiler state removed by P2.

This is intentionally a small source-level guard.  It is not a C++ parser and
therefore reports suspicious declarations for human review instead of trying
to prove ownership from syntax alone.
"""

from __future__ import annotations

import pathlib
import re
import sys


SOURCE_ROOTS = (
    "include/ilic",
    "source/core",
    "source/input",
    "source/metamodel",
    "source/output",
    "source/util",
    "source/main",
)
EXTENSIONS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".inc"}
EXCLUDED_PARTS = {"bck", "bck2", "generated"}

FORBIDDEN = (
    re.compile(r"\bextern\s+(?:util::)?Logger\s+Log\b"),
    re.compile(r"\bLogger\s+Log\b"),
    re.compile(r"\bcompilerMutex\b"),
    re.compile(r"\bactiveSourceManager\b"),
    re.compile(r"\bsetActiveSourceManager\b"),
    re.compile(r"\bActiveSourceManagerScope\b"),
    re.compile(r"\ball_ilifiles(?:_full)?\b"),
    re.compile(r"\ball_ilimodels\b"),
    re.compile(r"\bresetCompilerState\b"),
    re.compile(r"\breset_compiler_state\b"),
    re.compile(r"\breset_input_state\b"),
    re.compile(r"\breset_mmobjects\b"),
    re.compile(r"\bregister_mmobject\b"),
    re.compile(r"\bdestroy_mmobject\b"),
    re.compile(r"\bpush_context\b"),
    re.compile(r"\bpop_context\b"),
    re.compile(r"\bget_class_context\b"),
    re.compile(r"\bget_package_context\b"),
    re.compile(r"\bget_topic_context\b"),
    re.compile(r"\bget_model_context\b"),
    re.compile(r"\binput_file\b"),
    re.compile(r"\bUniversalClassesInitialized\b"),
    re.compile(r"\bCurrentSourceText\b"),
    re.compile(r"\bPendingMetaAttributes\b"),
    re.compile(r"\bPendingDocumentation\b"),
)

# These declarations are only suspicious at namespace/file scope.  Mutable
# members and local temporaries are expected and are intentionally ignored.
MUTABLE_FILE_STATIC = re.compile(
    r"^\s*(?:static\s+)?(?:thread_local\s+)?"
    r"(?:bool|char|double|float|int|long|size_t|std::string|string|"
    r"std::map<|std::unordered_map<|std::vector<|std::list<|std::set<|"
    r"std::unique_ptr<)"
)


def files_to_scan(root: pathlib.Path):
    for relative_root in SOURCE_ROOTS:
        directory = root / relative_root
        if not directory.exists():
            continue
        for path in directory.rglob("*"):
            if not path.is_file() or path.suffix not in EXTENSIONS:
                continue
            if any(part in EXCLUDED_PARTS for part in path.relative_to(root).parts):
                continue
            yield path


def strip_comments(line: str, in_block: bool) -> tuple[str, bool]:
    result: list[str] = []
    index = 0
    quote = ""
    while index < len(line):
        if in_block:
            end = line.find("*/", index)
            if end < 0:
                return "".join(result), True
            index = end + 2
            in_block = False
            continue
        character = line[index]
        if quote:
            result.append(character)
            if character == "\\" and index + 1 < len(line):
                result.append(line[index + 1])
                index += 2
                continue
            if character == quote:
                quote = ""
            index += 1
            continue
        if character in ("'", '"'):
            quote = character
            result.append(character)
            index += 1
        elif line.startswith("//", index):
            break
        elif line.startswith("/*", index):
            in_block = True
            index += 2
        else:
            result.append(character)
            index += 1
    return "".join(result), in_block


def brace_depth(line: str, depth: int) -> int:
    # Quoted text is removed before the count, so model strings cannot alter
    # the result.
    line = re.sub(r"\"(?:\\.|[^\"])*\"", "", line)
    line = re.sub(r"'(?:\\.|[^'])*'", "", line)
    return depth + line.count("{") - line.count("}")


def main(root: pathlib.Path) -> int:
    violations: list[str] = []
    for path in sorted(files_to_scan(root)):
        relative = path.relative_to(root).as_posix()
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except UnicodeDecodeError:
            continue

        depth = 0
        block_comment = False
        parenthesis_depth = 0
        for line_number, original in enumerate(lines, 1):
            line, block_comment = strip_comments(original, block_comment)
            if not line.strip():
                continue

            for pattern in FORBIDDEN:
                if pattern.search(line):
                    violations.append(f"{relative}:{line_number}: forbidden compiler state: {original.strip()}")

            if re.search(r"\bthread_local\b", line):
                violations.append(f"{relative}:{line_number}: thread_local compiler state: {original.strip()}")

            if depth == 0 and parenthesis_depth == 0 and MUTABLE_FILE_STATIC.match(line):
                if not re.search(r"\bconst\b|\bconstexpr\b", line):
                    # Function declarations are not variables and are safe.
                    if "(" not in line.split("=", 1)[0]:
                        violations.append(f"{relative}:{line_number}: mutable file-scope state: {original.strip()}")

            depth = brace_depth(line, depth)
            parenthesis_line = re.sub(r"\"(?:\\.|[^\"])*\"", "", line)
            parenthesis_line = re.sub(r"'(?:\\.|[^'])*'", "", parenthesis_line)
            parenthesis_depth += parenthesis_line.count("(") - parenthesis_line.count(")")

    if violations:
        print("P2 compiler global-state guard failed:", file=sys.stderr)
        print("\n".join(violations), file=sys.stderr)
        return 1
    print("P2 compiler global-state guard passed")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} SOURCE_ROOT", file=sys.stderr)
        sys.exit(2)
    sys.exit(main(pathlib.Path(sys.argv[1]).resolve()))
