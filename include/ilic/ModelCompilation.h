#pragma once

#include "ilic/Compiler.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace metamodel {
class MetaModelStore;
}

namespace ilic {

struct ModelSource final {
   std::string uri;
   std::string utf8;
   std::uint64_t version = 0;
};

struct ModelCompilationInput final {
   std::vector<ModelSource> sources;
   CompilationRequest request;
};

class ModelCompilation final {
public:
   explicit ModelCompilation(ModelCompilationInput input);
   ~ModelCompilation();

   ModelCompilation(const ModelCompilation &) = delete;
   ModelCompilation &operator=(const ModelCompilation &) = delete;
   ModelCompilation(ModelCompilation &&) noexcept;
   ModelCompilation &operator=(ModelCompilation &&) noexcept;

   bool success() const noexcept;
   const CompilationResult &result() const noexcept;
   const std::vector<std::string> &sourceUris() const noexcept;

   const metamodel::MetaModelStore &models() const;

private:
   struct Impl;
   std::unique_ptr<Impl> impl_;
};

} // namespace ilic
