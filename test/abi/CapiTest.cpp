#include "ilic/capi.h"

#include "ilic/test/TestHarness.h"
#include <cstdint>
#include <cstring>
#include <regex>
#include <string>

namespace {

std::string resultJson(std::uint32_t result)
{
   ILIC_REQUIRE(result != 0);
   std::size_t length = 0;
   const char *json = ilic_result_json(result,&length);
   ILIC_REQUIRE_MSG(json != nullptr,"result JSON must not be null");
   std::string value(json,length);
   ilic_result_destroy(result);
   return value;
}

std::int32_t put(std::uint32_t session,const char *uri,const char *source,std::uint64_t version = 1)
{
   return ilic_session_put_source(session,uri,std::strlen(uri),
      reinterpret_cast<const std::uint8_t *>(source),std::strlen(source),version);
}

} // namespace

int main()
{
   ILIC_REQUIRE(ilic_abi_version() == 1);
   const char *version = ilic_version();
   ILIC_REQUIRE(version != nullptr);
   ILIC_REQUIRE(std::strlen(version) != 0);

   const std::uint32_t session = ilic_session_create();
   ILIC_REQUIRE(session != 0);
   const char *uri = "memory:///AbiModel.ili";
   const char *source = R"ili(INTERLIS 2.3;
!!@ displayName = "ABI model"
MODEL AbiModel AT "https://example.invalid/ilic/tests" VERSION "1" =
  CLASS Item (ABSTRACT) =
    Name : TEXT;
  END Item;
END AbiModel.
)ili";
   ILIC_REQUIRE(put(session,uri,source) == 0);

   const std::string compileRequest =
      R"json({"schemaVersion":1,"roots":["memory:///AbiModel.ili"],"options":{"autoSearch":true}})json";
   std::string compilation = resultJson(ilic_compile(session,compileRequest.data(),compileRequest.size()));
   ILIC_REQUIRE(compilation.find("\"kind\":\"compilation\"") != std::string::npos);
   ILIC_REQUIRE(compilation.find("\"success\":true") != std::string::npos);
   ILIC_REQUIRE(compilation.find("\"displayName\"") != std::string::npos);
   ILIC_REQUIRE(std::regex_search(compilation,
      std::regex("ilic completed with no errors, no warnings [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}")));

   const std::string parseRequest =
      R"json({"schemaVersion":1,"uri":"memory:///AbiModel.ili"})json";
   std::string syntax = resultJson(ilic_parse(session,parseRequest.data(),parseRequest.size()));
   ILIC_REQUIRE(syntax.find("\"kind\":\"syntax\"") != std::string::npos);
   ILIC_REQUIRE(syntax.find("\"documentVersion\":1") != std::string::npos);
   ILIC_REQUIRE(syntax.find("\"modelDef\"") != std::string::npos);

   const std::string editorRequest =
      R"json({"schemaVersion":1,"uri":"memory:///AbiModel.ili"})json";
   std::string editor = resultJson(ilic_editor_snapshot(
      session,editorRequest.data(),editorRequest.size()));
   ILIC_REQUIRE(editor.find("\"kind\":\"editor\"") != std::string::npos);
   ILIC_REQUIRE(editor.find("\"recovered\":false") != std::string::npos);
   ILIC_REQUIRE(editor.find("\"complete\":true") != std::string::npos);
   ILIC_REQUIRE(editor.find("\"qualifiedName\":\"AbiModel.Item\"") != std::string::npos);

   std::string semantic = resultJson(ilic_analyze(session,compileRequest.data(),compileRequest.size()));
   ILIC_REQUIRE(semantic.find("\"kind\":\"semantic\"") != std::string::npos);
   ILIC_REQUIRE(semantic.find("\"qualifiedName\":\"AbiModel\"") != std::string::npos);
   ILIC_REQUIRE(semantic.find("\"title\":\"AbiModel\"") != std::string::npos);

   std::string combined = resultJson(
      ilic_compile_and_analyze(session,compileRequest.data(),compileRequest.size()));
   ILIC_REQUIRE(combined.find("\"kind\":\"compilation-analysis\"") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"compilation\":{\"abiVersion\":1") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"semantic\":{\"abiVersion\":1") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"cardinality\":\"0..1\"") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"declaringType\":\"\"") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"inlineEnumValues\":[]") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"operations\":[]") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"stereotypes\":") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"syntax\":[{") != std::string::npos);
   ILIC_REQUIRE(combined.find("\"transcript\":[") != std::string::npos);

   std::string incrementalStats = resultJson(ilic_incremental_stats(session));
   ILIC_REQUIRE(incrementalStats.find("\"kind\":\"incremental-stats\"")
      != std::string::npos);
   ILIC_REQUIRE(incrementalStats.find("\"parserBuilds\":") != std::string::npos);
   ILIC_REQUIRE(ilic_clear_incremental_caches(session) == 0);
   incrementalStats = resultJson(ilic_incremental_stats(session));
   ILIC_REQUIRE(incrementalStats.find("\"parserBytes\":0") != std::string::npos);

   const std::string formatRequest =
      R"json({"schemaVersion":1,"uri":"memory:///AbiModel.ili","options":{"indentSize":2}})json";
   std::string formatting = resultJson(ilic_format(session,formatRequest.data(),formatRequest.size()));
   ILIC_REQUIRE(formatting.find("\"kind\":\"formatting\"") != std::string::npos);
   ILIC_REQUIRE(formatting.find("\"success\":true") != std::string::npos);
   ILIC_REQUIRE(formatting.find("!!@ displayName") != std::string::npos);

   const char invalid[] = "{";
   std::string rejected = resultJson(ilic_compile(session,invalid,sizeof(invalid) - 1));
   ILIC_REQUIRE(rejected.find("ILIC-ABI-REQUEST") != std::string::npos);
   ILIC_REQUIRE(rejected.find("\"success\":false") != std::string::npos);

   const char *baseUri = "memory:///abi/Base.ili";
   const char *translatedUri = "memory:///abi/Translated.ili";
   const char *baseSource = R"ili(INTERLIS 2.3;
