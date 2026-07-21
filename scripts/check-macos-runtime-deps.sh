#!/usr/bin/env bash
set -euo pipefail

binary=${1:?usage: check-macos-runtime-deps.sh /path/to/ilic}
test "$(uname -m)" = "arm64"
test -x "$binary"

unexpected=$(
  otool -L "$binary" | tail -n +2 | awk '{print $1}' \
    | grep -Ev '^(/usr/lib/|/System/Library/)' || true
)
if [[ -n "$unexpected" ]]; then
  printf 'unexpected non-system runtime dependencies:\n%s\n' "$unexpected" >&2
  exit 1
fi
