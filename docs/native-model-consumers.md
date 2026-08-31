# Native Konsumenten des Metamodells

`ilic::ModelCompilation` ist die kleine native C++-API für Konsumenten, die den
kompilierten `metamodel::MetaModelStore` nach der Kompilation benötigen. Die API
ist additiv; bestehende Aufrufer von `CompilerSession`, C-ABI und WASM behalten
ihre wertbasierten Ergebnisverträge.

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
   // Store-Objekte nur während der Lebensdauer von `compilation` verwenden.
}
```

Das `ModelCompilation`-Objekt besitzt seinen `SourceManager`, Compiler-Kontext
und Metamodell-Store. Der Store und alle daraus erhaltenen Graphzeiger bleiben
gültig, bis die besitzende `ModelCompilation` zerstört oder per Move neu
zugewiesen wird. Ergebnisse und Source-URI-Listen sind wertbasiert. Doppelte
Source-URIs mit unterschiedlichem Inhalt führen zu `std::invalid_argument`;
bei fehlgeschlagener Kompilation wirft `models()` einen `std::logic_error`.

Jede Instanz besitzt unabhängigen Zustand und kann parallel zu anderen
Instanzen laufen. Eine einzelne Instanz ist keine gemeinsam veränderbare
Session. Die API bleibt bewusst nativ: Sie exponiert C++-Besitzverhältnisse des
Metamodells und gehört deshalb weder zur C-ABI noch zur WASM-JSON-Oberfläche.
