#include "CapiJsonProjectors.h"

namespace ilic::capi {
namespace {

const char *severity(DiagnosticSeverity value)
{
   switch (value) {
      case DiagnosticSeverity::Error: return "error";
      case DiagnosticSeverity::Warning: return "warning";
      case DiagnosticSeverity::Information: return "information";
      case DiagnosticSeverity::Hint: return "hint";
   }
   return "error";
}

const char *phase(DiagnosticPhase value)
{
   switch (value) {
      case DiagnosticPhase::Unknown: return "unknown";
      case DiagnosticPhase::Lexical: return "lexical";
      case DiagnosticPhase::Syntax: return "syntax";
      case DiagnosticPhase::EditorRecovery: return "editor-recovery";
      case DiagnosticPhase::ModelDiscovery: return "model-discovery";
      case DiagnosticPhase::Resolution: return "resolution";
      case DiagnosticPhase::Semantic: return "semantic";
      case DiagnosticPhase::Translation: return "translation";
      case DiagnosticPhase::Repository: return "repository";
      case DiagnosticPhase::Formatting: return "formatting";
      case DiagnosticPhase::Request: return "request";
      case DiagnosticPhase::Internal: return "internal";
   }
   return "unknown";
}

const char *tag(DiagnosticTag value)
{
   switch (value) {
      case DiagnosticTag::Primary: return "primary";
      case DiagnosticTag::Cascaded: return "cascaded";
      case DiagnosticTag::Recovery: return "recovery";
      case DiagnosticTag::Deprecated: return "deprecated";
      case DiagnosticTag::Unnecessary: return "unnecessary";
   }
   return "primary";
}

const char *level(LogLevel value)
{
   switch (value) {
      case LogLevel::Trace: return "trace";
      case LogLevel::Debug: return "debug";
      case LogLevel::Information: return "information";
      case LogLevel::Warning: return "warning";
      case LogLevel::Error: return "error";
   }
   return "information";
}

const char *editorSymbolKind(EditorSymbolKind value)
{
   switch (value) {
      case EditorSymbolKind::Model: return "model";
      case EditorSymbolKind::Topic: return "topic";
      case EditorSymbolKind::Class: return "class";
      case EditorSymbolKind::Structure: return "structure";
      case EditorSymbolKind::Association: return "association";
      case EditorSymbolKind::View: return "view";
      case EditorSymbolKind::Graphic: return "graphic";
      case EditorSymbolKind::Domain: return "domain";
      case EditorSymbolKind::Unit: return "unit";
      case EditorSymbolKind::Attribute: return "attribute";
   }
   return "attribute";
}

const char *editorReferenceKind(EditorReferenceKind value)
{
   switch (value) {
      case EditorReferenceKind::Extends: return "extends";
      case EditorReferenceKind::Type: return "type";
      case EditorReferenceKind::Collection: return "collection";
      case EditorReferenceKind::Reference: return "reference";
      case EditorReferenceKind::Unit: return "unit";
   }
   return "type";
}

Value range(const SourceRange &value)
{
   if (!value.valid) return nullptr;
   auto point = [](const Position &position) {
      return Value::Object{{"line",position.line},{"character",position.character},
         {"byteOffset",position.byteOffset}};
   };
   return Value::Object{{"uri",value.uri},{"start",point(value.start)},
      {"end",point(value.end)}};
}

Value logs(const std::vector<LogEvent> &values)
{
   Value::Array result;
   for (const auto &event : values) {
      Value::Object context;
      for (const auto &entry : event.context) context[entry.first] = entry.second;
      result.push_back(Value::Object{{"level",level(event.level)},
         {"category",event.category},{"message",event.message},
         {"context",std::move(context)}});
   }
   return result;
}

Value strings(const std::vector<std::string> &values)
{
   Value::Array result;
   for (const auto &value : values) result.emplace_back(value);
   return result;
}

Value metadata(const std::vector<CompiledModel::MetaAttributeValue> &values)
{
   Value::Array result;
   for (const auto &value : values)
      result.push_back(Value::Object{{"name",value.name},{"value",value.value}});
   return result;
}

Value documentationViewable(const DocumentationViewable &viewable)
{
   Value::Array rows;
   for (const auto &row : viewable.rows)
      rows.push_back(Value::Object{{"name",row.name},{"cardinality",row.cardinality},
         {"type",row.type},{"description",row.description}});
   return Value::Object{{"name",viewable.name},{"kind",viewable.kind},
      {"isAbstract",viewable.isAbstract},{"documentation",viewable.documentation},
      {"rows",std::move(rows)}};
}

Value documentationEnumeration(const DocumentationEnumeration &enumeration)
{
   Value::Array entries;
   for (const auto &entry : enumeration.entries)
      entries.push_back(Value::Object{{"value",entry.value},
         {"documentation",entry.documentation}});
   return Value::Object{{"name",enumeration.name},
      {"documentation",enumeration.documentation},{"entries",std::move(entries)}};
}

Value documentationModel(const DocumentationModel &model)
{
   Value::Array viewables;
   for (const auto &viewable : model.viewables) viewables.push_back(documentationViewable(viewable));
   Value::Array enumerations;
   for (const auto &enumeration : model.enumerations)
      enumerations.push_back(documentationEnumeration(enumeration));
   Value::Array topics;
   for (const auto &topic : model.topics) {
      Value::Array topicViewables;
      for (const auto &viewable : topic.viewables) topicViewables.push_back(documentationViewable(viewable));
      Value::Array topicEnumerations;
      for (const auto &enumeration : topic.enumerations)
         topicEnumerations.push_back(documentationEnumeration(enumeration));
      topics.push_back(Value::Object{{"name",topic.name},{"documentation",topic.documentation},
         {"viewables",std::move(topicViewables)},{"enumerations",std::move(topicEnumerations)}});
   }
   return Value::Object{{"name",model.name},{"uri",model.uri},{"title",model.title},
      {"shortDescription",model.shortDescription},{"topics",std::move(topics)},
      {"viewables",std::move(viewables)},{"enumerations",std::move(enumerations)}};
}

} // namespace

Value diagnostics(const std::vector<Diagnostic> &values)
{
   Value::Array result;
   for (const auto &diagnostic : values) {
      Value::Array related;
      for (const auto &information : diagnostic.relatedInformation)
         related.push_back(Value::Object{{"range",range(information.range)},
            {"message",information.message}});
      Value::Array notes;
      for (const auto &note : diagnostic.notes) notes.emplace_back(note);
      Value::Object item{{"severity",severity(diagnostic.severity)},
         {"code",diagnostic.code},{"message",diagnostic.message},
         {"range",range(diagnostic.range)},{"relatedInformation",std::move(related)},
         {"notes",std::move(notes)},{"treatedAsError",diagnostic.treatedAsError}};
      if (!diagnostic.source.empty()) item["source"] = diagnostic.source;
      if (diagnostic.phase != DiagnosticPhase::Unknown) item["phase"] = phase(diagnostic.phase);
      if (!diagnostic.tags.empty()) {
         Value::Array tags;
         for (const auto value : diagnostic.tags) tags.emplace_back(tag(value));
         item["tags"] = std::move(tags);
      }
      if (!diagnostic.helpId.empty()) item["helpId"] = diagnostic.helpId;
      if (!diagnostic.fingerprint.empty()) item["fingerprint"] = diagnostic.fingerprint;
      result.push_back(std::move(item));
   }
   return result;
}

Value errorResult(const char *kind,const std::string &message,const std::string &uri,
   std::uint64_t documentVersion)
{
   Value::Object result{{"schemaVersion",1},{"abiVersion",1},
      {"compilerVersion",version()},{"kind",kind},{"success",false},
      {"diagnostics",Value::Array{Value::Object{{"severity","error"},
         {"code","ILIC-ABI-REQUEST"},{"message",message},{"range",nullptr},
         {"relatedInformation",Value::Array{}},{"notes",Value::Array{}},
         {"treatedAsError",false}}}}};
   const std::string kindValue(kind);
   if (kindValue == "syntax") {
      result["uri"] = uri; result["documentVersion"] = static_cast<double>(documentVersion);
      result["iliVersion"] = ""; result["tokens"] = Value::Array{}; result["nodes"] = Value::Array{};
      result["contexts"] = Value::Array{}; result["imports"] = Value::Array{};
      result["importReferences"] = Value::Array{};
   } else if (kindValue == "editor") {
      result["recovered"] = false; result["complete"] = false; result["uri"] = uri;
      result["documentVersion"] = static_cast<double>(documentVersion); result["iliVersion"] = "unknown";
      result["declarations"] = Value::Array{}; result["references"] = Value::Array{};
      result["imports"] = Value::Array{}; result["contexts"] = Value::Array{};
   } else if (kindValue == "semantic") {
      result["cancelled"] = false; result["roots"] = Value::Array{};
      result["documentVersions"] = Value::Object{}; result["missingModels"] = Value::Array{};
      result["symbols"] = Value::Array{}; result["references"] = Value::Array{};
      result["dependencies"] = Value::Array{};
      result["diagram"] = Value::Object{{"nodes",Value::Array{}},{"edges",Value::Array{}}};
      result["documentation"] = Value::Object{{"title",""},{"sections",Value::Array{}}};
      result["logs"] = Value::Array{};
   } else if (kindValue == "compilation") {
      result["cancelled"] = false; result["errorCount"] = 1; result["warningCount"] = 0;
      result["missingModels"] = Value::Array{}; result["models"] = Value::Array{};
      result["logs"] = Value::Array{}; result["transcript"] = Value::Array{};
   } else if (kindValue == "formatting") {
      result["applicable"] = false; result["changed"] = false; result["text"] = "";
   }
   return result;
}

Value compileResult(const CompilationResult &result)
{
   Value::Array models;
   for (const auto &model : result.models)
      models.push_back(Value::Object{{"name",model.name},{"iliVersion",model.iliVersion},
         {"uri",model.uri},{"metaAttributes",metadata(model.metaAttributes)}});
   Value::Array missing;
   for (const auto &model : result.missingModels) missing.emplace_back(model);
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","compilation"},{"success",result.success},{"cancelled",result.cancelled},
      {"errorCount",result.errorCount},{"warningCount",result.warningCount},
      {"missingModels",std::move(missing)},{"models",std::move(models)},
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)},
      {"transcript",strings(result.transcript)}};
}

