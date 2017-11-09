#include <iostream>

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


class FibonacciSequence
{
private:

	//matrix multiplication
	int[2] multiply(const int a[2][2], const int b[2])
	{

	    int mul[2];
	    mul[0] = a[0][0]*b[0] + a[0][1]*b[1];
	    mul[1] = a[1][0]*b[0] + a[1][1]*b[1];

	 	return mul;
	}


	//matrix power
	// TODO find close form or divide and conquer
	int[2][2] power(int F[3][3], int n)
	{

		float phi = 0.5*(1+sqrt(5));
		float negphi = phi-1;
		float sqrt5 = sqrt(5);

		// close form S^n
		unsigned int S00 = 2/20*( (5-sqrt5)*negphi^n + (5+sqrt5)*phi^n );
		unsigned int S01 = 4/20*sqrt5* ( phi^n - negphi^n);
		unsigned int S10 = S01;
		unsigned int S11 = 2/20*( (5+sqrt5)*negphi^n + (5-sqrt5)*phi^n );

		return {{S00,S01}{S10,S11}}

	}


public:

	static int Get( unsigned int n)
	{
		if (n <=1) return n;
		else return multiply( power(n+1), {1,0} )[0];
	}


};
