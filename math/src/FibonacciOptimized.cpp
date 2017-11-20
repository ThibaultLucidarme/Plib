#include <iostream>
#include <cmath>

/*
	Time complexity Log(n)
	Using Matrix exponentiation algorithm

	F(n) = a*F(n-1) + b*F(n-2)
	F(n-1) = F(n-1)

	S = [[a b],
		 [0 1]]
	[Fn 	= S * [Fn-1
	 Fn-1]		   Fn-2]

with recurrence
	[Fn 	= S^(n-1) * [F1
	 Fn-1]		   		 F0]


	 using close form
	 https://www.wolframalpha.com/input/?i=%7B%7B1,1%7D,%7B1,0%7D%7D%5En

*/

typedef double type;

struct Mat2x2
{
  Mat2x2( type a, type b, type c, type d)
  {
    x[0][0] = a;
    x[0][1] = b;
    x[1][0] = c;
    x[1][1] = d;
  }

  type x[2][2];
};

struct Vec2
{
  Vec2(type a, type b)
  {
    x[0] = a;
    x[1] = b;
  }

  type x[2];
};



class FibonacciSequence
{
private:

	static type phi;
	static type negphi;
	static type sqrt5;


	FibonacciSequence(){};

	//matrix multiplication
	static Vec2 multiply( const Mat2x2 a, const Vec2 b)
	{
	    type V0 = a.x[0][0]*b.x[0] + a.x[0][1]*b.x[1];
	    type V1 = a.x[1][0]*b.x[0] + a.x[1][1]*b.x[1];

	 	return Vec2( V0,V1 );
	}


	//matrix power
	static Mat2x2 power( const unsigned int n)
	{

		type phin = pow(phi,n);
		type negphin = pow(negphi,n);

		// close form S^n
		type S00 = 2.0f/20.0f*( (5.0f-sqrt5)*negphin + (5.0f+sqrt5)*phin );
		type S01 = 4.0f/20.0f*sqrt5* ( phin - negphin );
		type S10 = S01;
		type S11 = 2.0f/20.0f*( (5.0f+sqrt5)*negphin + (5.0f-sqrt5)*phin );

		// std::cout << S00<<"\t"<<S01<<std::endl<<S10<<"\t"<<S11<< std::endl;

		return Mat2x2(S00,S01,S10,S11);

	}


public:

	static unsigned long long  Get( const unsigned int n)
	{

		 unsigned long long  result;

		if (n <=1) return n;
		else result = multiply( power(n-1), Vec2(1,0) ).x[0];

		if ( result<0 ) std::cerr<< "type insuficient to contain Fibonacci Sequence number" <<std::endl;
		return result;
	}


};

type FibonacciSequence::phi =  0.5*(1+sqrt(5));
type FibonacciSequence::negphi =  0.5*(1-sqrt(5));
type FibonacciSequence::sqrt5 =  sqrt(5);


int main ( int argc, char** argv)
{

	
	// TODO :  automatic type for more than 93 limit wilth double/ unsigned long long
	for ( int k=0; k<100; k++) 
		std::cout << k << "\t:\t" << FibonacciSequence::Get(k) << std::endl;
	

	return 0;
}

