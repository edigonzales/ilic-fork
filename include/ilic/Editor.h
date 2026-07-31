#pragma once

#include "Diagnostic.h"
#include "Syntax.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ilic {

enum class EditorSymbolKind {
   Model,
   Topic,
   Class,
   Structure,
   Association,
   View,
   Graphic,
   Domain,
   Unit,
   Attribute
};

enum class EditorReferenceKind {
   Extends,
   Type,
   Collection,
   Reference,
   Unit
};

struct EditorDeclaration {
   std::string id;
   std::string name;
   std::string qualifiedName;
   EditorSymbolKind kind = EditorSymbolKind::Attribute;
   std::string containerId;
   bool hasContainer = false;
   SourceRange range;
   SourceRange selectionRange;
   SourceRange endRange;
};

struct EditorReference {
   std::string text;
   EditorReferenceKind kind = EditorReferenceKind::Type;
   std::string sourceId;
   bool hasSource = false;
   SourceRange range;
};

struct EditorSnapshot {
   bool success = false;
   bool recovered = false;
   bool complete = false;
   std::string uri;
   std::uint64_t documentVersion = 0;
   std::string iliVersion;
   std::vector<EditorDeclaration> declarations;
   std::vector<EditorReference> references;
   std::vector<SyntaxImportReference> imports;
   std::vector<SyntaxContext> contexts;
   std::vector<Diagnostic> diagnostics;
};

} // namespace ilic
