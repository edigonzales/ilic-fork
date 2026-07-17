#include "Ili2Input_helper.h"
#include "../../util/StringUtil.h"
#include "../../util/Logger.h"

using namespace std;
using namespace util;

string input::visitString(antlr4::Token *t)
{
   string value = t->getText();
   return value.substr(1,value.size()-2);
}

static string simplify_type(string t)
{
   string ts = t;
   if (ts == "FormattedType") {
      ts = "TextType";
   }
   else if (ts == "EnumTreeValueType") {
      ts = "EnumType";
   }
   return ts;
}
   
bool input::check_type_compatibility(string t1,string t2)
{
   
   return true; // to do !!!

   if (t1 == "???" || t2 == "???") {
      return true;
   }

   if (simplify_type(t1) == simplify_type(t2)) {
      return true;
   }
   else {
      return false;
   }

}

bool input::check_type_extendability(metamodel::Type *base,metamodel::Type *extension,int line)
{

   if (base == nullptr || extension == nullptr) {
      return true;
   }
   if (base->getClass() != extension->getClass()) {
      Log.error("incompatible type",line);
      return false;
   }
   else if (base->getClass() == "TextType") {
      TextType *b = static_cast<TextType *>(base);
      TextType *e = static_cast<TextType *>(extension);
      if (e->MaxLength > b->MaxLength && b->MaxLength != -1) {
         Log.error("extended type may not exeed length of base type",line);
         return false;
      }
   }
   return true;
}

map<string,bool> input::get_properties(parser::Ili2Parser::PropertiesContext *ctx,vector<string> allowed_properties)
{

   map<string,bool> properties;
   int line = -1;
   
   if (ctx != nullptr) {
      line = ctx->start->getLine();
   }

   // ABSTRACT
   if (ctx == nullptr) {
      properties[ABSTRACT] = false;
   }
   else if (ctx->ABSTRACT().size() == 0) {
      properties[ABSTRACT] = false;
   }
   else if (ctx->ABSTRACT().size() == 1) {
      if (find_string(allowed_properties,ABSTRACT)) {
         properties[ABSTRACT] = true;
      }
      else {
         Log.error("property " + ABSTRACT + " not allowed",line);
      }
   }
   else if (ctx->ABSTRACT().size() > 1) {
      Log.warning("multiple declarations of " + ABSTRACT,line);
      properties[ABSTRACT] = true;
   }

   // EXTENDED
   if (ctx == nullptr) {
      properties[EXTENDED] = false;
   }
   else if (ctx->EXTENDED().size() == 0) {
      properties[EXTENDED] = false;
   }
   else if (ctx->EXTENDED().size() == 1) {
      if (find_string(allowed_properties,EXTENDED)) {
         properties[EXTENDED] = true;
      }
      else {
         Log.error("property " + EXTENDED + " not allowed",line);
      }
   }
   else if (EXTENDED.size() > 1) {
      Log.warning("multiple declarations of " + EXTENDED,line);
      properties[EXTENDED] = true;
   }

   // FINAL
   if (ctx == nullptr) {
      properties[FINAL] = false;
   }
   else if (ctx->FINAL().size() == 0) {
      properties[FINAL] = false;
   }
   else if (ctx->FINAL().size() == 1) {
      if (find_string(allowed_properties,FINAL)) {
         properties[FINAL] = true;
      }
      else {
         Log.error("property " + FINAL + " not allowed",line);
      }
   }
   else if (ctx->FINAL().size() > 1) {
      Log.warning("multiple declarations of " + FINAL,line);
      properties[FINAL] = true;
   }

   // TRANSIENT
   if (ctx == nullptr) {
      properties[TRANSIENT] = false;      
   }
   else if (ctx->TRANSIENT().size() == 0) {
      properties[TRANSIENT] = false;      
   }
   else if (ctx->TRANSIENT().size() == 1) {
      if (find_string(allowed_properties,TRANSIENT)) {
         properties[TRANSIENT] = true;
      }
      else {
         Log.error("property " + TRANSIENT + " not allowed",line);
      }
   }
   else if (ctx->TRANSIENT().size() > 1) {
      Log.warning("multiple declarations of " + TRANSIENT,line);
      properties[TRANSIENT] = true;
   }

   // OID
   if (ctx == nullptr) {
      properties[OID] = false;      
   }
   else if (ctx->OID().size() == 0) {
      properties[OID] = false;      
   }
   else if (ctx->OID().size() == 1) {
      if (find_string(allowed_properties,OID)) {
         properties[OID] = true;
      }
      else {
         Log.error("property " + OID + " not allowed",line);
      }
   }
   else if (ctx->OID().size() > 1) {
      Log.warning("multiple declarations of " + OID,line);
      properties[OID] = true;
   }

   // HIDING
   if (ctx == nullptr) {
      properties[HIDING] = false;      
   }
   else if (ctx->HIDING().size() == 0) {
      properties[HIDING] = false;      
   }
   else if (ctx->HIDING().size() == 1) {
      if (find_string(allowed_properties,HIDING)) {
         properties[HIDING] = true;
      }
      else {
         Log.error("property " + HIDING + " not allowed",line);
      }
   }
   else if (ctx->HIDING().size() > 1) {
      Log.warning("multiple declarations of " + HIDING,line);
      properties[HIDING] = true;
   }

   // ORDERED
   if (ctx == nullptr) {
      properties[ORDERED] = false;      
   }
   else if (ctx->ORDERED().size() == 0) {
      properties[ORDERED] = false;      
   }
   else if (ctx->ORDERED().size() == 1) {
      if (find_string(allowed_properties,ORDERED)) {
         properties[ORDERED] = true;
      }
      else {
         Log.error("property " + ORDERED + " not allowed",line);
      }
   }
   else if (ctx->ORDERED().size() > 1) {
      Log.warning("multiple declarations of " + ORDERED,line);
      properties[ORDERED] = true;
   }

   // EXTERNAL
   if (ctx == nullptr) {
      properties[EXTERNAL] = false;
   }
   else if (ctx->EXTERNAL().size() == 0) {
      properties[EXTERNAL] = false;
   }
   else if (ctx->EXTERNAL().size() == 1) {
      if (find_string(allowed_properties,EXTERNAL)) {
         properties[EXTERNAL] = true;
      }
      else {
         Log.error("property " + EXTERNAL + " not allowed",line);
      }
   }
   else if (ctx->EXTERNAL().size() > 1) {
      Log.warning("multiple declarations of " + EXTERNAL,line);
      properties[EXTERNAL] = true;
   }
   
   if (properties[ABSTRACT] && properties[FINAL]) {
      Log.error(ABSTRACT + " and " + FINAL + " can not be used at the same time",line);
   }

   return properties;

}

