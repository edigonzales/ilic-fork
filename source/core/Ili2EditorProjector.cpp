#include "Ili2EditorProjector.h"
#include "EditorSnapshotProjector.h"

namespace ilic::detail {

void Ili2EditorProjector::project(
   editor_snapshot_detail::EditorSnapshotAccumulator &output,
   const SourceRangeMapper &ranges,antlr4::tree::ParseTree *root) noexcept
{
   editor_snapshot_detail::collectIli2Editor(output,ranges,root,
      editor_snapshot_detail::EditorSnapshotAccumulator::noDeclaration,
      editor_snapshot_detail::EditorSnapshotAccumulator::noDeclaration);
}

} // namespace ilic::detail
