#pragma once

#include "../../include/ilic/Compiler.h"

#include <string>
#include <vector>

namespace ilic::detail {

SourceIdentity sourceIdentity(const SourceManager &sources,const std::string &uri);
SourceIdentity sourceIdentity(const SourceManager &sources,const SourceBuffer &source);
std::string requestKey(const CompilationRequest &request);
std::string rootKey(const std::string &base,const std::vector<std::string> &uris,
   const SourceManager &sources);

} // namespace ilic::detail
