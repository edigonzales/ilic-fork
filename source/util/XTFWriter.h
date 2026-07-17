#pragma once

#include <string>
#include <list>

#include "StringMap.h"
#include "XMLWriter.h"
#include "../metamodel/MetaModel.h"

using namespace std;

namespace util {

   class XTFWriter {
   public:

      void openFile(string xtf_file, list<metamodel::Model *> models);
      void closeFile();

      void openTag(string tag, StringMap attributes);
      void openTag(string tag);
      void closeTag();
      void writeTag(string tag,string content);
      void writeTag(string tag, StringMap attributes);
      void writeTag(string tag, StringMap attributes, string content);
      void writeReference(string tag, string target);
      void writeComment(string comment);

      void openBasket(string model, string topic, string bid);
      void closeBasket();

      void openObject(string model, string topic, string class_name, string tid, StringMap attributes);
      void openObject(string model, string topic, string class_name, string tid);
      void openObject(string model, string topic, string class_name);
      void closeObject();

   private:
      XMLWriter xml;
      void openTransfer();
      void closeTransfer();
      void openDataSection();
      void closeDataSection();
      void writeHeaderSection(list<metamodel::Model *> models);
   };

};
