# `@ilic/repository-core`

Platform-neutral INTERLIS repository resolution. The package owns traversal,
version selection, dependency ordering, cache policy and diagnostics, but never
performs network, filesystem or wall-clock access by itself.

## Ports

`RepositoryManagerCore` requires five small host-provided ports:

| Port | Contract |
| --- | --- |
| `transport.get(request)` | Return bytes and HTTP-like status metadata; enforce the supplied size and redirect limits. |
| `cache` | Read and write `{ value: Uint8Array, storedAt }` entries by normalized URI. |
| `clock.now()` | Return the current time in milliseconds for deterministic TTL decisions. |
| `metadataDecoder` | Decode `ilimodels.xml` and `ilisite.xml` without doing I/O. |
| `checksum.md5(bytes)` | Calculate the model checksum; it may return a value or a promise. |

The optional `observer` receives every emitted repository diagnostic. An
`AbortSignal` can be supplied per manager or per resolution call.

## Resolver contract

`resolveModel()` and `resolveWorkspace()` normalize repository URIs, traverse
configured repositories and optional site links, select the latest compatible
model version, resolve dependencies first and return UTF-8 sources in dependency
order. The built-in `INTERLIS` model is not downloaded. Unsafe relative paths,
cycles, missing models, invalid UTF-8, checksum mismatches and configured safety
limits fail explicitly.

The returned workspace contains `metadata`, canonical `uri`, `source`,
`fromCache` and `stale` for every downloaded model. The package does not write
the sources to disk and does not compile them.

## Cache behaviour

Metadata is fresh for one day and model files for seven days by default. A
fresh entry is returned without transport access. Offline mode accepts an
existing entry and marks an expired one as stale; a cache miss fails. Online
mode may use a stale entry after a transient transport error when
`allowStaleOnError` is enabled. Checksum-invalid entries are discarded and are
never used as stale fallbacks. Cache read/write failures become warnings so a
working transport remains usable.

All TTLs, byte limits, graph limits, redirect limits, site traversal, checksum
validation and stale fallback behaviour are configurable through `policy`.

## Errors

Resolution failures throw `RepositoryError` with structured `diagnostics` and,
where available, a `partialWorkspace`. `listModels()` throws an `AggregateError`
when configured repositories exist but none is available. Diagnostics use
stable `ILIC-REPO-*` codes; callers should not parse message text.

## Minimal example

```js
import {
  ManualRepositoryClock,
  Md5RepositoryChecksum,
  RepositoryManagerCore,
  SafeRepositoryMetadataDecoder,
} from "@ilic/repository-core";

const clock = new ManualRepositoryClock(Date.now());
const entries = new Map();
const cache = {
  async get(key) { return entries.get(key); },
  async putEntry(key, entry) { entries.set(key, entry); },
  async clear() { entries.clear(); },
};
const transport = {
  async get(request) {
    const response = await fetch(request.uri, { signal: request.signal });
    const body = new Uint8Array(await response.arrayBuffer());
    return {
      success: response.ok,
      statusCode: response.status,
      body,
      finalUri: response.url,
      error: response.ok ? "" : response.statusText,
      notFound: response.status === 404,
      retryable: response.status === 429 || response.status >= 500,
    };
  },
};
const decoder = new SafeRepositoryMetadataDecoder();
const manager = new RepositoryManagerCore({
  repositories: ["https://models.interlis.ch"],
  transport,
  cache,
  clock,
  metadataDecoder: decoder,
  checksum: new Md5RepositoryChecksum(),
});

const workspace = await manager.resolveModel("Units", "ili2_4");
```

See `index.d.ts` for the complete port and policy types.
