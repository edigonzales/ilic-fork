#pragma once

#include "Compiler.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ilic {

struct SemanticSymbol {
   std::string id;
   std::string name;
   std::string qualifiedName;
   std::string kind;
   std::string containerId;
   SourceRange range;
   SourceRange selectionRange;
   SourceRange endRange;
   bool abstract = false;
};

struct SemanticReference {
   std::string sourceId;
   std::string targetId;
   std::string kind;
   SourceRange range;
};

struct SemanticDependency {
   std::string sourceUri;
   std::string targetUri;
   std::string model;
   SourceRange range;
};

struct DiagramMember {
   std::string name;
   std::string type;
   std::string cardinality;
   std::string declaringType;
   bool inherited = false;
   std::vector<std::string> inlineEnumValues;
};

struct DiagramNode {
   std::string id;
   std::string containerId;
   std::string label;
   std::string kind;
   bool abstract = false;
   SourceRange range;
   std::vector<std::string> stereotypes;
   std::vector<DiagramMember> members;
   std::vector<std::string> enumValues;
   std::vector<std::string> operations;
};

struct DiagramEdge {
   std::string id;
   std::string sourceId;
   std::string targetId;
   std::string kind;
   std::string label;
   std::string cardinality;
   std::string sourceCardinality;
   std::string targetCardinality;
};

struct DiagramProjection {
   std::vector<DiagramNode> nodes;
   std::vector<DiagramEdge> edges;
};

struct DocumentationSection {
   std::string id;
   std::string title;
   std::string kind;
   std::string text;
   int level = 1;
};

struct DocumentationProjection {
   std::string title;
   std::vector<DocumentationSection> sections;
};

struct SemanticSnapshot {
   bool success = false;
   bool cancelled = false;
   std::vector<std::string> roots;
   std::map<std::string,std::uint64_t> documentVersions;
   std::vector<std::string> missingModels;
   std::vector<SemanticSymbol> symbols;
   std::vector<SemanticReference> references;
   std::vector<SemanticDependency> dependencies;
   DiagramProjection diagram;
   DocumentationProjection documentation;
   std::vector<Diagnostic> diagnostics;
   std::vector<LogEvent> logs;
};

struct CompilationAnalysisResult {
   CompilationResult compilation;
   SemanticSnapshot semantic;
   std::vector<SyntaxSnapshot> syntax;
};

SemanticSnapshot buildSemanticSnapshot(const SourceManager &sources,
   const CompilationRequest &request,const CompilationResult &compilation,
   const std::vector<std::string> &compilationSourceUris = {},
   std::vector<SyntaxSnapshot> *syntaxSnapshots = nullptr);

} // namespace ilic
