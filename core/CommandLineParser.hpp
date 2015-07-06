/*
  example code
 
  #include "CommandLineParser.hpp"
  p::CommandLineParser parser(argc, argv);

  int input = parser.addOption<int>("-i",-17,"test int");
  std::string allo = parser.addOption<std::string>("-s","coucou","test string");
  std::string peep = parser.addOption<std::string>("-p","salut");
  parser.addHelpOption();

*/

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <exception>
#include <map>

#include "version.hpp"


namespace p
{
class CommandLineParser
{
private:
int								   _numArg;
std::vector<std::string>		   _argList;
std::map<std::string, std::string> _argmap;
std::map<std::string, std::string> _descriptionmap;
bool _displayHelp;
std::string _appName;
//char* _appName;


 // STRING INT
    void ConvertType(std::string& s, int& i)
	{
		i = std::atoi( s.c_str() );		
	}

    void ConvertType(int& i, std::string& s)
    {
        std::stringstream ss;
        ss << i;
        ss >> s;
    }

    // STRING FLOAT
	void ConvertType(std::string& s, float& f)
	{
		f = (float)std::atof( s.c_str() );
	}
    
    void ConvertType(float& f, std::string& s)
    {
        std::stringstream ss;
        ss << f;
        ss >> s;
    }

    // STRING DOUBLE
	void ConvertType(std::string& s, double& d)
	{
		d = std::atof( s.c_str() );
	}
    
    void ConvertType(double& d, std::string& s)
    {
        std::stringstream ss;
        ss << d;
        ss >> s;
    }
	
    // STRING CHAR*
	void ConvertType(std::string& s, char* c)
	{
        //c_str returns a const char*
		c = const_cast<char*>( s.c_str() );
	}
    
    void ConvertType(char* c, std::string& s)
    {
        std::stringstream ss;
        ss << c;
        ss >> s;
    }



public:
CommandLineParser(int argc, char** argv)
{
	_numArg = argc;
	_displayHelp = false;
	_appName = argv[0];
	
	std::string key = "/";
	std::size_t found = _appName.rfind(key);
	if (found!=std::string::npos)
		_appName.replace (0,found+1,"");
	
	

	// convert char** to vector<string>

	for (int i = 1; i < argc; i++)
	{
		std::stringstream ss;
		std::string		  s;
		ss << argv[i];
		ss >> s;
		_argList.push_back(s);
		
		if(s=="-h" || s=="--help")
		{
			_displayHelp = true;
		}
		if(s=="--version")
		{
			DisplayOption();
		}
	}

	// map <option , Argument>

	for (unsigned int i = 0; i < _argList.size(); i++)
	{
		if (_argList[i][0] == '-')
		{
			if(i!=_argList.size()-1) // _argList[i + 1] exists
			{
				if (_argList[i + 1][0] != '-')
					_argmap.insert(std::pair<std::string, std::string>(_argList[i], _argList[i + 1]) );
				else
					_argmap.insert(std::pair<std::string, std::string>(_argList[i],  "") );
			}
			else
			{
				_argmap.insert(std::pair<std::string, std::string>(_argList[i],  "") );
			}
		}
	}
}

template<typename Type>
Type addOption(std::string optName, Type defaultValue, std::string description = "default description")
{
	Type result;

	//if option is found

	if (_argmap.find(optName) != _argmap.end() )
		ConvertType(_argmap.find(optName)->second, result);

	//if option not found, assign default value

	else
		result = defaultValue;

	std::string defaultVal;
	ConvertType(defaultValue, defaultVal);
	description = description+" [default:"+defaultVal+"]";
	_descriptionmap.insert(std::pair<std::string, std::string>(optName, description) );

	return result;
}

// Needs to be added at the end of all addOption calls
void addHelpOption(std::string s="")
{
	if(_displayHelp)
	{
		std::cout<<std::endl<<"Usage:"<<std::endl;
		std::cout<<_appName<<" [options]"<<std::endl;
		std::cout<<"Options:"<<std::endl;
		for (auto& opt: _descriptionmap) 
		{
			std::cout <<"\t"<< opt.first << ":\t" << opt.second << '\n';
		}
		std::cout <<"\t--version:\tPrint the version number of "+_appName+" and exit.\n";
		
		std::cout << std::endl<<s<<std::endl;
		
		exit(EXIT_SUCCESS);
		
	}
}

void DisplayOption()
{
	
	std::cout<<std::endl<<_appName<<" version: "<<PROJECT_VERSION<<std::endl<<std::endl;
	
	exit(EXIT_SUCCESS);
}


};

}




