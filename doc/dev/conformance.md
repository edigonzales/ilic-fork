# Semantic conformance and translation validation

## Sources and decision rules

Semantic corrections are checked against the language specification before the
implementation is changed. The sources are used in this order:

1. The version-specific INTERLIS reference manual is normative. For INTERLIS
   2.4, the relevant source sections are `refhb24_3_5.adoc` for models and
   translations, `refhb24_3_8.adoc` for domains, `refhb24_3_12.adoc` for
   constraints, `refhb24_3_13.adoc` for expressions and object paths, and
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

### Type-aware expressions and domain constraints

This cause covers 41 invalid models that ilic accepted. The exact case IDs are:

```text
ili23.constraints.existence-type-incompatibility
ili23.expressions.equals-attribute-path-type-fail
ili23.expressions.equals-class-type-fail
ili23.expressions.equals-coord-fail
ili23.expressions.equals-enum-all-of-const-fail
ili23.expressions.equals-enum-const-fail
ili23.expressions.equals-enum-extended-const-fail
ili23.expressions.equals-enum-fail
ili23.expressions.equals-formatted-fail
ili23.expressions.equals-function-fail
ili23.expressions.equals-numeric-fail
ili23.expressions.equals-object-fail
ili23.expressions.equals-oid-fail
ili23.expressions.equals-struct-fail
ili23.expressions.equals-text-fail
ili23.expressions.greater-equals-numeric-fail
ili23.expressions.greater-numeric-fail
ili23.expressions.less-coord-fail
ili23.expressions.less-enum-all-of-const-fail
ili23.expressions.less-enum-circular-fail
ili23.expressions.less-enum-domain-fail
ili23.expressions.less-enum-fail
ili23.expressions.less-enum-unordered-fail
ili23.expressions.less-equals-numeric-fail
ili23.expressions.less-formatted-fail
ili23.expressions.less-numeric-fail
ili23.expressions.logical-and-fail
ili23.expressions.logical-or-fail
ili23.expressions.not-equals-coord-fail
ili23.expressions.not-equals-enum-const-fail
ili23.expressions.not-equals-enum-fail
ili23.expressions.not-equals-formatted-fail
ili23.expressions.not-equals-numeric-fail
ili23.expressions.not-equals-object-fail
ili23.expressions.not-equals-text-fail
ili24.domain.compile-fail
ili24.expressions.div-numeric-fail
ili24.expressions.implication-fail
ili24.expressions.minus-numeric-fail
ili24.expressions.mul-numeric-fail
ili24.expressions.plus-numeric-fail
```

The parser previously reduced expression result types to non-semantic strings,
and no later pass validated them. It also discarded INTERLIS 2.4 domain
constraints. As a result, logical, arithmetic, equality, ordering, function,
enumeration, formatted-value, and existence checks all parsed successfully but
could not reject incompatible operands.

Reference-manual section 3.13 requires every logical expression to have Boolean
result type and defines the operand categories of logical, arithmetic, equality,
and ordering operators. Section 3.8 defines named domain constraints and their
domain-local `THIS` value; class, structure, and attribute-reference domains do
not support those constraints. Section 3.12 requires the two paths in an
existence constraint to have compatible attribute types. The implementation was
cross-checked against `validateEqualsArgumentTypes` and
`validateCompareArgumentTypes` in the ili2c 2.3 and 2.4 grammars, its
`DomainConstraint` and `ExistenceConstraint` metamodel classes, and the
corresponding ili2c tests. In particular, enum literals are resolved against the
domain on the other side of a comparison, ordering needs an `ORDERED`
enumeration, and formatted literals are checked against their format and range.

ilic now stores a resolved semantic descriptor on every expression and runs a
dedicated post-pass after all models have been loaded. The descriptor keeps the
value category, declared domain, object/structure target, and lexical occurrence
scope. The checker validates Boolean and numeric operators, equality and
ordering, enum membership and order, formatted values, function arguments,
domain `THIS`, and existence-path compatibility. Calculated view attributes
derive their result type from their factor so later constraints see their real
type. Class and attribute constants remain distinct metamodel nodes. Domain
constraints are retained by cloning and translation linking and are emitted
inline by the INTERLIS writer.

