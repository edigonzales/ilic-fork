# Native C ABI, JSON protocol and WebAssembly

The compiler core is exposed through the C header `include/ilic/capi.h`. The
same ABI is linked natively by `ilic-capi` and exported from the Emscripten
module. This keeps compilation and formatting semantics independent of the
JavaScript host.

## Handle and buffer ownership

1. Create a session with `ilic_session_create`.
2. Add UTF-8 buffers with `ilic_session_put_source`. The compiler copies URI
   and source bytes, so the caller may release its buffers immediately.
3. Call `ilic_compile` or `ilic_format` with UTF-8 JSON matching a schema in
   `schemas/`.
4. Read the immutable UTF-8 result using `ilic_result_json`. Its pointer stays
   valid until `ilic_result_destroy` is called for that handle.
5. Destroy sessions with `ilic_session_destroy`.

All source positions in JSON are zero-based. `character` uses UTF-16 code units
for direct use in the Language Server Protocol; `byteOffset` is the UTF-8 byte
offset. The integer returned by `ilic_abi_version` and `schemaVersion` are
versioned independently. An incompatible change requires a new ABI or schema
version instead of silently changing version 1.

## Building WebAssembly

The supported Emscripten release is pinned in `.emscripten-version`. With this
release active on `PATH`, run:

```sh
./scripts/build-wasm.sh
npm test --prefix packages/compiler-wasm
```

The script builds `build/wasm/ilic.mjs` and `ilic.wasm` and copies both generated
artifacts into `packages/compiler-wasm` for local testing or packaging. These
generated files are intentionally ignored by Git.

`@ilic/compiler-wasm` provides one compiler module with any number of explicit
sessions. A source need not exist on disk:

```js
import { createCompiler } from "@ilic/compiler-wasm";

const compiler = await createCompiler();
const session = compiler.createSession();
session.putSource("memory:///Example.ili", sourceText, documentVersion);
const result = session.compile({ roots: ["memory:///Example.ili"] });
session.dispose();
```

Repository I/O remains outside the sandboxed WASM module. In browsers and Node,
`@ilic/tools` resolves and caches the dependency workspace and
`session.putWorkspace(workspace)` copies it into the compiler. The native
command-line executable uses the C++ repository implementation directly.

For browser LSPs, `@ilic/compiler-wasm/worker` offers the same session operations
over a small request/response protocol. Running the compiler in a worker keeps
parsing and semantic validation off the editor UI thread and gives the host a
hard cancellation boundary by terminating the worker.
