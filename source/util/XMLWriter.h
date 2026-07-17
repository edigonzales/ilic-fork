#pragma once

#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include "TextWriter.h"
#include "StringMap.h"

using namespace std;

namespace util {

   class XMLWriter {
   public:
      void openFile(string output_file,string encoding);
      void closeFile();
      void openTag(string tag, StringMap attributes);
      void openTag(string tag);
      void closeTag();
      void writeTag(string tag, StringMap attributes);
      void writeTag(string tag, string content);
      void writeTag(string tag, StringMap attributes,string content);
      void writeComment(string content);
   private:
      TextWriter txt;
      int nestLevel = 0;
      list<string> tags;
      void ident();
      void writeAttributes(StringMap attributes);
      void writeContent(string content);
   };

};
