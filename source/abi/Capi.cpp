#include "../../include/ilic/capi.h"

#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Formatter.h"
#include "../../include/ilic/Semantic.h"
#include "Json.h"

#include <cstdlib>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace {

using ilic::json::Value;
std::mutex registryMutex;
std::map<std::uint32_t,std::shared_ptr<ilic::CompilerSession>> sessions;
std::map<std::uint32_t,std::string> results;
std::uint32_t nextSession = 1;
std::uint32_t nextResult = 1;

std::shared_ptr<ilic::CompilerSession> getSession(std::uint32_t handle)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   auto found = sessions.find(handle);
   return found == sessions.end() ? nullptr : found->second;
}

const ilic::SourceManager &sessionSources(
   const std::shared_ptr<ilic::CompilerSession> &session)
{
   return static_cast<const ilic::CompilerSession &>(*session).sources();
}

std::uint32_t storeJson(std::string value)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   const std::uint32_t handle = nextResult++;
   results[handle] = std::move(value);
   return handle;
}

std::uint32_t store(Value value)
{
   return storeJson(ilic::json::stringify(value));
}

const char *severity(ilic::DiagnosticSeverity value)
{
   switch (value) {
      case ilic::DiagnosticSeverity::Error: return "error";
      case ilic::DiagnosticSeverity::Warning: return "warning";
      case ilic::DiagnosticSeverity::Information: return "information";
      case ilic::DiagnosticSeverity::Hint: return "hint";
   }
   return "error";
}

const char *phase(ilic::DiagnosticPhase value)
{
   switch (value) {
      case ilic::DiagnosticPhase::Unknown: return "unknown";
      case ilic::DiagnosticPhase::Lexical: return "lexical";
      case ilic::DiagnosticPhase::Syntax: return "syntax";
      case ilic::DiagnosticPhase::EditorRecovery: return "editor-recovery";
      case ilic::DiagnosticPhase::ModelDiscovery: return "model-discovery";
      case ilic::DiagnosticPhase::Resolution: return "resolution";
      case ilic::DiagnosticPhase::Semantic: return "semantic";
      case ilic::DiagnosticPhase::Translation: return "translation";
      case ilic::DiagnosticPhase::Repository: return "repository";
      case ilic::DiagnosticPhase::Formatting: return "formatting";
      case ilic::DiagnosticPhase::Request: return "request";
      case ilic::DiagnosticPhase::Internal: return "internal";
   }
   return "unknown";
}

const char *tag(ilic::DiagnosticTag value)
{
   switch (value) {
      case ilic::DiagnosticTag::Primary: return "primary";
      case ilic::DiagnosticTag::Cascaded: return "cascaded";
      case ilic::DiagnosticTag::Recovery: return "recovery";
      case ilic::DiagnosticTag::Deprecated: return "deprecated";
      case ilic::DiagnosticTag::Unnecessary: return "unnecessary";
   }
   return "primary";
}

const char *level(ilic::LogLevel value)
{
   switch (value) {
      case ilic::LogLevel::Trace: return "trace";
      case ilic::LogLevel::Debug: return "debug";
      case ilic::LogLevel::Information: return "information";
      case ilic::LogLevel::Warning: return "warning";
      case ilic::LogLevel::Error: return "error";
   }
   return "information";
}

Value position(const ilic::Position &position)
{
   return Value::Object{{"line",position.line},{"character",position.character},
      {"byteOffset",position.byteOffset}};
}

Value range(const ilic::SourceRange &range)
{
   if (!range.valid) return nullptr;
   return Value::Object{{"uri",range.uri},{"start",position(range.start)},{"end",position(range.end)}};
}

Value diagnostics(const std::vector<ilic::Diagnostic> &diagnostics)
{
   Value::Array values;
   for (const auto &diagnostic : diagnostics) {
      Value::Array related;
      for (const auto &information : diagnostic.relatedInformation) {
         related.push_back(Value::Object{{"range",range(information.range)},{"message",information.message}});
      }
      Value::Array notes;
      for (const auto &note : diagnostic.notes) notes.emplace_back(note);
      Value::Object item{
         {"severity",severity(diagnostic.severity)},
         {"code",diagnostic.code},
         {"message",diagnostic.message},
         {"range",range(diagnostic.range)},
         {"relatedInformation",std::move(related)},
         {"notes",std::move(notes)},
         {"treatedAsError",diagnostic.treatedAsError}
      };
      if (!diagnostic.source.empty()) item["source"] = diagnostic.source;
      if (diagnostic.phase != ilic::DiagnosticPhase::Unknown)
         item["phase"] = phase(diagnostic.phase);
      if (!diagnostic.tags.empty()) {
         Value::Array tags;
         for (const auto value : diagnostic.tags) tags.emplace_back(tag(value));
         item["tags"] = std::move(tags);
      }
      if (!diagnostic.helpId.empty()) item["helpId"] = diagnostic.helpId;
      if (!diagnostic.fingerprint.empty()) item["fingerprint"] = diagnostic.fingerprint;
      values.push_back(std::move(item));
   }
   return values;
}

Value logs(const std::vector<ilic::LogEvent> &logs)
{
   Value::Array values;
   for (const auto &event : logs) {
      Value::Object context;
      for (const auto &entry : event.context) context[entry.first] = entry.second;
      values.push_back(Value::Object{{"level",level(event.level)},{"category",event.category},
         {"message",event.message},{"context",std::move(context)}});
   }
   return values;
}

