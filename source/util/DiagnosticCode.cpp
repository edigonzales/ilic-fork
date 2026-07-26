#include "DiagnosticCode.h"

#include <algorithm>
#include <cctype>
#include <initializer_list>

namespace util {

namespace {

std::string lower(std::string_view value)
{
   std::string result(value);
   std::transform(result.begin(),result.end(),result.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return result;
}

bool contains(const std::string &value,std::initializer_list<std::string_view> needles)
{
   return std::any_of(needles.begin(),needles.end(),[&](std::string_view needle) {
      return value.find(needle) != std::string::npos;
   });
}

}

std::string diagnosticCodeForMessage(std::string_view message)
{
   const std::string value = lower(message);

   if (contains(value,{"unsupported iliversion","unsupported interlis version"}))
      return "ILIC-INPUT-UNSUPPORTED-VERSION";
   if (contains(value,{"internal compiler failure","internal compiler error"}))
      return "ILIC-COMPILER-INTERNAL";
   if (contains(value,{"unable to load root source","unable to read","could not open"}))
      return "ILIC-INPUT-LOAD";
   if (contains(value,{"unable to order model dependencies","dependency cycle"}))
      return "ILIC-MODEL-DEPENDENCY";

   if (contains(value,{"translation mismatch"}))
      return "ILIC-TRANSLATION-MISMATCH";
   if (contains(value,{"translation of","translation base","translation language",
      "translation metadata","translation chain"}))
      return "ILIC-TRANSLATION-RULE";

   if (contains(value,{"not found","unknown ","there is no ","has no visible",
      "has no accessible","missing a context"})) {
      if (contains(value,{"attribute"})) return "ILIC-NAME-ATTRIBUTE-NOT-FOUND";
      if (contains(value,{"role"})) return "ILIC-NAME-ROLE-NOT-FOUND";
      if (contains(value,{"topic"})) return "ILIC-NAME-TOPIC-NOT-FOUND";
      if (contains(value,{"model"})) return "ILIC-NAME-MODEL-NOT-FOUND";
      if (contains(value,{"domain","type","unit"})) return "ILIC-NAME-TYPE-NOT-FOUND";
      if (contains(value,{"path","reference","viewable","class","structure","association","view"}))
         return "ILIC-NAME-ELEMENT-NOT-FOUND";
      return "ILIC-NAME-NOT-FOUND";
   }

   if (contains(value,{"does not match","should end with","must end with"," expected"}))
      return "ILIC-NAME-END-MISMATCH";
   if (contains(value,{"duplicate","multiple declaration","multiple declarations",
      "already exists","there is already","same name","name conflict","several attributes"}))
      return "ILIC-NAME-DUPLICATE";

   if (contains(value,{"cardinality","multiplicity"}))
      return "ILIC-CARDINALITY-RULE";
   if (contains(value,{"association requires at least","association must have at least"}))
      return "ILIC-ASSOCIATION-ROLE-COUNT";
   if (contains(value,{"extend","extension","extended","inherited","inheritance",
      "base class","base topic","base association","base attribute","base role",
      "base view","final base","subrange"}))
      return "ILIC-INHERITANCE-RULE";
   if (contains(value,{"topic dependency","depend on","requires model","to import",
      "cross-topic","cross topic","requires external","other topic"}))
      return "ILIC-DEPENDENCY-RULE";
   if (contains(value,{"constraint","unique","basket and local","set constraint",
      "required in"}))
      return "ILIC-CONSTRAINT-RULE";
   if (contains(value,{"generic","deferred","context definition","context default"}))
      return "ILIC-GENERIC-CONTEXT";
   if (contains(value,{"enumeration","enum ","#"}))
      return "ILIC-ENUMERATION-RULE";
   if (contains(value,{"function","argument"}))
      return "ILIC-FUNCTION-SIGNATURE";
   if (contains(value,{"path","reference","referenced","target role","roleaccess"}))
      return "ILIC-REFERENCE-RULE";
   if (contains(value,{"datatype","type","numeric","boolean","logical","text length",
      "formatted value","coordinate","coord ","structure","class ","association ",
      "view ","domain "}))
      return "ILIC-TYPE-MISMATCH";
   if (contains(value,{"minimum","maximum","range","precision","overlap","outside"}))
      return "ILIC-VALUE-RANGE";
   if (contains(value,{"abstract","final","transient","external","ordered","hiding",
      "mandatory","composition","aggregation","property ","properties "}))
      return "ILIC-PROPERTY-RULE";

   return "ILIC-MODEL-INVALID-DECLARATION";
}

}
