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

std::uint32_t store(Value value)
{
   std::lock_guard<std::mutex> lock(registryMutex);
   const std::uint32_t handle = nextResult++;
   results[handle] = ilic::json::stringify(value);
   return handle;
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
      values.push_back(Value::Object{
         {"severity",severity(diagnostic.severity)},
         {"code",diagnostic.code},
         {"message",diagnostic.message},
         {"range",range(diagnostic.range)},
         {"relatedInformation",std::move(related)},
         {"notes",std::move(notes)},
         {"treatedAsError",diagnostic.treatedAsError}
      });
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

Value errorResult(const char *kind,const std::string &message)
{
   return Value::Object{{"schemaVersion",1},{"kind",kind},{"success",false},
      {"diagnostics",Value::Array{Value::Object{{"severity","error"},{"code","ILIC-ABI-REQUEST"},
         {"message",message},{"range",nullptr},{"relatedInformation",Value::Array{}},
         {"notes",Value::Array{}},{"treatedAsError",false}}}}};
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
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)}
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
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","syntax"},{"success",result.success},{"uri",result.uri},
      {"documentVersion",static_cast<double>(result.documentVersion)},
      {"iliVersion",result.iliVersion},{"tokens",std::move(tokens)},
      {"nodes",std::move(nodes)},{"contexts",std::move(contexts)},
      {"imports",std::move(imports)},{"diagnostics",diagnostics(result.diagnostics)}};
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
         {"targetUri",dependency.targetUri},{"model",dependency.model}});
   }
   Value::Array diagramNodes;
   for (const auto &node : result.diagram.nodes) {
      Value::Array members;
      for (const auto &member : node.members)
         members.push_back(Value::Object{{"name",member.name},{"type",member.type},
            {"inherited",member.inherited}});
      diagramNodes.push_back(Value::Object{{"id",node.id},{"containerId",node.containerId},
         {"label",node.label},{"kind",node.kind},{"abstract",node.abstract},
         {"range",range(node.range)},{"members",std::move(members)}});
   }
   Value::Array diagramEdges;
   for (const auto &edge : result.diagram.edges) {
      diagramEdges.push_back(Value::Object{{"id",edge.id},{"sourceId",edge.sourceId},
         {"targetId",edge.targetId},{"kind",edge.kind},{"label",edge.label},
         {"cardinality",edge.cardinality}});
   }
   Value::Array sections;
   for (const auto &section : result.documentation.sections) {
      sections.push_back(Value::Object{{"id",section.id},{"title",section.title},
         {"kind",section.kind},{"text",section.text},{"level",section.level}});
   }
   return Value::Object{{"schemaVersion",1},{"abiVersion",1},{"compilerVersion",ilic::version()},
      {"kind","semantic"},{"success",result.success},{"cancelled",result.cancelled},
      {"roots",std::move(roots)},{"documentVersions",std::move(versions)},
      {"symbols",std::move(symbols)},{"references",std::move(references)},
      {"dependencies",std::move(dependencies)},
      {"diagram",Value::Object{{"nodes",std::move(diagramNodes)},{"edges",std::move(diagramEdges)}}},
      {"documentation",Value::Object{{"title",result.documentation.title},
         {"sections",std::move(sections)}}},
      {"diagnostics",diagnostics(result.diagnostics)},{"logs",logs(result.logs)}};
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
   value->putSource(std::string(uri,uriLength),
      std::string(reinterpret_cast<const char *>(utf8),utf8Length),documentVersion);
   return 0;
}

std::int32_t ilic_session_remove_source(std::uint32_t session,const char *uri,std::size_t uriLength)
{
   auto value = getSession(session);
   if (value == nullptr || uri == nullptr) return -1;
   return value->removeSource(std::string(uri,uriLength)) ? 0 : 1;
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
}

std::uint32_t ilic_parse(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = getSession(session);
   if (value == nullptr) return store(errorResult("syntax","invalid session handle"));
   if (requestJson == nullptr) return store(errorResult("syntax","request JSON is null"));
   try {
      Value json = ilic::json::parse(std::string(requestJson,requestLength));
      if (!json.isObject()) throw std::runtime_error("parse request must be an object");
      if (!json.get("schemaVersion").isNumber() ||
         static_cast<int>(json.get("schemaVersion").number()) != 1)
         throw std::runtime_error("unsupported schemaVersion");
      if (!json.get("uri").isString() || json.get("uri").string().empty())
         throw std::runtime_error("uri must be a non-empty string");
      return store(syntaxResult(value->parse(json.get("uri").string())));
   }
   catch (const std::exception &error) {
      return store(errorResult("syntax",error.what()));
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
      const ilic::SourceBuffer *source = value->sources().get(uri);
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
