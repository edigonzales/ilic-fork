#pragma once

#include "SourceManager.h"
#include "Incremental.h"
#include "Diagnostic.h"
#include "Syntax.h"
#include "Editor.h"

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

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
   explicit CompilerSession(IncrementalCacheOptions cacheOptions);
   ~CompilerSession();
   CompilerSession(const CompilerSession &) = delete;
   CompilerSession &operator=(const CompilerSession &) = delete;
   CompilerSession(CompilerSession &&) = delete;
   CompilerSession &operator=(CompilerSession &&) = delete;

   void putSource(std::string uri, std::string utf8, std::uint64_t version = 0);
   SourceUpdateResult updateSource(std::string uri, std::string utf8,
      std::uint64_t version = 0);
   bool removeSource(const std::string &uri);
   // Legacy mutable access is retained for source compatibility. Prefer
   // updateSource/removeSource so incremental classification and metrics are
   // recorded by the session façade.
   [[deprecated("use CompilerSession::updateSource/removeSource")]]
   SourceManager &sources();
   const SourceManager &sources() const;
   SyntaxSnapshot parse(const std::string &uri);
   EditorSnapshot editorSnapshot(const std::string &uri);
   SemanticSnapshot analyze(const CompilationRequest &request);
   CompilationAnalysisResult compileAndAnalyze(const CompilationRequest &request);
   CompilationResult compile(const CompilationRequest &request);
   IncrementalStats incrementalStats() const;
   IncrementalTrace lastIncrementalTrace() const;
   void clearIncrementalCaches();

private:
   struct Impl;
   friend struct CompilerSessionTestAccess;
   CompilationAnalysisResult compileAndAnalyzeUnlocked(const CompilationRequest &request);
   std::unique_ptr<Impl> impl_;
   std::uint64_t compileInvocationCount_ = 0;
};

const char *version();

} // namespace ilic