Value transcript(const std::vector<std::string> &lines)
{
   Value::Array values;
   for (const auto &line : lines) values.emplace_back(line);
   return values;
}

Value errorResult(const char *kind,const std::string &message,
   const std::string &uri = std::string(),std::uint64_t documentVersion = 0)
{
   Value::Object result{{"schemaVersion",1},{"abiVersion",1},
      {"compilerVersion",ilic::version()},{"kind",kind},{"success",false},
      {"diagnostics",Value::Array{Value::Object{{"severity","error"},
         {"code","ILIC-ABI-REQUEST"},{"message",message},{"range",nullptr},
         {"relatedInformation",Value::Array{}},{"notes",Value::Array{}},
         {"treatedAsError",false}}}}};
   const std::string value(kind);
   if (value == "syntax") {
      result["uri"] = uri;
      result["documentVersion"] = static_cast<double>(documentVersion);
      result["iliVersion"] = "";
      result["tokens"] = Value::Array{};
      result["nodes"] = Value::Array{};
      result["contexts"] = Value::Array{};
      result["imports"] = Value::Array{};
      result["importReferences"] = Value::Array{};
   }
   else if (value == "editor") {
      result["recovered"] = false;
      result["complete"] = false;
      result["uri"] = uri;
      result["documentVersion"] = static_cast<double>(documentVersion);
      result["iliVersion"] = "unknown";
      result["declarations"] = Value::Array{};
      result["references"] = Value::Array{};
      result["imports"] = Value::Array{};
      result["contexts"] = Value::Array{};
   }
   else if (value == "semantic") {
      result["cancelled"] = false;
      result["roots"] = Value::Array{};
      result["documentVersions"] = Value::Object{};
      result["missingModels"] = Value::Array{};
      result["symbols"] = Value::Array{};
      result["references"] = Value::Array{};
      result["dependencies"] = Value::Array{};
      result["diagram"] = Value::Object{{"nodes",Value::Array{}},{"edges",Value::Array{}}};
      result["documentation"] = Value::Object{{"title",""},{"sections",Value::Array{}}};
      result["logs"] = Value::Array{};
   }
   else if (value == "compilation") {
      result["cancelled"] = false;
      result["errorCount"] = 1;
      result["warningCount"] = 0;
      result["missingModels"] = Value::Array{};
      result["models"] = Value::Array{};
      result["logs"] = Value::Array{};
      result["transcript"] = Value::Array{};
   }
   else if (value == "formatting") {
      result["applicable"] = false;
      result["changed"] = false;
      result["text"] = "";
   }
   return result;
}

ilic::CompilationRequest compileRequest(const Value &json)
{
   if (!json.isObject()) throw std::runtime_error("compile request must be an object");
   if (!json.get("schemaVersion").isNumber() ||
      static_cast<int>(json.get("schemaVersion").number()) != 1)
      throw std::runtime_error("unsupported schemaVersion");
   ilic::CompilationRequest request;
   if (!json.get("roots").isArray() || json.get("roots").array().empty())
      throw std::runtime_error("roots must be a non-empty array");
   for (const auto &root : json.get("roots").array()) {
      if (!root.isString() || root.string().empty())
         throw std::runtime_error("roots must contain non-empty strings");
      request.roots.push_back(root.string());
   }
   const Value &options = json.get("options");
   if (!options.isNull() && !options.isObject()) throw std::runtime_error("options must be an object");
   if (!options.get("autoSearch").isNull() && !options.get("autoSearch").isBool())
      throw std::runtime_error("autoSearch must be a boolean");
   if (!options.get("warningsAsErrors").isNull() && !options.get("warningsAsErrors").isBool())
      throw std::runtime_error("warningsAsErrors must be a boolean");
   request.options.autoSearch = options.get("autoSearch").boolean(true);
   request.options.warningsAsErrors = options.get("warningsAsErrors").boolean(false);
   if (!options.get("modelDirectories").isNull() && !options.get("modelDirectories").isArray())
      throw std::runtime_error("modelDirectories must be an array");
   for (const auto &directory : options.get("modelDirectories").array())
      if (directory.isString() && !directory.string().empty())
         request.options.modelDirectories.push_back(directory.string());
      else throw std::runtime_error("modelDirectories must contain non-empty strings");
   if (!json.get("externalMetaAttributes").isNull() &&
      !json.get("externalMetaAttributes").isArray())
      throw std::runtime_error("externalMetaAttributes must be an array");
   for (const auto &attribute : json.get("externalMetaAttributes").array()) {
      if (!attribute.isObject() || !attribute.get("element").isString() ||
         attribute.get("element").string().empty() || !attribute.get("name").isString() ||
         attribute.get("name").string().empty() || !attribute.get("value").isString())
         throw std::runtime_error("invalid external meta attribute");
      request.externalMetaAttributes.push_back({attribute.get("element").string(),
         attribute.get("name").string(),attribute.get("value").string()});
   }
   return request;
}

