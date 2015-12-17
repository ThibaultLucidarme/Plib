#ifndef __pipeline__
#define __pipeline__

#include <iostream>
#include <vector>
#include <typeinfo>


template <class I, class O>
class Pipeline
{

private:
	static bool useThreads;
	bool inthread;

protected:

	std::vector<I> _input;
	std::vector<Pipeline*> _inputPipeline;
	O _output;

	std::string _name;
	bool _displayOutput, _isCalculated;
	virtual O Execute(std::vector<I>) = 0;
	virtual void Display(std::ostream& s = std::cout) {
		s <<_name<<" - "<< _output;
	}


public:

	Pipeline(std::string);
	// #ifdef __thread__
	// 	Pipeline(std::string, bool thread);
	// 	Pipeline(std::string, Thread* t);
	// #endif
	void Update(void);
	Pipeline* SetInput(Pipeline*);
	Pipeline* SetInput(const I);
	O GetOutput(void);
	bool HasBeenCalculated(void);

	template<class U,class V>
	friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>* p);

	template<class U,class V>
	friend std::ostream & operator << (std::ostream &os, Pipeline<U,V>& p);

};

template<class I, class O>
bool Pipeline<I,O>::useThreads = false;

template<class I, class O>
std::ostream & operator<<(std::ostream &os, Pipeline<I,O>* p)
{
	p->Display(os);
	return os;
}

template<class I, class O>
std::ostream & operator<<(std::ostream &os, Pipeline<I,O>& p)
{
	p.Display(os);
	return os;
}



template<class I, class O>
Pipeline<I,O>::Pipeline(std::string s) : _name(s) {
	_isCalculated = false;
	_displayOutput = false;
}

template<class I, class O>
void Pipeline<I,O>::Update(void)
{
	try{

		//for each input
		typename std::vector<Pipeline*>::iterator inputP = _inputPipeline.begin();
		Pipeline* input;

		while (inputP < _inputPipeline.end() && (input = *inputP) && input)
		{

			// Calculate only if needed (Memoisation)
			if( !input->_isCalculated )
			{
				input->Update();
			}

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
				<< input->_name << "' -> '" << this->_name <<"'"
				<< std::endl
				<< e.what()
				<< std::endl;
			}

			inputP++;
		}

		// Execute processing from input list
		std::cout<<"Executing :'"<< _name <<"'"<<std::endl;
		_output = Execute( _input );
		_isCalculated = true;

		if( _displayOutput )
		{
			Display();
		}

	}
	catch(std::exception& e)
	{
		std::cerr<<_name<<std::endl<<e.what()<<std::endl;
	}

}

template<class I, class O>
Pipeline<I,O>* Pipeline<I,O>::SetInput( Pipeline* p)
{
	_inputPipeline.push_back(p);
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

// void waitForAllThreadsJoin(void)
// {
// 	#ifdef __thread__
// 		if(_useThreads)
// 		{
// 			_threadList.foreach(join);
// 		}
// 	#endif
// }
// void startThread(void)
// {
// 	#ifdef __thread__
// 		if(_useThreads)
// 		{
// 			typename std::vector<Pipeline*>::iterator inputP = _inputPipeline.begin();
// 			inputP++; //first
// 			Pipeline* input;
// 			while (inputP < _inputPipeline.end() && (input = *inputP) && input)
// 			{
//
// 			}
// 		}
// 	#endif
// }


#endif
