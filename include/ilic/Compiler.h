#pragma once

#include "SourceManager.h"
#include "Diagnostic.h"

#include <string>
#include <vector>

namespace ilic {

struct CompilerOptions {
   bool autoSearch = true;
   bool warningsAsErrors = false;
   std::vector<std::string> modelDirectories;
};

struct CompilationRequest {
   std::vector<std::string> roots;
   CompilerOptions options;
};

struct CompiledModel {
   std::string name;
   std::string iliVersion;
   std::string uri;
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
};

class CompilerSession {
public:
   CompilerSession();
   ~CompilerSession();

   void putSource(std::string uri, std::string utf8, std::uint64_t version = 0);
   bool removeSource(const std::string &uri);
   SourceManager &sources();
   const SourceManager &sources() const;
   CompilationResult compile(const CompilationRequest &request);

private:
   SourceManager sources_;
};

const char *version();

} // namespace ilic
