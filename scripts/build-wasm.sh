#!/usr/bin/env sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
expected_version=$(tr -d '[:space:]' < "$project_dir/.emscripten-version")

die() {
  echo "build-wasm: $*" >&2
  exit 2
}

emcc_version() {
  if ! command -v emcc >/dev/null 2>&1; then
    return 1
  fi
  emcc --version 2>/dev/null | sed -n '1s/.*) \([0-9][0-9.]*\).*/\1/p'
}

has_expected_emcc() {
  actual_version=$(emcc_version || true)
  [ "$actual_version" = "$expected_version" ]
}

emsdk_dir() {
  if [ -n "${ILIC_EMSDK_DIR:-}" ]; then
    printf '%s\n' "$ILIC_EMSDK_DIR"
  elif [ -n "${EMSDK:-}" ] && [ -d "$EMSDK" ]; then
    printf '%s\n' "$EMSDK"
  else
    printf '%s\n' "$project_dir/../emsdk"
  fi
}

ensure_emscripten() {
  if has_expected_emcc; then
    actual_version=$(emcc_version)
    echo "Using Emscripten $actual_version from PATH."
    return
  fi

  auto_setup=${ILIC_WASM_AUTO_SETUP:-1}
  case "$auto_setup" in
    0|1) ;;
    *) die "ILIC_WASM_AUTO_SETUP must be 0 or 1, got '$auto_setup'." ;;
  esac

  sdk_dir=$(emsdk_dir)
  if [ -e "$sdk_dir" ] && [ ! -f "$sdk_dir/emsdk" ]; then
    die "SDK path exists but is not an emsdk checkout: $sdk_dir\nRemove it or set ILIC_EMSDK_DIR to a valid emsdk directory."
  fi

  if [ ! -e "$sdk_dir" ]; then
    if [ "$auto_setup" = 0 ]; then
      die "Emscripten $expected_version is not active and automatic SDK setup is disabled.\nSet ILIC_EMSDK_DIR, activate emsdk_env.sh, or unset ILIC_WASM_AUTO_SETUP."
    fi
    command -v git >/dev/null 2>&1 || die "git is required to install the Emscripten SDK automatically."
    mkdir -p "$(dirname "$sdk_dir")"
    echo "Cloning emsdk into $sdk_dir ..."
    git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$sdk_dir"
  fi

  [ -f "$sdk_dir/emsdk" ] || die "No emsdk executable found in $sdk_dir."
  [ -f "$sdk_dir/emsdk_env.sh" ] || die "No emsdk_env.sh found in $sdk_dir."

  if [ "$auto_setup" = 1 ]; then
    echo "Installing pinned Emscripten $expected_version ..."
    "$sdk_dir/emsdk" install "$expected_version"
  fi
  echo "Activating Emscripten $expected_version ..."
  "$sdk_dir/emsdk" activate "$expected_version"
  # shellcheck disable=SC1090
  . "$sdk_dir/emsdk_env.sh"

  has_expected_emcc || die "Expected Emscripten $expected_version after SDK activation, found ${actual_version:-unknown}."
  actual_version=$(emcc_version)
  echo "Using Emscripten $actual_version from $sdk_dir."
}

ensure_emscripten

command -v emcmake >/dev/null 2>&1 || die "emcmake is not available after activating Emscripten $expected_version."
command -v cmake >/dev/null 2>&1 || die "cmake is required to build the WASM compiler."

wasm_build_dir="$project_dir/build/wasm"
emcc_path=$(command -v emcc)
emscripten_dir=$(CDPATH= cd -- "$(dirname "$emcc_path")" && pwd)
expected_toolchain="$emscripten_dir/cmake/Modules/Platform/Emscripten.cmake"
wasm_cache="$wasm_build_dir/CMakeCache.txt"
wasm_system=$(find "$wasm_build_dir/CMakeFiles" -type f -name CMakeSystem.cmake -print -quit 2>/dev/null || true)

if [ -f "$wasm_cache" ] || [ -f "$wasm_system" ]; then
  cached_toolchain=$(sed -n 's/^CMAKE_TOOLCHAIN_FILE:[^=]*=//p' "$wasm_cache" 2>/dev/null | head -n 1 || true)
  cached_system_toolchain=$(sed -n 's/^include("\(.*\/Emscripten\.cmake\)")/\1/p' "$wasm_system" 2>/dev/null | head -n 1 || true)
  if { [ -n "$cached_toolchain" ] && [ "$cached_toolchain" != "$expected_toolchain" ]; } ||
    { [ -n "$cached_system_toolchain" ] && [ "$cached_system_toolchain" != "$expected_toolchain" ]; }; then
    echo "Resetting stale WASM CMake cache for Emscripten toolchain $expected_toolchain."
    rm -rf "$wasm_build_dir"
  fi
fi

emcmake cmake -S "$project_dir" -B "$wasm_build_dir" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF \
  -DILIC_ENABLE_NATIVE_REPOSITORY=OFF
cmake --build "$wasm_build_dir" --target ilic-wasm --parallel
cmake -E copy_if_different "$wasm_build_dir/ilic.mjs" \
  "$project_dir/packages/compiler-wasm/ilic.mjs"
cmake -E copy_if_different "$wasm_build_dir/ilic.wasm" \
  "$project_dir/packages/compiler-wasm/ilic.wasm"
