#pragma once

#include "../../include/ilic/Compiler.h"
#include "Json.h"

#include <cstdint>
#include <memory>
#include <map>
#include <mutex>
#include <string>

namespace ilic::capi {

class SessionRegistry final {
public:
   std::uint32_t create();
   void destroy(std::uint32_t handle);
   std::shared_ptr<CompilerSession> get(std::uint32_t handle) const;

private:
   mutable std::mutex mutex_;
   std::map<std::uint32_t,std::shared_ptr<CompilerSession>> values_;
   std::uint32_t next_ = 1;
};

class ResultRegistry final {
public:
   std::uint32_t storeJson(std::string value);
   std::uint32_t store(const json::Value &value);
   const char *json(std::uint32_t handle,std::size_t *length) const;
   void destroy(std::uint32_t handle);

private:
   mutable std::mutex mutex_;
   std::map<std::uint32_t,std::string> values_;
   std::uint32_t next_ = 1;
};

SessionRegistry &sessions();
ResultRegistry &results();

} // namespace ilic::capi
