#pragma once

#include "../../include/ilic/Editor.h"
#include "ParsedSourceArtifact.h"

#include <memory>

namespace ilic::detail {

struct SnapshotBundle final {
   SyntaxSnapshot syntax;
   EditorSnapshot editor;
   ParsedSourceArtifactPtr artifact;
   ParseMode mode = ParseMode::StrictCompiler;
};

class SnapshotPipeline final {
public:
   explicit SnapshotPipeline(const SourceManager &sources) noexcept : sources_(sources) {}

   SyntaxSnapshot syntax(const std::string &uri) const;
   EditorSnapshot editor(const std::string &uri) const;
   SnapshotBundle build(const std::string &uri,ParseMode mode) const;
   SnapshotBundle build(const SourceBuffer &source,ParseMode mode) const;

private:
   const SourceManager &sources_;
};

} // namespace ilic::detail
