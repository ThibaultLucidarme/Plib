#ifndef TYPECONVERTER_HPP
#define TYPECONVERTER_HPP

#include <iostream>
#include <cstdlib>

namespace p
{
    // STRING INT
    void ConvertType(std::string& s, int& i)
	{
		i = std::atoi( s.c_str() );		
	}

    void ConvertType(int& i, std::string& s)
    {
        stringstream ss;
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
        stringstream ss;
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
        stringstream ss;
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
        stringstream ss;
        ss << c;
        ss >> s;
    }
    
}

#endif


