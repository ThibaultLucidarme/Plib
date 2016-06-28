#ifndef __pipeline__
#define __pipeline__

#include <iostream>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <functional>

#include <omp.h>


namespace p
{

	template <class I, class O>
	class Pipeline
	{

	private:

		void BackPropagate(Pipeline*);
		void CheckPipelineConnection(Pipeline*);
		void RetroUpdate(Pipeline*);

	protected:

		std::vector<I> _input;
		std::vector<Pipeline*> _inputConnection;
		O _output;

		std::string _name;
		bool _displayOutput, _isCalculated;

		virtual O Execute(std::vector<I>) = 0;
		virtual void toString(std::ostream& s = std::cout) {
			s <<_name<<" - "<< _output;
		}


	public:

		Pipeline(std::string);
		void Update(void);
		Pipeline* SetInput(Pipeline*);
		Pipeline* SetInput(const I);
		O GetOutput(void);
		bool HasBeenCalculated(void);
		void EnableThreading(bool);

		template<class U,class V>
		friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>* p);

		template<class U,class V>
		friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>& p);

	};


	template<class I, class O>
	void Pipeline<I,O>::EnableThreading(bool b)
	{
		#ifdef _OPENMP
			if(b) omp_set_nested(1);
			else omp_set_nested(0);
		#endif
	}

	template<class I, class O>
	std::ostream & operator<<(std::ostream &os, Pipeline<I,O>* p)
	{
		p->toString(os);
		return os;
	}

	template<class I, class O>
	std::ostream & operator<<(std::ostream &os, Pipeline<I,O>& p)
	{
		p.toString(os);
		return os;
	}



	template<class I, class O>
	Pipeline<I,O>::Pipeline(std::string s) : _name(s) {
		EnableThreading(false);
		_isCalculated = false;
		_displayOutput = false;
	}

	template<class I, class O>
	void Pipeline<I,O>::BackPropagate(Pipeline* input)
	{
		// Calculate only if needed (Memoisation)
		if( !input->_isCalculated )
		{
			// #pragma omp single
			input->Update();
		}
	}

	template<class I, class O>
	void Pipeline<I,O>::CheckPipelineConnection(Pipeline* input)
	{
		// make sure input::output_type matche this::input_type
		try
		{
			// throw an error if cast fails
			I* typeTest = dynamic_cast<I*>( &(input->_output) );
			// if cast succeeds
			_input.push_back( input->_output );
		}
		catch (const std::bad_cast& e)
		{
			std::cerr<< "Could not connect Pipeline: '"
			<< input->_name << "' -> '" << _name <<"'"
			<< std::endl
			<< e.what()
			<< std::endl;
			throw e;
		}

	}


	template<class I, class O>
	void Pipeline<I,O>::RetroUpdate(Pipeline* input)
	{
		#pragma omp task
		{
		try
		{
			this->BackPropagate(input);
			this->CheckPipelineConnection(input);
		}
		catch (std::exception& e)
		{
			std::cerr<< "Could not backPropagate Pipeline: '"
			<< input->_name << "' from '" << _name <<"'"
			<< std::endl
			<< e.what()
			<< std::endl;
			throw e;
		}
		}
	}

	template<class I, class O>
	void Pipeline<I,O>::Update(void)
	{

		#pragma omp parallel
		{
			#pragma omp single
			std::for_each (std::next(_inputConnection.begin(),1),
				_inputConnection.end(),
				std::bind(&Pipeline::RetroUpdate,this,std::placeholders::_1));
		}

		// Execute processing from input list
		std::cout<<"Executing :'"<< _name <<"' #"<<omp_get_thread_num()<<std::endl;
		try
		{
			_output = Execute( _input );
			_isCalculated = true;
		}
		catch(std::exception& e)
		{
			std::cerr<< "Could not connect Execute: '"
			<< _name <<"'"
			<< std::endl
			<< e.what()
			<< std::endl;
			throw e;
		}
	}

	template<class I, class O>
	Pipeline<I,O>* Pipeline<I,O>::SetInput( Pipeline* p)
	{
		_inputConnection.push_back(p);
		_isCalculated = false;
		return this;
	}

	template<class I, class O>
	Pipeline<I,O>* Pipeline<I,O>::SetInput(const I i)
	{
		_input.push_back(i);
		_isCalculated = false;
		return this;
	}

	template<class I, class O>
	O Pipeline<I,O>::GetOutput(void)
	{
		return _output;
	}

	template<class I, class O>
	bool Pipeline<I,O>::HasBeenCalculated(void)
	{
		return _isCalculated;
	}

}

#endif



/******************************************************************************

Example

/******************************************************************************

// #include "Thread.hpp"
#include "Pipeline.hpp"

using namespace std;
using namespace p;


class FilterA : public Pipeline<string,string>
{
	private:
		string Execute(vector<string> inputs)
		{
			string out="";
			vector<string>::iterator it = inputs.begin();
			while(it < inputs.end()) {out+=(*it);it++;}
			return out;
		}
	public:
		FilterA(string s):Pipeline(s){}
};

class FilterB : public Pipeline<string,string>
{
	private:
		string Execute(vector<string> inputs)
		{
			string out="";
			vector<string>::iterator it = inputs.begin();
			while(it < inputs.end()) {out+=(*it);it++;}
			return out;
		}
	public:
		FilterB(string s):Pipeline(s){}
};

int main( int argc, char** argv)
{
	FilterA* f1 = new FilterA("filter 1");
	FilterB* f2 = new FilterB("filter 2");

	f1->SetInput("f1i1")->SetInput("f1i2");
	f2->SetInput(f1)->SetInput("f2i1");

	f1->Update();

	cout<<"caca"<<endl;

	f2->Update();

	cout<<(*f1)<<endl<<f2<<endl;


	delete f1,f2;
	return EXIT_SUCCESS;
}



*/
