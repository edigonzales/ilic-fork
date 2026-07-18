# Semantic conformance and translation validation

## Sources and decision rules

Semantic corrections are checked against the language specification before the
implementation is changed. The sources are used in this order:

1. The version-specific INTERLIS reference manual is normative. For INTERLIS
   2.4, the relevant source sections are `refhb24_3_5.adoc` for models and
   translations, `refhb24_3_13.adoc` for object paths, and
   `refhb24_3_15.adoc` for views and inspections in the
   `doc-interlis-refhb24` repository. The INTERLIS 2.3 manuals are also checked
   for 2.3 behaviour.
2. The `ili2c` tests and the `linkTranslationOf` and `checkTranslationOf`
   implementations in `ili2c-core/src/main/java/ch/interlis/ili2c/metamodel`
   are the behavioural oracle where the specification leaves implementation
   details open.
3. The `grammars-ng` grammar in `interlis-lsp` is an additional syntax
   comparison. `grammars-antlr4` is useful for its error-tolerant editor
   perspective, but is not a normative source.

The crash cases corrected in this change all passed lexing and parsing. Their
failures occurred while constructing or traversing the C++ metamodel. Therefore
`source/input/parser/grammar` and the checked-in generated parsers were not
changed. The `check-parser-regeneration` target confirms that the generated
files still match the bundled ANTLR 4.7.1 generator byte for byte.

### Known grammar follow-up

The comparison did expose a separate DrawingRule discrepancy that is not needed
for these fixes. Reference-manual section 3.16 and both LSP grammars separate
multiple `CondSignParamAssignment` clauses with commas; the current ilic grammar
uses semicolons at that level. Inside one clause, however, multiple
`SignParamAssignment` entries are separated with semicolons. The Roads fixture
used commas inside clauses and was corrected to the specified semicolons before
being promoted to a regression test. No grammar change is included here because
the tested rules contain only one conditional clause.

Other broad syntax questions, including the shared `restrictedRef` rule, remain
isolated grammar-conformance work. Any such correction must include its own
reference-manual evidence, ANTLR 4.7.1 regeneration, and generated-file diff.

## Conformance parity campaign

The post-translation baseline was reproduced from a clean Debug build of commit
`d399fe11ec3e20d420b30e55b498d96b1d53fd42`. The frozen 571-case corpus has
SHA-256 `5baa41c6172e169e7dd35b1241a9dc9ba6e60ab90f4918e864c90c988cc51a57`.
That run produced 468 conformant cases, 95 cases where ilic accepted an invalid
model, eight cases where ilic rejected a valid model, and no infrastructure
errors. Each cause below is checked with local regressions and a complete run
of that same corpus before it is committed.

### Lexer tokenization and nested comments

Cases `ili23.ili-symbols.dec-number-extended` and
`ili23.ili-symbols.nested-comment` were valid models rejected by ilic. The first
failure was caused by `SCALING` being an emitted lexer token. Consequently the
legal domain name `e1` was tokenized as a numeric exponent instead of `NAME`.
The second failure was caused by a non-recursive block-comment rule, which left
the outer closing delimiter in the parser token stream.

Reference-manual section 3.2 defines a name as starting with a letter and
defines `Scaling` only as a component of a decimal number. The same section
explicitly permits nested block comments. `SCALING` is therefore a lexer
fragment and the block-comment rule recursively consumes nested comments. Both
LSP lexer variants use standalone `Scaling` and non-recursive block comments;
those editor-oriented rules were deliberately not copied because they conflict
with the reference manual and the ili2c 2.3 acceptance tests.

Local tests cover a name beginning with `e`, valid scaled decimals, nested and
unterminated comments, and a malformed exponent. ANTLR 4.7.1 regeneration
changes only the Ili2 lexer and the Ili2 parser's generated token vocabulary;
`check-parser-regeneration` matches all 36 generated files. The full corpus
improves from 468 to 470 conformant cases, with 95 invalid models still accepted
and six valid models still rejected. No previously conformant case regresses.

### Attribute derivations and contextual object paths

This cause covers the five invalid cases
`ili23.attributes.attribute-path-type-non-class-type-attr-path-restriction-fail`,
`ili23.attributes.transient-attribute-without-afactor-fail`,
`ili23.expressions.expressions-parent-element-in-area-view-fail`,
`ili23.expressions.expressions-this-area-in-non-area-view-fail`, and
`ili23.expressions.expressions-that-area-in-non-area-view-fail`, plus the valid
case
`ili23.expressions.expressions-accepting-extended-object-with-role-of-class`.
The failures shared one omission: parsed derivations and `ATTRIBUTE OF` paths
were discarded, while object paths resolved names only against their declared
base class and not against extensions visible at the path's lexical occurrence.

