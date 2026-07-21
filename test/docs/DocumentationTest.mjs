import assert from "node:assert/strict";
import { existsSync, readFileSync, readdirSync, statSync } from "node:fs";
import { dirname, join, resolve } from "node:path";

const sourceRoot = resolve(process.argv[2] ?? ".");
const docsRoot = join(sourceRoot,"docs");
const required = [
  "README.md","funktionsumfang.md","build-und-installation.md","cli.md","formatter.md",
  "repositories.md","diagnostik-und-logging.md","native-api.md","wasm.md","conformance.md",
  "build-und-publikationspipeline.md","npm-publikation.md","examples/README.md"
];
for (const file of required) assert.ok(existsSync(join(docsRoot,file)),`missing docs/${file}`);

function markdownFiles(directory) {
  return readdirSync(directory).flatMap(name => {
    const path = join(directory,name);
    return statSync(path).isDirectory() ? markdownFiles(path) : path.endsWith(".md") ? [path] : [];
  });
}

function headingAnchors(markdown) {
  const anchors = new Set();
  const occurrences = new Map();
  for (const match of markdown.matchAll(/^#{1,6}\s+(.+?)\s*#*$/gm)) {
    const base = match[1]
      .replace(/\[([^\]]+)\]\([^)]+\)/g,"$1")
      .replace(/[`*_~]/g,"")
      .toLowerCase()
      .replace(/[^\p{L}\p{N}\s-]/gu,"")
      .trim()
      .replace(/\s+/g,"-");
    const occurrence = occurrences.get(base) ?? 0;
    occurrences.set(base,occurrence + 1);
    anchors.add(occurrence === 0 ? base : `${base}-${occurrence}`);
  }
  return anchors;
}

const checkedMarkdown = [
  ...markdownFiles(docsRoot),
  join(sourceRoot,"readme.md"),
  join(sourceRoot,"doc/dev/readme.md"),
  join(sourceRoot,"doc/dev/embedding.md"),
  join(sourceRoot,"packages/compiler-wasm/README.md"),
  join(sourceRoot,"packages/tools/README.md")
];

for (const markdown of checkedMarkdown) {
  const text = readFileSync(markdown,"utf8");
  const fences = text.match(/^```/gm) ?? [];
  assert.equal(fences.length % 2,0,`${markdown}: unclosed fenced code block`);
  assert.doesNotMatch(text,/\/Users\/stefan\//,`${markdown}: contains a machine-specific path`);
  for (const match of text.matchAll(/\[[^\]]*\]\(([^)]+)\)/g)) {
    let target = match[1].trim();
    if (target.startsWith("<") && target.endsWith(">")) target = target.slice(1,-1);
    if (!target || /^(?:https?:|mailto:)/.test(target)) continue;
    const [targetPath,fragment = ""] = target.split("#",2);
    const resolved = targetPath ? resolve(dirname(markdown),decodeURIComponent(targetPath)) : markdown;
    assert.ok(existsSync(resolved),
      `${markdown}: broken relative link ${match[1]}`);
    if (fragment && resolved.endsWith(".md")) {
      assert.ok(headingAnchors(readFileSync(resolved,"utf8")).has(decodeURIComponent(fragment)),
        `${markdown}: broken heading link ${match[1]}`);
    }
  }
}

const cli = readFileSync(join(docsRoot,"cli.md"),"utf8");
for (const actual of ["-no_auto","-no_warnings","-ident","-model_filter"])
  assert.match(cli,new RegExp(actual),`CLI reference must contain ${actual}`);
console.log(`validated ${checkedMarkdown.length} documentation pages`);
