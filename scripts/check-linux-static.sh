#!/bin/sh
set -eu

binary=${1:?usage: check-linux-static.sh /path/to/ilic}
test "$(uname -m)" = "x86_64"
test -x "$binary"
file "$binary" | grep -Eq 'ELF 64-bit.*x86-64.*statically linked'
ldd_output=$(ldd "$binary" 2>&1 || true)
if printf '%s\n' "$ldd_output" | grep -Eivq 'not a dynamic executable|statically linked|not a valid dynamic program'; then
  echo 'ldd reports dynamic runtime dependencies' >&2
  printf '%s\n' "$ldd_output" >&2
  exit 1
fi
if readelf -d "$binary" 2>/dev/null | grep -q '(NEEDED)'; then
  echo 'ELF binary contains NEEDED entries' >&2
  readelf -d "$binary" >&2
  exit 1
fi