MODEL AbiBase (de) AT "https://example.invalid" VERSION "1" =
  CLASS BaseClass = Value : TEXT * 20; END BaseClass;
END AbiBase.
)ili";
   const char *translatedSource = R"ili(INTERLIS 2.3;
MODEL AbiTranslated (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF AbiBase [ "1" ] =
  CLASS TranslatedClass = TranslatedValue : TEXT * 30; END TranslatedClass;
END AbiTranslated.
)ili";
   ILIC_REQUIRE(put(session,baseUri,baseSource) == 0);
   ILIC_REQUIRE(put(session,translatedUri,translatedSource) == 0);
   const std::string crossFileRequest =
      R"json({"schemaVersion":1,"roots":["memory:///abi/Translated.ili","memory:///abi/Base.ili"]})json";
   const std::string crossFile =
      resultJson(ilic_compile(session,crossFileRequest.data(),crossFileRequest.size()));
   ILIC_REQUIRE(crossFile.find("\"schemaVersion\":1") != std::string::npos);
   ILIC_REQUIRE(crossFile.find("ILIC-TRANSLATION-TYPE-PROPERTY-MISMATCH")
      != std::string::npos);
   ILIC_REQUIRE(crossFile.find("\"uri\":\"memory:///abi/Translated.ili\"")
      != std::string::npos);
   ILIC_REQUIRE(crossFile.find("\"uri\":\"memory:///abi/Base.ili\"")
      != std::string::npos);
   ILIC_REQUIRE(crossFile.find("\"relatedInformation\":[{\"message\":")
      != std::string::npos);

   ILIC_REQUIRE(ilic_session_remove_source(session,uri,std::strlen(uri)) == 0);
   ilic_session_destroy(session);
   ILIC_REQUIRE(ilic_session_put_source(session,uri,std::strlen(uri),nullptr,0,0) == -1);
   return 0;
}