Value syntaxResult(const SyntaxSnapshot &result)
{
   Value::Array tokens;
   for (const auto &token : result.tokens)
      tokens.push_back(Value::Object{{"kind",token.kind},{"text",token.text},
         {"channel",token.channel},{"range",range(token.range)}});
   Value::Array nodes;
   for (const auto &node : result.nodes)
      nodes.push_back(Value::Object{{"id",node.id},{"parent",node.hasParent ? Value(node.parent) : Value(nullptr)},
         {"kind",node.kind},{"range",range(node.range)}});
   Value::Array contexts;
   for (const auto &context : result.contexts)
      contexts.push_back(Value::Object{{"kind",context.kind},{"range",range(context.range)}});
   Value::Array imports;
   for (const auto &model : result.imports) imports.emplace_back(model);
   Value::Array references;
   for (const auto &reference : result.importReferences)
      references.push_back(Value::Object{{"model",reference.model},{"unqualified",reference.unqualified},
         {"range",range(reference.range)}});
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","syntax"},{"success",result.success},{"uri",result.uri},
      {"documentVersion",static_cast<double>(result.documentVersion)},{"iliVersion",result.iliVersion},
      {"tokens",std::move(tokens)},{"nodes",std::move(nodes)},{"contexts",std::move(contexts)},
      {"imports",std::move(imports)},{"importReferences",std::move(references)},
      {"diagnostics",diagnostics(result.diagnostics)}};
}