Value compileResult(const ilic::CompilationResult &result)
{
   Value::Array missing;
   for (const auto &model : result.missingModels) missing.emplace_back(model);
   Value::Array models;
   for (const auto &model : result.models) {
      Value::Array metadata;
      for (const auto &attribute : model.metaAttributes)
         metadata.push_back(Value::Object{{"name",attribute.name},{"value",attribute.value}});
      models.push_back(Value::Object{{"name",model.name},{"iliVersion",model.iliVersion},
         {"uri",model.uri},{"metaAttributes",std::move(metadata)}});
   }
   return Value::Object{
      {"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","compilation"},{"success",result.success},{"cancelled",result.cancelled},
      {"errorCount",result.errorCount},{"warningCount",result.warningCount},
      {"missingModels",std::move(missing)},{"models",std::move(models)},
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)},
      {"transcript",transcript(result.transcript)}
   };
}

Value syntaxResult(const ilic::SyntaxSnapshot &result)
{
   Value::Array tokens;
   for (const auto &token : result.tokens) {
      tokens.push_back(Value::Object{{"kind",token.kind},{"text",token.text},
         {"channel",token.channel},{"range",range(token.range)}});
   }
   Value::Array nodes;
   for (const auto &node : result.nodes) {
      nodes.push_back(Value::Object{{"id",node.id},
         {"parent",node.hasParent ? Value(node.parent) : Value(nullptr)},
         {"kind",node.kind},{"range",range(node.range)}});
   }
   Value::Array contexts;
   for (const auto &context : result.contexts)
      contexts.push_back(Value::Object{{"kind",context.kind},{"range",range(context.range)}});
   Value::Array imports;
   for (const auto &model : result.imports) imports.emplace_back(model);
   Value::Array importReferences;
   for (const auto &reference : result.importReferences)
      importReferences.push_back(Value::Object{{"model",reference.model},
         {"unqualified",reference.unqualified},{"range",range(reference.range)}});
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","syntax"},{"success",result.success},{"uri",result.uri},
      {"documentVersion",static_cast<double>(result.documentVersion)},
      {"iliVersion",result.iliVersion},{"tokens",std::move(tokens)},
      {"nodes",std::move(nodes)},{"contexts",std::move(contexts)},
      {"imports",std::move(imports)},{"importReferences",std::move(importReferences)},
      {"diagnostics",diagnostics(result.diagnostics)}};
}

const char *editorSymbolKind(ilic::EditorSymbolKind kind)
{
   switch (kind) {
      case ilic::EditorSymbolKind::Model: return "model";
      case ilic::EditorSymbolKind::Topic: return "topic";
      case ilic::EditorSymbolKind::Class: return "class";
      case ilic::EditorSymbolKind::Structure: return "structure";
      case ilic::EditorSymbolKind::Association: return "association";
      case ilic::EditorSymbolKind::View: return "view";
      case ilic::EditorSymbolKind::Graphic: return "graphic";
      case ilic::EditorSymbolKind::Domain: return "domain";
      case ilic::EditorSymbolKind::Unit: return "unit";
      case ilic::EditorSymbolKind::Attribute: return "attribute";
   }
   return "attribute";
}

const char *editorReferenceKind(ilic::EditorReferenceKind kind)
{
   switch (kind) {
      case ilic::EditorReferenceKind::Extends: return "extends";
      case ilic::EditorReferenceKind::Type: return "type";
      case ilic::EditorReferenceKind::Collection: return "collection";
      case ilic::EditorReferenceKind::Reference: return "reference";
      case ilic::EditorReferenceKind::Unit: return "unit";
   }
   return "type";
}

void appendJsonField(std::string &output,bool &first,const char *name)
{
   if (!first) output.push_back(',');
   first = false;
   ilic::json::appendQuoted(output,name);
   output.push_back(':');
}

void appendJsonItemSeparator(std::string &output,bool &first)
{
   if (!first) output.push_back(',');
   first = false;
}

void appendJsonPosition(std::string &output,const ilic::Position &value)
{
   output.push_back('{');
   bool first = true;
   appendJsonField(output,first,"line");
   output += std::to_string(value.line);
   appendJsonField(output,first,"character");
   output += std::to_string(value.character);
   appendJsonField(output,first,"byteOffset");
   output += std::to_string(value.byteOffset);
   output.push_back('}');
}

void appendJsonRange(std::string &output,const ilic::SourceRange &value)
{
   if (!value.valid) {
      output += "null";
      return;
   }
   output.push_back('{');
   bool first = true;
   appendJsonField(output,first,"uri");
   ilic::json::appendQuoted(output,value.uri);
   appendJsonField(output,first,"start");
   appendJsonPosition(output,value.start);
   appendJsonField(output,first,"end");
   appendJsonPosition(output,value.end);
   output.push_back('}');
}

