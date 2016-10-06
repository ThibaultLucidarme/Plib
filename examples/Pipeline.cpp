

/******************************************************************************

Example

/******************************************************************************/

// #include "Thread.hpp"
#include "../core/Pipeline.hpp"
#include <iterator>

using namespace std;
using namespace p;


class FilterA : public Pipeline<string,string>
{
	private:
		string Execute(vector<string>::iterator begin, vector<string>::iterator end)
		{
			string out="";
			while(begin < end) {out+=(*begin);begin++;}
			return out;
		}
	public:
		FilterA(string s):Pipeline(s){}
};

class FilterB : public Pipeline<string,string>
{
	private:
		string Execute(vector<string>::iterator begin, vector<string>::iterator end)
		{
			string out="";
			while(begin < end) {out+=(*begin);begin++;}
			return out;
		}
	public:
		FilterB(string s):Pipeline(s){}
};

int main( int argc, char** argv)
{
	FilterA* f1 = new FilterA("filter 1");
	FilterB* f2 = new FilterB("filter 2");
	FilterA* f3 = new FilterA("filter 3");
	FilterB* f4 = new FilterB("filter 4");
	FilterA* f5 = new FilterA("filter 5");
	FilterB* f6 = new FilterB("filter 6");
	FilterB* f7 = new FilterB("filter 7");

	f1->SetInput("f1i1")->SetInput("f1i2");
	f2->SetInput(f1)->SetInput("f2i1");
	f3->SetInput("f1i1")->SetInput("f1i2");
	f4->SetInput(f3)->SetInput("f1i2");
	f5->SetInput(f3)->SetInput("f1i2");
	f7->SetInput(f2)->SetInput("f1i2");
	f6->SetInput(f4)->SetInput(f7)->SetInput(f5);

	// /!\ Cycle
	f3->SetInput(f6);
	
	try
	{
		if( f5->ValidateDAG() ) 
		{
			f6->Update();
		}	
	}
	catch( exception& e  ) 
	{
		cout<<e.what()<<endl;
	}


	// cout<<endl<<(*f1)<<endl<<f2<<endl;


	delete f1,f2,f3,f4,f5,f6,f7;
	return EXIT_SUCCESS;
}