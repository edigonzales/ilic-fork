# CompilerContext

P2 trennt drei Lebensdauern: unveränderliche Prozessdaten, eine
`CompilerSession` mit ihrem `SourceManager` und einen einzelnen
Kompilationslauf. Jeder Lauf konstruiert intern einen neuen
`ilic::detail::CompilerContext`. Der Context besitzt Logger, Source-Sicht,
Dateikatalog, Metamodellstore und Builder; er ist weder kopier- noch bewegbar.

```text
CompilerSession::Impl
  ├── mutex
  └── SourceManager

CompilerContext (pro compile/analyze)
  ├── borrows session SourceManager
  ├── owns Logger and CompilationSourceStore
  ├── owns IliFileCatalog
  ├── owns MetaModelStore
  └── owns MetaModelBuilder
```

Untergeordnete Parser, Checker und Generatoren erhalten konkrete Referenzen auf
ihre Abhängigkeiten. Der Context ist nur Orchestrierungsgrenze und kein
Service-Locator. Nach dem wertbasierten Ergebnis endet seine Lebensdauer; kein
öffentlicher Snapshot enthält Pointer oder `string_view` auf ihn.

Die Struktur lässt später einen sessionlokalen inkrementellen Cache zu, ohne
Compilerfachzustand in Prozessglobals oder `thread_local`-Variablen abzulegen.
