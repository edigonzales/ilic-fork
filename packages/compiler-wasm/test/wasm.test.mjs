import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import test from "node:test";
import { createCompiler } from "../index.js";

const modulePath = resolve(import.meta.dirname, "../ilic.mjs");

test("compiles and formats through the real WASM ABI", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///WasmModel.ili";
  session.putSource(uri, `INTERLIS 2.3;
!! kept
MODEL WasmModel AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      value : TEXT;
    END Item;
  END Topic;
END WasmModel.
`, 7);
  const result = session.compile({ roots: [uri] });
  assert.equal(result.success, true, JSON.stringify(result.diagnostics));
  assert.ok(result.models.some(model => model.name === "WasmModel"));
  assert.equal(result.schemaVersion, 1);
  assert.equal(result.abiVersion, 1);
  assert.match(result.transcript.join("\n"), /ilic completed with no errors, no warnings \d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}/);
  assert.doesNotMatch(result.transcript.join("\n"), /compiler run (done|failed)/);

  const syntax = session.parse(uri);
  assert.equal(syntax.kind, "syntax");
  assert.equal(syntax.documentVersion, 7);
  assert.equal(syntax.iliVersion, "2.3");
  assert.ok(syntax.tokens.length > 0);
  assert.ok(syntax.nodes.some(node => node.kind === "modelDef"));

  const editor = session.editorSnapshot(uri);
  assert.equal(editor.kind, "editor");
  assert.equal(editor.documentVersion, 7);
  assert.deepEqual(
    editor.declarations.map(declaration => declaration.qualifiedName),
    [
      "WasmModel",
      "WasmModel.Topic",
      "WasmModel.Topic.Item",
      "WasmModel.Topic.Item.value",
    ],
  );
  assert.equal(
    JSON.stringify(editor).length < JSON.stringify(syntax).length,
    true,
  );

  const semantic = session.analyze({ roots: [uri] });
  assert.equal(semantic.kind, "semantic");
  assert.equal(semantic.success, true, JSON.stringify(semantic.diagnostics));
  const model = semantic.symbols.find(symbol => symbol.qualifiedName === "WasmModel");
  assert.ok(model);
  assert.equal(model.endRange?.start.line, 8);
  assert.equal(model.endRange?.start.character, 4);
  assert.equal(model.endRange?.end.character, 13);
  const topic = semantic.symbols.find(symbol => symbol.qualifiedName === "WasmModel.Topic");
  assert.ok(topic);
  assert.equal(topic.endRange?.start.line, 7);
  assert.equal(topic.endRange?.start.character, 6);
  const item = semantic.symbols.find(symbol => symbol.qualifiedName === "WasmModel.Topic.Item");
  assert.ok(item);
  assert.equal(item.endRange?.start.line, 6);
  assert.equal(item.endRange?.start.character, 8);
  assert.equal(semantic.documentVersions[uri], 7);
  assert.equal(semantic.documentation.models?.length, 1);
  assert.equal(semantic.documentation.models[0].name, "WasmModel");
  assert.deepEqual(
    semantic.documentation.models[0].topics.map(topic => topic.name),
    ["Topic"],
  );
  assert.deepEqual(
    semantic.documentation.models[0].topics[0].viewables[0].rows[0],
    {
      name: "value",
      cardinality: "0..1",
      type: "Text",
      description: "",
    },
  );

  const combined = session.compileAndAnalyze({ roots: [uri] });
  assert.equal(combined.kind, "compilation-analysis");
  assert.equal(combined.compilation.success, true);
  assert.equal(combined.semantic.success, true);
  assert.deepEqual(combined.semantic.diagnostics, combined.compilation.diagnostics);
  assert.deepEqual(combined.syntax.map(snapshot => snapshot.uri), [uri]);
  const itemNode = combined.semantic.diagram.nodes.find(node =>
    node.id === "class:WasmModel.Topic.Item");
  assert.ok(itemNode);
  assert.deepEqual(itemNode.stereotypes, []);
  assert.deepEqual(itemNode.operations, []);
  assert.deepEqual(itemNode.members[0], {
    name: "value",
    type: "TEXT",
    cardinality: "0..1",
    declaringType: "",
    inherited: false,
    inlineEnumValues: [],
  });

  const formatted = session.format(uri);
  assert.equal(formatted.success, true);
  assert.match(formatted.text, /!! kept/);

  const invalidUri = "memory:///WäsmInvalid.ili";
  session.putSource(invalidUri, `INTERLIS 2.3;
MODEL WaesmInvalid AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      Bezeichnung : UnbekannterTyp;
    END Item;
  END Topic;
END WaesmInvalid.
`);
  const invalid = session.compile({ roots: [invalidUri] });
  assert.equal(invalid.success, false);
  assert.match(invalid.transcript.join("\n"), /ilic completed with \d+ errors?, no warnings \d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}/);
  assert.doesNotMatch(invalid.transcript.join("\n"), /compiler run (done|failed)/);
  assert.ok(invalid.diagnostics.some(diagnostic =>
    diagnostic.range?.uri === invalidUri && diagnostic.range.start.line === 4));
  session.dispose();
});

