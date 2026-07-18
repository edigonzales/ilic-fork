import { pathToFileURL } from "node:url";
import { createCompiler } from "../../packages/compiler-wasm/index.js";

export async function runWasmSessionExample() {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///WasmExample.ili";
  session.putSource(uri, `INTERLIS 2.3;
!! This comment remains available to the formatter.
MODEL WasmExample AT "https://example.invalid/ilic/docs" VERSION "1" =
END WasmExample.
`, 1);

  try {
    const compilation = session.compile({ roots: [uri] });
    if (!compilation.success) throw new Error(JSON.stringify(compilation.diagnostics));
    const formatting = session.format(uri);
    if (!formatting.success) throw new Error(JSON.stringify(formatting.diagnostics));
    return { compilation, formatting };
  } finally {
    session.dispose();
  }
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const { compilation, formatting } = await runWasmSessionExample();
  console.log(`compiled=${compilation.success} formatted=${formatting.success}`);
}
