#include "xmlReader.hpp"

using namespace p;

int main(int argc, char** argv)
{
	XMLReader xmlr;
	for( int i=1; i<argc; i++)
	{
		xmlr.dump_to_stdout(argv[i]);
	}
	
	return 0;
}
