#ifndef XMLREADER_HPP
#define XMLREADER_HPP

#include "tinyxml.h"
#include <iostream>

namespace p {
class XMLReader {
private:

const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents );
const char * getIndentAlt( unsigned int numIndents );
int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );

public:
XMLReader();
~XMLReader();
// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------


void dump_to_stdout(const char* pFilename);

};
}



#endif
