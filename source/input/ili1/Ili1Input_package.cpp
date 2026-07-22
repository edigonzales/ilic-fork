#pragma once

#include "Ili1Input.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

extern string input_file;

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

   debug(ctx,">>> visitModelDef(" + modelname1 + ")");
   Log.incNestLevel();

   if (modelname1 != modelname2) {
      Log.error(
         "modelname " + modelname2 + " must match " + modelname1,
         ctx->modelname2->getLine()
      );
   }

   Model *m = new Model();
   init_package(m,ctx->modelname1->getLine());
   set_selection_source(m,ctx->modelname1);
   set_end_selection_source(m,ctx->modelname2);
   m->_ilifile = input_file;

   // Model Attributes
   m->Name = ctx->modelname1->getText();
   m->iliVersion = "1.0";
   m->Version = "1.0";
   m->At = "http://www.interlis.ch/ilic";
   m->Contracted = false;
   m->Kind = Model::NormalM;
   m->Language = "de";

   push_context(m);
   add_model(m);
   
   // Units
   Unit *u = nullptr;
   u = static_cast<Unit *>(find_unit("INTERLIS.m2",get_line(ctx))->clone());
   m->Element.push_back(u);
   u->ElementInPackage = m;
   add_unit(u);
   u = static_cast<Unit *>(find_unit("INTERLIS.grd",get_line(ctx))->clone());
   m->Element.push_back(u);
   u->ElementInPackage = m;
   add_unit(u);
   u = static_cast<Unit *>(find_unit("INTERLIS.dgr",get_line(ctx))->clone());
   m->Element.push_back(u);
   u->ElementInPackage = m;
   add_unit(u);

   if (ctx->domainDefs() != nullptr) {
      visitDomainDefs(ctx->domainDefs());
   }
   
   for (auto tctx : ctx->topicDef()) {
      visitTopicDef(tctx);
   }
   
   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitModelDef(" + modelname1 + ")");
      
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

   debug(ctx,">>> visitTopicDef(" + name1 + ")");
   Log.incNestLevel();

   if (name1 != name2) {
      Log.error(
         "topicname " + name2 + " must match " + name1,
         ctx->topicname2->getLine()
      );
   }

   SubModel *s = new SubModel();

   // SubModel Attributes
   s->Name = name1;
   init_package(s,get_line(ctx));
   set_selection_source(s,ctx->topicname1);
   set_end_selection_source(s,ctx->topicname2);

   push_context(s);

   for (auto dctx : ctx->domainDefs()) {      
      visitDomainDefs(dctx);
   }
   
   for (auto tctx : ctx->tableDef()) {
      visitTableDef(tctx);
   }

   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitTopicDef(" + name1 + ")");

   return s;

};
