#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

extern string input_file;

antlrcpp::Any Ili2Input::visitModelDef(Ili2Parser::ModelDefContext *ctx)
{

   /* modelDef
   : (contracted=CONTRACTED)? 
     (refsystemmodel=REFSYSTEM | symbologymodel=SYMBOLOGY | typemodel=TYPE)? 
     MODEL modelname1=NAME (LPAREN language=NAME RPAREN)?
     {ili24}? (noincrementaltransfer=NOINCREMENTALTRANSFER)?
     ATT issuerurl=string 
     VERSION modelversion=STRING (modelversion_expl=EXPLANATION)?
     (TRANSLATION OF translationOf=NAME LBRACE translationOfVersion=STRING RBRACE)?
     EQUAL
     {ili24}? (CHARSET iananame=STRING SEMI)?
     {ili24}? (XMLNS xmlns=STRING SEMI)?      
     (importDef)*
     (metaDataBasketDef
     |unitDecl
     |functionDef
     |lineFormTypeDef
     |domainDef
     |runTimeParameterDef
     |classDef
     |structureDef
     |topicDef 
     )*
     END modelname2=NAME DOT
   */
   
   string name1 = ctx->modelname1->getText();
   if (name1 == "ILIC_INTERLIS") {
      name1 = "INTERLIS";
   }
   string name2 = ctx->modelname2->getText();
   if (name2 == "ILIC_INTERLIS") {
      name2 = "INTERLIS";
   }

   debug(ctx,">>> visitModelDef(" + name1 + ")");
   Log.incNestLevel();
   if (name1 != name2) {
      Log.error(
         "modelname " + name2 + " must match " + name1,
         ctx->modelname2->getLine()
      );
   }
   
   Model *m = new Model();
   init_package(m,get_line(ctx));

   // Model Attributes
   m->Name = name1;
   m->iliVersion = iliversion;
   if (ctx->contracted != nullptr) {
      m->Contracted = true;
   }
   else {
      m->Contracted = false;
   }
   if (ctx->typemodel != nullptr) {
      m->Kind = Model::TypeM;
   }
   else if (ctx->refsystemmodel != nullptr) {
      m->Kind = Model::RefSystemM;
   }
   else if (ctx->symbologymodel != nullptr) {
      m->Kind = Model::SymbologyM;
   }
   else {
      m->Kind = Model::NormalM;
   }
   if (ctx->language != nullptr) {
      m->Language = ctx->language->getText();
   }
   m->At = visitString(ctx->issuerurl);
   m->Version = visitString(ctx->modelversion);
   if (ctx->modelversion_expl != nullptr) { // nullptr test required, because modelversion_expl is optional
      m->VersionExplanation = ctx->modelversion_expl->getText();
   }

   if (ctx->translationOf != nullptr) {
      m->_translationOfName = ctx->translationOf->getText();
      m->_translationOfVersion = visitString(ctx->translationOfVersion);
   }

   if (ili24) {
      if (ctx->NOINCREMENTALTRANSFER() != nullptr) {
         m->NoIncrementalTransfer = true; // 2.4
      }
      if (ctx->iananame != nullptr) {
         m->CharSetIANAName = ctx->iananame->getText(); // 2.4
      }
      if (ctx->xmlns != nullptr) {
         m->xmlns = ctx->xmlns->getText(); // 2.4
         debug(ctx,"xmlns is " + m->xmlns);
      }
   }

   m->_ilifile = input_file;
   add_model(m);
   add_package(m);
   push_context(m);

   visitChildren(ctx);

/*
   for (auto u : ctx->unitDecl()) {
      visitUnitDecl(u);
   }
   for (auto s : ctx->structureDef()) {
      visitStructureDef(s);
   }
   for (auto c : ctx->classDef()) {
      visitClassDef(c);
   }
   for (auto f : ctx->functionDef()) {
      visitFunctionDef(f);
   }
   for (auto d : ctx->domainDef()) {
      visitDomainDef(d);
   }
   for (auto t : ctx->topicDef()) {
      visitTopicDef(t);
   }
*/

   pop_context();
   Log.decNestLevel();
   debug(ctx,"<<< visitModelDef(" + ctx->modelname1->getText() + ")");

   return nullptr;

}

antlrcpp::Any Ili2Input::visitImporting(parser::Ili2Parser::ImportingContext *ctx)
{

   /* importDef
   : IMPORTS importing (COMMA importing)* SEMI

   importing
   : UNQUALIFIED? (INTERLIS | NAME)
   */

   debug(ctx,"visitImportDef()");
   Import *i = new Import();
   i->ImportingP = get_package_context();

   if (ctx->INTERLIS() != nullptr) {
      i->ImportedP = find_model(ctx->INTERLIS()->getText(), get_line(ctx));
   }
   else {
      i->ImportedP = find_model(ctx->NAME()->getText(), get_line(ctx));
   }
   if (ctx->UNQUALIFIED() != nullptr) {
      i->_unqualified = true;
   }
   add_import(i);

   return nullptr;

}