test("validates numeric range bounds through the WASM ABI", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const validUri = "memory:///WasmNumericRangeValid.ili";
  const invalidUri = "memory:///WasmNumericRangeInvalid.ili";
  session.putSource(validUri, `INTERLIS 2.4;
MODEL WasmNumericRangeValid (en) AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      equal : 0..0;
      negative : -9..0;
      scientific : 1.0E2..100;
    END Item;
  END Topic;
END WasmNumericRangeValid.
`);
  session.putSource(invalidUri, `INTERLIS 2.4;
MODEL WasmNumericRangeInvalid (en) AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      attr1 : 0..-9;
    END Item;
  END Topic;
END WasmNumericRangeInvalid.
`);

  const valid = session.compile({ roots: [validUri] });
  assert.equal(valid.success, true, JSON.stringify(valid.diagnostics));
  const invalid = session.compile({ roots: [invalidUri] });
  assert.equal(invalid.success, false, JSON.stringify(invalid.diagnostics));
  assert.ok(invalid.diagnostics.some(diagnostic =>
    diagnostic.message.includes("minimum value must not exceed maximum value")));
  session.dispose();
});

test("keeps the WASM session usable while an editor buffer is incomplete", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///LiveEdit.ili";
  const source = `INTERLIS 2.4;
MODEL LiveEdit (en) AT "https://example.invalid" VERSION "1" =
END LiveEdit.
`;
  for (let length = 1; length < source.length; length += 3) {
    session.putSource(uri, source.slice(0, length), length);
    const syntax = session.parse(uri);
    assert.equal(syntax.kind, "syntax");
    assert.equal(syntax.documentVersion, length);
  }
  session.putSource(uri, source, source.length);
  const syntax = session.parse(uri);
  assert.equal(syntax.success, true, JSON.stringify(syntax.diagnostics));
  const compilation = session.compile({ roots: [uri] });
  assert.equal(
    compilation.success,
    true,
    JSON.stringify(compilation.diagnostics),
  );
  session.dispose();
});

test("exports functions and views as stereotyped diagram nodes", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///WasmDiagram.ili";
  session.putSource(uri, `INTERLIS 2.4;
MODEL WasmDiagram (en) AT "https://example.invalid" VERSION "1" =
  FUNCTION RootFunction(value : TEXT) : TEXT;
  TOPIC Data =
    CLASS Source =
      Name : TEXT;
    END Source;
    FUNCTION TopicFunction(value : TEXT) : TEXT;
    VIEW Overview
      PROJECTION OF source ~ Source;
    =
      ATTRIBUTE ALL OF source;
    END Overview;
  END Data;
END WasmDiagram.
`);

  const semantic = session.analyze({ roots: [uri] });
  assert.equal(semantic.success, true, JSON.stringify(semantic.diagnostics));
  const modelScope = semantic.diagram.nodes.find(node =>
    node.kind === "modelScope");
  const topic = semantic.diagram.nodes.find(node =>
    node.kind === "topic" && node.label === "Data (WasmDiagram)");
  assert.ok(modelScope);
  assert.ok(topic);
  assert.ok(semantic.diagram.nodes.some(node =>
    node.kind === "function" &&
    node.label === "RootFunction" &&
    node.containerId === modelScope.id &&
    node.stereotypes.includes("Function")));
  assert.ok(semantic.diagram.nodes.some(node =>
    node.kind === "function" &&
    node.label === "TopicFunction" &&
    node.containerId === topic.id &&
    node.stereotypes.includes("Function")));
  assert.ok(semantic.diagram.nodes.some(node =>
    node.kind === "view" &&
    node.label === "Overview" &&
    node.containerId === topic.id &&
    node.stereotypes.includes("View")));
  session.dispose();
});

