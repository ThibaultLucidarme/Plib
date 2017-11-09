#include <iostream>
#include <bitset>

/*
Time Complexity: Time complexity of the above solution is O(n^1.59). Can be further improved using another Divide and Conquer algorithm, fast Fourier transform.

TODO: Extend the above program to return a string instead of a long int value.
*/


class Integer // int/long int/long long
// implemented as non-members to maintain symmetry int*long = long*int
{
public:
	Integer& operator*=(const Integer& b)
	{
		// transform into equal length binary strings
		int length = ( ( sizeof(*this) > sizeof(b)) ? sizeof(*this) : sizeof(b) )*CHAR_BIT;
		std::string binary1 = std::bitset< length >(*this).to_string();
		std::string binary2 = std::bitset< length >(b).to_string();


		*this = multiplyBitStrings(binary1,binary2); 

		//TODO
		// string binaryResult = multiplyBitStrings(binary1,binary2);
		// *this = std::bitset< binaryResult.length() >( binaryResult ).to_ulong();

	   	return *this;

	}

	friend Integer operator*( Integer a, const Integer& b)
	{
		a *= b;
		return a;
	}


private:
	string addBitStrings( string first, string second )
	{
	    string result;  // To store the sum bits
	 
	    // make the lengths same before adding
	    int length = makeEqualLength(first, second);
	    int carry = 0;  // Initialize carry
	 
	    // Add all bits one by one
	    for (int i = length-1 ; i >= 0 ; i--)
	    {
	        int firstBit = first.at(i) - '0';
	        int secondBit = second.at(i) - '0';
	 
	        // boolean expression for sum of 3 bits
	        int sum = (firstBit ^ secondBit ^ carry)+'0';
	 
	        result = (char)sum + result;
	 
	        // boolean expression for 3-bit addition
	        carry = (firstBit&secondBit) | (secondBit&carry) | (firstBit&carry);
	    }
	 
	    // if overflow, then add a leading 1
	    if (carry)  result = '1' + result;
	 
	    return result;
	}
	 
	// The main function that multiplies two bit strings X and Y and returns
	// result as long integer
	long int multiplyBitStrings(string X, string Y)
	{
		//length
		int length = (X.length()>X.length()) ? X.length() : X.length();

		// Base cases
	    if (length == 0) return 0;
	    if (length == 1) return (a[0] - '0')*(b[0] - '0');
	 
	 	// Divide and Conquer
	    int fh = length/2;   // First half of string, floor(n/2)
	    int sh = (length-fh); // Second half of string, ceil(n/2)
	 
	    // Find the first half and second half of first string.
	    string a1 = X.substr(0, fh);
	    string a2 = X.substr(fh, sh);
	 
	    // Find the first half and second half of second string
	    string b1 = Y.substr(0, fh);
	    string b2 = Y.substr(fh, sh);
	 
	    // Recursively calculate the three products of inputs of size n/2
	    long int P1 = multiplyBitStrings(a1, b1);
	    long int P2 = multiplyBitStrings(a2, b2);
	    long int P3 = multiplyBitStrings(addBitStrings(a1, a2), addBitStrings(b1, b2));
	 
	    // Combine the three products to get the final result.
	    return P1*(1<<(2*sh)) + (P3 - P1 - P2)*(1<<sh) + P2;
	}
};