antlrcpp::Any Ili2Input::visitTopicDef(Ili2Parser::TopicDefContext *ctx)
{

   /* topicDef
   : VIEW? TOPIC topicname1=NAME
     properties? // ABSTRACT|FINAL
     (EXTENDS topicbase=path)?
     EQUAL
     (BASKET OID AS basketOid=path SEMI)?
     (OID AS topicOid=path SEMI)?
     (DEPENDS ON topicPath (COMMA topicPath)* SEMI)*
     (metaDataBasketDef
     |unitDecl
     |functionDef
     |domainDef
     |classDef
     |structureDef
     |associationDef
     |constraintsDef
     |viewDef
     |graphicDef
     )*
     END topicname2=NAME SEMI
   */
   
   /* class Model : public Package {
   public:
      string iliVersion;
      bool Contracted = false;
      enum { NormalM, TypeM, RefSystemM, SymbologyM } Kind;
      string Language = "en";
      string At = "";
      string Version = "";
      string VersionExplanation = "";
      bool NoIncrementalTransfer = true; // 2.4
      string CharSetIANAName = ""; // 2.4
      string xmlns = ""; // 2.4
      string ili1Transfername = "";
      Ili1Format *ili1Format = nullptr;
      string _ilifile = "internal";
      virtual string getClass() { return "Model"; }
      virtual string getBaseClass() { return "Package"; };
   */

   /* class SubModel : public Package {
      // MetaElement.Name := TopicName as defined in the INTERLIS-Model
   public:
   */

   /* class DataUnit : public ExtendableME {
   public:
      // Name (EXTENDED): TEXT := "BASKET";
      bool ViewUnit = false;
      string DataUnitName;
      // role from ASSOCIATION MetaDataUnit 
      list <MetaBasketDef *> MetaBasketDef;
      // role from ASSOCIATION BasketOID
      DomainType *Oid = nullptr; // RESTRICTION(TextType; NumType; AnyOIDType);
   */

   string name1 = ctx->topicname1->getText();
   string name2 = ctx->topicname2->getText();

   debug(ctx,">>> visitTopicDef(" + name1 + ")");
   Log.incNestLevel();

   if (name1 != name2) {
      Log.error(
         "topicname " + name2 + " must match " + name1,
         get_line(ctx->topicname2)
      );
   }

   // init topic
   SubModel *s = new SubModel();
   init_package(s,get_line(ctx->topicname1));
   s->Name = name1;
   add_package(s);
   push_context(s);

   // init dataunit
   DataUnit *d = new DataUnit();
   init_extendableme(d,get_line(ctx->topicname1));
   d->Name = "BASKET";
   add_dataunit(d);
   s->_dataunit = d;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL}));
   d->Abstract = properties[ABSTRACT];
   d->Final = properties[FINAL];

   if (ctx->topicbase != nullptr) {
      SubModel *ss = find_topic(visitPath(ctx->topicbase), get_line(ctx->topicbase));
      s->_super = ss;
      if (ss != nullptr) {
         if (ss->_dataunit->Final) {
            Log.error("topic " + name1 + " can not extend FINAL topic " + get_path(s->_super),s->_line);
         }
         d->Super = find_dataunit(get_path(ss),get_line(ctx->topicbase));
         if (d->Super != nullptr) {
            s->_super->_sub.push_back(s);
            d->Super->Sub.push_back(d);
         }
      }
   }

   // DataUnit attributes
   if (ctx->VIEW() != nullptr) {
      d->ViewUnit = true;
   }
   // role from ASSOCIATION MetaDataUnit, to do !!!
   if (ctx->basketOid != nullptr) {
      d->Oid = find_domaintype(ctx->basketOid->getText(),get_line(ctx->basketOid));
   }
   if (ctx->topicOid != nullptr) {
      d->TopicOid = find_domaintype(ctx->topicOid->getText(),get_line(ctx->topicOid));
   }

   if (ctx->DEPENDS().size() > 0) {
      // (DEPENDS ON topicPath (COMMA topicPath)* SEMI)*
      /* class Dependency : public MMObject { // m:n ASSOCIATION
         DataUnit *Using = nullptr;
         DataUnit *Dependent = nullptr;
      */
      for (auto p : ctx->topicPath()) {
         string path = visitPath(p->path());
         DataUnit *du = find_dataunit(path,get_line(ctx));
         if (du != nullptr) {
            Dependency *dd = new Dependency();
            init_mmobject(dd,p->start->getLine());
            dd->Using = d;
            dd->Dependent = du;
            add_dependency(dd);
         }
      }
   }

   if (ctx->deferredGenerics() != nullptr) {
      for (auto path : ctx->deferredGenerics()->path()) {
         string name = visitPath(path);
         s->DeferredGenerics.push_back({name,find_domaintype(name,get_line(path)),get_line(path)});
      }
   }

   // metaDataBasketDef
   // unitDecl
   // functionDef
   // domainDef
   // classDef
   // structureDef
   // associationDef
   // constraintsDef
   // viewDef
   // graphicDef
   visitChildren(ctx);
   pop_context();
   
   Log.decNestLevel();
   debug(ctx,"<<< visitTopicDef(" + name1 + ")");

   return nullptr;

};
