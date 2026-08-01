#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "InterlisModel.h"
#include "../parser/IliParserErrorListener.h"
#include "../parser/generated/Ili2Lexer.cpp"
#include "../parser/generated/Ili2Parser.cpp"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"
#include "../../util/StringUtil.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

// general methods

/*
antlrcpp::Any::Base::~Base()
{
   // to satisfy microsoft linker
}
*/

void input::parseIli2(const ilic::SourceBuffer &source,
   metamodel::MetaModelBuilder &builder,util::Logger &logger)
{

   try {
      
      auto sourceScope = builder.enterSource(source);
      auto categoryScope = logger.categoryScope("parser");

      antlr4::ANTLRInputStream inputstream(source.text);
      int errors = logger.getErrorCount();

      logger.debug("creating ili2 lexer ...");
      lexer::Ili2Lexer ili2lexer(&inputstream);
      antlr4::CommonTokenStream tokens(&ili2lexer);

      logger.debug("creating ili2 parser ...");
      parser::IliParserErrorListener errorListener(logger);
      parser::Ili2Parser ili2parser(&tokens);
      ili2parser.removeErrorListeners();
      ili2parser.addErrorListener(&errorListener);
      parser::Ili2Parser::Interlis2DefContext *ili2d = ili2parser.interlis2Def();

      if (logger.getErrorCount() != errors) {
         logger.info("compiling aborted due to parsing errors");
         return;
      }

      input::visitIli2(source,ili2d,builder,logger);

   }
   catch (const exception &e) {
      logger.setLevel(1);
      logger.internal_error(string(e.what()),1);
   }
   
}

void input::visitIli2(const ilic::SourceBuffer &source,
   parser::Ili2Parser::Interlis2DefContext *root,
   metamodel::MetaModelBuilder &builder,util::Logger &logger)
{
   if (root == nullptr) return;
   auto sourceScope = builder.enterSource(source);
   auto categoryScope = logger.categoryScope("parser");
   logger.debug("ili2 building meta model ...");
   input::Ili2Input ili2input(builder,logger);
   ili2input.visit(root);
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

   const string iliversion = ctx->iliversion->getText();
   builder_.setLanguageVersion(iliversion == "2.4" ?
      IliLanguageVersion::Ili24 : IliLanguageVersion::Ili23);
   builder_.debug(ctx,">>> visitInterlis2Def(" + iliversion + ")");
   logger_.incNestLevel();
   visitChildren(ctx);
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitInterlis2Def(" + iliversion + ")");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitMetaDataBasketRef(parser::Ili2Parser::MetaDataBasketRefContext *ctx)
{
   builder_.debug(ctx,">>> visitMetaDataBasketRef()");
   // to do !!!
   builder_.debug(ctx,"<<< visitMetaDataBasketRef()");
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

MetaBasketDef *find_meta_basket(MetaModelBuilder &builder,const string &path)
{
   string name = path;
   size_t dot = name.rfind('.');
   if (dot != string::npos) name = name.substr(dot + 1);
   if (MetaBasketDef *found = find_meta_basket_in(builder.currentPackage(),name)) return found;
   for (Model *model : builder.store().models()) {
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

   builder_.debug(ctx,">>> visitMetaDataBasketDef()");
   MetaBasketDef *basket = builder_.store().make<MetaBasketDef>();
   builder_.initExtendable(basket,builder_.line(ctx));
   builder_.setSelectionSource(basket,ctx->basketname);
   basket->Name = ctx->basketname->getText();
   basket->Kind = ctx->SIGN() == nullptr ? MetaBasketDef::RefSystemB : MetaBasketDef::SignB;
   basket->Final = ctx->FINAL() != nullptr;
   basket->MetaDataTopic = builder_.findDataUnit(visitPath(ctx->path()),builder_.line(ctx->path()));
   if (ctx->metaDataBasketRef() != nullptr) {
      basket->Super = find_meta_basket(builder_,ctx->metaDataBasketRef()->getText());
      if (basket->Super == nullptr) {
         logger_.error(DiagnosticId::MetadataBasketNotFound,
            "metadata basket " + ctx->metaDataBasketRef()->getText() + " not found",
            builder_.line(ctx));
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
            ? builder_.currentPackage() : basket->MetaDataTopic->ElementInPackage;
         objectClass = find_viewable_in(objectPackage,text);
         if (objectClass == nullptr) {
            logger_.error(DiagnosticId::NameViewableNotFound,
               "viewable " + text + " not found",builder_.line(terminal));
         }
         expectClass = false;
      }
      else if (expectObject) {
         MetaObjectDef *object = builder_.store().make<MetaObjectDef>();
         builder_.initObject(object,builder_.line(terminal));
         object->Name = text;
         object->Class = objectClass;
         object->MetaBasketDef.push_back(basket);
         basket->Members.push_back(object);
      }
   }
   builder_.debug(ctx,"<<< visitMetaDataBasketDef()");

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

   builder_.debug(ctx,">>> visitRunTimeParameterDef()");

   for (auto p : ctx->runTimeParameter()) {
      AttrOrParam *a = builder_.store().make<AttrOrParam>();
      builder_.initObject(a,builder_.line(p->runtimeparametername));
      builder_.setSelectionSource(a,p->runtimeparametername);
      a->Name = p->runtimeparametername->getText();
      a->ElementInPackage = builder_.currentModel();
      builder_.pushContext(*a);
      a->Type = visitAttrTypeDef(p->attrTypeDef());
      builder_.popContext();
      if (a->Type != nullptr) {
         a->Type->_attr = a;
      }
      builder_.currentModel()->_runtimeparameter.push_back(a);
   }

   builder_.debug(ctx,"<<< visitRunTimeParameterDef()");

   return nullptr;

}
