#include "SnapshotPipeline.h"

#include "ParsedSourceArtifactFactory.h"

namespace ilic::detail {

SyntaxSnapshot SnapshotPipeline::syntax(const std::string &uri) const
{
   return ParsedSourceArtifactFactory::build(sources_,uri,ParseMode::StrictCompiler).syntax;
}

EditorSnapshot SnapshotPipeline::editor(const std::string &uri) const
{
   return ParsedSourceArtifactFactory::build(sources_,uri,ParseMode::TolerantEditor).editor;
}

SnapshotBundle SnapshotPipeline::build(const std::string &uri,ParseMode mode) const
{
   return ParsedSourceArtifactFactory::build(sources_,uri,mode);
}

SnapshotBundle SnapshotPipeline::build(const SourceBuffer &source,ParseMode mode) const
{
   return ParsedSourceArtifactFactory::build(source,mode);
}

} // namespace ilic::detail
