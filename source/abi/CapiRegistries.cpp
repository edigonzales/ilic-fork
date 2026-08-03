#include "CapiRegistries.h"

#include <map>
#include <mutex>

namespace ilic::capi {

std::uint32_t SessionRegistry::create()
{
   std::lock_guard<std::mutex> lock(mutex_);
   const std::uint32_t handle = next_++;
   values_[handle] = std::make_shared<CompilerSession>();
   return handle;
}

void SessionRegistry::destroy(std::uint32_t handle)
{
   std::lock_guard<std::mutex> lock(mutex_);
   values_.erase(handle);
}

std::shared_ptr<CompilerSession> SessionRegistry::get(std::uint32_t handle) const
{
   std::lock_guard<std::mutex> lock(mutex_);
   const auto found = values_.find(handle);
   return found == values_.end() ? nullptr : found->second;
}

std::uint32_t ResultRegistry::storeJson(std::string value)
{
   std::lock_guard<std::mutex> lock(mutex_);
   const std::uint32_t handle = next_++;
   values_[handle] = std::move(value);
   return handle;
}

std::uint32_t ResultRegistry::store(const json::Value &value)
{
   return storeJson(json::stringify(value));
}

const char *ResultRegistry::json(std::uint32_t handle,std::size_t *length) const
{
   std::lock_guard<std::mutex> lock(mutex_);
   const auto found = values_.find(handle);
   if (found == values_.end()) {
      if (length != nullptr) *length = 0;
      return nullptr;
   }
   if (length != nullptr) *length = found->second.size();
   return found->second.c_str();
}

void ResultRegistry::destroy(std::uint32_t handle)
{
   std::lock_guard<std::mutex> lock(mutex_);
   values_.erase(handle);
}

SessionRegistry &sessions()
{
   static SessionRegistry registry;
   return registry;
}

ResultRegistry &results()
{
   static ResultRegistry registry;
   return registry;
}

} // namespace ilic::capi