void input::check_references(Class *c,string name,int line)
{
   if (c == nullptr) {
      return;
   }
   
   Log.debug(">>> check_references() " + get_path(c));
   Log.incNestLevel();
   
   for (auto a: c->ClassAttribute) {
      // Name resolution errors are reported while building the attribute. A
      // later semantic pass must tolerate the unresolved edge.
      if (a == nullptr || a->Type == nullptr) {
         continue;
      }
      int l = line;
      if (line == 0) {
         l = a->_line;
      }
      string n;
      if (name == "") {
         n = a->Name;
      }
      else {
         n = name + "->" + a->Name;
      }
      if (a->Type->getClass() == "ReferenceType") {
         ReferenceType *t = static_cast<ReferenceType *>(a->Type);
         list<Class *> classrestriction;
         if (t->_classrestriction.size() == 0) {
            classrestriction.push_back(t->_baseclass);
         }
         else {
            classrestriction = t->_classrestriction;            
         }
         for (auto c: classrestriction) {
            if (c != nullptr && c->ElementInPackage != nullptr &&
                get_package_context() != c->ElementInPackage && !t->External) {
               Log.error("reference to other topic must be declared EXTERNAL",l);
               break;
            }
         }
         for (auto c: classrestriction) {
            if (c != nullptr && !depends_on(c->ElementInPackage)) {
               Log.error(n + " requires topic dependency on " + get_path(t->_baseclass->ElementInPackage),l);
               break;
            }
         }
      }
      else if (a->Type->getClass() == "MultiValue") {
         MultiValue *t = static_cast<MultiValue *>(a->Type);
         if (t->TypeRestriction.size() == 0) {
            if (t->BaseType != nullptr && t->BaseType->getClass() == "Class") {
               check_references(static_cast<Class *>(t->BaseType),n,l);
            }
         }
         else {
            for (auto c: t->TypeRestriction) {
               if (c != nullptr && c->getClass() == "Class") {
                  check_references(static_cast<Class *>(c),n,l);
               }
            }
         }
      }
   }

   Log.decNestLevel();
   Log.debug("<<< check_references() " + get_path(c));

}
