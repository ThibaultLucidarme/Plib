
#include "NeuralNetwork.hpp"

namespace p
{
namespace NN
{
NNEntry::NNEntry(void)
{
	std::cout << "WARNING: creating empty NNEntry";
}

NNEntry::NNEntry(p::Array<double> v, p::Array<double> tv) : _value(v), _targetValue(tv)
{
	//std::cout << " Allocate E memory slot " << this << std::endl;
}

NNEntry::~NNEntry()
{
	//std::cout << " Free E memory slot " << this << std::endl;
}

void NNEntry::SetName(const std::string s)
{
	_name = s;
}

void NNEntry::SetValue(const p::Array<double> v)
{
	_value = v;
}

void NNEntry::SetValue(const double val, const unsigned int index)
{
	_value.at(index) = val;
}

void NNEntry::SetTargetValue(const std::vector<double> v)
{
	_targetValue = v;
}

void NNEntry::SetTargetValue(const double val, const unsigned int index)
{
	_targetValue.at(index) = val;
}

std::vector<double> NNEntry::GetValue(void)
{
	return _value;
}

std::vector<double> NNEntry::GetTargetValue(void)
{
	return _targetValue;
}

std::string NNEntry::GetName(void)
{
	return _name;
}

NeuralNetwork::NeuralNetwork(const std::vector< unsigned int > nbNodesPerLayer)
{
	//std::cout << " Allocate memory NN slot " << this << std::endl;

	srand(time(0) );

	/**********************************
	INIT
	Allocate structure size
	**********************************/
	
	_nbLayers		= nbNodesPerLayer.size();
	_nbNodes		= nbNodesPerLayer;
	_NB_INPUT_NODE	= nbNodesPerLayer[0];
	_NB_OUTPUT_NODE = nbNodesPerLayer[_nbLayers];

	_neurons.resize(_nbLayers);
	_weight.resize(_nbLayers - 1);
	_Dweight.resize(_nbLayers - 1);
	_cumulDweight.resize(_nbLayers - 1);

	// for every layer except the output
	for (unsigned int layer = 0; layer < _nbLayers - 1; layer++)
	{
		//include a bias node
		_nbNodes[layer] += 1;
		//allocate neurons in the layer
		_neurons[layer].resize(_nbNodes[layer], 0.0);
		// set bias
		_neurons[layer][0] = 1.0;
	}

	//last layer (output) has no bias
	_neurons[_nbLayers - 1].resize(_nbNodes[_nbLayers - 1], 0.0);


	// init weights and weight updates between layers
	for (unsigned int layer = 0; layer <= 1; layer++)
	{
		//allocate size with initial value
		std::vector<double> tmp;
		tmp.resize(_nbNodes[layer + 1], 0.5);

		_weight[layer].resize(_nbNodes[layer], tmp);
		_Dweight[layer].resize(_nbNodes[layer], tmp);
		_cumulDweight[layer].resize(_nbNodes[layer], tmp);

		//randomize initial weights
		for (unsigned int i = 0; i < _nbNodes[layer]; i++)
			for (unsigned int j = 0; j < _nbNodes[layer + 1]; j++)
			{
				_weight[layer][i][j] = (double) (rand() % 101 - 50) / 100;
				//std::cout<<_weight[layer][i][j]<<" ";
			}
		//std::cin.get();
	}



	//set default parameter values

	_epoch				 = 0;
	_maxEpochs			 = 1;
	_learningRate		 = 0.0001;
	_accuracyRequired	 = 0.90;
	_batchLearning		 = true;
	_incrementalLearning = false;

	//set logs

	_enableTrainingLog		 = false;
	_enableGeneralizationLog = false;
	_enableValidationLog	 = false;

	_resultLog.open("./Result.log");
}

NeuralNetwork::NeuralNetwork(const p::Array< unsigned int > nbNodesPerLayer)
{
	//std::cout << " Allocate memory NN slot " << this << std::endl;

	srand(time(0) );

	//init and allocate structure size

	_nbLayers		= nbNodesPerLayer.length();
	_nbNodes		= nbNodesPerLayer;
	_NB_INPUT_NODE	= nbNodesPerLayer[0];
	_NB_OUTPUT_NODE = nbNodesPerLayer[_nbLayers - 1];


	unsigned int max = -1;
	for (unsigned int i = 0; i < nbNodesPerLayer; i++)
		max = std::max(max, nbNodesPerLayer[i]);

	// CAREFUL!! Array is set to max, but not all cases will be used
	//           Output layers is not supposed to have a bias;
	// Todo -> add sparse option to Array.hpp
	_neurons	  = new p::Array<double>(2, _nbLayers, max + 1);  //include bias
	_weight		  = new p::Array<double>(3, _nbLayers, max + 1, max);
	_Dweight	  = new p::Array<double>(3, _nbLayers, max + 1, max);
	_cumulDweight = new p::Array<double>(3, _nbLayers, max + 1, max);

	_neurons.Fill(1.0);
	_weight.Fill(-0.5, 0.5);
	_Dweight.Fill(0.0f);
	_cumulDweight.Fill(0.0f);

	for (unsigned int i = 0; i < _nbNodes.at(layer); i++)
		for (unsigned int j = 0; j < _nbNodes.at(layer + 1); j++)
			_weight.at(layer, i, j) = (double) (rand() % 101 - 50) / 100;

	//set default parameter values
	_epoch				 = 0;
	_maxEpochs			 = 1;
	_learningRate		 = 0.0001;
	_accuracyRequired	 = 0.90;
	_batchLearning		 = true;
	_incrementalLearning = false;

	//set logs

	_enableTrainingLog		 = false;
	_enableGeneralizationLog = false;
	_enableValidationLog	 = false;

	_resultLog.open("./Result.log");
}

NeuralNetwork::~NeuralNetwork()
{
	if (_enableTrainingLog)
		_trainingLog.close();

	if (_enableGeneralizationLog)
		_generalizationLog.close();

	if (_enableValidationLog)
		_validationLog.close();

	_resultLog.close();

	//if use DeleteFunctor, training set and generalization set vectors are destroyed also in the main
	//-> not delete here in case of further re-use in the main

	//std::for_each(_trainingSet.begin(), _trainingSet.end(), DeleteFunctor() );
	//std::for_each(_generalizationSet.begin(), _generalizationSet.end(), DeleteFunctor() );

	//std::cout << " Free memory NN slot " << this << std::endl;
}

void NeuralNetwork::LoadDefault(void)
{
	//std::cout<<"Enabling Incremental training"<<std::endl;
	SetBatchLearning();

	//std::cout<<"Enabling training log"<<std::endl;
	SetTrainingLog("./training.log");

	//std::cout<<"Enabling generalization log"<<std::endl;
	SetGeneralizationLog("./generalization.log");

	//std::cout<<"Setting LearningRate"<<std::endl;
	SetLearningRate(0.001);

	//std::cout<<"Setting max epoch"<<std::endl;
	SetMaxEpochs(10000);

	//std::cout<<"Setting desired accuracy"<<std::endl;
	SetDesiredAccuracy(90.0);
}

/*
   void NeuralNetwork::LoadDefault( std::string configFileName)
   {
    ifstream configFile( configFileName );

    SetIncrementalLearning();
    SetTrainingLog( tLog );
    SetGeneralizationLog( gLog );
    SetLearningRate( lr );
    SetMaxEpochs( maxE );
    SetDesiredAccuracy( acc );
   }
 */

void NeuralNetwork::SetBatchLearning(void)
{
	_batchLearning		 = true;
	_incrementalLearning = false;
}

void NeuralNetwork::SetIncrementalLearning(void)
{
	_batchLearning		 = false;
	_incrementalLearning = true;
}

void NeuralNetwork::SetTrainingLog(const std::string filename)
{
	_enableTrainingLog = true;
	_trainingLog.open(filename.c_str() );
}

void NeuralNetwork::SetGeneralizationLog(const std::string filename)
{
	_enableGeneralizationLog = true;
	_generalizationLog.open(filename.c_str() );
}

void NeuralNetwork::SetValidationLog(const std::string filename)
{
	_enableValidationLog = true;
	_validationLog.open(filename.c_str() );
}

void NeuralNetwork::SetLearningRate(const double lr)
{
	_learningRate = lr;
}

void NeuralNetwork::SetMaxEpochs(const int max)
{
	_maxEpochs = max;
}

void NeuralNetwork::SetDesiredAccuracy(const float a)
{
	_accuracyRequired = a;
}

void NeuralNetwork::LoadTrainingSet(const std::vector<NNEntry*> ts)
{
	_trainingSet = ts;
}

void NeuralNetwork::LoadGeneralizationSet(const std::vector<NNEntry*> gs)
{
	_generalizationSet = gs;
}

void NeuralNetwork::TrainNetwork(void)
{
	_epoch = 0;

	//set log headers
	if (_enableTrainingLog)
		_trainingLog << "epoch accuracy error" << std::endl;
	if (_enableGeneralizationLog)
		_generalizationLog << "epoch accuracy error output[]" << std::endl;

	do
	{
		RunSingleTraining(&_trainingSet);

		//get accuracy and error

		_generalizationAccuracy = GetAverageAccuracy(&_generalizationSet);
		_generalizationError	= GetMSE(&_generalizationSet);

		//log intermediate results

		if (_enableGeneralizationLog)
		{
			_generalizationLog << "epoch " << _epoch <<
			" accuracy " << _generalizationAccuracy <<
			" MSE " << _generalizationError << std::endl;
		}

		//display progress

		if (_epoch / floor(_maxEpochs / 10) == (int) (_epoch / floor(_maxEpochs / 10) ) )
			std::cout << "\t" << (_epoch / floor(_maxEpochs / 10) ) * 10 << "% --- done" << std::endl;

		_epoch++;
	}
	while (/*_generalizationAccuracy < _accuracyRequired && */ _epoch < _maxEpochs);

	//display and write results

	OutputTrainingResult();
}

void NeuralNetwork::OutputTrainingResult(void)
{
	//Display results on console
	if (_verbose > 0)
	{
		std::cout << std::endl;
		std::cout << "*********************************" << std::endl;
		std::cout << "training set size : " << _trainingSet.size() << std::endl;
		std::cout << "generalization set size : " << _generalizationSet.size() << std::endl << std::endl;
		std::cout << "--> total : " << _generalizationSet.size() + _trainingSet.size() << std::endl << std::endl;
		std::cout << "---------------------------------" << std::endl;
		std::cout << "nb_epoch : " << _epoch << std::endl;
		std::cout << "average accuracy : " << _generalizationAccuracy << std::endl;
		std::cout << "error : " << _generalizationError << std::endl << std::endl;
		std::cout << "*********************************" << std::endl;
	}

	//Write results in file

	_resultLog << std::endl;
	_resultLog << "*********************************" << std::endl;
	_resultLog << "training set size : " << _trainingSet.size() << std::endl;
	_resultLog << "generalization set size : " << _generalizationSet.size() << std::endl << std::endl;
	_resultLog << "--> total : " << _generalizationSet.size() + _trainingSet.size() << std::endl << std::endl;
	_resultLog << "---------------------------------" << std::endl;
	_resultLog << "nb_epoch : " << _epoch << std::endl;
	_resultLog << "average accuracy : " << _generalizationAccuracy << std::endl;
	_resultLog << "error : " << _generalizationError << std::endl << std::endl;
	_resultLog << "*********************************" << std::endl;
}

void NeuralNetwork::RunSingleTraining(std::vector<NNEntry*>* trainingSet)
{
	std::vector<NNEntry*>::iterator it;

	// FF and BP each element of the training set

	for (unsigned int i = 0; i < trainingSet->size(); i++)
	{
		FeedForward( (*trainingSet)[i]->GetValue() );

		Backpropagate( (*trainingSet)[i]->GetTargetValue() );

		// print result
		if (_verbose > 1)
		{
			_trainingLog << "epoch : " << _epoch << std::endl;
			PrintInfo(true, _trainingLog);
			_trainingLog << "expected : ";
			for (unsigned int j = 0; j < (*trainingSet)[i]->GetTargetValue().size(); j++)
				_trainingLog << std::setprecision(4) << (*trainingSet)[i]->GetTargetValue()[j] << " ";
			_trainingLog << std::endl << std::endl;
		}
	}
}

void NeuralNetwork::FeedForward(const std::vector<double>& inputs)
{
	/*
	   for (unsigned int i =1; i<_nbNodes[0]; i++)
	    _neurons[0][i] = inputs[i-1];
	   _neurons[0][0] = 0.5;

	   for(unsigned int i = 1; i< _nbNodes[1]; i++)
	   {
	    double sum =0.0;
	    for(unsigned int j = 0; j< _nbNodes[0]; j++)
	        sum += _neurons[0][j] * _weight[0][j][i];

	    _neurons[1][i] = ActivationFunction(sum);
	   }
	   _neurons[1][0] = 0.5;

	   for(unsigned int i = 0; i< _nbNodes[2]; i++)
	   {
	    double sum =0.0;
	    for(unsigned int j = 0; j< _nbNodes[1]; j++)
	        sum += _neurons[1][j] * _weight[1][j][i];

	    _neurons[2][i] = ActivationFunction(sum);
	   }
	 */
//*method 2:

	//set input neurons to input values
	for (unsigned int i = 1; i < _nbNodes.at(0); i++)
		_neurons.at(0, i) = inputs[i - 1];

	//feed each layer to the next (except output layer)
	for (unsigned int layer = 0; layer < _nbLayers - 1; layer++)
		for (unsigned int nextNode = 1; nextNode < _nbNodes.at(layer + 1); nextNode++)
		{
			for (unsigned int node = 0; node < _nbNodes[layer]; node++)
				_neurons.at(layer + 1, nextNode) += _neurons.at(layer, node) * _weight.at(layer, node, nextNode);

			_neurons.at(layer + 1, nextNode) = ActivationFunction(_neurons.at(layer + 1, nextNode) );
		}

	//compensate the output's first node who does not have a bias
	for (unsigned int node = 0; node < _nbNodes.at(_nbLayers - 2); node++)
		_neurons.at(_nbLayers - 1, 0) += _neurons.at(_nbLayers - 2, node) * _weight.at(_nbLayers - 2, node, 0);
	_neurons.at(_nbLayers - 1, 0) = ActivationFunction(_neurons.at(_nbLayers - 1, 0) );
	//*/
}

void NeuralNetwork::Backpropagate(std::vector<double> expectedValues)
{
	p::Array<double> dnode = new p::Array(_neurons);
	dnode.Fill(0);

	if (expectedValues.size() != _NB_OUTPUT_NODE)
		exit(EXIT_FAILURE);

	/**
	   TODO
	   generalize error formula to any layer for the back-propagation
	 */

	//calculate error for output layer
	for (unsigned int node = 0; node < _NB_OUTPUT_NODE; node++)
		dnode.at(_nbLayers - 1, node) = InverseActivationFunc(_neurons.at(_nbLayers - 1, node) ) * (_neurons.at(_nbLayers - 1, node) - expectedValues[j]);

	// Calculate error for each node
	for (unsigned int layer = _nbLayers - 2; layer > 0; layer--)
		for (unsigned int node = 0; node < _nbNodes.at(layer); node++)
		{
			double sum = 0.0f;
			for (unsigned int nextNode = 0; nextNode < _nbNodes.at(layer + 1); nextNode++)
				sum += dnode.at(layer + 1, nextNode) * _weight.at(layer, node, nextNode);
			dnode.at(layer, node) = InverseActivationFunc(_neurons.at(layer, node) ) * sum;
		}

	//backpropagate

	//std::cout<<"DweightHO ";
	for (unsigned int layer = 0; layer < _nbLayers - 1; layer++)
		for (unsigned int node = 0; node < _nbNodes.at(layer); node++)
			for (unsigned int nextNode = 0; nextNode < _nbNodes.at(layer + 1); nextNode++)
				_Dweight.at(layer, node, nextNode) = _neurons(layer, node) * dnode.at(layer, node);

	UpdateWeights();
}

void NeuralNetwork::UpdateWeights()
{
	if (_batchLearning) //for batch learning, we update the weights only after feeding all the elements of the given set
	{
		static unsigned int patternNumber = 1;

		for (unsigned int layer = 0; layer < _nbLayers - 1; layer++)
			for (unsigned int node = 0; node < _nbNodes.at(layer); node++)
				for (unsigned int nextNode = 0; nextNode < _nbNodes.at(layer + 1); nextNode++)
				{
					_cumulDweight.at(layer, node, nextNode) += _Dweight.at(layer, node, nextNode);

					if (patternNumber == _trainingSet.size() )     //if the last element has been fed to the network
					{
						_weight.at(layer, node, nextNode)	   += _learningRate * _cumulDweight.at(layer, node, nextNode);
						_cumulDweight.at(layer, node, nextNode) = 0.0;      //reset cumul of errors for next epoch
						patternNumber							= 0;
					}
				}

		patternNumber++;
	}

	else if (_incrementalLearning) // for incremental learning, we update the weights after each element of the set
	{
		for (unsigned int layer = 0; layer < _nbLayers - 1; layer++)
			for (unsigned int node = 0; node < _nbNodes.at(layer); node++)
				for (unsigned int nextNode = 0; nextNode < _nbNodes.at(layer + 1); nextNode++)
					_weight.at(layer, node, nextNode) += _learningRate * _neurons(layer, node) * _weight.at(layer, node, nextNode);
	}
}

double NeuralNetwork::InverseActivationFunc(double x, double a)
{
	return -log(1.0 / x - 1.0) / a; //inverse of sigmoid function
}

double NeuralNetwork::DifferentialActivationFunc(double x, double a)
{
	return a * exp(a * x) / pow(1.0 + exp(a * x), 2); //differential of the sigmoid function
}

double NeuralNetwork::ActivationFunction(double x, double a)
{
	return 1.0 / (1.0 + exp(-a * x) );
}

void NeuralNetwork::PrintInfo(unsigned int verbose, std::ostream& out, unsigned int precision)
{
	if (verbose > 1)
	{
		out << "inputs : " << std::endl << "\t";
		for (unsigned int node = 0; node < _nbNodes.at(0); node++)
			out << std::setprecision(precision) << _neurons.at(0, node) << " ";
		out << std::endl;
	}

	if (verbose > 2)
	{
		for (unsigned int layer = 1; layer < _nbLayers - 1; layer++)
		{
			out << "hidden layer : " << layer << std::endl << "\t";
			for (unsigned int node = 0; node < _nbNodes.at(layer); node++)
				out << std::setprecision(precision) << _neurons.at(layer, node) << " ";
			out << std::endl;
		}
	}

	if (verbose > 0)
	{
		out << "output : " << std::endl << "\t";
		for (unsigned int node = 0; node < _nbNodes.at(_nbLayers - 1); node++)
			out << std::setprecision(precision) << _neurons.at(_nbLayers - 1, node) << " ";
		out << std::endl;
	}
}

double NeuralNetwork::GetAverageAccuracy(std::vector<NNEntry*>* set)
{
	double							meanAcc = 0;

	std::vector<NNEntry*>::iterator it;
	for (it = set->begin(); it < set->end(); it++)
		meanAcc += GetAccuracy(*it);

	return meanAcc / set->size();
}

double NeuralNetwork::GetAccuracy(NNEntry* entry)
{
	double acc;
	int	   nbOutput = _nbNodes[_nbLayers - 1]; //size of the output

	FeedForward(entry->GetValue() );

	//check all outputs from neural network against expected values
	for (int k = 0; k < nbOutput; k++)
	{
		if (abs(_neurons[_nbLayers - 1][k] - entry->GetTargetValue()[k]) < 0.5)
			acc++;
	}

	return acc / nbOutput;
}

double NeuralNetwork::GetMSE(std::vector<NNEntry*>* set)
{
	double							mse = 0.0;

	std::vector<NNEntry*>::iterator it;
	for (it = set->begin(); it < set->end(); it++)
	{
		//feed inputs through network and backpropagate errors
		FeedForward( (*it)->GetValue() );

		for (unsigned int j = 0; j < _nbNodes[_nbLayers - 1]; j++)
			mse += pow( (_neurons[_nbLayers - 1][j] - (*it)->GetTargetValue()[j]), 2);
	}

	//return error as percentage
	return mse / (_nbNodes[_nbLayers - 1] * set->size() );
}
}     //end namespace NN
} // end namespace p
