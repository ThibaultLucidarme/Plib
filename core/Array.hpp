/**
 * TODO
 * default argument function pointer (MaxIdx / MaxValue / Sort)
 */

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <list>
#include <cmath>

namespace p
{
    /**
     An implementation of a dynamically allocated array class
     */
    template< class T>
    class Array
    {
    private:
        
        //dimension of the Array
        unsigned int _dimension;
        
        //size of each dimension
        unsigned int* _size;
        
        //number of elements in the Array
        unsigned int _length;
        
        //1D array containing the data
        T* _data;
        
        /*
         Quicksort algorithm
         is used by the public void sort( int (*Comparator)(T, T)) methode
         */
        void Sort(unsigned int left, unsigned int right, int (*Comparator)(T, T) )
        {
            int i = left, j = right;
            T	tmp;
            T	pivot = _data[(i + j) / 2];
            
            /* partition */
            while (i <= j)
            {
                while (Comparator(_data[i], pivot) < 0)
                    i++;
                while (Comparator(_data[i], pivot) > 0)
                    j--;
                if (i <= j)
                {
                    tmp		 = _data[i];
                    _data[i] = _data[j];
                    _data[j] = tmp;
                    i++;
                    j--;
                }
            }
            
            /* recursion */
            if (left < j)
                sort(left, j, Comparator);
            if (i < right)
                sort(i, right, Comparator);
        }
        
        
    public:
        
        /**
         First argument is number of dimension
         Rest is number of elements in each dimension
         */
        Array(unsigned int dim, ...)
        {
            _dimension = dim;
            _size	   = new unsigned int[_dimension];
            
            va_list ap;
            va_start(ap, dim);
            
            _length = 1;
            
            for (unsigned int i = 0; i < _dimension; i++)
            {
                _size[i] = va_arg(ap, unsigned int);
                _length *= _size[i];
            }
            
            _data = new T[_length];
        }
        
        /**
         First argument is number of dimension
         Rest is array of number of elements in each dimension
         */
        Array(unsigned int dim, unsigned int* size)
        {
            try
            {
                _dimension = dim;
                _size	   = new unsigned int[_dimension];
                _length	   = 1;
                for (int i = 0; i < _dimension; i++)
                {
                    _size[i] = size[i];
                    _length *= _size[i];
                }
                
                _data = new T[_length];
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        
        /**
         Offset initialisation
         */
        Create(unsigned int dim, unsigned int* size)
        {
            try
            {
                _dimension = dim;
                _size	   = new unsigned int[_dimension];
                _length	   = 1;
                for (int i = 0; i < _dimension; i++)
                {
                    _size[i] = size[i];
                    _length *= _size[i];
                }
                
                _data = new T[_length];
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        
        /**
         Offset initialisation
         */
        Create(unsigned int dim, unsigned int* size)
        {
            try
            {
                _dimension = dim;
                _size	   = new unsigned int[_dimension];
                _length	   = 1;
                for (int i = 0; i < _dimension; i++)
                {
                    _size[i] = size[i];
                    _length *= _size[i];
                }
                
                _data = new T[_length];
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        
        Array( const p::Array<T>& a) : _dimension(a._dimension), _length(a._length)
        {
            _size = new unsigned int[_dimension];
            for (unsigned int i = 0; i < _dimension; i++)
                _size[i] = a._size[i];
            
            _data = new T[_length];
            for (unsigned int i = 0; i < _length; i++)
                _data[i] = a._data[i];
        }
        
        /**
         Free dynamically allocated memory
         */
        ~Array()
        {
            delete[] _data;
            delete[] _size;
        }
        
        inline T& at(int idx1, ...)
        {
            va_list		   ap;
            va_start(ap, idx1);
            std::list<int> arg;
            int			   idx = 0;
            
            //Allow for negative indexing
            idx1 = (idx1 < 0) ? (_length + idx1) : idx1;
            
            for (unsigned int i = 1; i < _dimension; i++)
            {
                int value = va_arg(ap, int);
                value = (value < 0) ? (_size[i] + value) : value;
                arg.push_back(value);
            }
            
            if (!arg.empty() )
            {
                //flat[ (z * DEPTH +y) * WIDTH + x] = original[x, y, z]
                for (unsigned int i = _dimension - 1; i > 0; i--)
                {
                    idx = (idx + arg.back() ) * _size[i];
                    arg.pop_back();
                }
            }
            
            idx += idx1;
            
            if (idx >= _length)
                exit(EXIT_FAILURE);
            
            return _data[idx];
        }
        
        Array<unsigned int> toMultiDimIdx( int idx)
        {
            Array<unsigned int> result(1, this->_dimension);
            
            unsigned int sizedims = this->_length;
            
            if (idx > sizedims)
                exit(EXIT_FAILURE);
            
            for ( int k = _dimension - 1; k >= 0; k--)
            {
                sizedims	= std::floor( (double)sizedims / (double)_size[k]);       // reduction of the product by one term
                result.at(k) = std::floor( (double)idx / (double)sizedims ); // automatic flooring
                idx			-= sizedims * result.at(k);
            }
            
            if (idx != 0)
                exit(EXIT_FAILURE);
            
            return result;
        }
        
        /**
         Fills the entire array with value
         */
        inline void Fill(T value)
        {
            for (unsigned int i = 0; i < _length; i++)
                _data[i] = value;
        }
        
        /**
         return an array[dimension] that contains the size of the Array
         */
        unsigned int* size(void)
        {
            return _size;
        }
        
        /**
         returns the size of the i-th dimension
         */
        unsigned int size(unsigned int i)
        {
            unsigned int s;
            if (i >= _dimension)
                s = 0;
            else
                s = _size[i];
            
            return s;
        }
        
        /**
         returns the number of elements in the Array
         */
        unsigned int length(void)
        {
            return _length;
        }
        
        /**
         returns the dimension of the Array
         */
        unsigned int dimension(void)
        {
            return _dimension;
        }
        
        /*
         clears the content of the Array
         */
        void clear()
        {
            _dimension = 0;
            delete[] _size;
            _length = 0;
            delete[] _data;
        }
        
        /*
         quicksorts the elements
         */
        void Sort(int (*Comparator)(T, T) = std::less<T>() )
        {
            this->Sort(0, _length - 1, Comparator);   //private overload
        }
        
        /*
         reverses the order of the element in the flattened array
         */
        void Reverse()
        {
            T			 tmp;
            unsigned int idx1 = 0, idx2 = _length - 1;
            
            while (idx1 < idx2)
            {
                tmp			= _data[idx1];
                _data[idx1] = _data[idx2];
                _data[idx2] = tmp;
                
                idx1++;
                idx2--;
            }
        }
        
        /*
         returns the 1D index of the first element with the maximum value
         calculated using Comparator provided (default is less)
         */
        unsigned int MaxIdx(int (*Comparator)(T, T) = std::less<T>() )
        {
            unsigned int M;
            for (unsigned int i = 0; i < _length; i++)
                M = (Comparator(_data[M], _data[i]) > 0) ? M : i;
        }
        
        /*
         returns the maximum value in the array
         calculated using Comparator provided (default is less)
         */
        T MaxValue(int (*Comparator)(T, T) = std::less<T>() )
        {
            T M;
            for (unsigned int i = 0; i < _length; i++)
                M = (Comparator(M, _data[i]) >= 0) ? M : _data[i];
        }
    };
}

#endif


