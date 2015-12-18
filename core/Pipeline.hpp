#ifndef __pipeline__
#define __pipeline__

#include <iostream>
#include <vector>
#include <typeinfo>
#include <algorithm>


template <class I, class O>
class Pipeline
{

private:
	static bool useThreads;
	bool inthread;

	void BackPropagate(Pipeline*);
	void CheckPipelineConnection(Pipeline*);

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
void Pipeline<I,O>::BackPropagate(Pipeline* input)
{
	// Calculate only if needed (Memoisation)
	if( !input->_isCalculated )
	{
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
	}

}

// #ifndef __thread__

template<class I, class O>
void Pipeline<I,O>::Update(void)
{
	for_each (_inputPipeline.begin(),
				_inputPipeline.end(),
				[this](Pipeline* input){
					this->BackPropagate(input);
					this->CheckPipelineConnection(input);
				});

	// Execute processing from input list
	std::cout<<"Executing :'"<< _name <<"'"<<std::endl;
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
	}
}
//
// #else
//
//
// template<class I, class O>
// void Pipeline<I,O>::Update(void)
// {
// 	static std::vector<std::unique_ptr<Thread>> parallelizationThreads;
//
// 	for_each (_inputPipeline.begin(),
// 				_inputPipeline.end(),
// 				[](Pipeline* inputP){
//
// 					input = *inputP;
// 					try
// 					{
//
// 						// Calculate only if needed (Memoisation)
// 						if( !input->_isCalculated )
// 						{
// 							std::unique_ptr<Thread> t = new Thread(std::bind(&Pipeline::Update, input);)
// 							parallelizationThreads.push_back(std::move(t));
// 						}
//
// 						// make sure input::output_type matche this::input_type
// 						// throw an error if cast fails
// 						I* typeTest = dynamic_cast<I*>( &(input->_output) );
// 						// if cast succeeds
// 						_input.push_back( input->_output );
// 					}
// 					catch (const std::bad_cast& e)
// 					{
// 						std::cerr<< "Could not connect Pipeline: '"
// 						<< input->_name << "' -> '" << this->_name <<"'"
// 						<< std::endl
// 						<< e.what()
// 						<< std::endl;
// 					}
//
// 				});
//
// 	//join all threads before we can continue execution
// 	for_each (parallelizationThreads.begin(),
// 				parallelizationThreads.end(),
// 				[](std::unique_ptr<Thread> t){t->join();});
// 	parallelizationThreads.clear()
//
//
// 	// Execute processing from input list
// 	std::cout<<"Executing :'"<< _name <<"'"<<std::endl;
// 	try
// 	{
// 		_output = Execute( _input );
// 		_isCalculated = true;
// 	}
// 	catch(std::exception& e)
// 	{
// 		std::cerr<< "Could not connect Execute: '"
// 		<< input->_name <<"'"
// 		<< std::endl
// 		<< e.what()
// 		<< std::endl;
// 	}
// 	if( _displayOutput )
// 	{
// 		Display();
// 	}
// }
//
// #endif

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


#endif