void appendJsonDiagnostics(std::string &output,const std::vector<ilic::Diagnostic> &values)
{
   output.push_back('[');
   bool firstDiagnostic = true;
   for (const auto &diagnostic : values) {
      appendJsonItemSeparator(output,firstDiagnostic);
      output.push_back('{');
      bool first = true;
      appendJsonField(output,first,"severity");
      ilic::json::appendQuoted(output,severity(diagnostic.severity));
      appendJsonField(output,first,"code");
      ilic::json::appendQuoted(output,diagnostic.code);
      appendJsonField(output,first,"message");
      ilic::json::appendQuoted(output,diagnostic.message);
      appendJsonField(output,first,"range");
      appendJsonRange(output,diagnostic.range);
      appendJsonField(output,first,"relatedInformation");
      output.push_back('[');
      bool firstRelated = true;
      for (const auto &information : diagnostic.relatedInformation) {
         appendJsonItemSeparator(output,firstRelated);
         output.push_back('{');
         bool firstInformation = true;
         appendJsonField(output,firstInformation,"range");
         appendJsonRange(output,information.range);
         appendJsonField(output,firstInformation,"message");
         ilic::json::appendQuoted(output,information.message);
         output.push_back('}');
      }
      output.push_back(']');
      appendJsonField(output,first,"notes");
      output.push_back('[');
      bool firstNote = true;
      for (const auto &note : diagnostic.notes) {
         appendJsonItemSeparator(output,firstNote);
         ilic::json::appendQuoted(output,note);
      }
      output.push_back(']');
      appendJsonField(output,first,"treatedAsError");
      output += diagnostic.treatedAsError ? "true" : "false";
      if (!diagnostic.source.empty()) {
         appendJsonField(output,first,"source");
         ilic::json::appendQuoted(output,diagnostic.source);
      }
      if (diagnostic.phase != ilic::DiagnosticPhase::Unknown) {
         appendJsonField(output,first,"phase");
         ilic::json::appendQuoted(output,phase(diagnostic.phase));
      }
      if (!diagnostic.tags.empty()) {
         appendJsonField(output,first,"tags");
         output.push_back('[');
         bool firstTag = true;
         for (const auto value : diagnostic.tags) {
            appendJsonItemSeparator(output,firstTag);
            ilic::json::appendQuoted(output,tag(value));
         }
         output.push_back(']');
      }
      if (!diagnostic.helpId.empty()) {
         appendJsonField(output,first,"helpId");
         ilic::json::appendQuoted(output,diagnostic.helpId);
      }
      if (!diagnostic.fingerprint.empty()) {
         appendJsonField(output,first,"fingerprint");
         ilic::json::appendQuoted(output,diagnostic.fingerprint);
      }
      output.push_back('}');
   }
   output.push_back(']');
}

std::string editorResultJson(const ilic::EditorSnapshot &result)
{
   std::string output;
   output.reserve(256 + result.declarations.size() * 360);
   output.push_back('{');
   bool first = true;
   appendJsonField(output,first,"schemaVersion");
   output += "1";
   appendJsonField(output,first,"abiVersion");
   output += "1";
   appendJsonField(output,first,"compilerVersion");
   ilic::json::appendQuoted(output,ilic::version());
   appendJsonField(output,first,"kind");
   output += "\"editor\"";
   appendJsonField(output,first,"success");
   output += result.success ? "true" : "false";
   appendJsonField(output,first,"recovered");
   output += result.recovered ? "true" : "false";
   appendJsonField(output,first,"complete");
   output += result.complete ? "true" : "false";
   appendJsonField(output,first,"uri");
   ilic::json::appendQuoted(output,result.uri);
   appendJsonField(output,first,"documentVersion");
   output += std::to_string(result.documentVersion);
   appendJsonField(output,first,"iliVersion");
   ilic::json::appendQuoted(output,result.iliVersion);
   appendJsonField(output,first,"declarations");
   output.push_back('[');
   bool firstDeclaration = true;
   for (const auto &declaration : result.declarations) {
      appendJsonItemSeparator(output,firstDeclaration);
      output.push_back('{');
      bool firstValue = true;
      appendJsonField(output,firstValue,"id");
      ilic::json::appendQuoted(output,declaration.id);
      appendJsonField(output,firstValue,"name");
      ilic::json::appendQuoted(output,declaration.name);
      appendJsonField(output,firstValue,"qualifiedName");
      ilic::json::appendQuoted(output,declaration.qualifiedName);
      appendJsonField(output,firstValue,"kind");
      ilic::json::appendQuoted(output,editorSymbolKind(declaration.kind));
      appendJsonField(output,firstValue,"containerId");
      if (declaration.hasContainer) ilic::json::appendQuoted(output,declaration.containerId);
      else output += "null";
      appendJsonField(output,firstValue,"range");
      appendJsonRange(output,declaration.range);
      appendJsonField(output,firstValue,"selectionRange");
      appendJsonRange(output,declaration.selectionRange);
      appendJsonField(output,firstValue,"endRange");
      appendJsonRange(output,declaration.endRange);
      output.push_back('}');
   }
   output.push_back(']');
   appendJsonField(output,first,"references");
   output.push_back('[');
   bool firstReference = true;
   for (const auto &reference : result.references) {
      appendJsonItemSeparator(output,firstReference);
      output.push_back('{');
      bool firstValue = true;
      appendJsonField(output,firstValue,"text");
      ilic::json::appendQuoted(output,reference.text);
      appendJsonField(output,firstValue,"kind");
      ilic::json::appendQuoted(output,editorReferenceKind(reference.kind));
      appendJsonField(output,firstValue,"sourceId");
      if (reference.hasSource) ilic::json::appendQuoted(output,reference.sourceId);
      else output += "null";
      appendJsonField(output,firstValue,"range");
      appendJsonRange(output,reference.range);
      output.push_back('}');
   }
   output.push_back(']');
   appendJsonField(output,first,"imports");
   output.push_back('[');
   bool firstImport = true;
   for (const auto &reference : result.imports) {
      appendJsonItemSeparator(output,firstImport);
      output.push_back('{');
      bool firstValue = true;
      appendJsonField(output,firstValue,"model");
      ilic::json::appendQuoted(output,reference.model);
      appendJsonField(output,firstValue,"unqualified");
      output += reference.unqualified ? "true" : "false";
      appendJsonField(output,firstValue,"range");
      appendJsonRange(output,reference.range);
      output.push_back('}');
   }
   output.push_back(']');
   appendJsonField(output,first,"contexts");
   output.push_back('[');
   bool firstContext = true;
   for (const auto &context : result.contexts) {
      appendJsonItemSeparator(output,firstContext);
      output.push_back('{');
      bool firstValue = true;
      appendJsonField(output,firstValue,"kind");
      ilic::json::appendQuoted(output,context.kind);
      appendJsonField(output,firstValue,"range");
      appendJsonRange(output,context.range);
      output.push_back('}');
   }
   output.push_back(']');
   appendJsonField(output,first,"diagnostics");
   appendJsonDiagnostics(output,result.diagnostics);
   output.push_back('}');
   return output;
}

