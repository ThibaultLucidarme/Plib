#include "gmm.hpp"

GmmDistribution::GmmDistribution()
{
}

void GmmDistribution::setNumGauss(unsigned int ng)
{
	_numGauss = ng;
}

void GmmDistributionfit(p::Array<T> data, T* threshold = NULL, int (*Comparator)(T, T) = std::less<T>() )
{
	_mu.clear();
	_sigma.clear();
	_mu	   = new p::Array<double>(1, data.dimension() );
	_sigma = new p::Array<double>(2, data.dimension(), data.dimension() );

	// initialize mu with maximum
	unsigned int maxIdx = data.MaxIdx();
}

void GmmDistribution::Estep()
{
}

void GmmDistribution::Mstep()
{
}