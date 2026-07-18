#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "InterlisModel.h"
#include "../parser/IliParserErrorListener.h"
#include "../parser/generated/Ili2Lexer.cpp"
#include "../parser/generated/Ili2Parser.cpp"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"
#include "../../util/StringUtil.h"

using namespace input;
using namespace parser;
using namespace metamodel;

string input_file = "";

// general methods

/*
antlrcpp::Any::Base::~Base()
{
   // to satisfy microsoft linker
}
*/

void input::parseIli2(string ilifile)
{

   try {
      
      string input;
      if (ilifile == "INTERLIS") {
         input = getInterlisModel23();
         input_file = "internal";
      }
      else {
         input = util::load_filtered_string_from_file(ilifile);
         input_file = ilifile;
      }

      antlr4::ANTLRInputStream inputstream(input);
      prepare_meta_attributes(input);
      int errors = Log.getErrorCount();

      Log.debug("creating ili2 lexer ...");
      lexer::Ili2Lexer ili2lexer(&inputstream);
      antlr4::CommonTokenStream tokens(&ili2lexer);

      Log.debug("creating ili2 parser ...");
      parser::Ili2Parser ili2parser(&tokens);
      ili2parser.removeErrorListeners();
      ili2parser.addErrorListener(new parser::IliParserErrorListener());
      parser::Ili2Parser::Interlis2DefContext *ili2d = ili2parser.interlis2Def();

      if (Log.getErrorCount() != errors) {
         Log.info("compiling aborted due to parsing errors");
         return;
      }

      Log.debug("ili2 building meta model ...");
      input::Ili2Input ili2input;
      ili2input.visit(ili2d);

   }
   catch (exception e) {
      Log.setLevel(1);
      Log.internal_error(string(e.what()),1);
   }
   
}

// parser visitor interface

