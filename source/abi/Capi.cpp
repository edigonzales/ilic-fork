#include "../../include/ilic/capi.h"

#include "../../include/ilic/Formatter.h"
#include "CapiJsonProjectors.h"
#include "CapiRegistries.h"
#include "CapiRequestDecoder.h"

#include <cstdlib>
#include <stdexcept>

namespace {

using ilic::capi::Value;

std::uint32_t store(const Value &value) { return ilic::capi::results().store(value); }

const ilic::SourceManager &sources(
   const std::shared_ptr<ilic::CompilerSession> &session)
{
   return static_cast<const ilic::CompilerSession &>(*session).sources();
}

std::string requestUri(const Value &json,const char *kind)
{
   if (!json.isObject()) throw std::runtime_error(std::string(kind) + " request must be an object");
   if (!json.get("schemaVersion").isNumber() ||
      static_cast<int>(json.get("schemaVersion").number()) != 1)
      throw std::runtime_error("unsupported schemaVersion");
   if (!json.get("uri").isString() || json.get("uri").string().empty())
      throw std::runtime_error("uri must be a non-empty string");
   return json.get("uri").string();
}

} // namespace

extern "C" {

std::uint32_t ilic_abi_version(void) { return 1; }
const char *ilic_version(void) { return ilic::version(); }
void *ilic_alloc(std::size_t length) { return std::malloc(length); }
void ilic_free(void *memory) { std::free(memory); }

std::uint32_t ilic_session_create(void) { return ilic::capi::sessions().create(); }
void ilic_session_destroy(std::uint32_t session) { ilic::capi::sessions().destroy(session); }

std::int32_t ilic_session_put_source(std::uint32_t session,const char *uri,std::size_t uriLength,
   const std::uint8_t *utf8,std::size_t utf8Length,std::uint64_t documentVersion)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr || uri == nullptr || (utf8 == nullptr && utf8Length != 0)) return -1;
   try {
      const auto update = value->updateSource(std::string(uri,uriLength),
         std::string(reinterpret_cast<const char *>(utf8),utf8Length),documentVersion);
      return update.accepted ? 0 : -3;
   } catch (...) { return -2; }
}

std::int32_t ilic_session_remove_source(std::uint32_t session,const char *uri,std::size_t uriLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr || uri == nullptr) return -1;
   try { return value->removeSource(std::string(uri,uriLength)) ? 0 : 1; }
   catch (...) { return -2; }
}

std::uint32_t ilic_incremental_stats(std::uint32_t session)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("incremental-stats","invalid session handle"));
   try { return store(ilic::capi::incrementalStatsResult(value->incrementalStats())); }
   catch (const std::exception &error) { return store(ilic::capi::errorResult("incremental-stats",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("incremental-stats","unknown C++ exception")); }
}

std::uint32_t ilic_incremental_trace(std::uint32_t session)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("incremental-trace","invalid session handle"));
   try { return store(ilic::capi::incrementalTraceResult(value->lastIncrementalTrace())); }
   catch (const std::exception &error) { return store(ilic::capi::errorResult("incremental-trace",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("incremental-trace","unknown C++ exception")); }
}

std::uint32_t ilic_incremental_cache_snapshot(std::uint32_t session)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("incremental-cache-snapshot","invalid session handle"));
   try { return store(ilic::capi::incrementalCacheSnapshotResult(value->incrementalCacheSnapshot())); }
   catch (const std::exception &error) { return store(ilic::capi::errorResult("incremental-cache-snapshot",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("incremental-cache-snapshot","unknown C++ exception")); }
}

std::int32_t ilic_reset_incremental_stats(std::uint32_t session)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return -1;
   try { value->resetIncrementalStats(); return 0; } catch (...) { return -2; }
}

std::int32_t ilic_clear_incremental_caches(std::uint32_t session)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return -1;
   try { value->clearIncrementalCaches(); return 0; } catch (...) { return -2; }
}