Reference-manual section 3.6 requires a factor for a concrete `TRANSIENT`
attribute. An abstract transient attribute may leave that factor to a concrete
extension; the 2.3 and 2.4 `ili2c` `attributeDef` productions enforce exactly
that distinction. Section 3.8 defines an `ATTRIBUTE OF` restriction as an
attribute path whose terminal value is a class-valued attribute. Sections 3.13
and 3.15 restrict `PARENT` to ordinary inspections and `THISAREA`/`THATAREA` to
area inspections. The `ili2c` path production additionally passes the lexical
container into `Viewable.findAttributeInExtendedClass`, so an attribute added
by a class extension in the current topic is visible after navigation through
a role or reference.

ilic now stores every derivation factor, rejects only concrete transient
attributes without a factor, stores and validates `ATTRIBUTE OF` paths, and
resolves contextual attributes in the current model/topic scope. More than one
visible contextual match is diagnosed as ambiguous. The stored data is also
preserved by cloning, translation comparison, and INTERLIS output. Both LSP
grammars recognize the same path keywords and derivation syntax but do not
encode these context-sensitive restrictions; consequently no grammar change
was made.

Nine local tests cover both outcomes of `ATTRIBUTE OF`, concrete transient
attributes with and without a factor, the abstract exception, the three
inspection keywords, and contextual extension lookup. All 31 local CTests pass.
The complete corpus improves from 470 to 476 conformant cases: 90 invalid
models remain accepted, five valid models remain rejected, and no previously
conformant case regresses. The recorded result was produced by the cause commit
containing this section (the report's candidate commit is verified after the
commit is created).

## Object-path context transitions

Object paths are resolved one element at a time. The resolver carries the
current navigable class and, where applicable, the enclosing view. Its required
transitions follow reference-manual sections 3.13 and 3.15:

| Path element | Required context | Next context |
| --- | --- | --- |
| start | class or structure | that class or structure |
| start in a view | view | that view and its named bases |
| start in a graphic | graphic | the graphic's base class |
| `THIS` | any current object | unchanged |
| `PARENT` | normal inspection | inspected parent class |
| `THISAREA`, `THATAREA` | area inspection | inspected area class |
| named view base | view | base class represented by the alias |
| attribute | class, structure, or view base | the attribute's object, reference, or multivalue target if navigable |
| reference attribute | class or structure | referenced class |
| role | class, structure, or view base | role target class |
| association path (`\role`) | class or structure | association class |

If an element has no navigable target, the current context becomes empty. A
following path element then produces a semantic diagnostic instead of
dereferencing a stale or null class. `find_role`, `find_attribute`, dependency
checks, and unresolved-type traversal follow the same null-safe rule.

## `TRANSLATION OF`

The model header stores both the base-model name and the documentary base
version. The base model is a compilation dependency, but this does not create an
INTERLIS `IMPORTS` relation. After all input files have been read, a separate
translation phase:

+ rejects missing base models, self-translations, and cycles;
+ pairs declarations by definition order without comparing translated names;
+ records the corresponding base element on every paired metamodel element;
+ compares references through the root of their translation chain;
+ compares imports, topic dependencies, and line forms without imposing an
  order where the language does not define one;
+ checks container structure, types, classes, attributes, roles, associations,
  views, paths, metadata baskets, constraints, and expressions; and
+ reports the translated element, base element, and differing property.

The base version is preserved for INTERLIS output but is not compared with the
loaded model version. This follows reference-manual section 3.5, where the value
is documentary, and matches `ili2c` behaviour. INTERLIS output emits the full
`TRANSLATION OF Base ["version"]` clause, so it survives a round trip.

## Regression and conformance tests

The local CTest driver distinguishes expected-valid and expected-invalid input
and explicitly fails if `ilic` terminates by signal. It includes one minimized
test for each of the six original crash causes, the Roads graphics/path model,
translation success and failure cases, dependency loading, graph errors, and an
INTERLIS-output round trip.

The external conformance suite can be run after building `ilic`:

```sh
cd /path/to/interlis-compiler-conformance
ILIC_REPO=/path/to/ilic \
ILIC_EXECUTABLE=/path/to/ilic/build/macos/ilic \
ILI2C_JAR=/path/to/ili2c-5.6.8/ili2c.jar \
ILI2C_SOURCE_REPO=/path/to/pinned/ili2c \
./gradlew runConformance generateConformanceReport
```

For the 571-case corpus, measured against the same `ili2c` reference and corpus,
the result changed as follows:

| Result | Initial macOS baseline | Translation/crash fixes | Lexer fixes | Path fixes |
| --- | ---: | ---: | ---: | ---: |
| conformant | 268 | 468 | 470 | 476 |
| candidate accepts invalid | 280 | 95 | 95 | 90 |
| candidate rejects valid | 12 | 8 | 6 | 5 |
| infrastructure error | 11 | 0 | 0 | 0 |

All 251 `TRANSLATION OF` cases are conformant, and no case that was conformant
in baseline commit `979bf560c4eb6c6374cd436370d9af86063bc3ef` regressed. The
remaining non-conformant cases are regular semantic-validation backlog; none is
an infrastructure error.