antlrcpp::Any Ili2Input::visitInterlis2Def(Ili2Parser::Interlis2DefContext *ctx)
{

   /* interlis2Def
   : INTERLIS 
     ({ili23}? iliversion=ILI23 
     |{ili24}? iliversion=ILI24
     )
     SEMI (modelDef)* EOF
   */

   iliversion = ctx->iliversion->getText();
   if (iliversion == "2.3") {
      ili23 = true;
      ili24 = false;
   }
   if (iliversion == "2.4") {
      ili24 = true;
      ili23 = false;
   }
   debug(ctx,">>> visitInterlis2Def(" + iliversion + ")");
   Log.incNestLevel();
   visitChildren(ctx);
   Log.decNestLevel();
   debug(ctx,"<<< visitInterlis2Def(" + iliversion + ")");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitMetaDataBasketRef(parser::Ili2Parser::MetaDataBasketRefContext *ctx)
{
   debug(ctx,">>> visitMetaDataBasketRef()");
   // to do !!!
   debug(ctx,"<<< visitMetaDataBasketRef()");
   return nullptr;
}

namespace {

MetaBasketDef *find_meta_basket_in(Package *package,const string &name)
{
   if (package == nullptr) return nullptr;
   for (MetaElement *element : package->Element) {
      if (element->getClass() == "MetaBasketDef" && element->Name == name) {
         return static_cast<MetaBasketDef *>(element);
      }
      if (auto child = dynamic_cast<Package *>(element)) {
         if (MetaBasketDef *found = find_meta_basket_in(child,name)) return found;
      }
   }
   return nullptr;
}

MetaBasketDef *find_meta_basket(const string &path)
{
   string name = path;
   size_t dot = name.rfind('.');
   if (dot != string::npos) name = name.substr(dot + 1);
   if (MetaBasketDef *found = find_meta_basket_in(get_package_context(),name)) return found;
   for (Model *model : get_all_models()) {
      if (MetaBasketDef *found = find_meta_basket_in(model,name)) return found;
   }
   return nullptr;
}

Class *find_viewable_in(Package *package,const string &name)
{
   if (package == nullptr) return nullptr;
   for (MetaElement *element : package->Element) {
      if ((element->getClass() == "Class" || element->getClass() == "View") && element->Name == name) {
         return static_cast<Class *>(element);
      }
   }
   return nullptr;
}

}

antlrcpp::Any Ili2Input::visitMetaDataBasketDef(parser::Ili2Parser::MetaDataBasketDefContext *ctx)
{

   /* metaDataBasketDef
   : (SIGN | REFSYSTEM) BASKET basketname=NAME
     (LPAREN FINAL RPAREN)?
     (EXTENDS metaDataBasketRef)?
     TILDE path 
     (OBJECTS OF classname=NAME COLON metabjectname=NAME 
                         ( COMMA metaobjectname=NAME)*)* SEMI
   */

   debug(ctx,">>> visitMetaDataBasketDef()");
   MetaBasketDef *basket = new MetaBasketDef();
   init_extendableme(basket,get_line(ctx));
   basket->Name = ctx->basketname->getText();
   basket->Kind = ctx->SIGN() == nullptr ? MetaBasketDef::RefSystemB : MetaBasketDef::SignB;
   basket->Final = ctx->FINAL() != nullptr;
   basket->MetaDataTopic = find_dataunit(visitPath(ctx->path()),get_line(ctx->path()));
   if (ctx->metaDataBasketRef() != nullptr) {
      basket->Super = find_meta_basket(ctx->metaDataBasketRef()->getText());
      if (basket->Super == nullptr) {
         Log.error("metadata basket " + ctx->metaDataBasketRef()->getText() + " not found",get_line(ctx));
      }
   }

   bool afterTopic = false;
   bool expectClass = false;
   bool expectObject = false;
   Class *objectClass = nullptr;
   for (antlr4::tree::ParseTree *child : ctx->children) {
      if (child == ctx->path()) {
         afterTopic = true;
         continue;
      }
      if (!afterTopic) continue;
      string text = child->getText();
      if (text == "OBJECTS") {
         expectClass = true;
         expectObject = false;
         objectClass = nullptr;
         continue;
      }
      if (text == "OF" || text == ",") continue;
      if (text == ":") {
         expectObject = true;
         continue;
      }
      auto terminal = dynamic_cast<antlr4::tree::TerminalNode *>(child);
      if (terminal == nullptr || terminal->getSymbol()->getType() != parser::Ili2Parser::NAME) continue;
      if (expectClass) {
         Package *objectPackage = basket->MetaDataTopic == nullptr
            ? get_package_context() : basket->MetaDataTopic->ElementInPackage;
         objectClass = find_viewable_in(objectPackage,text);
         if (objectClass == nullptr) Log.error("viewable " + text + " not found",get_line(terminal));
         expectClass = false;
      }
      else if (expectObject) {
         MetaObjectDef *object = new MetaObjectDef();
         init_mmobject(object,get_line(terminal));
         object->Name = text;
         object->Class = objectClass;
         object->MetaBasketDef.push_back(basket);
         basket->Members.push_back(object);
      }
   }
   debug(ctx,"<<< visitMetaDataBasketDef()");

   return basket;

}

antlrcpp::Any Ili2Input::visitRunTimeParameterDef(parser::Ili2Parser::RunTimeParameterDefContext *ctx)
{

   /* runTimeParameterDef
   : PARAMETER runTimeParameter*
   */

   /* runTimeParameter
   : runtimeparametername=NAME COLON attrTypeDef SEMI
   */

   debug(ctx,">>> visitRunTimeParameterDef()");

   for (auto p : ctx->runTimeParameter()) {
      AttrOrParam *a = new AttrOrParam;
      a->_line = get_line(p->runtimeparametername);
      a->Name = p->runtimeparametername->getText();
      a->Type = visitAttrTypeDef(p->attrTypeDef());
      get_model_context()->_runtimeparameter.push_back(a);
   }

   debug(ctx,"<<< visitRunTimeParameterDef()");

   return nullptr;

}
