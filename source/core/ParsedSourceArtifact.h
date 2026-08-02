#pragma once

#include "../../include/ilic/Diagnostic.h"

#include <memory>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ilic {
struct SourceBuffer;
}

namespace metamodel {
class MetaModelBuilder;
}

namespace util {
class Logger;
}

namespace ilic {
namespace detail {

enum class ParseMode : std::uint8_t {
   StrictCompiler,
   TolerantEditor
};

const char *toString(ParseMode mode) noexcept;

struct ParsedSourceHeader {
   std::string iliVersion;
   std::vector<std::string> models;
   std::vector<std::string> imports;
   std::vector<std::string> requiredModels;
};

// Owns the parser state needed by all consumers of one immutable source
// revision. The concrete ANTLR parser/lexer types stay private to the parser
// implementation while the compiler can visit the same tree later.
class ParsedSourceArtifact {
public:
   virtual ~ParsedSourceArtifact() = default;
   virtual ParseMode mode() const noexcept = 0;
   virtual bool supportsMetaModelBuild() const noexcept = 0;
   virtual ParsedSourceHeader header() const = 0;
   virtual const std::vector<Diagnostic> &parserDiagnostics() const noexcept = 0;
   virtual std::size_t tokenCount() const noexcept = 0;
   virtual std::size_t parseTreeNodeCount() const noexcept = 0;
   virtual std::size_t estimatedRetainedBytes() const noexcept = 0;
   virtual void reportParserDiagnostics(util::Logger &logger) const = 0;
   virtual void buildMetaModel(metamodel::MetaModelBuilder &builder,
      util::Logger &logger) const = 0;
};

using ParsedSourceArtifactPtr = std::shared_ptr<const ParsedSourceArtifact>;

} // namespace detail
} // namespace ilic
