#include "../../include/ilic/Syntax.h"

#include "SnapshotPipeline.h"

namespace ilic {

SyntaxSnapshot parseSyntax(const SourceManager &sources,const std::string &uri)
{
   return detail::SnapshotPipeline(sources).syntax(uri);
}

} // namespace ilic
