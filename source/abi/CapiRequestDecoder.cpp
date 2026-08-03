#include "CapiRequestDecoder.h"

#include <stdexcept>

namespace ilic::capi {

CompilationRequest decodeCompilationRequest(const json::Value &json)
{
   if (!json.isObject()) throw std::runtime_error("compile request must be an object");
   if (!json.get("schemaVersion").isNumber() ||
      static_cast<int>(json.get("schemaVersion").number()) != 1)
      throw std::runtime_error("unsupported schemaVersion");
   CompilationRequest request;
   if (!json.get("roots").isArray() || json.get("roots").array().empty())
      throw std::runtime_error("roots must be a non-empty array");
   for (const auto &root : json.get("roots").array()) {
      if (!root.isString() || root.string().empty())
         throw std::runtime_error("roots must contain non-empty strings");
      request.roots.push_back(root.string());
   }
   const auto &options = json.get("options");
   if (!options.isNull() && !options.isObject()) throw std::runtime_error("options must be an object");
   if (!options.get("autoSearch").isNull() && !options.get("autoSearch").isBool())
      throw std::runtime_error("autoSearch must be a boolean");
   if (!options.get("warningsAsErrors").isNull() && !options.get("warningsAsErrors").isBool())
      throw std::runtime_error("warningsAsErrors must be a boolean");
   request.options.autoSearch = options.get("autoSearch").boolean(true);
   request.options.warningsAsErrors = options.get("warningsAsErrors").boolean(false);
   if (!options.get("modelDirectories").isNull() && !options.get("modelDirectories").isArray())
      throw std::runtime_error("modelDirectories must be an array");
   for (const auto &directory : options.get("modelDirectories").array()) {
      if (!directory.isString() || directory.string().empty())
         throw std::runtime_error("modelDirectories must contain non-empty strings");
      request.options.modelDirectories.push_back(directory.string());
   }
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

} // namespace ilic::capi
