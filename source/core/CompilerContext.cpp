#include "CompilerContext.h"

#include <utility>

namespace ilic {
namespace detail {

CompilerContext::CompilerContext(SourceManager &sessionSources,const CompilerOptions &options,
   ParsedSourceArtifactProvider parsedSourceProvider)
   : sessionSources_(sessionSources),options_(options),
     compilationSources_(sessionSources_),files_(compilationSources_,logger_,options_,
        parsedSourceProvider ? ::util::IliFileCatalog::ParsedSourceHeaderProvider(
           [parsedSourceProvider](const SourceBuffer &source) {
              const auto artifact = parsedSourceProvider(source);
              return artifact == nullptr ? ParsedSourceHeader{} : artifact->header();
           }) : ::util::IliFileCatalog::ParsedSourceHeaderProvider{}),
     builder_(models_,logger_),parsedSourceProvider_(std::move(parsedSourceProvider))
{
   files_.setAutoSearch(options_.autoSearch);
   files_.setModelDirectories(options_.modelDirectories);
}

} // namespace detail
} // namespace ilic