test("exports inheritance references for EXTENDED attributes", {
  skip: existsSync(modulePath) ? false : "WASM package artifacts have not been built"
}, async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "memory:///Extended.ili";
  session.putSource(uri, `INTERLIS 2.4;
MODEL Extended AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Data =
    CLASS Base (ABSTRACT) =
      Name : TEXT * 80;
    END Base;
    CLASS Child EXTENDS Base =
      Name (EXTENDED) : TEXT * 80;
    END Child;
  END Data;
END Extended.
`, 1);
  const result = session.compileAndAnalyze({ roots: [uri] });
  assert.equal(result.compilation.success, true, JSON.stringify(result.compilation.diagnostics));
  const base = result.semantic.symbols.find(symbol =>
    symbol.qualifiedName === "Extended.Data.Base.Name");
  const child = result.semantic.symbols.find(symbol =>
    symbol.qualifiedName === "Extended.Data.Child.Name");
  assert.ok(base);
  assert.ok(child);
  assert.ok(result.semantic.references.some(reference =>
    reference.kind === "inheritance" &&
    reference.sourceId === child.id &&
    reference.targetId === base.id &&
    reference.range?.uri === uri));
  session.dispose();
});

test("projects imports, references, END names, and Unicode positions for editors", async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "file:///editor.ili";
  session.putSource(
    uri,
    `INTERLIS 2.4;
!! Zürich 😀
MODEL Editor (de) AT "https://example.com" VERSION "1" =
  IMPORTS UNQUALIFIED Base, Units;
  TOPIC T =
    CLASS C EXTENDS Base.Root =
      values: LIST {0..*} OF S;
      parent: REFERENCE TO Base.Root;
    END C;
    STRUCTURE S =
    END S;
  END T;
END Editor.
`,
    11,
  );

  const snapshot = session.editorSnapshot(uri);
  assert.equal(snapshot.schemaVersion, 1);
  assert.equal(snapshot.documentVersion, 11);
  assert.deepEqual(
    snapshot.imports.map(entry => [entry.model, entry.unqualified]),
    [["Base", true], ["Units", true]],
  );
  assert.deepEqual(
    snapshot.references.map(reference => [reference.kind, reference.text]),
    [
      ["extends", "Base.Root"],
      ["collection", "S"],
      ["reference", "Base.Root"],
    ],
  );
  const model = snapshot.declarations.find(value => value.name === "Editor");
  assert.equal(model?.endRange?.start.line, 12);
  assert.equal(
    model?.selectionRange.start.byteOffset,
    new TextEncoder().encode("INTERLIS 2.4;\n!! Zürich 😀\nMODEL ").length,
  );
  assert.deepEqual(
    snapshot.contexts.map(context => context.kind),
    ["modelDef", "topicDef", "classDef", "structureDef"],
  );
  session.dispose();
});

test("keeps editor diagnostics limited to structurally safe findings", async () => {
  const compiler = await createCompiler();
  const session = compiler.createSession();
  const uri = "file:///editor-diagnostics.ili";
  session.putSource(uri, `INTERLIS 2.4;
MODEL Diagnostics =
  CLASS Item =
  END Wrong;
  CLASS Duplicate =
  END Duplicate;
  CLASS Duplicate =
  END Duplicate;
END Diagnostics.
`, 2);
  const snapshot = session.editorSnapshot(uri);
  assert.deepEqual(
    snapshot.diagnostics.map(diagnostic => diagnostic.code),
    ["ILIC-LIVE-END-NAME", "ILIC-LIVE-DUPLICATE"],
  );
  assert.equal(
    snapshot.diagnostics.every(value => value.source === "live"),
    true,
  );
  session.dispose();
});

test("keeps editor snapshots within the interactive latency budget", async () => {
  const compiler = await createCompiler();
  for (const [count, budget] of [[550, 150], [3_500, 750]]) {
    const session = compiler.createSession();
    const uri = `file:///performance-${count}.ili`;
    const attributes = Array.from(
      { length: count },
      (_, index) => `    attribute${index}: TEXT*20;`,
    ).join("\n");
    const source = `INTERLIS 2.4;
MODEL Performance (en) AT "https://example.com" VERSION "1" =
  TOPIC T =
    CLASS C =
${attributes}
    END C;
  END T;
END Performance.
`;
    session.putSource(uri, source, 1);
    const started = performance.now();
    const snapshot = session.editorSnapshot(uri);
    const elapsed = performance.now() - started;
    assert.equal(snapshot.declarations.length, count + 3);
    assert.ok(
      elapsed < budget,
      `${new TextEncoder().encode(source).length} bytes took ${elapsed.toFixed(1)} ms (budget ${budget} ms)`,
    );
    session.dispose();
  }
});
