#include <iostream>
#include <cmath>

template<class T>
class GmmDistribution
{
private:
unsigned int	 _numGauss;
unsigned int	 _dim;
p::Array<double> _mu;
p::Array<double> _sigma;
T				 NO_DATA_VALUE

public:

GmmDistribution();
setNumGauss(unsigned int);
void fit(p::Array<T> data, T* threshold = NULL, int (*Comparator)(T, T) = std::less<T>() );
}
