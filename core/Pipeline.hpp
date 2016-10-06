#ifndef __pipeline__
#define __pipeline__

#include <iostream>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <exception>

// #include <omp.h>
/***************************** * 
 OpenMP instead of native threading for improved portability 
https://software.intel.com/en-us/articles/choosing-between-openmp-and-explicit-threading-methods
 * 
 *****************************/

namespace p
{

	template <typename I, typename O>
	class Pipeline
	{

	private:

		void BackPropagate(Pipeline*);
		void CheckPipelineConnection(Pipeline*);
		void RetroUpdate(Pipeline*);
		O Execute(I* inputArray, int size);

		static short level; //indentation level
	

	protected:

		std::vector<I> _input;
		std::vector<Pipeline*> _inputConnection;
		O _output;
		std::string _name;
		bool _displayOutput, _isCalculated;

		Pipeline* ValidateDAG(Pipeline*);

		virtual O Execute(typename std::vector<I>::iterator begin, typename std::vector<I>::iterator end) = 0; //generic iterator
		virtual void toString(std::ostream& s = std::cout) {
			s <<_name<<" - "<< _output;
		}


	public:

		Pipeline(std::string);
		void Update(void);
		Pipeline* ValidateDAG(void);
		Pipeline* SetInput(Pipeline*);
		Pipeline* SetInput(const I&);
		const O GetOutput(void);
		const bool HasBeenCalculated(void);
		void EnableThreading(bool);

		template<template<typename ELEM, typename ALLOC=std::allocator<ELEM> > class Container>
		Pipeline* SetInput(const Container<I> i);

		template<class U,class V>
		friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>* p);

		template<class U,class V>
		friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>& p);

	};

	class CyclicPipelineException : public std::exception
	{
	public:
		std::string _a, _b;
		CyclicPipelineException(std::string a, std::string b):_a(a),_b(b)
		{}
		virtual const char* what() const throw()
		{
			return ("Pipeline is not Acyclic: "+_a+" -> "+_b+" -> ... -> "+_a).c_str() ;
		}
	};

	template<class I, class O>
	short Pipeline<I,O>::level =0;

	template<class I, class O>
	Pipeline<I,O>* Pipeline<I,O>::ValidateDAG( Pipeline* root )
	{	
		Pipeline* cycle = nullptr;
		static std::set<Pipeline*> finishedNodes; //black
		static std::set<Pipeline*> knownNodes; //grey

		// in case no cycle is found, all nodes are black from previous run i.e. finishedNodes is still full
		if(knownNodes.empty()) finishedNodes.clear();
		knownNodes.insert(root);

		std::all_of(root->_inputConnection.begin(),
					root->_inputConnection.end(),
					[&](Pipeline* p){ 
						// if node already visited -> cycle
						if ( knownNodes.find( p ) != knownNodes.end() ) 
						{
							cycle = p;
							finishedNodes.clear();
							knownNodes.clear();
							throw CyclicPipelineException( p->_name, root->_name );
							return false; // exit (break does not work in for_each)
						}
						else // dfs
						{
							cycle = ValidateDAG( p );
							finishedNodes.insert( p );
							knownNodes.erase( p );
							return true; // continue
						}

					 });

		return cycle;
	}

	template<class I, class O>
	Pipeline<I,O>* Pipeline<I,O>::ValidateDAG( void )
	{	
		ValidateDAG( this );
	}

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
	Pipeline<I,O>::Pipeline(std::string s) : _name(s) 
	{
		// level = 0;

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
		// make sure input::output_type matches this::input_type
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
				level++;
				this->BackPropagate(input);
				this->CheckPipelineConnection(input);
				level--;
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
		static bool checkForCycle=true;
		if( checkForCycle )
		{

		}


		std::cout<< std::string(3*level,' ') << "Executing :'"<< _name <<"'";
		#ifdef _OPENMP
			std::cout<<" #"<<omp_get_thread_num();<<std::endl;
		#endif
		std::cout<<std::endl;

		try{ //omp pragma not threadsafe in try statement; try only for debug

			#pragma omp parallel
			{
				if(!_inputConnection.empty())
				#pragma omp single
				std::for_each (std::next(_inputConnection.begin(),0),
					_inputConnection.end(),
					std::bind(&Pipeline::RetroUpdate,this,std::placeholders::_1));
			}

		}
		catch(std::exception& e)
		{
			std::cerr << e.what()
			<< std::endl;
			// throw e;
		}

		// Execute processing from input list
		try
		{
			_output = Execute( _input.begin(), _input.end() );
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
	Pipeline<I,O>* Pipeline<I,O>::SetInput(const I& i)
	{
		_input.push_back(i);
		_isCalculated = false;
		return this;
	}

	template<class I, class O>
	template<template<typename ELEM, typename ALLOC=std::allocator<ELEM> > class Container>
	Pipeline<I,O>* Pipeline<I,O>::SetInput(const Container<I> i)
	{
		std::copy(i.begin(), i.end(), std::back_inserter(_input));
		_isCalculated = false;
		return this;
	}

	template<class I, class O>
	const O Pipeline<I,O>::GetOutput(void)
	{
		return _output;
	}

	template<class I, class O>
	const bool Pipeline<I,O>::HasBeenCalculated(void)
	{
		return _isCalculated;
	}

}

#endif

/******************************************************************************

Applications

/******************************************************************************

Preprocessing::cleaning
Preprocessing::formatting
Preprocessing::filtering
Preprocessing::validation
Preprocessing::removal / imputing missing data
Preprocessing::normalization

FeatureExtraction::transformation (FFT, Wavelet, etc.)
FeatureExtraction::model comparison
FeatureExtraction::parameter estimation
FeatureExtraction::statistical parameters
FeatureExtraction::geometric measurements

Classifiers/Regressor::K-Nearest Neighbors
Classifiers/Regressor::Logistic Regression
Classifiers/Regressor::Neural Network
Classifiers/Regressor::Fuzzy Logic
Classifiers/Regressor::Decision Tree
Classifiers/Regressor::Random Forest
Classifiers/Regressor::Naïve Bayes


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
