#include "RepositoryTransport.h"

#include <curl/curl.h>

#include <array>

namespace ilic::repository {
namespace {

class CurlGlobal {
public:
   CurlGlobal() : status(curl_global_init(CURL_GLOBAL_DEFAULT)) {}
   ~CurlGlobal() { if (status == CURLE_OK) curl_global_cleanup(); }
   CURLcode status;
};

CurlGlobal &curlGlobal()
{
   static CurlGlobal global;
   return global;
}

std::size_t writeBody(char *data,std::size_t size,std::size_t count,void *target)
{
   static_cast<std::string *>(target)->append(data,size * count);
   return size * count;
}

} // namespace

TransportResponse CurlRepositoryTransport::get(const std::string &uri)
{
   TransportResponse response;
   if (curlGlobal().status != CURLE_OK) {
      response.error = std::string("unable to initialize libcurl: ")
         + curl_easy_strerror(curlGlobal().status);
      return response;
   }
   CURL *curl = curl_easy_init();
   if (curl == nullptr) {
      response.error = "unable to create libcurl request";
      return response;
   }
   std::array<char,CURL_ERROR_SIZE> errorBuffer{};
   curl_easy_setopt(curl,CURLOPT_URL,uri.c_str());
   curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
   curl_easy_setopt(curl,CURLOPT_FAILONERROR,1L);
   curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,15L);
   curl_easy_setopt(curl,CURLOPT_TIMEOUT,60L);
   curl_easy_setopt(curl,CURLOPT_USERAGENT,"ilic/0.9.9");
   curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeBody);
   curl_easy_setopt(curl,CURLOPT_WRITEDATA,&response.body);
   curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errorBuffer.data());
   const CURLcode status = curl_easy_perform(curl);
   curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response.statusCode);
   response.success = status == CURLE_OK && response.statusCode >= 200 && response.statusCode < 300;
   if (!response.success) {
      response.error = errorBuffer.front() == '\0' ? curl_easy_strerror(status) : errorBuffer.data();
      if (response.statusCode != 0) response.error += " (HTTP " + std::to_string(response.statusCode) + ')';
   }
   curl_easy_cleanup(curl);
   return response;
}

} // namespace ilic::repository
