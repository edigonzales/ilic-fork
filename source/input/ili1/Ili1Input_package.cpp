#include "Ili1Input.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili1Input::visitModelDef(Ili1Parser::ModelDefContext *ctx)
{
   
   /*
   class Model : public Package {
   public:
      string ili Version;
      bool Contracted = false;
      enum { NormalM, TypeM, RefSystemM, SymbologyM } Kind;
      string Language;
      string At;
      string Version;
      string VersionExplanation;
      bool NoIncrementalTransfer = true; // 2.4
      string CharSetIANAName; // 2.4
      string xmlns; // 2.4
      string ili1Transfername;
      Ili1Format *ili1Format;
   };
   */

   /* modelDef
   : MODEL modelname1=NAME
     domainDefs?
     topicDef+
     END modelname2=NAME DOT
   */

   string modelname1 = ctx->modelname1->getText();
   string modelname2 = ctx->modelname2->getText();

   builder_.debug(ctx,">>> visitModelDef(" + modelname1 + ")");
   logger_.incNestLevel();

   if (modelname1 != modelname2) {
      logger_.error(
         "modelname " + modelname2 + " must match " + modelname1,
         ctx->modelname2->getLine()
      );
   }

   Model *m = builder_.store().make<Model>();
   builder_.initPackage(m,ctx->modelname1->getLine());
   builder_.setSelectionSource(m,ctx->modelname1);
   builder_.setEndSelectionSource(m,ctx->modelname2);
   m->_ilifile = builder_.currentSourceUri();

   // Model Attributes
   m->Name = ctx->modelname1->getText();
   m->iliVersion = "1.0";
   m->Version = "1.0";
   m->At = "http://www.interlis.ch/ilic";
   m->Contracted = false;
   m->Kind = Model::NormalM;
   m->Language = "de";

   builder_.pushContext(*m);
   builder_.addModel(m);
   
   // Units
   Unit *u = nullptr;
   u = static_cast<Unit *>(builder_.clone(*builder_.findUnit("INTERLIS.m2",builder_.line(ctx))));
   m->Element.push_back(u);
   u->ElementInPackage = m;
   builder_.addUnit(u);
   u = static_cast<Unit *>(builder_.clone(*builder_.findUnit("INTERLIS.grd",builder_.line(ctx))));
   m->Element.push_back(u);
   u->ElementInPackage = m;
   builder_.addUnit(u);
   u = static_cast<Unit *>(builder_.clone(*builder_.findUnit("INTERLIS.dgr",builder_.line(ctx))));
   m->Element.push_back(u);
   u->ElementInPackage = m;
   builder_.addUnit(u);

   if (ctx->domainDefs() != nullptr) {
      visitDomainDefs(ctx->domainDefs());
   }
   
   for (auto tctx : ctx->topicDef()) {
      visitTopicDef(tctx);
   }
   
   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitModelDef(" + modelname1 + ")");
      
   return m;

}

antlrcpp::Any Ili1Input::visitTopicDef(Ili1Parser::TopicDefContext *ctx)
{

   /* topicDef
   : TOPIC topicname1=NAME EQUAL
     (tableDef | domainDefs)+
     END topicname2=NAME
     DOT
   */

   string name1 = ctx->topicname1->getText();
   string name2 = ctx->topicname2->getText();

   builder_.debug(ctx,">>> visitTopicDef(" + name1 + ")");
   logger_.incNestLevel();

   if (name1 != name2) {
      logger_.error(
         "topicname " + name2 + " must match " + name1,
         ctx->topicname2->getLine()
      );
   }

   SubModel *s = builder_.store().make<SubModel>();

   // SubModel Attributes
   s->Name = name1;
   builder_.initPackage(s,builder_.line(ctx));
   builder_.setSelectionSource(s,ctx->topicname1);
   builder_.setEndSelectionSource(s,ctx->topicname2);

   builder_.pushContext(*s);

   for (auto dctx : ctx->domainDefs()) {      
      visitDomainDefs(dctx);
   }
   
   for (auto tctx : ctx->tableDef()) {
      visitTableDef(tctx);
   }

   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitTopicDef(" + name1 + ")");

   return s;

};
