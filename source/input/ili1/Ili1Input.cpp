#include "Ili1Input.h"
#include "../parser/IliParserErrorListener.h"
#include "../parser/generated/Ili1Lexer.cpp"
#include "../parser/generated/Ili1Parser.cpp"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"
#include "../../util/StringUtil.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili1Input::visitDecimal(Ili1Parser::DecimalContext *ctx)
{

   /* decimal
	: DEC
	| POSNUMBER
	| NUMBER
	*/
   
   debug(ctx,">>> visitDecimal()");
   Log.incNestLevel();
   
   string value;

   if (ctx->DEC() != nullptr) {
      value = ctx->DEC()->getText();
   }
   else if (ctx->POSNUMBER() != nullptr) {
      value = ctx->POSNUMBER()->getText();
   }
   else {
      value = ctx->NUMBER()->getText();
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitDecimal(" + value + ")");

   return value;

}

extern string input_file;

void input::parseIli1(string input)
{

   antlr4::ANTLRInputStream inputstream(util::load_filtered_string_from_file(input));
   input_file = input;
   Log.setCurrentSource(input);
   Log.setCategory("parser");
   int errors = Log.getErrorCount();

   Log.debug("creating ili1 lexer ...");
   lexer::Ili1Lexer ili1lexer(&inputstream);
   antlr4::CommonTokenStream tokens(&ili1lexer);

   Log.debug("creating ili1 parser ...");
   parser::Ili1Parser ili1parser(&tokens);
   ili1parser.removeErrorListeners();
   ili1parser.addErrorListener(new parser::IliParserErrorListener());
   parser::Ili1Parser::Interlis1DefContext *ili1d = ili1parser.interlis1Def();
   
   if (Log.getErrorCount() != errors) {
      Log.info("compiling aborted due to parsing errors");
      return;
   }

   Log.debug("ili1 building meta model ...");
   input::Ili1Input ili1input;
   ili1input.visit(ili1d);

}

antlrcpp::Any Ili1Input::visitInterlis1Def(Ili1Parser::Interlis1DefContext *ctx)
{

   /*
   interlis1Def
   : TRANSFER transfername=NAME SEMI
     domainDefs?
     derivatives?
     modelDef
     view*
     formatEncoding
     EOF
   ;
   */
  
   debug(ctx,">>> visitInterlis1Def()");
   Log.incNestLevel();

   if (ctx->domainDefs() != nullptr) {      
      visitDomainDefs(ctx->domainDefs());
   }

   if (ctx->derivatives() != nullptr) {      
      visitDerivatives(ctx->derivatives());
   }

   Model *m = visitModelDef(ctx->modelDef());
   m->ili1Format = visitFormatEncoding(ctx->formatEncoding());
   m->ili1Transfername = ctx->transfername->getText();
   
   Log.decNestLevel();
   debug(ctx,"<<< visitInterlis1Def()");

   return nullptr;
  
}

antlrcpp::Any Ili1Input::visitDerivatives(Ili1Parser::DerivativesContext *ctx)
{

   /* derivatives
   : DERIVATIVES derivativename1=NAME
     domainDefs?
     topicDef+
     END derivativename2=NAME
   */

   string name1 = ctx->derivativename1->getText();
   string name2 = ctx->derivativename2->getText();
   debug(ctx,">>> visitDerivatives(" + name1 + ")");
   Log.incNestLevel();
   
   if (name1 != name2) {
      Log.error(
         name2 + " must match " + name1,
         ctx->derivativename2->getLine()
      );
   }
   
   if (ctx->domainDefs() != nullptr) {
      visitDomainDefs(ctx->domainDefs());
   }
   
   for (auto tctx : ctx->topicDef()) {
      visitTopicDef(tctx);
   }      
   
   Log.decNestLevel();
   debug(ctx,"<<< visitDerivatives(" + name1 + ")");

   return nullptr;

}

antlrcpp::Any Ili1Input::visitFormatEncoding(Ili1Parser::FormatEncodingContext *ctx)
{

   /*
   struct Ili1Format : public MMObject {
   public:
      bool isFree = false;
      int LineSize = 80;
      int tidSize = 10;
      char blankCode = ", ';
      char undefinedCode = '@';
      char continueCode = '\\';
      string Font = "";
      enum { TID_I16, TID_I32, TID_ANY, TID_EXPLANATION } tidKind = TID_ANY;
      string tidExplanation = ""; // defined when tidKind == TID_EXPLANATION
   };
   */

   /* formatEncoding
   : FORMAT 
   (
     FREE
   | FIX WITH
     LINESIZE EQUAL lineSize = POSNUMBER COMMA
     TIDSIZE EQUAL tidSize = POSNUMBER
   )
   SEMI
   font?
   BLANK EQUAL ( DEFAULT | blankcode=code) COMMA
   UNDEFINED EQUAL (DEFAULT | undefinedcode=code) COMMA
   CONTINUE EQUAL (DEFAULT | continuecode=code) SEMI
   TID EQUAL (I16 | I32 | ANY | EXPLANATION) SEMI
   END DOT
   */
   
   debug(ctx,">>> visitFormatEncoding()");
   Log.incNestLevel();
   
   Ili1Format *f = new Ili1Format();
   
   if (ctx->FREE() != nullptr) {
      init_mmobject(f,ctx->FREE()->getSymbol()->getLine());
      f->isFree = true;
   }
   if (ctx->FIX() != nullptr) {
      init_mmobject(f,ctx->FIX()->getSymbol()->getLine());
      f->LineSize = atoi(ctx->lineSize->getText().c_str());
      f->tidSize = atoi(ctx->tidSize->getText().c_str());
   }

   if (ctx->font() != nullptr) {
      string font = visitFont(ctx->font()); 
      f->Font = font;
   }

   f->blankCode = '_'; // DEFAULT
   if (ctx->blankcode != nullptr) {
      char code = visitCode(ctx->blankcode);
      f->blankCode = code;
   }
   f->undefinedCode = '@'; // DEFAULT
   if (ctx->undefinedcode != nullptr) {
      char code = visitCode(ctx->undefinedcode);
      f->undefinedCode = code;
   }
   f->continueCode = '\\'; // DEFAULT
   if (ctx->continuecode != nullptr) {
      char code = visitCode(ctx->continuecode);
      f->continueCode = code;
   }

   if (ctx->I16() != nullptr) {
      f->tidKind = Ili1Format::TID_I16;
   }
   else if (ctx->I32() != nullptr) {
      f->tidKind = Ili1Format::TID_I32;
   }
   else if (ctx->ANY() != nullptr) {
      f->tidKind = Ili1Format::TID_ANY;
   }
   else { // EXPLANATION
      f->tidKind = Ili1Format::TID_EXPLANATION;
      f->tidExplanation = ctx->EXPLANATION()->getSymbol()->getText();
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitFormatEncoding()");

   return f;

}

antlrcpp::Any Ili1Input::visitFont(Ili1Parser::FontContext *ctx)
{

   /* font
   : FONT EQUAL expl=EXPLANATION SEMI
   */
   
   debug(ctx,">>> visitFont()");
   string value = ctx->expl->getText();
   debug(ctx,"<<< visitFont(" + value + ")");
   
   return value;

}

antlrcpp::Any Ili1Input::visitCode(Ili1Parser::CodeContext *ctx)
{

   /* code
   : POSNUMBER
   | HEXNUMBER
   */

   debug(ctx,">>> visitCode()");
   
   unsigned int code;

   if (ctx->POSNUMBER() != nullptr) {
      sscanf(ctx->POSNUMBER()->getSymbol()->getText().c_str(),"%d",&code);
   }
   else {
      sscanf(ctx->HEXNUMBER()->getSymbol()->getText().c_str(),"%x", &code);
   }
 
   debug(ctx,"<<< visitCode(" + to_string(code) + ")");
   
   return (char)code;

}