std::uint32_t ilic_compile(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("compilation","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("compilation","request JSON is null"));
   try {
      const Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(ilic::capi::compileResult(value->compile(ilic::capi::decodeCompilationRequest(json))));
   } catch (const std::exception &error) { return store(ilic::capi::errorResult("compilation",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("compilation","unknown C++ exception")); }
}

std::uint32_t ilic_parse(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("syntax","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("syntax","request JSON is null"));
   std::string uri;
   try {
      uri = requestUri(ilic::json::parse(std::string(requestJson,requestLength)),"parse");
      return store(ilic::capi::syntaxResult(value->parse(uri)));
   } catch (const std::exception &error) {
      const auto *source = uri.empty() ? nullptr : sources(value).get(uri);
      return store(ilic::capi::errorResult("syntax",error.what(),uri,source == nullptr ? 0 : source->version));
   } catch (...) { return store(ilic::capi::errorResult("syntax","unknown C++ exception",uri)); }
}

std::uint32_t ilic_editor_snapshot(std::uint32_t session,const char *requestJson,
   std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("editor","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("editor","request JSON is null"));
   std::string uri;
   try {
      uri = requestUri(ilic::json::parse(std::string(requestJson,requestLength)),"editor");
      return store(ilic::capi::editorResult(value->editorSnapshot(uri)));
   } catch (const std::exception &error) {
      const auto *source = uri.empty() ? nullptr : sources(value).get(uri);
      return store(ilic::capi::errorResult("editor",error.what(),uri,source == nullptr ? 0 : source->version));
   } catch (...) { return store(ilic::capi::errorResult("editor","unknown C++ exception",uri)); }
}

std::uint32_t ilic_analyze(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("semantic","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("semantic","request JSON is null"));
   try {
      const Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(ilic::capi::semanticResult(value->analyze(ilic::capi::decodeCompilationRequest(json))));
   } catch (const std::exception &error) { return store(ilic::capi::errorResult("semantic",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("semantic","unknown C++ exception")); }
}

std::uint32_t ilic_compile_and_analyze(std::uint32_t session,const char *requestJson,
   std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("compilation-analysis","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("compilation-analysis","request JSON is null"));
   try {
      const Value json = ilic::json::parse(std::string(requestJson,requestLength));
      return store(ilic::capi::compilationAnalysisResult(
         value->compileAndAnalyze(ilic::capi::decodeCompilationRequest(json))));
   } catch (const std::exception &error) { return store(ilic::capi::errorResult("compilation-analysis",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("compilation-analysis","unknown C++ exception")); }
}

std::uint32_t ilic_format(std::uint32_t session,const char *requestJson,std::size_t requestLength)
{
   auto value = ilic::capi::sessions().get(session);
   if (value == nullptr) return store(ilic::capi::errorResult("formatting","invalid session handle"));
   if (requestJson == nullptr) return store(ilic::capi::errorResult("formatting","request JSON is null"));
   try {
      const Value json = ilic::json::parse(std::string(requestJson,requestLength));
      const std::string uri = requestUri(json,"format");
      const auto *source = sources(value).get(uri);
      if (source == nullptr) throw std::runtime_error("format source is not registered");
      ilic::FormatOptions options;
      const Value &jsonOptions = json.get("options");
      if (!jsonOptions.isNull() && !jsonOptions.isObject()) throw std::runtime_error("options must be an object");
      if (!jsonOptions.get("indentSize").isNull() && !jsonOptions.get("indentSize").isNumber())
         throw std::runtime_error("indentSize must be a number");
      if (!jsonOptions.get("requireValidSyntax").isNull() && !jsonOptions.get("requireValidSyntax").isBool())
         throw std::runtime_error("requireValidSyntax must be a boolean");
      const double indentSize = jsonOptions.get("indentSize").number(2);
      if (indentSize < 1 || indentSize > 16 || indentSize != static_cast<std::size_t>(indentSize))
         throw std::runtime_error("indentSize must be an integer between 1 and 16");
      options.indentSize = static_cast<std::size_t>(indentSize);
      options.requireValidSyntax = jsonOptions.get("requireValidSyntax").boolean(true);
      return store(ilic::capi::formattingResult(ilic::Formatter().format(uri,source->text,options)));
   } catch (const std::exception &error) { return store(ilic::capi::errorResult("formatting",error.what())); }
   catch (...) { return store(ilic::capi::errorResult("formatting","unknown C++ exception")); }
}

const char *ilic_result_json(std::uint32_t result,std::size_t *resultLength)
{
   return ilic::capi::results().json(result,resultLength);
}

void ilic_result_destroy(std::uint32_t result) { ilic::capi::results().destroy(result); }

} // extern "C"
