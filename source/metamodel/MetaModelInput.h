#pragma once

#include <string>
#include <vector>
#include <map>
#include "antlr4-runtime.h"
#include "MetaModel.h"

using namespace std;

namespace metamodel {

   // helper functions for interlis 1 / 2 input modules
   extern bool ili23;
   extern bool ili24;
   extern string iliversion;
   void reset_input_state();

   // mmobject helpers
   void init_mmobject(MMObject *o, int line);
   void set_selection_source(MetaElement *element,antlr4::Token *token);
   void set_end_selection_source(MetaElement *element,antlr4::Token *token);
   void set_reference_source(MMObject *object,const string &kind,antlr4::Token *token);
   void set_reference_source(MMObject *object,const string &kind,
      antlr4::ParserRuleContext *context);
   
   // metaelement helpers
   void init_metaelement(MetaElement *e, int line);
   void prepare_meta_attributes(const string &source);
   void init_extendableme(ExtendableME *e, int line);

   // package helpers
   void init_package(Package *p, int line);

   // Model helpers
   Model *find_model(string name,int line);

   // Topic / DataUnit helpers
   DataUnit* find_dataunit(string name,int line);
   void add_package(Package* p);
   Package* find_package(string name,int line);
   SubModel* find_topic(string name,int line);

   // Unit helpers
   void add_unit(Unit *u);
   Unit* find_unit(string name,int line);
   
   // Type helpers
   void init_type(Type *t, int line);
   void add_type(Type *t);
   Type* find_type(string name,int line);
   string get_type_string(Type *t);
   void init_domaintype(DomainType *t, int line);
   DomainType* find_domaintype(string name,int line);
   
   // Class helpers
   void init_class(Class *c,int line);
   void add_class(Class *c);
   Class* find_class(string name,int line);
   Class* find_class_type(string name,int line);
   Class* find_class(Package *p,string name,int line);
   Class* find_class_or_view(string name, int line);
   Class* find_class_or_structure(string name, int line);
   Class* find_structure(string name,int line );
   Class* find_structure(Package *p,string name,int line);
   View* find_view(string name, int line);
   Class* find_association(string name,int line);
   Class* find_association(Package *p,string name,int line);
   AttrOrParam* find_attribute(Class *c,string name);
   Role* find_role(Class *c,string name);
   AttrOrParam* find_parameter(Class *c,string name,int line);
   
   // Graphic helpers
   void init_graphic(Graphic *g,int line);
   void add_graphic(Graphic *g);
   Graphic* find_graphic(string name,int line);

   // Function helpers
   void init_function(FunctionDef *f, int line);
   void add_function(FunctionDef *function);
   FunctionDef* find_function(string name,int line);

   // LineForm helpers
   void init_lineform(LineForm *f, int line);
   void add_lineform(LineForm *lineform);
   LineForm* find_lineform(string name,int line);

   // Expression helpers
   void init_expression(Expression *e, int line);
   void init_factor(Factor *f, int line);
   
   // Constraint helpers
   void init_constraint(Constraint *c, int line);
   
   // other helpers
   void debug(antlr4::ParserRuleContext *ctx, string message);
   Type* any_to_type(antlrcpp::Any any);
   DomainType* any_to_domaintype(antlrcpp::Any any);
   int get_line(antlr4::ParserRuleContext *ctx);
   int get_line(antlr4::Token *token);
   int get_line(antlr4::tree::TerminalNode* node);
   bool is_reserved_name(string name);

}
