#include "MetaModelInput.h"
#include "../../include/ilic/SourceManager.h"
#include "../util/StringUtil.h"
#include "../util/Logger.h"

#include <algorithm>
#include <sstream>
#include <cctype>

using namespace util;
using namespace metamodel;

namespace metamodel {

   // golbal variables

   bool ili23 = true;
   bool ili24 = true;
   string iliversion;

   static list <Package*> AllPackages;
   static list <Type*> AllTypes;
   static list <Unit*> AllUnits;
   static list <Import*> AllImports;
   static list <FunctionDef*> AllFunctions;
   static list <LineForm*> AllLineForms;
   static list <Graphic*> AllGraphics;
   static Class AnyClass;
   static Class AnyStructure;
   static bool UniversalClassesInitialized = false;
   static string CurrentSourceText;
   struct PendingMetaAttribute {
      string Name;
      string Value;
      string RawText;
      int Line = -1;
      int Column = 0;
   };
   static map<int,list<PendingMetaAttribute>> PendingMetaAttributes;

   static string trim_copy(const string &value)
   {
      size_t first = 0;
      while (first < value.size() && isspace(static_cast<unsigned char>(value[first]))) ++first;
      size_t last = value.size();
      while (last > first && isspace(static_cast<unsigned char>(value[last - 1]))) --last;
      return value.substr(first,last - first);
   }

   static string decode_meta_value(string value)
   {
      value = trim_copy(value);
      if (value.size() < 2 || value.front() != '"' || value.back() != '"') return value;
      string decoded;
      for (size_t i = 1; i + 1 < value.size(); ++i) {
         if (value[i] == '\\' && i + 2 < value.size()
             && (value[i + 1] == '\\' || value[i + 1] == '"')) {
            decoded += value[++i];
         }
         else decoded += value[i];
      }
      return decoded;
   }

   void reset_input_state()
   {
      AllPackages.clear();
      AllTypes.clear();
      AllUnits.clear();
      AllImports.clear();
      AllFunctions.clear();
      AllLineForms.clear();
      AllGraphics.clear();
      PendingMetaAttributes.clear();
      CurrentSourceText.clear();
      UniversalClassesInitialized = false;
      ili23 = true;
      ili24 = true;
      iliversion.clear();
   }

   void prepare_meta_attributes(const string &source)
   {
      CurrentSourceText = source;
      PendingMetaAttributes.clear();
      list<PendingMetaAttribute> pending;
      istringstream lines(source);
      string line;
      int lineNumber = 0;
      bool inBlockComment = false;
      while (getline(lines,line)) {
         ++lineNumber;
         size_t first = line.find_first_not_of(" \t\r");
         string trimmed = first == string::npos ? "" : line.substr(first);
         if (inBlockComment) {
            size_t end = trimmed.find("*/");
            if (end == string::npos) continue;
            inBlockComment = false;
            trimmed = trim_copy(trimmed.substr(end + 2));
            if (trimmed.empty()) continue;
         }
         while (trimmed.rfind("/*",0) == 0) {
            size_t end = trimmed.find("*/",2);
            if (end == string::npos) {
               inBlockComment = true;
               trimmed.clear();
               break;
            }
            trimmed = trim_copy(trimmed.substr(end + 2));
         }
         if (trimmed.empty()) continue;
         if (trimmed.rfind("!!@",0) == 0) {
            string option = trim_copy(trimmed.substr(3));
            size_t equals = option.find('=');
            string name = equals == string::npos ? "" : trim_copy(option.substr(0,equals));
            if (equals == string::npos || name.empty()) {
               Log.error("invalid meta attribute; expected !!@ name=value",lineNumber,
                  static_cast<int>(first == string::npos ? 0 : first),"ILIC-META-SYNTAX");
               continue;
            }
            PendingMetaAttribute attribute;
            attribute.Name = std::move(name);
            attribute.Value = decode_meta_value(option.substr(equals + 1));
            attribute.RawText = trimmed;
            attribute.Line = lineNumber;
            attribute.Column = static_cast<int>(first == string::npos ? 0 : first);
            pending.push_back(std::move(attribute));
         }
         else if (trimmed.rfind("!!",0) != 0 && !pending.empty()) {
            PendingMetaAttributes[lineNumber] = pending;
            pending.clear();
         }
      }
      if (!pending.empty()) {
         const PendingMetaAttribute &attribute = pending.front();
         Log.error("meta attribute is not followed by a model element",attribute.Line,
            attribute.Column,"ILIC-META-DANGLING");
      }
   }

