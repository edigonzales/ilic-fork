#pragma once

#include "../../include/ilic/Editor.h"
#include "../../include/ilic/Syntax.h"

#include <string>

namespace ilic::detail {

class CompilerSessionState;

class SnapshotService final {
public:
   static SyntaxSnapshot parse(CompilerSessionState &state,const std::string &uri);
   static EditorSnapshot editor(CompilerSessionState &state,const std::string &uri);
};

} // namespace ilic::detail
