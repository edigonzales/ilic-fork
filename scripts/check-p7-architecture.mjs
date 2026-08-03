#!/usr/bin/env node

import fs from "node:fs";
import path from "node:path";

const root = path.resolve(new URL("..", import.meta.url).pathname);
const languageTools = path.resolve(root, "../interlis-language-tools");
const webIde = path.resolve(root, "../interlis-web-ide");

const nonEmptyLines = (file) => fs.readFileSync(file, "utf8").split(/\r?\n/u).filter((line) => line.trim()).length;
const has = (file, pattern) => pattern.test(fs.readFileSync(file, "utf8"));
const check = (id, ok, detail) => ({ id, ok, detail });

const checks = [
  check("snapshot-pipeline-budget", nonEmptyLines(path.join(root, "source/core/SnapshotPipeline.cpp")) <= 180, "SnapshotPipeline.cpp <= 180 non-empty lines"),
  check("compiler-facade-budget", nonEmptyLines(path.join(root, "source/core/Compiler.cpp")) <= 300, "Compiler.cpp <= 300 non-empty lines"),
  check("capi-facade-budget", nonEmptyLines(path.join(root, "source/abi/Capi.cpp")) <= 300, "Capi.cpp <= 300 non-empty lines"),
  check("wasm-index-budget", nonEmptyLines(path.join(root, "packages/compiler-wasm/index.js")) <= 100, "compiler-wasm/index.js <= 100 non-empty lines"),
  check("compiler-no-generated-parser", !has(path.join(root, "source/core/Compiler.cpp"), /generated\/|Parser\.h/u), "Compiler.cpp does not include generated parser headers"),
  check("capi-no-projectors", !has(path.join(root, "source/abi/Capi.cpp"), /Value::Object|appendJsonDiagnostics|\n(?:Value|std::string)\s+semanticResult\s*\(/u), "Capi.cpp delegates JSON projection"),
  check("wasm-no-legacy-projection", !has(path.join(root, "packages/compiler-wasm/index.js"), /projectLegacyEditorSnapshot|legacyContainsRange/u), "index.js has no legacy projection"),
  check("language-service-budget", nonEmptyLines(path.join(languageTools, "packages/language-service/src/service.ts")) <= 500, "service.ts <= 500 non-empty lines"),
  check("worker-budget", nonEmptyLines(path.join(languageTools, "packages/language-service/src/compiler-worker.ts")) <= 200, "compiler-worker.ts <= 200 non-empty lines"),
  check("workbench-budget", nonEmptyLines(path.join(webIde, "src/workbench/workbench.ts")) <= 650, "workbench.ts <= 650 non-empty lines"),
  check("language-facade-no-legacy-state", !has(path.join(languageTools, "packages/language-service/src/service.ts"), /new Map|new Set|setTimeout|pendingCompilations/u), "service.ts owns no legacy maps, queues, or timers"),
  check("worker-facade-no-transport", !has(path.join(languageTools, "packages/language-service/src/compiler-worker.ts"), /new Map|pending|postMessage\(\{ id/u), "compiler-worker.ts owns no transport state"),
  check("workbench-facade-no-legacy-state", !has(path.join(webIde, "src/workbench/workbench.ts"), /new Map|new Set|setTimeout|AbortController|addEventListener/u), "workbench.ts owns no legacy maps, timers, abort controllers, or listeners"),
  check("language-component-boundaries", [
    "source/source-registry.ts", "syntax/syntax-snapshot-store.ts", "editor/editor-analysis-controller.ts",
    "semantic/semantic-snapshot-store.ts", "compilation/compilation-scheduler.ts",
    "compilation/compilation-run-coordinator.ts", "repository/repository-model-controller.ts",
    "diagnostics/diagnostic-coordinator.ts", "semantic/dependency-index.ts",
    "features/language-feature-coordinator.ts", "events/language-service-event-hub.ts",
    "workers/worker-rpc-client.ts", "workers/worker-source-mirror.ts", "workers/worker-lifecycle-tracker.ts",
  ].every((file) => fs.existsSync(path.join(languageTools, "packages/language-service/src", file))), "language-service component modules exist"),
  check("workbench-component-boundaries", [
    "command-registry.ts", "workbench-view.ts", "tab-controller.ts", "save-controller.ts", "recovery-controller.ts",
    "workbench-context.ts", "workspace-controller.ts", "compilation-controller.ts", "problems-controller.ts",
    "outline-controller.ts", "diagram-panel-controller.ts", "layout-controller.ts", "suggestion-controller.ts",
  ].every((file) => fs.existsSync(path.join(webIde, "src/workbench", file))), "workbench component modules exist"),
  check("native-component-boundaries", [
    "CompilerSessionState.cpp", "SourceUpdateCoordinator.cpp", "SnapshotService.cpp", "CompilationOrchestrator.cpp",
    "CompilationRun.cpp", "CompilationTranscript.cpp", "ExternalMetaAttributeApplier.cpp",
    "ParsedSourceArtifactFactory.cpp", "SnapshotParserSupport.cpp", "SyntaxSnapshotProjector.h", "EditorSnapshotProjector.h",
  ].every((file) => fs.existsSync(path.join(root, "source/core", file))), "native component modules exist"),
  check("capi-component-boundaries", ["CapiJsonProjectors.cpp", "CapiRegistries.cpp", "CapiRequestDecoder.cpp"].every((file) => fs.existsSync(path.join(root, "source/abi", file))), "C-ABI component modules exist"),
];

if (process.argv.includes("--canary")) {
  const catchesProjector = (source) => /Value::Object|semanticResult\s*\(/u.test(source);
  if (!catchesProjector("Value::Object{}")) process.exit(2);
  console.log("P7 negative canary passed: projector implementation is detected.");
  process.exit(0);
}

for (const result of checks) console.log(`${result.ok ? "PASS" : "FAIL"} ${result.id}: ${result.detail}`);
if (process.argv.includes("--strict") && checks.some((result) => !result.ok)) process.exit(1);
