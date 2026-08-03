#include "Ili1EditorProjector.h"
#include "EditorSnapshotProjector.h"

namespace ilic::detail {

void Ili1EditorProjector::project(
   editor_snapshot_detail::EditorSnapshotAccumulator &output,
   antlr4::tree::ParseTree *root) noexcept
{
   editor_snapshot_detail::collectIli1Editor(output,root,
      editor_snapshot_detail::EditorSnapshotAccumulator::noDeclaration,
      editor_snapshot_detail::EditorSnapshotAccumulator::noDeclaration);
}

} // namespace ilic::detail
