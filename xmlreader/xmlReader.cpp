#include "xmlReader.hpp"


p::XMLReader::XMLReader()
{
}
p::XMLReader::~XMLReader()
{
}
const char * p::XMLReader::getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

const char * p::XMLReader::getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

int p::XMLReader::dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	
	std::cout<<std::endl;
	while (pAttrib)
	{
		std::cout<<pIndent<< pAttrib->Name()<<": value=["<<pAttrib->Value()<<"]";

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    std::cout<<" int="<< ival;
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) std::cout<<" d="<<dval;
		std::cout<<std::endl;
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;	
}

void p::XMLReader::dump_to_stdout( TiXmlNode* pParent, unsigned int indent )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;	
	int num;
	int t = pParent->Type();
	std::cout<< getIndent(indent);


	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		std::cout<< "Document";
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		std::cout<< "Element ["<< pParent->Value() <<"]";
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  std::cout<< getIndentAlt(indent)<< "(No attributes)"; break;
			case 1:  std::cout<< getIndentAlt(indent)<< "1 attribute"; break;
			default: std::cout<< getIndentAlt(indent)<<num<< " attributes"; break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		std::cout<<  "Comment: ["<<pParent->Value()<<"]";
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		std::cout<< "Unknown";
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		std::cout<< "Text: ["<<pText->Value()<<"]";
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		std::cout<<  "Declaration" ;
		break;
	default:
		break;
	}
	std::cout<< std::endl;
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

void p::XMLReader::dump_to_stdout(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		std::cout<< std::endl<< pFilename<<std::endl;
		dump_to_stdout( &doc ); // defined later in the tutorial
	}
	else
	{
		std::cout<< "Failed to load file "<<pFilename;
	}
}