Value editorResult(const EditorSnapshot &result)
{
   Value::Array declarations;
   for (const auto &declaration : result.declarations)
      declarations.push_back(Value::Object{{"id",declaration.id},{"name",declaration.name},
         {"qualifiedName",declaration.qualifiedName},{"kind",editorSymbolKind(declaration.kind)},
         {"containerId",declaration.hasContainer ? Value(declaration.containerId) : Value(nullptr)},
         {"range",range(declaration.range)},{"selectionRange",range(declaration.selectionRange)},
         {"endRange",range(declaration.endRange)}});
   Value::Array references;
   for (const auto &reference : result.references)
      references.push_back(Value::Object{{"text",reference.text},{"kind",editorReferenceKind(reference.kind)},
         {"sourceId",reference.hasSource ? Value(reference.sourceId) : Value(nullptr)},
         {"range",range(reference.range)}});
   Value::Array imports;
   for (const auto &reference : result.imports)
      imports.push_back(Value::Object{{"model",reference.model},{"unqualified",reference.unqualified},
         {"range",range(reference.range)}});
   Value::Array contexts;
   for (const auto &context : result.contexts)
      contexts.push_back(Value::Object{{"kind",context.kind},{"range",range(context.range)}});
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","editor"},{"success",result.success},{"recovered",result.recovered},
      {"complete",result.complete},{"uri",result.uri},
      {"documentVersion",static_cast<double>(result.documentVersion)},{"iliVersion",result.iliVersion},
      {"declarations",std::move(declarations)},{"references",std::move(references)},
      {"imports",std::move(imports)},{"contexts",std::move(contexts)},
      {"diagnostics",diagnostics(result.diagnostics)}};
}

