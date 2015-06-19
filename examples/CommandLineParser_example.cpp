#include "CommandLineParser.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	p::CommandLineParser parser(argc, argv);

	int					 input = parser.addOption<int>("-i", -17, "test int");
	std::string			 allo  = parser.addOption<std::string>("-s", "coucou", "test string");
	std::string			 peep  = parser.addOption<std::string>("-p", "salut");
	std::cout << allo << std::endl << input << std::endl << peep << std::endl;

	return EXIT_SUCCESS;
}
