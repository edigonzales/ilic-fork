#include "../../include/ilic/ModelCompilation.h"

#include "CompilationRun.h"
#include "CompilerContext.h"

#include <map>
#include <stdexcept>
#include <utility>

namespace ilic {

struct ModelCompilation::Impl final {
   SourceManager sources;
   std::unique_ptr<detail::CompilerContext> context;
   CompilationResult result;
   std::vector<std::string> sourceUris;
};

namespace {

const CompilationResult &emptyResult() noexcept
{
   static const CompilationResult result;
   return result;
}

const std::vector<std::string> &emptySourceUris() noexcept
{
   static const std::vector<std::string> uris;
   return uris;
}

} // namespace

ModelCompilation::ModelCompilation(ModelCompilationInput input)
   : impl_(std::make_unique<Impl>())
{
   std::map<std::string, std::string> sourceContents;
   for (const auto &source : input.sources) {
      const auto found = sourceContents.find(source.uri);
      if (found != sourceContents.end() && found->second != source.utf8)
         throw std::invalid_argument(
            "ModelCompilation received duplicate source URI with different content: " +
            source.uri);
      sourceContents.emplace(source.uri,source.utf8);
   }

   for (auto &source : input.sources) {
      const SourceUpdateResult update = impl_->sources.update(
         std::move(source.uri),std::move(source.utf8),source.version);
      if (!update.accepted)
         throw std::invalid_argument("ModelCompilation rejected source URI");
   }

   impl_->context = std::make_unique<detail::CompilerContext>(
      impl_->sources,input.request.options);
   detail::CompilationRun run(*impl_->context);
   impl_->result = run.run(input.request,impl_->sourceUris);
}

ModelCompilation::~ModelCompilation() = default;

ModelCompilation::ModelCompilation(ModelCompilation &&) noexcept = default;

ModelCompilation &ModelCompilation::operator=(ModelCompilation &&) noexcept = default;

bool ModelCompilation::success() const noexcept
{
   return impl_ != nullptr && impl_->result.success;
}

const CompilationResult &ModelCompilation::result() const noexcept
{
   return impl_ == nullptr ? emptyResult() : impl_->result;
}

const std::vector<std::string> &ModelCompilation::sourceUris() const noexcept
{
   return impl_ == nullptr ? emptySourceUris() : impl_->sourceUris;
}

const metamodel::MetaModelStore &ModelCompilation::models() const
{
   if (impl_ == nullptr || !impl_->result.success || impl_->context == nullptr)
      throw std::logic_error(
         "ModelCompilation::models() requires a successful compilation");
   return impl_->context->models();
}

} // namespace ilic