Value semanticResult(const SemanticSnapshot &result)
{
   Value::Array symbols;
   for (const auto &symbol : result.symbols)
      symbols.push_back(Value::Object{{"id",symbol.id},{"name",symbol.name},
         {"qualifiedName",symbol.qualifiedName},{"kind",symbol.kind},{"containerId",symbol.containerId},
         {"range",range(symbol.range)},{"selectionRange",range(symbol.selectionRange)},
         {"endRange",range(symbol.endRange)},{"abstract",symbol.abstract}});
   Value::Array references;
   for (const auto &reference : result.references)
      references.push_back(Value::Object{{"sourceId",reference.sourceId},{"targetId",reference.targetId},
         {"kind",reference.kind},{"range",range(reference.range)}});
   Value::Array dependencies;
   for (const auto &dependency : result.dependencies)
      dependencies.push_back(Value::Object{{"sourceUri",dependency.sourceUri},
         {"targetUri",dependency.targetUri},{"model",dependency.model},{"range",range(dependency.range)}});
   Value::Array nodes;
   for (const auto &node : result.diagram.nodes) {
      Value::Array members;
      for (const auto &member : node.members)
         members.push_back(Value::Object{{"name",member.name},{"type",member.type},
            {"cardinality",member.cardinality},{"declaringType",member.declaringType},
            {"inherited",member.inherited},{"inlineEnumValues",strings(member.inlineEnumValues)}});
      nodes.push_back(Value::Object{{"id",node.id},{"containerId",node.containerId},
         {"label",node.label},{"kind",node.kind},{"abstract",node.abstract},{"range",range(node.range)},
         {"stereotypes",strings(node.stereotypes)},{"enumValues",strings(node.enumValues)},
         {"operations",strings(node.operations)},{"members",std::move(members)}});
   }
   Value::Array edges;
   for (const auto &edge : result.diagram.edges)
      edges.push_back(Value::Object{{"id",edge.id},{"sourceId",edge.sourceId},{"targetId",edge.targetId},
         {"kind",edge.kind},{"label",edge.label},{"cardinality",edge.cardinality},
         {"sourceCardinality",edge.sourceCardinality},{"targetCardinality",edge.targetCardinality}});
   Value::Array sections;
   for (const auto &section : result.documentation.sections)
      sections.push_back(Value::Object{{"id",section.id},{"title",section.title},
         {"kind",section.kind},{"text",section.text},{"level",section.level}});
   Value::Array models;
   for (const auto &model : result.documentation.models) models.push_back(documentationModel(model));
   Value::Object versions;
   for (const auto &entry : result.documentVersions) versions[entry.first] = static_cast<double>(entry.second);
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","semantic"},{"success",result.success},{"cancelled",result.cancelled},
      {"roots",strings(result.roots)},{"documentVersions",std::move(versions)},
      {"missingModels",strings(result.missingModels)},{"symbols",std::move(symbols)},
      {"references",std::move(references)},{"dependencies",std::move(dependencies)},
      {"diagram",Value::Object{{"nodes",std::move(nodes)},{"edges",std::move(edges)}}},
      {"documentation",Value::Object{{"title",result.documentation.title},
         {"sections",std::move(sections)},{"models",std::move(models)}}},
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)}};
}

Value compilationAnalysisResult(const CompilationAnalysisResult &result)
{
   Value::Array syntax;
   for (const auto &snapshot : result.syntax) syntax.push_back(syntaxResult(snapshot));
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","compilation-analysis"},{"compilation",compileResult(result.compilation)},
      {"semantic",semanticResult(result.semantic)},{"syntax",std::move(syntax)}};
}

