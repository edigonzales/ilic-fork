#include "SnapshotService.h"

#include "CompilerSessionState.h"
#include "SnapshotPipeline.h"

namespace ilic::detail {

SyntaxSnapshot SnapshotService::parse(CompilerSessionState &state,const std::string &uri)
{
   state.trace = {};
   state.trace.operation = "parse";
   state.trace.roots.push_back(uri);
   ++state.stats.parseRequests;
   const auto *source = state.sources.get(uri);
   if (source == nullptr) return SnapshotPipeline(state.sources).syntax(uri);
   const auto entry = state.ensureParsed(*source,ParseMode::StrictCompiler);
   ++state.stats.syntaxMaterializations;
   state.syncCacheStats();
   if (entry == nullptr) return SnapshotPipeline(state.sources).syntax(uri);
   SyntaxSnapshot syntax = entry->syntax;
   syntax.documentVersion = source->version;
   return syntax;
}

EditorSnapshot SnapshotService::editor(CompilerSessionState &state,const std::string &uri)
{
   state.trace = {};
   state.trace.operation = "editorSnapshot";
   state.trace.roots.push_back(uri);
   ++state.stats.editorSnapshotRequests;
   const auto *source = state.sources.get(uri);
   if (source == nullptr) return SnapshotPipeline(state.sources).editor(uri);
   const auto entry = state.ensureParsed(*source,ParseMode::TolerantEditor);
   ++state.stats.editorMaterializations;
   state.syncCacheStats();
   if (entry == nullptr) return SnapshotPipeline(state.sources).editor(uri);
   EditorSnapshot editor = entry->editor;
   editor.documentVersion = source->version;
   return editor;
}

} // namespace ilic::detail
