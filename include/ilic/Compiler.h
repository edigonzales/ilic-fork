#pragma once

#include "SourceManager.h"
#include "Diagnostic.h"
#include "Syntax.h"

#include <string>
#include <cstdint>
#include <vector>

namespace ilic {

struct SemanticSnapshot;
struct CompilationAnalysisResult;

struct CompilerOptions {
   bool autoSearch = true;
   bool warningsAsErrors = false;
   std::vector<std::string> modelDirectories;
};

struct CompilationRequest {
   std::vector<std::string> roots;
   CompilerOptions options;
   struct ExternalMetaAttribute {
      std::string element;
      std::string name;
      std::string value;
   };
   std::vector<ExternalMetaAttribute> externalMetaAttributes;
};

struct CompiledModel {
   struct MetaAttributeValue {
      std::string name;
      std::string value;
   };
   std::string name;
   std::string iliVersion;
   std::string uri;
   std::vector<MetaAttributeValue> metaAttributes;
};

struct CompilationResult {
   bool success = false;
   bool cancelled = false;
   int errorCount = 0;
   int warningCount = 0;
   std::vector<std::string> missingModels;
   std::vector<CompiledModel> models;
   std::vector<Diagnostic> diagnostics;
   std::vector<LogEvent> logs;
   std::vector<std::string> transcript;
};

class CompilerSession {
public:
   CompilerSession();
   ~CompilerSession();

   void putSource(std::string uri, std::string utf8, std::uint64_t version = 0);
   bool removeSource(const std::string &uri);
   SourceManager &sources();
   const SourceManager &sources() const;
   SyntaxSnapshot parse(const std::string &uri);
   SemanticSnapshot analyze(const CompilationRequest &request);
   CompilationAnalysisResult compileAndAnalyze(const CompilationRequest &request);
   CompilationResult compile(const CompilationRequest &request);

private:
   friend struct CompilerSessionTestAccess;
   CompilationResult compileUnlocked(const CompilationRequest &request);
   CompilationAnalysisResult compileAndAnalyzeUnlocked(const CompilationRequest &request);
   SourceManager sources_;
   std::uint64_t compileInvocationCount_ = 0;
   std::vector<std::string> lastCompilationSourceUris_;
};

const char *version();

} // namespace ilic
