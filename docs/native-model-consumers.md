# Native model consumers

`ilic::ModelCompilation` is the small native C++ API for consumers that need the
compiled `metamodel::MetaModelStore` after compilation. It is additive; existing
`CompilerSession`, C-ABI, and WASM callers keep their current value-based result
contracts.

```cpp
#include "ilic/ModelCompilation.h"

#include <utility>

ilic::ModelCompilationInput input;
input.sources.push_back({
   "memory:///Example.ili",
   "INTERLIS 2.3; MODEL Example AT \"https://example.invalid\" VERSION \"1\" =\n"
   "END Example.\n",
   1
});
input.request.roots.push_back("memory:///Example.ili");

ilic::ModelCompilation compilation(std::move(input));
if (compilation.success()) {
   const auto &store = compilation.models();
   // Inspect store-backed model objects while `compilation` is alive.
}
```

The `ModelCompilation` object owns its `SourceManager`, compiler context, and
metamodel store. The store and all graph pointers returned by it remain valid
until the owning `ModelCompilation` is destroyed or move-assigned. Results and
source URI lists are value-based. Duplicate source URIs with different content
are rejected with `std::invalid_argument`; `models()` throws `std::logic_error`
for failed compilations.

Each instance owns independent state and can run concurrently with other
instances. A single instance is not a shared mutable session. The API is
intentionally native-only: it exposes C++ metamodel ownership and therefore is
not added to the C-ABI or the WASM JSON surface.
