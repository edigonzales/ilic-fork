#pragma once

#include "../../include/ilic/Compiler.h"
#include "CompilationSourceStore.h"
#include "../metamodel/MetaModelBuilder.h"
#include "../metamodel/MetaModelStore.h"
#include "../util/IliFile.h"
#include "../util/Logger.h"

namespace ilic {
namespace detail {

// All mutable compiler state for one compilation run. No object in
// this aggregate is shared with another CompilerContext.
class CompilerContext final {
public:
   CompilerContext(SourceManager &sessionSources,const CompilerOptions &options);
   ~CompilerContext() noexcept = default;
   CompilerContext(const CompilerContext &) = delete;
   CompilerContext &operator=(const CompilerContext &) = delete;
   CompilerContext(CompilerContext &&) = delete;
   CompilerContext &operator=(CompilerContext &&) = delete;

   SourceManager &sessionSources() noexcept { return sessionSources_; }
   const SourceManager &sessionSources() const noexcept { return sessionSources_; }
   const CompilerOptions &options() const noexcept { return options_; }
   util::Logger &logger() noexcept { return logger_; }
   const util::Logger &logger() const noexcept { return logger_; }
   CompilationSourceStore &compilationSources() noexcept { return compilationSources_; }
   const CompilationSourceStore &compilationSources() const noexcept { return compilationSources_; }
   util::IliFileCatalog &files() noexcept { return files_; }
   const util::IliFileCatalog &files() const noexcept { return files_; }
   metamodel::MetaModelStore &models() noexcept { return models_; }
   const metamodel::MetaModelStore &models() const noexcept { return models_; }
   metamodel::MetaModelBuilder &builder() noexcept { return builder_; }
   const metamodel::MetaModelBuilder &builder() const noexcept { return builder_; }

private:
   SourceManager &sessionSources_;
   CompilerOptions options_;
   util::Logger logger_;
   CompilationSourceStore compilationSources_;
   util::IliFileCatalog files_;
   metamodel::MetaModelStore models_;
   metamodel::MetaModelBuilder builder_;
};

} // namespace detail
} // namespace ilic
