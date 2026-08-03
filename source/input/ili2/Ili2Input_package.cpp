#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/DiagnosticUtil.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

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

   builder_.debug(ctx,">>> visitModelDef(" + name1 + ")");
   logger_.incNestLevel();
   if (name1 != name2) {
      logger_.error(DiagnosticId::NameEndMismatch,
         "modelname " + name2 + " must match " + name1,
         ctx->modelname2->getLine()
      );
   }
   
   Model *m = builder_.store().make<Model>();
   builder_.initPackage(m,builder_.line(ctx));
   builder_.setSelectionSource(m,ctx->modelname1);
   builder_.setEndSelectionSource(m,ctx->modelname2);

   // Model Attributes
   m->Name = name1;
   m->iliVersion = builder_.languageVersionName();
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

   if (builder_.isIli24()) {
      if (ctx->NOINCREMENTALTRANSFER() != nullptr) {
         m->NoIncrementalTransfer = true; // 2.4
      }
      if (ctx->iananame != nullptr) {
         m->CharSetIANAName = ctx->iananame->getText(); // 2.4
      }
      if (ctx->xmlns != nullptr) {
         m->xmlns = ctx->xmlns->getText(); // 2.4
         builder_.debug(ctx,"xmlns is " + m->xmlns);
      }
   }

   m->_ilifile = builder_.currentSourceUri();
   builder_.addModel(m);
   builder_.addPackage(m);
   builder_.pushContext(*m);

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

   builder_.popContext();
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitModelDef(" + ctx->modelname1->getText() + ")");

   return nullptr;

}

antlrcpp::Any Ili2Input::visitImporting(parser::Ili2Parser::ImportingContext *ctx)
{

   /* importDef
   : IMPORTS importing (COMMA importing)* SEMI

   importing
   : UNQUALIFIED? (INTERLIS | NAME)
   */

   builder_.debug(ctx,"visitImportDef()");
   Import *i = builder_.store().make<Import>();
   i->ImportingP = builder_.currentPackage();

   if (ctx->INTERLIS() != nullptr) {
      i->ImportedP = builder_.findModel(ctx->INTERLIS()->getText(), builder_.line(ctx));
   }
   else {
      i->ImportedP = builder_.findModel(ctx->NAME()->getText(), builder_.line(ctx));
   }
   if (ctx->UNQUALIFIED() != nullptr) {
      i->_unqualified = true;
   }
   builder_.addImport(i);

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

   builder_.debug(ctx,">>> visitTopicDef(" + name1 + ")");
   logger_.incNestLevel();

   if (name1 != name2) {
      logger_.error(DiagnosticId::NameEndMismatch,
         "topicname " + name2 + " must match " + name1,
         builder_.line(ctx->topicname2)
      );
   }

   // init topic
   SubModel *s = builder_.store().make<SubModel>();
   builder_.initPackage(s,builder_.line(ctx->topicname1));
   builder_.setSelectionSource(s,ctx->topicname1);
   builder_.setEndSelectionSource(s,ctx->topicname2);
   s->Name = name1;
   builder_.addPackage(s);
   builder_.pushContext(*s);

   // init dataunit
   DataUnit *d = builder_.store().make<DataUnit>();
   builder_.initExtendable(d,builder_.line(ctx->topicname1));
   d->Name = "BASKET";
   builder_.addDataUnit(d);
   s->_dataunit = d;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,FINAL}));
   d->Abstract = properties[ABSTRACT];
   d->Final = properties[FINAL];

   if (ctx->topicbase != nullptr) {
      builder_.setReferenceSource(s,"inheritance",ctx->topicbase);
      SubModel *ss = builder_.findTopic(visitPath(ctx->topicbase), builder_.line(ctx->topicbase));
      s->_super = ss;
      if (ss != nullptr) {
         if (ss->_dataunit->Final) {
            logger_.error(DiagnosticId::InheritanceFinalBase,
               "topic " + name1 + " can not extend FINAL topic " +
                  get_path(s->_super),diagnostic_range(s));
         }
         d->Super = builder_.findDataUnit(get_path(ss),builder_.line(ctx->topicbase));
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
      d->Oid = builder_.findDomainType(ctx->basketOid->getText(),builder_.line(ctx->basketOid));
   }
   if (ctx->topicOid != nullptr) {
      d->TopicOid = builder_.findDomainType(ctx->topicOid->getText(),builder_.line(ctx->topicOid));
   }

   if (ctx->DEPENDS().size() > 0) {
      // (DEPENDS ON topicPath (COMMA topicPath)* SEMI)*
      /* class Dependency : public MMObject { // m:n ASSOCIATION
         DataUnit *Using = nullptr;
         DataUnit *Dependent = nullptr;
      */
      for (auto p : ctx->topicPath()) {
         string path = visitPath(p->path());
         DataUnit *du = builder_.findDataUnit(path,builder_.line(ctx));
         if (du != nullptr) {
            Dependency *dd = builder_.store().make<Dependency>();
            builder_.initObject(dd,p->start->getLine());
            dd->Using = d;
            dd->Dependent = du;
            builder_.addDependency(dd);
         }
      }
   }

   if (ctx->deferredGenerics() != nullptr) {
      for (auto path : ctx->deferredGenerics()->path()) {
         string name = visitPath(path);
         s->DeferredGenerics.push_back({name,builder_.findDomainType(name,builder_.line(path)),builder_.line(path)});
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
   builder_.popContext();
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitTopicDef(" + name1 + ")");

   return nullptr;

};
