#!/usr/bin/env sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
expected_version=$(tr -d '[:space:]' < "$project_dir/.emscripten-version")
actual_version=$(emcc --version | sed -n '1s/.*emcc ([^)]*) //p')

case "$actual_version" in
  "$expected_version"*) ;;
  *)
    echo "Expected Emscripten $expected_version, found ${actual_version:-unknown}" >&2
    exit 2
    ;;
esac

emcmake cmake -S "$project_dir" -B "$project_dir/build/wasm" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF
cmake --build "$project_dir/build/wasm" --target ilic-wasm --parallel
cmake -E copy_if_different "$project_dir/build/wasm/ilic.mjs" \
  "$project_dir/packages/compiler-wasm/ilic.mjs"
cmake -E copy_if_different "$project_dir/build/wasm/ilic.wasm" \
  "$project_dir/packages/compiler-wasm/ilic.wasm"
