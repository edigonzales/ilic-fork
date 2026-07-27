#include "DiagnosticCode.h"
#include "Logger.h"

#include <cassert>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

namespace {

void assert_explicit_semantic_diagnostic_ids()
{
   namespace fs = std::filesystem;
   const fs::path root = ILIC_SOURCE_DIR;
   std::vector<fs::path> files;
   for (const auto &entry : fs::recursive_directory_iterator(root / "source/input/ili2")) {
      if (entry.path().extension() == ".cpp") files.push_back(entry.path());
   }
   files.push_back(root / "source/input/parser/IliParserErrorListener.cpp");
   files.push_back(root / "source/metamodel/MetaModel.cpp");
   files.push_back(root / "source/metamodel/MetaModelInput.cpp");
   files.push_back(root / "source/metamodel/SemanticChecker.cpp");
   files.push_back(root / "source/metamodel/TranslationChecker.cpp");
   files.push_back(root / "source/core/Compiler.cpp");

   for (const auto &file : files) {
      std::ifstream input(file);
      std::ostringstream buffer;
      buffer << input.rdbuf();
      std::string source = buffer.str();
      if (file.filename() == "TranslationChecker.cpp") {
         assert(source.find("translation_diagnostic_id") == std::string::npos);
      }
      size_t position = 0;
      while ((position = source.find("Log.error(",position)) != std::string::npos) {
         size_t argument = position + std::string("Log.error(").size();
         while (argument < source.size() &&
                std::isspace(static_cast<unsigned char>(source[argument]))) {
            ++argument;
         }
         const bool codeLess =
            source[argument] == '"' ||
            source.compare(argument,std::string("string(").size(),"string(") == 0 ||
            source.compare(argument,std::string("std::string(").size(),"std::string(") == 0;
         assert(!codeLess);
         position = argument;
      }
   }
}

}

int main()
{
   assert_explicit_semantic_diagnostic_ids();

   std::set<std::string_view> codes;
   const std::regex publicCode("^ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)*$");
   for (const auto &definition : util::diagnosticDefinitions()) {
      assert(std::regex_match(definition.code.begin(),definition.code.end(),publicCode));
      assert(codes.insert(definition.code).second);
      assert(util::diagnosticCode(definition.id) == definition.code);
   }
   assert(!codes.empty());

   Log.reset();
   Log.displayErrors(false);
   ilic::SourceRange range;
   range.valid = true;
   range.uri = "memory:///range.ili";
   range.start.line = 4;
   range.start.character = 2;
   range.end.line = 4;
   range.end.character = 12;
   ilic::SourceRange relatedRange = range;
   relatedRange.uri = "memory:///base.ili";
   Log.error(
      util::DiagnosticId::TranslationCoordDimensionMismatch,
      "precisely ranged diagnostic",
      range,
      {{relatedRange,"Base declaration"}}
   );
   const auto &diagnostic = Log.getDiagnostics().front();
   assert(diagnostic.code == "ILIC-TRANSLATION-COORD-DIMENSION-MISMATCH");
   assert(diagnostic.range.uri == "memory:///range.ili");
   assert(diagnostic.range.start.line == 4);
   assert(diagnostic.range.end.character == 12);
   assert(diagnostic.relatedInformation.front().range.uri == "memory:///base.ili");
   Log.reset();
   return 0;
}