Value semanticResult(const ilic::SemanticSnapshot &result)
{
   Value::Array roots;
   for (const auto &root : result.roots) roots.emplace_back(root);
   Value::Object versions;
   for (const auto &entry : result.documentVersions)
      versions[entry.first] = static_cast<double>(entry.second);
   Value::Array symbols;
   for (const auto &symbol : result.symbols) {
      symbols.push_back(Value::Object{{"id",symbol.id},{"name",symbol.name},
         {"qualifiedName",symbol.qualifiedName},{"kind",symbol.kind},
         {"containerId",symbol.containerId},{"range",range(symbol.range)},
         {"selectionRange",range(symbol.selectionRange)},
         {"endRange",range(symbol.endRange)},
         {"abstract",symbol.abstract}});
   }
   Value::Array references;
   for (const auto &reference : result.references) {
      references.push_back(Value::Object{{"sourceId",reference.sourceId},
         {"targetId",reference.targetId},{"kind",reference.kind},
         {"range",range(reference.range)}});
   }
   Value::Array dependencies;
   for (const auto &dependency : result.dependencies) {
      dependencies.push_back(Value::Object{{"sourceUri",dependency.sourceUri},
         {"targetUri",dependency.targetUri},{"model",dependency.model},
         {"range",range(dependency.range)}});
   }
   Value::Array diagramNodes;
   for (const auto &node : result.diagram.nodes) {
      Value::Array members;
      for (const auto &member : node.members) {
         Value::Array inlineEnumValues;
         for (const auto &value : member.inlineEnumValues)
            inlineEnumValues.emplace_back(value);
         members.push_back(Value::Object{{"name",member.name},{"type",member.type},
            {"cardinality",member.cardinality},{"declaringType",member.declaringType},
            {"inherited",member.inherited},
            {"inlineEnumValues",std::move(inlineEnumValues)}});
      }
      Value::Array stereotypes;
      for (const auto &stereotype : node.stereotypes)
         stereotypes.emplace_back(stereotype);
      Value::Array enumValues;
      for (const auto &value : node.enumValues) enumValues.emplace_back(value);
      Value::Array operations;
      for (const auto &operation : node.operations)
         operations.emplace_back(operation);
      diagramNodes.push_back(Value::Object{{"id",node.id},{"containerId",node.containerId},
         {"label",node.label},{"kind",node.kind},{"abstract",node.abstract},
         {"range",range(node.range)},{"stereotypes",std::move(stereotypes)},
         {"members",std::move(members)},{"enumValues",std::move(enumValues)},
         {"operations",std::move(operations)}});
   }
   Value::Array diagramEdges;
   for (const auto &edge : result.diagram.edges) {
      diagramEdges.push_back(Value::Object{{"id",edge.id},{"sourceId",edge.sourceId},
         {"targetId",edge.targetId},{"kind",edge.kind},{"label",edge.label},
         {"cardinality",edge.cardinality},
         {"sourceCardinality",edge.sourceCardinality},
         {"targetCardinality",edge.targetCardinality}});
   }
   Value::Array sections;
   for (const auto &section : result.documentation.sections) {
      sections.push_back(Value::Object{{"id",section.id},{"title",section.title},
         {"kind",section.kind},{"text",section.text},{"level",section.level}});
   }
   Value::Array models;
   for (const auto &model : result.documentation.models) {
      Value::Array viewables;
      for (const auto &viewable : model.viewables) {
         Value::Array rows;
         for (const auto &row : viewable.rows) {
            rows.push_back(Value::Object{{"name",row.name},
               {"cardinality",row.cardinality},{"type",row.type},
               {"description",row.description}});
         }
         viewables.push_back(Value::Object{{"name",viewable.name},
            {"kind",viewable.kind},{"isAbstract",viewable.isAbstract},
            {"documentation",viewable.documentation},{"rows",std::move(rows)}});
      }
      Value::Array enumerations;
      for (const auto &enumeration : model.enumerations) {
         Value::Array entries;
         for (const auto &entry : enumeration.entries)
            entries.push_back(Value::Object{{"value",entry.value},
               {"documentation",entry.documentation}});
         enumerations.push_back(Value::Object{{"name",enumeration.name},
            {"documentation",enumeration.documentation},{"entries",std::move(entries)}});
      }
      Value::Array topics;
      for (const auto &topic : model.topics) {
         Value::Array topicViewables;
         for (const auto &viewable : topic.viewables) {
            Value::Array rows;
            for (const auto &row : viewable.rows)
               rows.push_back(Value::Object{{"name",row.name},
                  {"cardinality",row.cardinality},{"type",row.type},
                  {"description",row.description}});
            topicViewables.push_back(Value::Object{{"name",viewable.name},
               {"kind",viewable.kind},{"isAbstract",viewable.isAbstract},
               {"documentation",viewable.documentation},{"rows",std::move(rows)}});
         }
         Value::Array topicEnumerations;
         for (const auto &enumeration : topic.enumerations) {
            Value::Array entries;
            for (const auto &entry : enumeration.entries)
               entries.push_back(Value::Object{{"value",entry.value},
                  {"documentation",entry.documentation}});
            topicEnumerations.push_back(Value::Object{{"name",enumeration.name},
               {"documentation",enumeration.documentation},{"entries",std::move(entries)}});
         }
         topics.push_back(Value::Object{{"name",topic.name},{"documentation",topic.documentation},
            {"viewables",std::move(topicViewables)},
            {"enumerations",std::move(topicEnumerations)}});
      }
      models.push_back(Value::Object{{"name",model.name},{"uri",model.uri},
         {"title",model.title},{"shortDescription",model.shortDescription},
         {"topics",std::move(topics)},{"viewables",std::move(viewables)},
         {"enumerations",std::move(enumerations)}});
   }
   Value::Array missingModels;
   for (const auto &model : result.missingModels) missingModels.emplace_back(model);
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","semantic"},{"success",result.success},{"cancelled",result.cancelled},
      {"roots",std::move(roots)},{"documentVersions",std::move(versions)},
      {"missingModels",std::move(missingModels)},
      {"symbols",std::move(symbols)},{"references",std::move(references)},
      {"dependencies",std::move(dependencies)},
      {"diagram",Value::Object{{"nodes",std::move(diagramNodes)},{"edges",std::move(diagramEdges)}}},
      {"documentation",Value::Object{{"title",result.documentation.title},
         {"sections",std::move(sections)},{"models",std::move(models)}}},
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)}};
}

