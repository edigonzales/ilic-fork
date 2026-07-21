#pragma once

#include <string>

namespace ilic::repository {

struct TransportResponse {
   bool success = false;
   long statusCode = 0;
   std::string body;
   std::string error;
};

class RepositoryTransport {
public:
   virtual ~RepositoryTransport() = default;
   virtual TransportResponse get(const std::string &uri) = 0;
};

class CurlRepositoryTransport final : public RepositoryTransport {
public:
   TransportResponse get(const std::string &uri) override;
};

} // namespace ilic::repository
