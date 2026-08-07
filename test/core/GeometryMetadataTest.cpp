#include "ilic/ModelCompilation.h"

#include "../../source/metamodel/MetaModelStore.h"
#include "ilic/test/TestHarness.h"

#include <algorithm>
#include <iostream>

namespace {

const char *modelUri = "memory:///GeometryMeta.ili";

ilic::ModelCompilationInput input()
{
   ilic::ModelCompilationInput result;
   result.sources.push_back({modelUri,R"ili(INTERLIS 2.3;
MODEL GeometryMeta (en) AT "https://example.test" VERSION "1.0" =
  DOMAIN
    Coord2 = COORD
      0.00 .. 1000.00,
      0.00 .. 1000.00;

    Coord3 = COORD
      0.00 .. 1000.00,
      0.00 .. 1000.00,
      0.00 .. 100.00;

    SurfaceDomain = SURFACE WITH (STRAIGHTS, ARCS)
      VERTEX Coord2
      WITHOUT OVERLAPS > 0.02;

    AreaDomain = AREA WITH (STRAIGHTS)
      VERTEX Coord2;

  STRUCTURE BoundaryAttributes =
    Weight : 0.0 .. 100.0;
  END BoundaryAttributes;

  LINE FORM CustomForm: BoundaryAttributes;

  TOPIC Data =
    CLASS Feature =
      Geometry : SurfaceDomain;
      AreaGeometry : AreaDomain;
      LocalGeometry : POLYLINE WITH (STRAIGHTS) VERTEX Coord2;
      Point3D : Coord3;
      CustomGeometry : POLYLINE WITH (CustomForm) VERTEX Coord2
        LINE ATTRIBUTES BoundaryAttributes;
    END Feature;
  END Data;
END GeometryMeta.
)ili",1});
   result.request.roots.push_back(modelUri);
   return result;
}

metamodel::Model *model(const metamodel::MetaModelStore &store)
{
   for (auto *candidate : store.models())
      if (candidate != nullptr && candidate->Name == "GeometryMeta") return candidate;
   return nullptr;
}

metamodel::Class *feature(const metamodel::MetaModelStore &store)
{
   auto *compiledModel = model(store);
   if (compiledModel == nullptr) return nullptr;
   for (auto *element : compiledModel->Element) {
      auto *topic = dynamic_cast<metamodel::SubModel *>(element);
      if (topic == nullptr || topic->Name != "Data") continue;
      for (auto *topicElement : topic->Element) {
         auto *candidate = dynamic_cast<metamodel::Class *>(topicElement);
         if (candidate != nullptr && candidate->Name == "Feature") return candidate;
      }
   }
   return nullptr;
}

metamodel::LineType *lineType(metamodel::Class *klass,const char *name)
{
   if (klass == nullptr) return nullptr;
   for (auto *attribute : klass->ClassAttribute) {
      if (attribute == nullptr || attribute->Name != name) continue;
      return dynamic_cast<metamodel::LineType *>(attribute->Type);
   }
   return nullptr;
}

metamodel::CoordType *coordType(metamodel::Class *klass,const char *name)
{
   if (klass == nullptr) return nullptr;
   for (auto *attribute : klass->ClassAttribute) {
      if (attribute == nullptr || attribute->Name != name) continue;
      return dynamic_cast<metamodel::CoordType *>(attribute->Type);
   }
   return nullptr;
}

} // namespace

int main()
{
   ilic::ModelCompilation compilation(input());
   if (!compilation.success()) {
      for (const auto &diagnostic : compilation.result().diagnostics)
         std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
   }
   ILIC_REQUIRE(compilation.success());

   const auto &store = compilation.models();
   auto *compiledModel = model(store);
   ILIC_REQUIRE(compiledModel != nullptr);
   auto *compiledFeature = feature(store);
   ILIC_REQUIRE(compiledFeature != nullptr);

   auto *surface = lineType(compiledFeature,"Geometry");
   ILIC_REQUIRE(surface != nullptr);
   ILIC_REQUIRE(surface->Kind == metamodel::LineType::Surface);
   ILIC_REQUIRE(surface->MaxOverlap == "0.02");
   ILIC_REQUIRE(surface->CoordType != nullptr);
   ILIC_REQUIRE(surface->CoordType->Axis.size() == 2);
   ILIC_REQUIRE(surface->LAStructure == nullptr);
   ILIC_REQUIRE(std::any_of(surface->LineForm.begin(),surface->LineForm.end(),
      [](const auto *form) { return form != nullptr && form->Name == "STRAIGHTS"; }));
   ILIC_REQUIRE(std::any_of(surface->LineForm.begin(),surface->LineForm.end(),
      [](const auto *form) { return form != nullptr && form->Name == "ARCS"; }));

   auto *area = lineType(compiledFeature,"AreaGeometry");
   ILIC_REQUIRE(area != nullptr);
   ILIC_REQUIRE(area->Kind == metamodel::LineType::Area);

   auto *local = lineType(compiledFeature,"LocalGeometry");
   ILIC_REQUIRE(local != nullptr);
   ILIC_REQUIRE(local->Kind == metamodel::LineType::Polyline);
   ILIC_REQUIRE(local->MaxOverlap.empty());

   auto *point3D = coordType(compiledFeature,"Point3D");
   ILIC_REQUIRE(point3D != nullptr);
   ILIC_REQUIRE(point3D->Axis.size() == 3);
   ILIC_REQUIRE(point3D->Axis.front()->Name == "C1");
   ILIC_REQUIRE(point3D->Axis.back()->Name == "C3");
   ILIC_REQUIRE(point3D->Axis.back()->Min == "0.00");
   ILIC_REQUIRE(point3D->Axis.back()->Max == "100.00");

   auto *custom = lineType(compiledFeature,"CustomGeometry");
   ILIC_REQUIRE(custom != nullptr);
   ILIC_REQUIRE(custom->LAStructure != nullptr);
   ILIC_REQUIRE(custom->LAStructure->Name == "BoundaryAttributes");
   ILIC_REQUIRE(custom->LineForm.size() == 1);
   ILIC_REQUIRE(custom->LineForm.front()->Name == "CustomForm");
   return 0;
}
