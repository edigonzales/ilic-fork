#pragma once

#include <string>
#include <map>
#include <vector>
#include "antlr4-runtime.h"
#include "../../metamodel/metamodel.h"
#include "../parser/generated/Ili2Parser.h"

using namespace std;
using namespace metamodel;

namespace input {

   const string ABSTRACT  = "ABSTRACT";
   const string EXTENDED  = "EXTENDED";
   const string FINAL     = "FINAL";
   const string TRANSIENT = "TRANSIENT";
   const string OID       = "OID";
   const string HIDING    = "HIDING";
   const string ORDERED   = "ORDERED";
   const string EXTERNAL  = "EXTERNAL";
   const string GENERIC   = "GENERIC";

   string visitString(antlr4::Token *t);
   map<string,bool> get_properties(parser::Ili2Parser::PropertiesContext *ctx,vector<string> allowed_properties);
   void check_references(metamodel::Class *c,string name,int line);
   bool check_type_compatibility(string base,string extension);
   bool check_type_extendability(metamodel::Type *base,metamodel::Type *extension,int line);

}
