# P2 Global-State-Inventar

Dieses Inventar dokumentiert den Zustand vor dem P2-Refactoring auf dem Ausgangs-
Commit `5ed35f8129fc6f984e2ca56bac8fb9d42a9418cf`. Es wird während der Migration
aktualisiert; der Nachher-Status ist in der letzten Spalte festzuhalten.

| Fund | Ausgangsdatei | Lebensdauer / Zugriff | Ziel | Nachher |
|---|---|---|---|---|
| `Log` | `source/util/Logger.cpp`, `source/util/Logger.h` | Prozessweit; alle Parser, Checker und CLI | Instanzlokaler `Logger` im `CompilerContext` | erledigt: kein globaler Logger; Besitz pro Lauf |
| `compilerMutex` | `source/core/Compiler.cpp` | Prozessweit; serialisiert jede Session | Mutex in `CompilerSession::Impl` | erledigt: nur noch per-Session-Mutex |
| `currentSourceManager` | `source/core/SourceManager.cpp` | `thread_local`; impliziter Sourcezugriff | Expliziter `CompilationSourceStore` | erledigt: Session-SourceManager wird explizit geborgt |
| `all_ilifiles` und Begleitlisten | `source/util/IliFile.cpp` | Prozessweit; manuelle Löschung und Reset | Besitzender `IliFileCatalog` | erledigt: `unique_ptr`-Besitz pro Context |
| `AllModels`, `AllImports`, weitere Listen | `source/metamodel/MetaModel.cpp` | Prozessweit; freie Getter/Setter | Besitzender `MetaModelStore` | erledigt: Store-Listen pro Lauf |
| `owned_objects` | `source/metamodel/MetaModel.cpp` | Prozessweit; Deleter-Map | `std::unique_ptr` im `MetaModelStore` | erledigt: RAII-Arena mit `owns`/`discardUnlinked` |
| Kontextstack | `source/metamodel/MetaModel.cpp` | Prozessweit; Parser-Push/Pop | `MetaModelBuilder::ContextScope` | erledigt: Builderstack pro Lauf, RAII-API vorhanden |
| `ili23`, `ili24`, `iliversion` | `source/metamodel/MetaModelInput.cpp` | Prozessweit; Parser liest Flags | Builderinstanz | erledigt: Sprachflags im Builder/Parserobjekt |
| Pending-Metadaten | `source/metamodel/MetaModelInput.cpp` | Prozessweit; Sourcewechselabhängig | Source-lokaler Builderzustand | erledigt: SourceScope restauriert Source/Pending-State |
| `input_file` | `source/input/ili1`, `source/input/ili2` | Prozessweit; Parserquelle | Expliziter `SourceBuffer` | erledigt: Parser erhält SourceBuffer als Argument |
| mutable Output-Statics | `source/output/Ili1Output.cpp`, `Ili2Output.cpp` | Prozessweit; Generatorlauf | Generatorinstanzen | erledigt: Writer/Flags/aktives Modell sind Instanzmember |
| globale Checkerfunktionen | `source/metamodel/SemanticChecker.cpp`, `TranslationChecker.cpp` | Lesen globaler Registries und Logger | Instanzgebundene Checker | erledigt: Store und Logger werden explizit injiziert |
| CLI-Resetfolge | `source/main/ilic.cpp` | Manuelles Cleanup bei Prozesspfaden | Context-Destruktion / CLI-Rand | erledigt: lokaler CLI-Context; kein globaler Cleanup |

## Invarianten für die Migration

* Ein Compilerlauf besitzt genau einen `CompilerContext`, `MetaModelStore`,
  `IliFileCatalog` und `Logger`.
* Metamodellobjekte werden ausschließlich durch den Store erzeugt und zerstört.
* Pointer im Metamodellgraph bleiben nicht besitzend und dürfen keine Storegrenze
  überschreiten.
* Eine `CompilerSession` serialisiert nur ihre eigenen Quellen und Operationen.
* Öffentliche Resultate enthalten nur Werte und dürfen nach Contextzerstörung
  gelesen werden.
* Der externe Conformance-Runner bleibt unverändert und verwendet dieselbe
  Baseline `conformance/known-failures.json`.

Der maschinelle Wächter liegt in `scripts/check-core-global-state.py` und läuft als
CTest `ilic_no_mutable_compiler_globals`. Die ANTLR-Grammatiktabellen bleiben als
unveränderliche statische Programmdaten erhalten; die zuvor gemeinsam genutzten
mutable DFA-/Prediction-Caches sind nun Parser-/Lexerinstanzen. Die globale
Handle-Registry in `source/abi/Capi.cpp` bleibt die ausdrücklich erlaubte C-ABI-
Ausnahme und ist durch `registryMutex` geschützt.
