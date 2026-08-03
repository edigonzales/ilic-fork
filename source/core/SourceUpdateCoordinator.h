#pragma once

#include "../../include/ilic/SourceManager.h"

#include <cstdint>
#include <string>

namespace ilic::detail {

class CompilerSessionState;

class SourceUpdateCoordinator final {
public:
   static SourceUpdateResult update(CompilerSessionState &state,std::string uri,
      std::string utf8,std::uint64_t version);
   static bool remove(CompilerSessionState &state,const std::string &uri);
};

} // namespace ilic::detail
