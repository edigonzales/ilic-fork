import type { EditorSnapshot } from "../index.js";

// This intentionally models a pre-P3 consumer result. The additive P3 fields
// remain optional in the public type declaration for this compatibility case.
const legacyFixture: EditorSnapshot = {
  schemaVersion: 1,
  abiVersion: 1,
  compilerVersion: "fixture",
  kind: "editor",
  success: true,
  uri: "memory:///Example.ili",
  documentVersion: 1,
  iliVersion: "2.3",
  declarations: [],
  references: [],
  imports: [],
  contexts: [],
  diagnostics: [],
};

export default legacyFixture;
