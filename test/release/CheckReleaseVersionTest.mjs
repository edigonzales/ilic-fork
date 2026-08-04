import assert from "node:assert/strict";
import test from "node:test";

import {
  normalizeReleaseTag,
  readCMakeProjectVersion,
  validateReleaseVersionState,
} from "../../scripts/check-release-version.mjs";

function state(overrides = {}) {
  return {
    cmakeVersion: "0.9.10",
    repositoryCoreVersion: "0.9.10",
    toolsVersion: "0.9.10",
    compilerWasmVersion: "0.9.10",
    toolsRepositoryCoreDependency: "0.9.10",
    ...overrides,
  };
}

test("accepts a coordinated stable release version", () => {
  const result = validateReleaseVersionState(state(), "0.9.10", "v0.9.10");
  assert.equal(result.expectedVersion, "0.9.10");
});

test("reports each mismatched version source", () => {
  for (const [field, expectedMessage] of [
    ["cmakeVersion", /CMake project version/i],
    ["toolsRepositoryCoreDependency", /dependency @ilic\/repository-core/i],
  ]) {
    assert.throws(
      () =>
        validateReleaseVersionState(
          state({ [field]: "0.9.9" }),
          "0.9.10",
          "v0.9.10",
        ),
      expectedMessage,
    );
  }
  assert.throws(
    () =>
      validateReleaseVersionState(
        state({ compilerWasmVersion: "0.9.10-SNAPSHOT.20260804120000" }),
        "0.9.10",
        "v0.9.10",
      ),
    /@ilic\/compiler-wasm version/i,
  );
});

test("rejects malformed and mismatched tags", () => {
  assert.throws(() => normalizeReleaseTag("0.9.10"), /vX\.Y\.Z/i);
  assert.throws(
    () => validateReleaseVersionState(state(), "0.9.10", "v0.9.11"),
    /does not match/i,
  );
});

test("requires exactly one CMake version declaration", () => {
  assert.equal(
    readCMakeProjectVersion("project(ilic VERSION 0.9.10 LANGUAGES C CXX)"),
    "0.9.10",
  );
  assert.throws(() => readCMakeProjectVersion("project(other VERSION 1.0.0)"));
  assert.throws(() =>
    readCMakeProjectVersion(
      "project(ilic VERSION 0.9.10)\nproject(ilic VERSION 0.9.11)",
    ),
  );
});