Both LSP grammars contain the required expression, domain-constraint,
enumeration, formatted-value, and existence-constraint syntax, but neither
grammar can express these semantic compatibility rules. They therefore agreed
that the failing inputs were syntactically valid and provided no basis for a
grammar change. No generated parser file changes in this cause.

Ten local model fixtures cover positive and negative relation, Boolean,
arithmetic, existence, and domain-constraint behaviour. A separate roundtrip
test writes a constrained domain as INTERLIS, reparses it, and checks that its
named constraint remains present. All 42 local CTests pass. The complete frozen
corpus improves from 476 to 517 conformant cases: 49 invalid models remain
accepted, five valid models remain rejected, and no previously conformant case
regresses. The recorded result was produced by the cause commit containing this
section (the report's candidate commit is verified after the commit is created).

### Association role semantics and visibility

This cause covers seven invalid models accepted by ilic and two valid models
rejected by ilic:

```text
ili23.association.association-3roles
ili23.association.association-same-rolename-in-diff-ext-topics
ili23.association.association-cardinalities-in-composition-role-fail
ili23.association.association-cross-topic-role-fail
ili23.association.association-detect-multiple-aggregation-roles-fail
ili23.association.association-dublicating-role-names-fail
ili23.association.association-greater-card-size-than-base-fail
ili23.association.association-role-points-to-structure-fail
ili23.association.association-role-with-less-aggregation-than-base-fail
```

Reference-manual section 3.7 requires at least two uniquely named roles, allows
roles to target identifiable classes or associations but not structures, and
permits only one aggregate or composite end. A composition end has maximum
cardinality one. An extended role may narrow its cardinality, strengthen its
aggregation, and specialize its target, but may not do the reverse. A role into
an unrelated topic requires `EXTERNAL`; a target in a base topic of the current
topic does not. Association accesses are added only to target classes belonging
to the association's topic. An inherited class receives such accesses only if
it is explicitly extended in that topic.

The implementation was cross-checked against the ili2c 2.3 `roleDef` parser
action, `AssociationDef.fixupRoles`, and `RoleDef.setExtending`. ilic now records
whether cardinality syntax was present, which distinguishes inheritance from an
explicit `{*}`. The semantic post-pass computes effective defaults and validates
composition maxima, cardinality subsets, strength, target specialization,
target kind, cross-topic `EXTERNAL`, and duplicate/aggregate roles. The parser's
access installation now follows the topic ownership rule. Reaching the same
role access repeatedly in a ternary self-association is deduplicated by identity;
it is not mistaken for a name collision. Likewise, associations in two
independent extensions of a topic do not mutate their common inherited class.

Both LSP grammars contain the same `RoleDef`, cardinality, strength, and
`EXTERNAL` syntax but do not model effective role inheritance or topic-owned
association accesses. They support the conclusion that these are semantic and
ownership corrections; no grammar or generated parser file changed.

Six local fixtures cover ternary and independent-topic positive cases, plus
cardinality, strength, structure target, cross-topic visibility, and duplicate
role failures. All 48 local CTests and all nine unchanged upstream cases pass.
The complete frozen corpus improves from 517 to 526 conformant cases: 42 invalid
models remain accepted, three valid models remain rejected, and no previously
conformant case regresses. The recorded result was produced by the cause commit
containing this section (the report's candidate commit is verified after the
commit is created).

### Type, attribute, enumeration, and class extension rules

This cause covers 14 invalid INTERLIS 2.3 models that ilic accepted:

```text
ili23.attributes.extended-attr-ref-with-diff-cardinality-fail
ili23.attributes.extended-attr-simple-with-diff-cardinality-fail
ili23.attributes.extended-attr-struct-with-diff-cardinality-fail
ili23.attributes.extended-attr-with-diff-transient-mode-fail
ili23.classes.class-extension-detect-class-spec-of-ext-base-class-fail
ili23.classes.class-extension-detect-ext-class-of-spec-base-class-fail
ili23.classes.class-extension-extending-class-in-extending-topic-fail
ili23.enumerations.enumeration-unique-elements-in-extend-enum
ili23.enumerations.enumeration-uniqueness-elements-in-basic-enum
ili23.enumerations.extended-attr-fail
ili23.enumerations.extended-enum-fail
ili23.line-type.overlap-too-small-fail
ili23.text-type.string-limit-length-extended-to-unlimited-length-fail
ili23.text-type.string-multiline-text-extended-to-single-line-fail
```

The missing checks were independent manifestations of extension compatibility.
An extending attribute must narrow, not widen, its effective cardinality and
must retain `TRANSIENT`. A named attribute domain must be the same domain or an
extension of it. Inline text types retain `TEXT` versus `MTEXT` and may only
reduce their maximum length. Enumeration children must be unique among their
siblings; a vertical or horizontal extension can add children and can mark an
inherited leaf `FINAL`, but cannot redeclare that inherited leaf. A line type's
maximum overlap cannot use finer precision than its coordinate domain.

Reference-manual sections 3.5, 3.6, and 3.8 specify the class, attribute, and
domain extension invariants. The line-type accuracy rule is defined by the
line-type production in section 3.8. The implementation was cross-checked
against `Table.checkIntegrity`, `AttributeDef.setExtending`,
`TextType.checkTypeExtension`, `EnumerationType.checkTypeExtension`, and the
2.3 parser's line-type validation in ili2c. Both LSP grammars recognize the
same declaration syntax but do not perform these metamodel compatibility
checks; no grammar or generated parser file was changed.

Class specialization across inherited topics has two distinct forms. A class
retaining its inherited name must use `EXTENDED`; `EXTENDS` introduces a new
name. In one topic-inheritance chain the same base cannot simultaneously be
extended in place and specialized under another name. The semantic post-pass
therefore examines the completed topic, including declarations that occur
after the `EXTENDED` class, and all base topics. It does not prohibit differently
named specializations in independent topic extensions.

The parser clones the inherited inline type for the shorthand
`attribute (EXTENDED): MANDATORY`. A new metamodel flag distinguishes that
shorthand from an explicit type redefinition, preventing the clone from being
misclassified as an enum redeclaration. Likewise, `FINAL` on an inherited enum
leaf is retained as a legal restriction. These two rules are covered by the
unchanged valid upstream cases
`ili23.enumerations.enumeration-extended-enumeration-type` and
`ili23.enumerations.extended-enum` and prevent regressions introduced by the
new checks.

Eleven local fixtures cover valid and invalid cardinality, transient, named and
inline type, enum, overlap, and class-specialization behaviour. The Roads
regression fixture previously widened a mandatory enum attribute to optional;
ili2c 5.6.8 also rejects that input. Its extended enum is now explicitly
`MANDATORY`, preserving the intended base cardinality. All 59 local CTests and
all affected unchanged upstream cases pass.

The complete frozen corpus improves from 526 to 540 conformant cases: 28 invalid
models remain accepted, three valid models remain rejected, and no previously
conformant case regresses. The measured improvement is two cases larger than
the planning estimate because the three class-specialization cases share this
same extension invariant. The recorded result was produced by the cause commit
containing this section (the report's candidate commit is verified after the
commit is created).

### Abstract classes and views in concrete topics

This cause covers the invalid model
`ili23.topics.extended-abstract-topic-abstract-class-fail` and the valid model
`ili23.view.abstract-view-accept-abstract-base-in-abstract-join-def`. The old
parser-time check inspected every local `ExtendableME`, so it rejected an
abstract view in a concrete topic. Conversely, its inherited-element branch was
disabled and it accepted a concrete topic that inherited an abstract class
without providing any concrete extension.

Reference-manual section 3.5 requires an abstract definition left
unconcretized in a topic to make that topic abstract. For transfer-relevant
viewables, the corresponding ili2c implementation in
`Topic.checkIntegrityAbstract` applies this rule specifically to identifiable
tables, i.e. classes, and searches the topic's effective inherited viewables
for a concrete extension. Section 3.15 separately permits abstract views and
even concrete views on abstract bases, subject to the attributes actually used.
An abstract view therefore does not by itself make its topic abstract.

The check now runs in the semantic post-pass, after every topic declaration is
available. It constructs the effective class set from the complete topic
inheritance chain, hides base classes replaced with `EXTENDED`, and accepts an
abstract class only when a concrete same-name or differently named extension is
effective in that topic. Structures, associations, and views are not treated as
identifiable classes. This follows ili2c's `AbstractPatternDef.getViewables` and
`Topic.containsConcreteExtensionOfTable` behaviour without copying its Java
container implementation.

Both LSP grammars accept the same topic, class, and view properties but do not
encode the effective-inheritance check. No grammar or generated parser file
changed. Three local fixtures cover an abstract view in a concrete topic, a
missing inherited concretization, and valid concretization under both naming
forms. All 62 local CTests and both unchanged upstream cases pass.

The complete frozen corpus improves from 540 to 542 conformant cases: 27 invalid
models remain accepted, two valid models remain rejected, and no previously
conformant case regresses. The recorded result was produced by the cause commit
containing this section (the report's candidate commit is verified after the
commit is created).

### Declaration namespaces and closing names

This cause covers eight invalid INTERLIS 2.3 models accepted by ilic:

```text
ili23.classes.class-detect-non-matching-names-fail
ili23.models.models-domain-and-topic-same-names-fail
ili23.models.models-dublicate-runtime-param
ili23.models.models-non-matching-names-fail
ili23.topics.topics-detect-name-conflicts-fail
ili23.topics.topics-detect-non-matching-name-fail
ili23.view.view-detect-attrname-conflict-with-base-viewname-fail
ili23.view.view-detect-dublicate-attrname-in-view-fail
```

Reference-manual section 3.5 defines separate type-name, component-name, and
metaobject-name categories for every modelling element. Type names include
domains and topics and are inherited into an extending topic; a local type name
may collide with an inherited name only for an explicit `EXTENDED` declaration.
View-base aliases and attributes are component names, so `ALL OF` may neither
collide with its base alias nor import the same attribute name from two bases.
Runtime parameters are also component names and must be unique among themselves,
but a runtime parameter and a domain may legally share a name because the
domain is a type name. The valid upstream case
`ili23.models.models-domain-and-runtime-param-same-name` and a local regression
make this category boundary explicit.

The grammar productions in section 3.5 write the opening name again after
`END` for models, topics, classes, and structures. ili2c treats a mismatch as a
semantic error. ilic already did so for structures and views, but emitted only
a warning for models, topics, and classes; those three checks now report errors.
Both LSP grammars preserve the paired-name syntax, but name equality and scope
collisions remain semantic checks, so no grammar or generated parser changed.

The semantic post-pass now validates package type-name namespaces across their
complete topic inheritance chain and validates each class/view component-name
set after `ALL OF` expansion. Only actual package members whose
`ElementInPackage` still names that package participate. Internal metamodel
objects such as the synthetic `BASKET` data unit and coordinate-axis types are
therefore not mistaken for source declarations. Same-name `EXTENDED` classes,
associations, and views remain legal. Association-role visibility continues to
use its dedicated rules and is not folded into the package namespace.

Seven local fixtures cover all three closing-name mismatches, direct and
inherited package collisions, duplicate runtime parameters, both view-attribute
collisions, and positive category/extension cases. All 69 local CTests, all
eight unchanged invalid upstream cases, and the valid separate-category case
pass.

The complete frozen corpus improves from 542 to 550 conformant cases: 19 invalid
models remain accepted, two valid models remain rejected, and no previously
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

| Result | Initial macOS baseline | Translation/crash fixes | Lexer fixes | Path fixes | Expression fixes | Association fixes | Extension fixes | Abstract fixes | Namespace fixes |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| conformant | 268 | 468 | 470 | 476 | 517 | 526 | 540 | 542 | 550 |
| candidate accepts invalid | 280 | 95 | 95 | 90 | 49 | 42 | 28 | 27 | 19 |
| candidate rejects valid | 12 | 8 | 6 | 5 | 5 | 3 | 3 | 2 | 2 |
| infrastructure error | 11 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |

All 251 `TRANSLATION OF` cases are conformant, and no case that was conformant
in baseline commit `979bf560c4eb6c6374cd436370d9af86063bc3ef` regressed. The
remaining non-conformant cases are regular semantic-validation backlog; none is
an infrastructure error.
