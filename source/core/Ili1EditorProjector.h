#pragma once

#include "SourceRangeMapper.h"
#include "tree/ParseTree.h"

namespace ilic::detail::editor_snapshot_detail { class EditorSnapshotAccumulator; }

namespace ilic::detail {

class Ili1EditorProjector final {
public:
   static void project(editor_snapshot_detail::EditorSnapshotAccumulator &output,
      antlr4::tree::ParseTree *root) noexcept;
};

} // namespace ilic::detail
