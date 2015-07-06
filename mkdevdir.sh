#!/bin/sh

for dir in "$@"
do
	mkdir $dir
	mkdir $dir/src
	mkdir $dir/bin
	mkdir $dir/build
	mkdir $dir/doc
	mkdir $dir/data
	
	# .gitignore build directory content
	echo '*

!.gitignore'>$dir/build/.gitignore
	
	# CMakeLists.txt for compile
	echo 'cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME TemplateName)
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(CMAKE_BUILD_TYPE Debug)
project( ${PROJECT_NAME} )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set (CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -g -std=c++11") 

# Versioning
# Replace @VARIABLES@ with CMake values
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/.version.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp"
  )

# Source
file(GLOB_RECURSE  
	Project_SOURCES
       ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )
add_executable( ${PROJECT_NAME} ${Project_SOURCES} )


########################### Libraries ##########################################

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
if (OPENGL_FOUND)
  target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
  target_link_libraries(${PROJECT_NAME} m)  # if using maths.h
endif()

# SFML
# SFML unknown to CMAKE by default -> need to copy FindSFML.cmake to /usr/shared/cmake-2.8/Modules
#find_package(SFML 1.6 REQUIRED system window graphics network audio)
find_package(SFML 2 REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIR})
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
endif()


# boost::filesystem
find_package(Boost 1.34.0 REQUIRED system filesystem)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

# Create installer
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
set(CPACK_PACKAGE_VENDOR "Thibault Lucidarme")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}\\\\InstallIcon.bmp")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\github.com\\\\ThibaultLucidarme?tab=repositories")
  set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  set(CPACK_NSIS_MODIFY_PATH ON)
else(WIN32 AND NOT UNIX)
  set(CPACK_STRIP_FILES "bin/${PROJECT_NAME}")
  set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
include(CPack)

'> $dir/CMakeLists.txt

# .version.in needed by CMakeLists.txt to generate version.hpp used by CommandLineParser.hpp
echo'
#ifndef VERSION_HPP
#define VERSION_HPP


#define PROJECT_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define PROJECT_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define PROJECT_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define PROJECT_VERSION "@PROJECT_VERSION_MAJOR@.@PROJECT_VERSION_MINOR@.@PROJECT_VERSION_PATCH@"


#endif
'>$dir/.version.in


# CommandLineParser.hpp
echo '
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

'> $dir/src/CommandLineParser.hpp
	
	
done



