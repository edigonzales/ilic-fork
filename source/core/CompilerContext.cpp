#include "CompilerContext.h"

namespace ilic {
namespace detail {

CompilerContext::CompilerContext(SourceManager &sessionSources,const CompilerOptions &options)
   : sessionSources_(sessionSources),options_(options),
     compilationSources_(sessionSources_),files_(compilationSources_,logger_,options_),
     builder_(models_,logger_)
{
   files_.setAutoSearch(options_.autoSearch);
   files_.setModelDirectories(options_.modelDirectories);
}

} // namespace detail
} // namespace ilic