   static void initialize_universal_classes()
   {
      if (UniversalClassesInitialized) {
         return;
      }
      AnyClass.Name = "ANYCLASS";
      AnyClass.Kind = Class::ClassVal;
      AnyStructure.Name = "ANYSTRUCTURE";
      AnyStructure.Kind = Class::Structure;
      UniversalClassesInitialized = true;
   }

   // mmobject helpers

   void init_mmobject(MMObject* o, int line)
   {
      o->_line = line;
      if (line > 0 && !Log.getCurrentSource().empty()) {
         o->_source.valid = true;
         o->_source.uri = Log.getCurrentSource();
         o->_source.start.line = static_cast<size_t>(line - 1);
         o->_source.end = o->_source.start;
         o->_source.end.character = 1;
      }
   }

   static size_t utf8_byte_offset(size_t codepointOffset)
   {
      size_t byteOffset = 0;
      for (size_t index = 0;
           index < codepointOffset && byteOffset < CurrentSourceText.size();++index) {
         const unsigned char lead = static_cast<unsigned char>(CurrentSourceText[byteOffset]);
         size_t width = 1;
         if ((lead & 0xe0) == 0xc0) width = 2;
         else if ((lead & 0xf0) == 0xe0) width = 3;
         else if ((lead & 0xf8) == 0xf0) width = 4;
         byteOffset = min(CurrentSourceText.size(),byteOffset + width);
      }
      return byteOffset;
   }

   static ilic::SourceRange token_source(antlr4::Token *token)
   {
      ilic::SourceRange result;
      if (token == nullptr || token->getStartIndex() > CurrentSourceText.size() ||
          Log.getCurrentSource().empty()) return result;
      const size_t start = utf8_byte_offset(token->getStartIndex());
      const size_t end = utf8_byte_offset(token->getStopIndex() + 1);
      ilic::SourceManager *sources = ilic::activeSourceManager();
      if (sources == nullptr || sources->get(Log.getCurrentSource()) == nullptr) return result;
      const ilic::SourcePosition startPosition = sources->position(Log.getCurrentSource(),start);
      const ilic::SourcePosition endPosition = sources->position(Log.getCurrentSource(),end);
      result.valid = true;
      result.uri = Log.getCurrentSource();
      result.start = {startPosition.line,startPosition.utf16Column,startPosition.offset};
      result.end = {endPosition.line,endPosition.utf16Column,endPosition.offset};
      return result;
   }

   void set_selection_source(MetaElement *element,antlr4::Token *token)
   {
      if (element != nullptr) element->_selectionSource = token_source(token);
   }

   void set_reference_source(MMObject *object,const string &kind,antlr4::Token *token)
   {
      if (object == nullptr) return;
      const ilic::SourceRange source = token_source(token);
      if (source.valid) object->_referenceSources[kind] = source;
   }

   void set_reference_source(MMObject *object,const string &kind,
      antlr4::ParserRuleContext *context)
   {
      set_reference_source(object,kind,context == nullptr ? nullptr : context->getStop());
   }

   // metaelement helpers