Value compilationAnalysisResult(const ilic::CompilationAnalysisResult &result)
{
   Value::Array syntax;
   for (const auto &snapshot : result.syntax) syntax.push_back(syntaxResult(snapshot));
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","compilation-analysis"},{"compilation",compileResult(result.compilation)},
      {"semantic",semanticResult(result.semantic)},{"syntax",std::move(syntax)}};
}

Value incrementalStatsResult(const ilic::IncrementalStats &stats)
{
   const auto count = [](std::uint64_t value) {
      return Value(static_cast<double>(value));
   };
   return Value::Object{
      {"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","incremental-stats"},
      {"sourceAdds",count(stats.sourceAdds)},{"sourceRemoves",count(stats.sourceRemoves)},
      {"sourceNoOps",count(stats.sourceNoOps)},{"versionOnlyUpdates",count(stats.versionOnlyUpdates)},
      {"contentChanges",count(stats.contentChanges)},{"sourceReintroductions",count(stats.sourceReintroductions)},
      {"rejectedUpdates",count(stats.rejectedUpdates)},
      {"parseRequests",count(stats.parseRequests)},
      {"editorSnapshotRequests",count(stats.editorSnapshotRequests)},
      {"strictParserBuilds",count(stats.strictParserBuilds)},
      {"tolerantParserBuilds",count(stats.tolerantParserBuilds)},
      {"strictParserHits",count(stats.strictParserHits)},
      {"tolerantParserHits",count(stats.tolerantParserHits)},
      {"parserEntries",count(stats.parserEntries)},
      {"parserRetainedBytes",count(stats.parserRetainedBytes)},
      {"parserBuilds",count(stats.parserBuilds)},
      {"parserHits",count(stats.parserHits)},{"parserEvictions",count(stats.parserEvictions)},
      {"parserBytes",count(stats.parserBytes)},{"syntaxMaterializations",count(stats.syntaxMaterializations)},
      {"editorMaterializations",count(stats.editorMaterializations)},
      {"rootAnalysisHits",count(stats.rootAnalysisHits)},{"rootAnalysisMisses",count(stats.rootAnalysisMisses)},
      {"rootAnalysisBuilds",count(stats.rootAnalysisBuilds)},
      {"rootAnalysisEvictions",count(stats.rootAnalysisEvictions)},
      {"rootEntries",count(stats.rootEntries)},
      {"rootRetainedBytes",count(stats.rootRetainedBytes)},
      {"invalidatedRootEntries",count(stats.invalidatedRootEntries)},
      {"reusedClosureSources",count(stats.reusedClosureSources)},
      {"reparsedClosureSources",count(stats.reparsedClosureSources)},
      {"compilationInvocations",count(stats.compilationInvocations)},
      {"compileRequests",count(stats.compileRequests)},
      {"compileExecutions",count(stats.compileExecutions)},
      {"cancelledPlans",count(stats.cancelledPlans)}
   };
}

