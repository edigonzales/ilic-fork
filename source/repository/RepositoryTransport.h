#pragma once

#include <cstddef>
#include <string>

namespace ilic::repository {

enum class RepositoryResourceKind {
   ModelIndex,
   SiteIndex,
   Model
};

struct RepositoryTransportRequest {
   std::string uri;
   RepositoryResourceKind kind = RepositoryResourceKind::Model;
   std::size_t maxBytes = 64 * 1024 * 1024;
   std::size_t maxRedirects = 8;
};

struct TransportResponse {
   bool success = false;
   long statusCode = 0;
   std::string body;
   std::string error;
   std::string finalUri;
   bool notFound = false;
   bool retryable = false;
};

class RepositoryTransport {
public:
   virtual ~RepositoryTransport() = default;
   virtual TransportResponse get(const std::string &uri) = 0;
   virtual TransportResponse get(const RepositoryTransportRequest &request)
   {
      return get(request.uri);
   }
};

class CurlRepositoryTransport final : public RepositoryTransport {
public:
   TransportResponse get(const std::string &uri) override;
};

} // namespace ilic::repository