   void init_metaelement(MetaElement* e, int line)
   {

      // list <DocText> Documentation;
      // ROLE from ASSOCIATION MetaAttributes
      // list <MetaAttribute *> MetaAttribute;

      init_mmobject(e, line);
      auto metadata = PendingMetaAttributes.find(line);
      if (metadata != PendingMetaAttributes.end()) {
         for (const auto &entry : metadata->second) {
            MetaAttribute *attribute = new MetaAttribute();
            init_mmobject(attribute,entry.Line);
            attribute->_source.start.character = static_cast<size_t>(entry.Column);
            attribute->_source.end = attribute->_source.start;
            attribute->_source.end.character += entry.RawText.size();
            attribute->Name = entry.Name;
            attribute->Value = entry.Value;
            attribute->_rawText = entry.RawText;
            attribute->MetaElement = e;
            e->MetaAttribute.push_back(attribute);
         }
      }
      MMObject *ctx = get_context();
      if (ctx == nullptr) {
         return;
      }

      string context = ctx->getClass();
      if (context == "Model" || context == "SubModel") {
         e->ElementInPackage = static_cast<Package *>(ctx);
         e->ElementInPackage->Element.push_back(e);
      }
      else if (context == "Class") {
         if (e->getClass() == "AttrOrParam") {
            e->ElementInPackage = nullptr;
            AttrOrParam* t = static_cast<AttrOrParam*>(e);
            t->AttrParent = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
         else if (e->isSubClassOf("Constraint")) {
            e->ElementInPackage = nullptr;
            Constraint* c = static_cast<Constraint*>(e);
            c->ToClass = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
         else if (e->getClass() == "Role") {
            e->ElementInPackage = nullptr;
            Role* r = static_cast<Role*>(e);
            r->Association = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
      }
      else if (context == "FunctionDef") {
         e->ElementInPackage = nullptr;
         FunctionDef* f = static_cast<FunctionDef*>(ctx);
         if (e->getClass() == "Argument") {
            Argument *a = static_cast<Argument*>(e);
            a->Function = f;
         }
         else { // ResultType
            Type* t = static_cast<Type*>(e);
            t->LFTParent = f;
         }
      }
      else if (context == "AttrOrParam") {
         e->ElementInPackage = nullptr;
         if (e->isSubClassOf("Type")) {
            Type* t = static_cast<Type*>(e);
            AttrOrParam *a = static_cast<AttrOrParam *>(ctx);
            t->_attr = a;
            a->Type = t; // localtype ???, to do !!!
         }
      }

   }

   void init_extendableme(ExtendableME* e, int line)
   {
      init_metaelement(e, line);
      // bool Abstract;
      // bool Final;
      // ROLE from ASSOCIATION Inheritance
      // ExtendableME *Super;
      // ROLE from ASSOCIATION Inheritance
      // list <ExtendableME *> Sub;
   }

   // package helpers

   void init_package(Package* p, int line)
   {
      init_metaelement(p, line);
      // ROLE from ASSOCIATION PackageElements
      // list <MetaElement *> Element;
   }

   // Model helpers

   Model* find_model(string name, int line)
   {

      Log.debug("find_model " + name);

      for (Model* m : get_all_models()) {
         if (m->Name == name) {
            return m;
         }
      }

      Log.error("model " + name + " not found.", line);
      return nullptr;

   }

   // Topic / DataUnit helpers

   DataUnit* find_dataunit(string name, int line)
   {
      Log.debug("find_dataunit " + name);
      for (DataUnit* u : get_all_dataunits()) {
         if (get_path(u) == (get_path(get_model_context()) + "." + name + ".BASKET")) {
            return u;
         }
         else if (get_path(u) == (name + ".BASKET")) {
            return u;
         }
      }
      Log.error("unknown topic " + name, line);
      return nullptr;
   }

   void add_package(Package* p)
   {
      if (p == nullptr) {
         return;
      }
      for (auto pp : AllPackages) {
         if (get_path(pp) == get_path(p)) {
            Log.error("multiple declarations of " + get_path(p),p->_line);
            return;
         }
      }
      AllPackages.push_back(p);
   }

   Package* find_package(string name, int line)
   {
      string package_name = name;
      if (ends_with(package_name,".BASKET")) {
         // DataUnit
         package_name = package_name.substr(0,package_name.length()-7);
      }
      Log.debug("find_package " + package_name);
      for (Package* p : AllPackages) {
         for (auto unqualified : get_all_unqualified_imports(get_model_context()->Name)) {
            if (get_path(p) == unqualified + "." + package_name) {
               return p;
            }
         }
      }
      for (Package* p : AllPackages) {
         if (get_path(p) == package_name) {
            return p;
         }
         else if (p->Name == package_name) {
            return p;
         }
      }
      Log.error("unknown package " + package_name, line);
      return nullptr;
   }

   SubModel* find_topic(string name, int line)
   {
      Log.debug("find_topic " + name);
      Package *p = find_package(name,line);
      if (p->getClass() == "SubModel") {
         return static_cast<SubModel *>(p);
      }
      else {
         Log.error(name + " is not a topic");
         return nullptr;
      }         
   }

   // Unit helpers

   void add_unit(Unit* u)
   {
      if (u == nullptr) {
         return;
      }
      for (Unit* uu : AllUnits) {
         if (get_path(uu) == get_path(u)) {
            Log.error("multiple declaration of unit " + u->Name, u->_line);
            return;
         }
      }
      AllUnits.push_back(u);
   }

   Unit* find_unit(string name, int line)
   {
      Log.debug("find_unit " + name);

      // Qualified unit references identify their declaration directly. The
      // semantic checker separately enforces that the declaring model was
      // imported at this lexical occurrence.
      for (Unit* u : AllUnits) {
         if (get_path(u) == name) {
            return u;
         }
      }

      Model *model = get_model_context();
      auto matches = [&name](Unit *unit) {
         return unit != nullptr && (unit->Name == name || unit->_unitname == name);
      };

      // Short unit names are resolved in the current model first and then in
      // directly imported models. This avoids the old load-order-dependent
      // global lookup while retaining the RefHB unit-short-name notation.
      for (Unit *u : AllUnits) {
         if (matches(u) && u->ElementInPackage == model) {
            return u;
         }
      }
      for (Import *import : get_all_imports()) {
         if (import == nullptr || import->ImportingP != model) {
            continue;
         }
         for (Unit *u : AllUnits) {
            if (matches(u) && u->ElementInPackage == import->ImportedP) {
               return u;
            }
         }
      }

      // Preserve a useful semantic "missing import" diagnostic for an
      // otherwise uniquely named unit instead of reducing it to an unknown
      // symbol solely because the model omitted IMPORTS.
      for (Unit *u : AllUnits) {
         if (matches(u)) {
            return u;
         }
      }
      Log.error("unknown unit " + name, line);
      return nullptr;
   }

   // Type helpers

   void init_type(Type* t, int line)
   {
      init_extendableme(t, line);
      // FunctionDef *LFTParent;
   }

   void add_type(Type* t)
   {
      if (t == nullptr) {
         return;
      }
      if (util::starts_with(t->Name, "ILIC_")) {
         t->Name = t->Name.substr(5);
      }
      Log.debug(">>> add_type " + get_path(t));
      for (Type* tt : AllTypes) {
         if (get_path(tt) == get_path(t) && t->Name != "???") {
            Log.error("multiple declarations of " + get_path(t),t->_line);
            return;
         }
      }
      Log.debug("<<< add_type " + get_path(t));
      AllTypes.push_back(t);
   }

   static Type* find_type(string name, int line, bool error)
   {
      
      string search;
      if (util::starts_with(name, "ILIC_")) {
         search = name.substr(5);
      }
      else {
         search = name;
      }
      
      if (search == "HALIGNMENT") {
         search = "INTERLIS.HALIGNMENT";
      }
      else if (search == "VALIGNMENT") {
         search = "INTERLIS.VALIGNMENT";
      }

      MetaElement *ctx = get_package_context();
         
      string package_path = get_path(get_package_context());
      Log.debug(">>> find_type <" + search + "> in context " + package_path);

      // A class inherited through an extended topic may be addressed through
      // the extending topic's qualified path. The element itself remains
      // owned by the base topic, so include the corresponding base path in
      // the lookup candidates.
      vector<string> searches({search});
      for (size_t index = 0; index < searches.size(); ++index) {
         string candidate = searches[index];
         for (Package *package : AllPackages) {
            if (package->getClass() != "SubModel") {
               continue;
            }
            SubModel *topic = static_cast<SubModel *>(package);
            if (topic->_super == nullptr) {
               continue;
            }
            string prefix = get_path(topic) + ".";
            if (util::starts_with(candidate,prefix)) {
               string inherited = get_path(topic->_super) + "." + candidate.substr(prefix.length());
               if (find(searches.begin(),searches.end(),inherited) == searches.end()) {
                  searches.push_back(inherited);
               }
            }
         }
      }

      Type *found = nullptr;
      for (Type* t : AllTypes) {
         string path = get_path(t);
         for (auto candidate : searches) {
            if (candidate == path) {
               found = t;
               break;
            }
         }
         if (path == package_path + "." + search) {
            found = t;
         }
         if (ctx->getClass() == "Model") {
            string first_unqualified_match = "";
            for (auto unqualified : get_all_unqualified_imports(ctx->Name)) {
               if (path == unqualified + "." + search) {
                  if (first_unqualified_match == "") {
                     first_unqualified_match = unqualified;
                  }
                  else {
                     Log.error("ambiguous path " + search + " found in " + first_unqualified_match + " and " + unqualified,line);
                  }
                  found = t;
                  break;
               }
            }
         }
         else if (ctx->getClass() == "SubModel") {
            if (path == get_parent_path(ctx) + "." + search) {
               found = t;
            }
            for (auto unqualified : get_all_unqualified_imports(get_parent_path(ctx))) {
               string first_unqualified_match = "";
               if (path == unqualified + "." + search) {
                  if (first_unqualified_match == "") {
                     first_unqualified_match = unqualified;
                  }
                  else {
                     Log.error("ambiguous path " + search + " found in " + first_unqualified_match + " and " + unqualified,line);
                  }
                  found = t;
                  break;
               }
            }
            SubModel *s = static_cast<SubModel *>(ctx);
            while (s->_super != nullptr) {
               if (path == get_path(s->_super) + "." + search) {
                  found = t;
                  break;
               }
               s = static_cast<SubModel *>(s->_super);
            }
         }
      }
      
      if (found != nullptr) {
         Log.debug("<<< find_type " + get_path(found));
         return found;
      }

      if (error) {
         Log.error("type " + search + " not found.", line);
      }

      return nullptr;

   }

   Type* find_type(string name, int line)
   {
      return find_type(name, line, true);
   }

   string get_type_string(Type *t)
   {
      if (t == nullptr) {
         return "???";
      }
      if (t->Name == "BOOLEAN") {
         return "BooleanType";
      }
      else {
         return t->getClass();
      }
   }

   void init_domaintype(DomainType* t, int line)
   {
      init_type(t, line);
   }

   DomainType* find_domaintype(string name, int line)
   {
      Type* t = find_type(name, line);
      if (t == nullptr) {
         return nullptr;
      }
      if (t->isSubClassOf("DomainType")) {
         return static_cast<DomainType*>(t);
      }
      else {
         return nullptr;
      }
   }

   // Class helpers

   void init_class(Class* c, int line)
   {
      init_type(c, line);
   }

   void add_class(Class* c)
   {
      add_type(c);
   }

   Class* find_class_or_view(string name,int line)
   {

      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class" && t->getClass() != "View") {
         Log.error(name + " is no class or view",line);
         return nullptr;
      }

      return static_cast<Class*>(t);

   }

   Class* find_class_or_structure(string name,int line)
   {

      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class") {
         Log.error(name + " is no class or structure",line);
         return nullptr;
      }
      
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::ClassVal && c->Kind != Class::Structure) {
         Log.error(name + " is no class or structure",line);
         return nullptr;
      }

      return c;

   }