Value incrementalStatsResult(const IncrementalStats &stats)
{
   const auto count = [](std::uint64_t value) { return Value(static_cast<double>(value)); };
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","incremental-stats"},{"sourceAdds",count(stats.sourceAdds)},
      {"sourceRemoves",count(stats.sourceRemoves)},{"sourceNoOps",count(stats.sourceNoOps)},
      {"versionOnlyUpdates",count(stats.versionOnlyUpdates)},{"contentChanges",count(stats.contentChanges)},
      {"sourceReintroductions",count(stats.sourceReintroductions)},
      {"rejectedUpdates",count(stats.rejectedUpdates)},{"parseRequests",count(stats.parseRequests)},
      {"editorSnapshotRequests",count(stats.editorSnapshotRequests)},
      {"strictParserBuilds",count(stats.strictParserBuilds)},{"tolerantParserBuilds",count(stats.tolerantParserBuilds)},
      {"strictParserHits",count(stats.strictParserHits)},{"tolerantParserHits",count(stats.tolerantParserHits)},
      {"parserEntries",count(stats.parserEntries)},{"parserRetainedBytes",count(stats.parserRetainedBytes)},
      {"parserBuilds",count(stats.parserBuilds)},{"parserHits",count(stats.parserHits)},
      {"parserEvictions",count(stats.parserEvictions)},{"parserBytes",count(stats.parserBytes)},
      {"syntaxMaterializations",count(stats.syntaxMaterializations)},
      {"editorMaterializations",count(stats.editorMaterializations)},
      {"rootAnalysisHits",count(stats.rootAnalysisHits)},{"rootAnalysisMisses",count(stats.rootAnalysisMisses)},
      {"rootAnalysisBuilds",count(stats.rootAnalysisBuilds)},{"rootAnalysisEvictions",count(stats.rootAnalysisEvictions)},
      {"rootEntries",count(stats.rootEntries)},{"rootRetainedBytes",count(stats.rootRetainedBytes)},
      {"invalidatedRootEntries",count(stats.invalidatedRootEntries)},
      {"reusedClosureSources",count(stats.reusedClosureSources)},
      {"reparsedClosureSources",count(stats.reparsedClosureSources)},
      {"compilationInvocations",count(stats.compilationInvocations)},
      {"compileRequests",count(stats.compileRequests)},{"compileExecutions",count(stats.compileExecutions)},
      {"cancelledPlans",count(stats.cancelledPlans)}};
}

Value incrementalTraceResult(const IncrementalTrace &trace)
{
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","incremental-trace"},{"operation",trace.operation},{"planKind",trace.planKind},
      {"roots",strings(trace.roots)},{"closure",strings(trace.closure)},
      {"parserHits",strings(trace.parserHits)},{"parserMisses",strings(trace.parserMisses)},
      {"strictParserHits",strings(trace.strictParserHits)},
      {"strictParserBuilds",strings(trace.strictParserBuilds)},
      {"tolerantParserHits",strings(trace.tolerantParserHits)},
      {"tolerantParserBuilds",strings(trace.tolerantParserBuilds)},
      {"invalidatedRoots",strings(trace.invalidatedRoots)},{"reasons",strings(trace.reasons)},
      {"bytesRetained",static_cast<double>(trace.bytesRetained)},
      {"bytesReleased",static_cast<double>(trace.bytesReleased)}};
}

Value incrementalCacheSnapshotResult(const IncrementalCacheSnapshot &snapshot)
{
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","incremental-cache-snapshot"},{"parserEntries",static_cast<double>(snapshot.parserEntries)},
      {"parserRetainedBytes",static_cast<double>(snapshot.parserRetainedBytes)},
      {"parserEvictions",static_cast<double>(snapshot.parserEvictions)},
      {"rootEntries",static_cast<double>(snapshot.rootEntries)},
      {"rootRetainedBytes",static_cast<double>(snapshot.rootRetainedBytes)},
      {"rootEvictions",static_cast<double>(snapshot.rootEvictions)},
      {"parserInvariants",snapshot.parserInvariants},{"rootInvariants",snapshot.rootInvariants}};
}

Value formattingResult(const FormatResult &result)
{
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",version()},
      {"kind","formatting"},{"success",result.success},{"applicable",result.applicable},
      {"changed",result.changed},{"text",result.text},{"diagnostics",diagnostics(result.diagnostics)}};
}

} // namespace ilic::capi
