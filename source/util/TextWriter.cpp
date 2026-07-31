#include "TextWriter.h"

#include <stdexcept>

using namespace util;

//----------------------------------------------------------------------
// private interface
//----------------------------------------------------------------------

void TextWriter::ident(int nestlevel)
{
   for (int i = 0; i < nestlevel; i++) {
      out << "   ";
   }
};

//----------------------------------------------------------------------
// public interface
//----------------------------------------------------------------------

void TextWriter::openFile(string output_file)
{
   out.open(output_file);
};

void TextWriter::closeFile()
{
   out.close();
};

void TextWriter::setNestLevel(int nestlevel)
{
   this->nestlevel = nestlevel;
};

int TextWriter::getNestLevel()
{
   return this->nestlevel;
};

void TextWriter::incNestLevel()
{
   nestlevel++;
};

void TextWriter::decNestLevel()
{
   nestlevel--;
   if (nestlevel < 0) {
      throw std::runtime_error("TextWriter: nestlevel is " + to_string(nestlevel));
   }
};

void TextWriter::write(string buffer)
{
   ident(nestlevel);
   out << buffer;
};

void TextWriter::writeNoIdent(string buffer)
{
   out << buffer;
};

void TextWriter::write(int nestlevel,string buffer)
{
   ident(nestlevel);
   out << buffer;
};

void TextWriter::writeln(string buffer)
{
   write(buffer);
   out << "\n" << flush;
};

void TextWriter::writelnNoIdent(string buffer)
{
   writeNoIdent(buffer);
   out << "\n" << flush;
};

void TextWriter::writelnIncNestLevel(string buffer)
{
   incNestLevel();
   write(buffer);
   out << "\n" << flush;
};

void TextWriter::writelnDecNestLevel(string buffer)
{
   decNestLevel();
   write(buffer);
   out << "\n" << flush;
};

void TextWriter::writeln(int nestlevel,string buffer)
{
   write(nestlevel,buffer);
   out << "\n" << flush;
};