   Class* find_class_type(string name,int line)
   {

      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class") {
         Log.error(name + " is no class, structure or association ",line);
         return nullptr;
      }
      
      return static_cast<Class *>(t);

   }

   Class* find_class(string name, int line)
   {
      Log.debug("find_class " + name);
      Class* c;
      if (name == "ANYCLASS" || name == "INTERLIS.ANYCLASS" || name == "CLASS" || name == "INTERLIS.CLASS") {
         initialize_universal_classes();
         c = &AnyClass;
      }
      else if (name == "ANYSTRUCTURE" || name == "INTERLIS.ANYSTRUCTURE" || name == "STRUCTURE" || name == "INTERLIS.STRUCTURE") {
         initialize_universal_classes();
         c = &AnyStructure;
      }
      else {
         Type* t = find_type(name, line, false);
         if (t == nullptr) {
            Log.error("class " + name + " not found",line);
            return nullptr;
         }
         if (t->getClass() != "Class") {
            Log.error(name + " is no class",line);
            return nullptr;
         }
         c = static_cast<Class *>(t);
         if (c->Kind != Class::ClassVal) {
            Log.error(name + " is no class",line);
         }
      }
      return c;
   }

   Class* find_class(Package* p, string name, int line)
   {
      Log.debug("find_class " + name);
      if (p == nullptr) {
         return nullptr;
      }
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::ClassVal) {
               return c;
            }
         }
      }
      Log.error("class " + name + " not found", line);
      return nullptr;
   }

   Class* find_structure(string name, int line)
   {
      Log.debug("find_structure " + name);
      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("structure " + name + " not found",line);
         return nullptr;
      }
      if (t->getClass() != "Class") {
         Log.error(name + " is no structure",line);
         return nullptr;
      }
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::Structure) {
         Log.error(name + " is no structure",line);
      }
      return c;
   }

   Class* find_structure(Package* p, string name, int line)
   {
      Log.debug("find_structure " + name);
      if (p == nullptr) {
         return nullptr;
      }
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::Structure) {
               return c;
            }
         }
      }
      Log.error("structure " + name + " not found", line);
      return nullptr;
   }

   Class* find_association(string name, int line)
   {
      Log.debug("find_association " + name);
      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("association " + name + " not found",line);
         return nullptr;
      }
      if (t->getClass() != "Class") {
         Log.error(name + " is no association",line);
         return nullptr;
      }
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::Association) {
         Log.error(name + " is no association",line);
      }
      return c;
   }

   Class* find_association(Package* p, string name, int line)
   {
      if (p == nullptr) {
         return nullptr;
      }
      Log.debug("find_association " + name);
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::Association) {
               return c;
            }
         }
      }
      Log.error("association " + name + " not found", line);
      return nullptr;
   }

   View* find_view(string name, int line)
   {
      Log.debug("find_view " + name);
      Type* t = find_type(name, line, false);
      if (t == nullptr) {
         Log.error("view " + name + " not found",line);
         return nullptr;
      }
      if (t->getClass() != "View") {
         Log.error(name + " is no view",line);
         return nullptr;
      }
      return static_cast<View *>(t);
   }

   AttrOrParam* find_attribute(Class* c,string name)
   {
      if (c == nullptr) {
         return nullptr;
      }
      Log.debug("find_attribute " + name + " in context " + get_path(c));
      for (auto a : c->ClassAttribute) {
         if (a->Name == name) {
            return a;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return find_attribute(s,name);
      }
      else {
         return nullptr;
      }
   }

   Role* find_role(Class* c,string name)
   {
      if (c == nullptr) {
         return nullptr;
      }
      Log.debug("find_role " + name + " in context " + get_path(c) + " " + to_string(c->_roleaccess.size()));
      for (auto r : c->Role) {
         if (r->Name == name) {
            return r;
         }
      }
      for (auto r : c->_roleaccess) {
         if (r->Name == name) {
            return r;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return find_role(s,name);
      }
      else {
         return nullptr;
      }
   }

   AttrOrParam* find_parameter(Class* c,string name,int line)
   {
      Log.debug("find_paramer " + name);
      if (c == nullptr) {
         return nullptr;
      }
      for (auto a : c->ClassParameter) {
         if (a->Name == name) {
            return a;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return find_parameter(s,name,line);
      }
      else {
         Log.error("parameter " + name + " not found", line);
         return nullptr;
      }
   }

   // Graphic helpers

   void init_graphic(Graphic* g, int line)
   {
      init_extendableme(g, line);
   }

   void add_graphic(Graphic* g)
   {
      if (g == nullptr) {
         return;
      }
      Log.debug(">>> add_graphic " + get_path(g));
      for (Graphic* gg : AllGraphics) {
         if (get_path(gg) == get_path(g)) {
            Log.error("multiple declarations of " + get_path(g),g->_line);
            return;
         }
      }
      Log.debug("<<< add_graphic " + get_path(g));
      AllGraphics.push_back(g);
   }

   Graphic* find_graphic(string name,int line)
   {

      string search;
      search = name;

      MetaElement *ctx = get_package_context();
         
      string package_path = get_path(get_package_context());
      Log.debug(">>> find_graphic <" + search + "> in context " + package_path);

      Graphic *found = nullptr;
      for (Graphic* g : AllGraphics) {
         string path = get_path(g);
         if (path == search) {
            found = g;
            break;
         }
         else if (path == package_path + "." + search) {
            found = g;
            break;
         }
         else if (ctx->getClass() == "Model") {
            for (auto unqualified : get_all_unqualified_imports(ctx->Name)) {
               if (path == unqualified + "." + search) {
                  found = g;
                  break;
               }
            }
         }
         else if (ctx->getClass() == "SubModel") {
            SubModel *s = static_cast<SubModel *>(ctx);
            while (s->_super != nullptr) {
               if (path == get_path(s->_super) + "." + search) {
                  found = g;
                  break;
               }
               s = static_cast<SubModel *>(s->_super);
            }
            if (path == get_parent_path(ctx) + "." + search) {
               found = g;
               break;
            }
            for (auto unqualified : get_all_unqualified_imports(get_parent_path(ctx))) {
               if (path == unqualified + "." + search) {
                  found = g;
                  break;
               }
            }
         }
      }
      
      Log.debug("<<< find_graphic " + get_path(found));
      
      return found;

   }

   // expression helpers

   void init_expression(Expression* e, int line)
   {
      init_mmobject(e, line);
      e->OccurrenceScope = get_context();
      e->OccurrencePackage = get_package_context();
   }

   void init_factor(Factor* f, int line)
   {
      init_expression(f, line);
   }

   // function helpers

   void init_function(FunctionDef* f, int line)
   {
      init_metaelement(f, line);
   }

   void add_function(FunctionDef* function)
   {
      if (function == nullptr) {
         return;
      }
      Log.debug("add_function " + function->Name + "(" + get_path(function) + ")");
      for (FunctionDef* f : AllFunctions) {
         if (f->Name == function->Name) {
            // add domain type only once
            return;
         }
      }
      AllFunctions.push_back(function);
   }

   FunctionDef* find_function(string name, int line)
   {

      Log.debug("find_function " + name);

      for (FunctionDef* f : AllFunctions) {
         if (get_path(f) == name) {
            return f;
         }
         else if (get_path(f) == get_parent_path(f) + "." + name) {
            return f;
         }
      }

      Log.error("function " + name + " not found", line);
      return nullptr;

   }

   // lineform helpers

   void init_lineform(LineForm* f, int line)
   {
      init_metaelement(f, line);
   }

   void add_lineform(LineForm* lineform)
   {
      if (lineform == nullptr) {
         return;
      }
      Log.debug("add_lineform " + lineform->Name + "(" + get_path(lineform) + ")");
      for (LineForm* f : AllLineForms) {
         if (f->Name == lineform->Name) {
            // add domain type only once
            return;
         }
      }
      AllLineForms.push_back(lineform);
   }

   LineForm* find_lineform(string name, int line)
   {

      Log.debug("find_lineform " + name);

      for (LineForm* f : AllLineForms) {
         if (get_path(f) == name) {
            return f;
         }
         else if (get_path(f) == get_parent_path(f) + "." + name) {
            return f;
         }
      }

      Log.error("lineform " + name + " not found.", line);
      return nullptr;

   }

   // constraint helpers

   void init_constraint(Constraint* c, int line)
   {
      init_metaelement(c, line);
   }

   // other helpers

   void debug(antlr4::ParserRuleContext *ctx, string message)
   {
      Log.debug(message + ", line=" + to_string(ctx->start->getLine()));
   }

   Type* any_to_type(antlrcpp::Any any)
   {

      Log.debug(">>> any_to_type()");
      Type* t;

      try {
         t = any.as<TypeRelatedType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<ClassRelatedType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<BooleanType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<TextType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<BlackboxType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<NumType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<CoordType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         AnyOIDType* t = any.as<AnyOIDType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         AttributeRefType* t = any.as<AttributeRefType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<EnumType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<EnumTreeValueType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<LineType*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<Class*>();
         Log.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<DomainType*>();
         Log.debug(">>> DomainType class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      Log.internal_error("any_to_type: unsupported type", 1);
      return nullptr;

   }

   DomainType* any_to_domaintype(antlrcpp::Any any)
   {
      Type* t = any_to_type(any);
      try {
         return static_cast<DomainType*>(t);
      }
      catch (exception e) {
         Log.internal_error("unable to cast " + t->getClass() + " to DomainType (" + e.what(), 1);
         return nullptr;
      }
   }

   int get_line(antlr4::ParserRuleContext *ctx)
   {
      return ctx->start->getLine();
   }

   int get_line(antlr4::Token *token)
   {
      return token->getLine();
   }
   
   int get_line(antlr4::tree::TerminalNode* node)
   {
      return node->getSymbol()->getLine();
   }
   
   static list <string> reserved_names = {
      "INTERLIS",
      "CONTRACTED",
      "REFSYSTEM",
      "SYMBOLOGY",
      "TYPE",
      "MODEL",
      "END",
      "VERSION",
      "NOINCREMENTALTRANSFER",
      "TRANSLATION",
      "OF",
      "AT",
      "CHARSET",
      "XMLNS",
      "IMPORTS",
      "UNQUALIFIED",
      "TOPIC",
      "DEPENDS",
      "ON",
      "AS",
      "VIEW",
      "EXTENDS",
      "BASKET",
      "OID",
      "BOOLEAN",
      "HALIGNMENT",
      "VALIGNMENT",
      "CLASS",
      "ABSTRACT",
      "EXTENDED",
      "FINAL",
      "TRANSIENT",
      "CONTINUOUS",
      "SUBDIVISION",
      "NO",
      "ATTRIBUTE",
      "TEXT",
      "DATE",
      "TIMEOFDAY",
      "DATETIME",
      "STRUCTURE",
      "PARAMETER",
      "MANDATORY",
      "GENERIC",
      "GENERICS",
      "DEFERRED",
      "CONTEXT",
      "MULTICOORD",
      "MULTIPOLYLINE",
      "MULTISURFACE",
      "MULTIAREA",
      "BAG",
      "LIST",
      "REFERENCE",
      "TO",
      "ANYCLASS",
      "RESTRICTION",
      "ASSOCIATION",
      "DERIVED",
      "EXTERNAL",
      "FROM",
      "UNDEFINED",
      "MTEXT",
      "NAME",
      "URI",
      "ALL",
      "CIRCULAR",
      "OTHERS",
      "NUMERIC",
      "CLOCKWISE",
      "COUNTERCLOCKWISE",
      "CARDINALITY",
      "OR",
      "HIDING",
      "ORDERED",
      "DOMAIN",
      "PI",
      "LNBASE",
      "FORMAT",
      "INHERIT",
      "COORD",
      "ROTATION",
      "ANY",
      "BLACKBOX",
      "XML",
      "BINARY",
      "DIRECTED",
      "POLYLINE",
      "SURFACE",
      "AREA",
      "WITH",
      "STRAIGHTS",
      "ARCS",
      "VERTEX",
      "WITHOUT",
      "OVERLAPS",
      "LINE",
      "ATTRIBUTES",
      "FORM",
      "UNIT",
      "FUNCTION",
      "SIGN",
      "OBJECTS",
      "METAOBJECT",
      "CONSTRAINT",
      "CONSTRAINTS",
      "EXISTENCE",
      "REQUIRED",
      "IN",
      "UNIQUE",
      "WHERE",
      "LOCAL",
      "SET",
      "AND",
      "NOT",
      "BASED",
      "BASE",
      "INHERITANCE",
      "DEFINED",
      "INSPECTION",
      "THIS",
      "THISAREA",
      "THATAREA",
      "PARENT",
      "FIRST",
      "LAST",
      "AGGREGATES",
      "OBJECT",
      "ENUMVAL",
      "ENUMTREEVAL",
      "PROJECTION",
      "JOIN",
      "NULL",
      "UNION",
      "AGGREGATION",
      "BY",
      "GRAPHIC",
      "ACCORDING",
      "WHEN",
      "ANYSTRUCTURE",
      "TRANSFER",
      "URL"
   };

   bool is_reserved_name(string name) 
   {
      for (auto n: reserved_names) {
         if (n == name) {
            return true;
         }
      }
      return false;
   }

}
