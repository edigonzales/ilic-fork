#include "ilic/capi.h"

#include <cassert>
#include <cstring>
#include <string>

namespace {

std::string resultJson(std::uint32_t result)
{
   std::size_t length = 0;
   const char *json = ilic_result_json(result,&length);
   assert(json != nullptr);
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
   assert(ilic_abi_version() == 1);
   assert(std::strlen(ilic_version()) != 0);

   const std::uint32_t session = ilic_session_create();
   const char *uri = "memory:///AbiModel.ili";
   const char *source = R"ili(INTERLIS 2.3;
!!@ displayName = "ABI model"
MODEL AbiModel AT "https://example.invalid/ilic/tests" VERSION "1" =
END AbiModel.
)ili";
   assert(put(session,uri,source) == 0);

   const std::string compileRequest =
      R"json({"schemaVersion":1,"roots":["memory:///AbiModel.ili"],"options":{"autoSearch":true}})json";
   std::string compilation = resultJson(ilic_compile(session,compileRequest.data(),compileRequest.size()));
   assert(compilation.find("\"kind\":\"compilation\"") != std::string::npos);
   assert(compilation.find("\"success\":true") != std::string::npos);
   assert(compilation.find("\"displayName\"") != std::string::npos);

   const std::string formatRequest =
      R"json({"schemaVersion":1,"uri":"memory:///AbiModel.ili","options":{"indentSize":2}})json";
   std::string formatting = resultJson(ilic_format(session,formatRequest.data(),formatRequest.size()));
   assert(formatting.find("\"kind\":\"formatting\"") != std::string::npos);
   assert(formatting.find("\"success\":true") != std::string::npos);
   assert(formatting.find("!!@ displayName") != std::string::npos);

   const char invalid[] = "{";
   std::string rejected = resultJson(ilic_compile(session,invalid,sizeof(invalid) - 1));
   assert(rejected.find("ILIC-ABI-REQUEST") != std::string::npos);
   assert(rejected.find("\"success\":false") != std::string::npos);

   assert(ilic_session_remove_source(session,uri,std::strlen(uri)) == 0);
   ilic_session_destroy(session);
   assert(ilic_session_put_source(session,uri,std::strlen(uri),nullptr,0,0) == -1);
   return 0;
}
