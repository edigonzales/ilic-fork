#pragma once

#include "../../include/ilic/Editor.h"

namespace ilic::detail {

struct SnapshotBundle final {
   SyntaxSnapshot syntax;
   EditorSnapshot editor;
};

class SnapshotPipeline final {
public:
   explicit SnapshotPipeline(const SourceManager &sources) noexcept : sources_(sources) {}

   SyntaxSnapshot syntax(const std::string &uri) const;
   EditorSnapshot editor(const std::string &uri) const;
   SnapshotBundle build(const std::string &uri,bool includeEditor) const;

private:
   const SourceManager &sources_;
};

} // namespace ilic::detail
