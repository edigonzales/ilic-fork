# P7 architecture boundaries

This document records the implemented P7 boundaries in the three-repository
workspace. It is deliberately implementation-oriented: public C++/C ABI,
WASM, npm and language-service contracts remain the source of truth.

## Native compiler

`CompilerSession` is a thin lock/lifecycle façade. Mutable session state lives
in `CompilerSessionState`; source invalidation is owned by
`SourceUpdateCoordinator`; syntax/editor materialization by `SnapshotService`;
compilation by `CompilationOrchestrator` and `CompilationRun`; transcript
formatting by `CompilationTranscript`; and external meta attributes by
`ExternalMetaAttributeApplier`.

The C ABI is split into `CapiRegistries`, `CapiRequestDecoder` and
`CapiJsonProjectors`. `Capi.cpp` only translates handles, requests and
exceptions to the stable ABI. The Emscripten target compiles the same C ABI
components as the native static library.

## WASM wrapper

`index.js` is a public export façade. `CompilerSession` owns the session
lifecycle, `wasm-memory.js` owns pointer/result handling, and
`legacy-editor-projection.js` is an explicit compatibility path. The native
editor snapshot is preferred whenever the ABI advertises it.

## Verification

The architecture guard is `scripts/check-p7-architecture.mjs`. Run it normally
for a report, with `--canary` for the negative guard, and with `--strict` when
all target budgets are expected to be met.

The current work deliberately reports remaining façade-budget exceptions for
the legacy LanguageService, compiler-worker and WebIDE workbench. Their new
component boundaries are present and tested, but those legacy owners still
need the full state migration before P7 can be declared complete.