Value incrementalTraceResult(const ilic::IncrementalTrace &trace)
{
   const auto strings = [](const std::vector<std::string> &values) {
      Value::Array result;
      for (const auto &value : values) result.emplace_back(value);
      return result;
   };
   return Value::Object{
      {"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","incremental-trace"},{"operation",trace.operation},{"planKind",trace.planKind},
      {"roots",strings(trace.roots)},{"closure",strings(trace.closure)},
      {"parserHits",strings(trace.parserHits)},{"parserMisses",strings(trace.parserMisses)},
      {"strictParserHits",strings(trace.strictParserHits)},
      {"strictParserBuilds",strings(trace.strictParserBuilds)},
      {"tolerantParserHits",strings(trace.tolerantParserHits)},
      {"tolerantParserBuilds",strings(trace.tolerantParserBuilds)},
      {"invalidatedRoots",strings(trace.invalidatedRoots)},
      {"reasons",strings(trace.reasons)},
      {"bytesRetained",static_cast<double>(trace.bytesRetained)},
      {"bytesReleased",static_cast<double>(trace.bytesReleased)}
   };
}

Value incrementalCacheSnapshotResult(const ilic::IncrementalCacheSnapshot &snapshot)
{
   return Value::Object{
      {"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","incremental-cache-snapshot"},
      {"parserEntries",static_cast<double>(snapshot.parserEntries)},
      {"parserRetainedBytes",static_cast<double>(snapshot.parserRetainedBytes)},
      {"parserEvictions",static_cast<double>(snapshot.parserEvictions)},
      {"rootEntries",static_cast<double>(snapshot.rootEntries)},
      {"rootRetainedBytes",static_cast<double>(snapshot.rootRetainedBytes)},
      {"rootEvictions",static_cast<double>(snapshot.rootEvictions)},
      {"parserInvariants",snapshot.parserInvariants},
      {"rootInvariants",snapshot.rootInvariants}
   };
}

} // namespace

extern "C" {

std::uint32_t ilic_abi_version(void) { return 1; }
const char *ilic_version(void) { return ilic::version(); }
void *ilic_alloc(std::size_t length) { return std::malloc(length); }
void ilic_free(void *memory) { std::free(memory); }

std::uint32_t ilic_session_create(void)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   const std::uint32_t handle = nextSession++;
   sessions[handle] = std::make_shared<ilic::CompilerSession>();
   return handle;
}

void ilic_session_destroy(std::uint32_t session)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   sessions.erase(session);
}

std::int32_t ilic_session_put_source(std::uint32_t session,const char *uri,std::size_t uriLength,
   const std::uint8_t *utf8,std::size_t utf8Length,std::uint64_t documentVersion)
{
   auto value = getSession(session);
   if (value == nullptr || uri == nullptr || (utf8 == nullptr && utf8Length != 0)) return -1;
   try {
      const auto update = value->updateSource(std::string(uri,uriLength),
         std::string(reinterpret_cast<const char *>(utf8),utf8Length),documentVersion);
      return update.accepted ? 0 : -3;
   }
   catch (...) { return -2; }
}

std::uint32_t ilic_incremental_stats(std::uint32_t session)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("incremental-stats","invalid session handle"));
   try { return store(incrementalStatsResult(value->incrementalStats())); }
   catch (const std::exception &error) { return store(errorResult("incremental-stats",error.what())); }
   catch (...) { return store(errorResult("incremental-stats","unknown C++ exception")); }
}

std::uint32_t ilic_incremental_trace(std::uint32_t session)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("incremental-trace","invalid session handle"));
   try { return store(incrementalTraceResult(value->lastIncrementalTrace())); }
   catch (const std::exception &error) { return store(errorResult("incremental-trace",error.what())); }
   catch (...) { return store(errorResult("incremental-trace","unknown C++ exception")); }
}

std::uint32_t ilic_incremental_cache_snapshot(std::uint32_t session)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("incremental-cache-snapshot","invalid session handle"));
   try { return store(incrementalCacheSnapshotResult(value->incrementalCacheSnapshot())); }
   catch (const std::exception &error) { return store(errorResult("incremental-cache-snapshot",error.what())); }
   catch (...) { return store(errorResult("incremental-cache-snapshot","unknown C++ exception")); }
}

std::int32_t ilic_reset_incremental_stats(std::uint32_t session)
{
   auto value = getSession(session);
   if (value == nullptr) return -1;
   try { value->resetIncrementalStats(); return 0; }
   catch (...) { return -2; }
}

std::int32_t ilic_clear_incremental_caches(std::uint32_t session)
{
   auto value = getSession(session);
   if (value == nullptr) return -1;
   try { value->clearIncrementalCaches(); return 0; }
   catch (...) { return -2; }
}

std::int32_t ilic_session_remove_source(std::uint32_t session,const char *uri,std::size_t uriLength)
{
   auto value = getSession(session);
   if (value == nullptr || uri == nullptr) return -1;
   try { return value->removeSource(std::string(uri,uriLength)) ? 0 : 1; }
   catch (...) { return -2; }
}

std::uint32_t ilic_compile(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("compilation","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("compilation","request JSON is null"));
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(compileResult(value->compile(compileRequest(json))));
   }
   catch (const std::exception &error) {
      return store(errorResult("compilation",error.what()));
   }
   catch (...) { return store(errorResult("compilation","unknown C++ exception")); }
}

