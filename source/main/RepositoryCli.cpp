#include "RepositoryCli.h"

#include "RepositoryUri.h"
#include "../util/IliFile.h"
#include "../util/Logger.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

namespace ilic::cli {
namespace {

bool hasArgument(util::StringMap &arguments,const std::string &key)
{
   const auto keys = arguments.getKeys();
   return std::find(keys.begin(),keys.end(),key) != keys.end();
}

std::string trim(std::string value)
{
   const auto nonSpace = [](unsigned char character) { return !std::isspace(character); };
   value.erase(value.begin(),std::find_if(value.begin(),value.end(),nonSpace));
   value.erase(std::find_if(value.rbegin(),value.rend(),nonSpace).base(),value.end());
   return value;
}

} // namespace

std::vector<std::string> splitList(const std::string &value,std::string_view delimiters)
{
   std::vector<std::string> values;
   std::size_t start = 0;
   while (start <= value.size()) {
      const std::size_t delimiter = value.find_first_of(delimiters,start);
      std::string item = trim(value.substr(start,delimiter == std::string::npos
         ? std::string::npos : delimiter - start));
      if (!item.empty()) values.push_back(std::move(item));
      if (delimiter == std::string::npos) break;
      start = delimiter + 1;
   }
   return values;
}

std::optional<std::string> schemaLanguageFromIliVersion(const std::string &value)
{
   if (value == "1.0") return "ili1";
   if (value == "2.3") return "ili2_3";
   if (value == "2.4") return "ili2_4";
   return std::nullopt;
}

CliRepositoryConfig buildRepositoryConfig(util::StringMap &arguments)
{
   CliRepositoryConfig config;
   config.explicitRepositories = splitList(arguments.get("repositories"),";,");
   config.requestedModels = splitList(arguments.get("models"),";,");
   config.useDefaultRepositories = !hasArgument(arguments,"no-default-repositories");
   if (auto language = schemaLanguageFromIliVersion(arguments.get("ili-version")))
      config.schemaLanguage = *language;

   std::unordered_set<std::string> seen;
   auto addRepositories = [&](const std::vector<std::string> &repositories) {
      for (const auto &value : repositories) {
         auto uri = repository::RepositoryUri::parse(value);
         const std::string key = uri ? uri->cacheKey() : value;
         if (seen.insert(key).second) config.effectiveRepositories.push_back(
            uri ? uri->normalized() : value);
      }
   };
   addRepositories(config.explicitRepositories);
   if (config.useDefaultRepositories) addRepositories(RepositoryManager::defaultRepositories());
   return config;
}

std::optional<std::string> validateRepositoryCliConfig(util::StringMap &arguments,
   const CliRepositoryConfig &config,bool hasExplicitIliFiles)
{
   const std::string requestedVersion = arguments.get("ili-version");
   if (hasArgument(arguments,"ili-version")
      && !schemaLanguageFromIliVersion(requestedVersion))
      return "invalid --ili-version '" + requestedVersion + "' (expected 1.0, 2.3 or 2.4)";
   if (!hasExplicitIliFiles && config.requestedModels.empty())
      return "no input .ili files or root models specified";
   if (!config.requestedModels.empty() && config.effectiveRepositories.empty())
      return "no repositories configured";
   return std::nullopt;
}

std::unique_ptr<RepositoryManager> createRepositoryManager(const CliRepositoryConfig &config)
{
   if (config.effectiveRepositories.empty()) return nullptr;
   RepositoryOptions options;
   options.repositories = config.effectiveRepositories;
   return std::make_unique<RepositoryManager>(std::move(options));
}

void logRepositoryDiagnostics(const std::vector<Diagnostic> &diagnostics)
{
   for (const auto &diagnostic : diagnostics) {
      const std::string message = diagnostic.code.empty() ? diagnostic.message
         : diagnostic.code + ": " + diagnostic.message;
      switch (diagnostic.severity) {
      case DiagnosticSeverity::Error: Log.error(message); break;
      case DiagnosticSeverity::Warning: Log.warning(message); break;
      case DiagnosticSeverity::Information: Log.info(message); break;
      case DiagnosticSeverity::Hint: Log.debug(message); break;
      }
   }
}

bool loadResolvedRepositoryModels(const RepositoryResult &resolved)
{
   bool success = true;
   for (const auto &model : resolved.models)
      success = util::load_ilifiles_by_file(model.localPath.string()) != nullptr && success;
   return success;
}

} // namespace ilic::cli
