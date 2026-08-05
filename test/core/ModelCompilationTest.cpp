#include "ilic/ModelCompilation.h"

#include "../../source/metamodel/MetaModelStore.h"
#include "ilic/test/TestHarness.h"

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

const char *modelUri = "memory:///ModelCompilation.ili";

ilic::ModelCompilationInput input()
{
   ilic::ModelCompilationInput result;
   result.sources.push_back({modelUri,R"ili(INTERLIS 2.3;
MODEL ModelCompilation AT "https://example.invalid/ilic/model-compilation" VERSION "1" =
END ModelCompilation.
)ili",7});
   result.request.roots.push_back(modelUri);
   return result;
}

} // namespace

int main()
{
   static_assert(!std::is_copy_constructible_v<ilic::ModelCompilation>);
   static_assert(!std::is_copy_assignable_v<ilic::ModelCompilation>);
   static_assert(std::is_move_constructible_v<ilic::ModelCompilation>);
   static_assert(std::is_move_assignable_v<ilic::ModelCompilation>);

   ilic::ModelCompilation compilation(input());
   ILIC_REQUIRE(compilation.success());
   ILIC_REQUIRE(compilation.result().success);
   ILIC_REQUIRE(!compilation.result().models.empty());
   ILIC_REQUIRE(std::find(compilation.sourceUris().begin(),compilation.sourceUris().end(),
      modelUri) != compilation.sourceUris().end());
   ILIC_REQUIRE(std::any_of(compilation.models().models().begin(),
      compilation.models().models().end(),[](const auto *model) {
         return model != nullptr && model->Name == "ModelCompilation";
      }));

   const auto *firstStore = &compilation.models();
   ilic::ModelCompilation moved(std::move(compilation));
   ILIC_REQUIRE(moved.success());
   ILIC_REQUIRE(&moved.models() == firstStore);
   ILIC_REQUIRE(compilation.result().success == false);

   ilic::ModelCompilation assigned(input());
   const auto *assignedStore = &assigned.models();
   assigned = std::move(moved);
   ILIC_REQUIRE(assigned.success());
   ILIC_REQUIRE(&assigned.models() == firstStore);
   ILIC_REQUIRE(&assigned.models() != assignedStore);

   ilic::ModelCompilation invalid([] {
      auto result = input();
      result.sources.front().utf8 = R"ili(INTERLIS 2.3;
MODEL Broken AT "https://example.invalid/ilic/model-compilation" VERSION "1" =
  TOPIC Missing =
    CLASS Item =
      Value : MissingDomain;
    END Item;
  END Missing;
END Broken.
)ili";
      return result;
   }());
   ILIC_REQUIRE(!invalid.success());
   bool threw = false;
   try {
      (void)invalid.models();
   } catch (const std::logic_error &) {
      threw = true;
   }
   ILIC_REQUIRE(threw);

   bool duplicateThrew = false;
   try {
      auto duplicate = input();
      duplicate.sources.push_back({modelUri,"INTERLIS 2.3;\n",8});
      ilic::ModelCompilation rejected(std::move(duplicate));
      (void)rejected;
   } catch (const std::invalid_argument &) {
      duplicateThrew = true;
   }
   ILIC_REQUIRE(duplicateThrew);

   ilic::ModelCompilation left(input());
   ilic::ModelCompilation right(input());
   ILIC_REQUIRE(&left.models() != &right.models());
   ILIC_REQUIRE(left.models().models().front() != right.models().models().front());
   return 0;
}