std::uint32_t ilic_parse(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("syntax","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("syntax","request JSON is null"));
   std::string uri;
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      if (!json.isObject()) throw std::runtime_error("parse request must be an object");
      if (!json.get("schemaVersion").isNumber() ||
         static_cast<int>(json.get("schemaVersion").number()) != 1)
         throw std::runtime_error("unsupported schemaVersion");
      if (!json.get("uri").isString() || json.get("uri").string().empty())
         throw std::runtime_error("uri must be a non-empty string");
      uri = json.get("uri").string();
      return store(syntaxResult(value->parse(uri)));
   }
   catch (const std::exception &error) {
      const ilic::SourceBuffer *source = uri.empty() ? nullptr : sessionSources(value).get(uri);
      return store(errorResult("syntax",error.what(),uri,source == nullptr ? 0 : source->version));
   }
   catch (...) {
      const ilic::SourceBuffer *source = uri.empty() ? nullptr : sessionSources(value).get(uri);
      return store(errorResult("syntax","unknown C++ exception",uri,
         source == nullptr ? 0 : source->version));
   }
}

std::uint32_t ilic_editor_snapshot(std::uint32_t session,const char *requestJson,
   std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("editor","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("editor","request JSON is null"));
   std::string uri;
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      if (!json.isObject()) throw std::runtime_error("editor request must be an object");
      if (!json.get("schemaVersion").isNumber() ||
         static_cast<int>(json.get("schemaVersion").number()) != 1)
         throw std::runtime_error("unsupported schemaVersion");
      if (!json.get("uri").isString() || json.get("uri").string().empty())
         throw std::runtime_error("uri must be a non-empty string");
      uri = json.get("uri").string();
      return storeJson(editorResultJson(value->editorSnapshot(uri)));
   }
   catch (const std::exception &error) {
      const ilic::SourceBuffer *source = uri.empty() ? nullptr : sessionSources(value).get(uri);
      return store(errorResult("editor",error.what(),uri,
         source == nullptr ? 0 : source->version));
   }
   catch (...) {
      const ilic::SourceBuffer *source = uri.empty() ? nullptr : sessionSources(value).get(uri);
      return store(errorResult("editor","unknown C++ exception",uri,
         source == nullptr ? 0 : source->version));
   }
}

std::uint32_t ilic_analyze(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("semantic","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("semantic","request JSON is null"));
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(semanticResult(value->analyze(compileRequest(json))));
   }
   catch (const std::exception &error) {
      return store(errorResult("semantic",error.what()));
   }
   catch (...) { return store(errorResult("semantic","unknown C++ exception")); }
}

std::uint32_t ilic_compile_and_analyze(std::uint32_t session,const char *requestJson,
   std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("compilation-analysis","invalid session handle"));
   if (requestJson == nullptr)
      return store(errorResult("compilation-analysis","request JSON is null"));
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(compilationAnalysisResult(value->compileAndAnalyze(compileRequest(json))));
   }
   catch (const std::exception &error) {
      return store(errorResult("compilation-analysis",error.what()));
   }
   catch (...) {
      return store(errorResult("compilation-analysis","unknown C++ exception"));
   }
}

std::uint32_t ilic_format(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("formatting","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("formatting","request JSON is null"));
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      if (!json.isObject()) throw std::runtime_error("format request must be an object");
      if (!json.get("schemaVersion").isNumber() ||
         static_cast<int>(json.get("schemaVersion").number()) != 1)
         throw std::runtime_error("unsupported schemaVersion");
      if (!json.get("uri").isString() || json.get("uri").string().empty())
         throw std::runtime_error("uri must be a non-empty string");
      const std::string uri = json.get("uri").string();
      const ilic::SourceBuffer *source = sessionSources(value).get(uri);
      if (source == nullptr) throw std::runtime_error("format source is not registered");
      ilic::FormatOptions options;
      const Value &jsonOptions = json.get("options");
      if (!jsonOptions.isNull() && !jsonOptions.isObject())
         throw std::runtime_error("options must be an object");
      if (!jsonOptions.get("indentSize").isNull() && !jsonOptions.get("indentSize").isNumber())
         throw std::runtime_error("indentSize must be a number");
      if (!jsonOptions.get("requireValidSyntax").isNull() &&
         !jsonOptions.get("requireValidSyntax").isBool())
         throw std::runtime_error("requireValidSyntax must be a boolean");
      const double indentSize = jsonOptions.get("indentSize").number(2);
      if (indentSize < 1 || indentSize > 16 || indentSize != static_cast<std::size_t>(indentSize))
         throw std::runtime_error("indentSize must be an integer between 1 and 16");
      options.indentSize = static_cast<std::size_t>(indentSize);
      options.requireValidSyntax = jsonOptions.get("requireValidSyntax").boolean(true);
      ilic::FormatResult formatted = ilic::Formatter().format(uri,source->text,options);
      return store(Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
         {"kind","formatting"},{"success",formatted.success},{"applicable",formatted.applicable},
         {"changed",formatted.changed},{"text",formatted.text},
         {"diagnostics",diagnostics(formatted.diagnostics)}});
   }
   catch (const std::exception &error) {
      return store(errorResult("formatting",error.what()));
   }
   catch (...) { return store(errorResult("formatting","unknown C++ exception")); }
}

const char *ilic_result_json(std::uint32_t result,std::size_t *resultLength)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   auto found = results.find(result);
   if (found == results.end()) {
      if (resultLength != nullptr) *resultLength = 0;
      return nullptr;
   }
   if (resultLength != nullptr) *resultLength = found->second.size();
   return found->second.c_str();
}

void ilic_result_destroy(std::uint32_t result)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   results.erase(result);
}

} // extern "C"
