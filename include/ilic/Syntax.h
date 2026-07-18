#pragma once

#include "Diagnostic.h"
#include "SourceManager.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ilic {

struct SyntaxToken {
   std::string kind;
   std::string text;
   std::size_t channel = 0;
   SourceRange range;
};

struct SyntaxNode {
   std::size_t id = 0;
   std::size_t parent = 0;
   bool hasParent = false;
   std::string kind;
   SourceRange range;
};

struct SyntaxContext {
   std::string kind;
   SourceRange range;
};

struct SyntaxSnapshot {
   bool success = false;
   std::string uri;
   std::uint64_t documentVersion = 0;
   std::string iliVersion;
   std::vector<SyntaxToken> tokens;
   std::vector<SyntaxNode> nodes;
   std::vector<SyntaxContext> contexts;
   std::vector<std::string> imports;
   std::vector<Diagnostic> diagnostics;
};

SyntaxSnapshot parseSyntax(const SourceManager &sources,const std::string &uri);

} // namespace ilic
