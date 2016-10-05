#include <type_traits>
#include <iterator>
#include <vector>
#include <utility>


//USE SFINAE
// https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error
// https://rmf.io/cxx11/almost-static-if/

template <typename T>
struct is_iterator {
  static char test(...);

  template <typename U,
  typename=typename std::iterator_traits<U>::difference_type,
  typename=typename std::iterator_traits<U>::pointer,
  typename=typename std::iterator_traits<U>::reference,
  typename=typename std::iterator_traits<U>::value_type,
  typename=typename std::iterator_traits<U>::iterator_category
  > 
  static long test(U&&);

  constexpr static bool value = std::is_same<decltype(test(std::declval<T>())),long>::value;
};

struct Foo {};


//Returns true
bool f() { return is_iterator<typename std::vector<int>::iterator>::value; }
//Returns true    
bool fc() { return is_iterator<typename std::vector<int>::const_iterator>::value; }
//Returns true
bool fr() { return is_iterator<typename std::vector<int>::reverse_iterator>::value; }
//Returns true
bool fcr() { return is_iterator<typename std::vector<int>::const_reverse_iterator>::value; }
//Returns true
bool g() { return is_iterator<int*>::value; }
//Returns true
bool gc() { return is_iterator<const int*>::value; }
//Returns false
bool h() { return is_iterator<int>::value; }
//Returns false
bool i() { return is_iterator<Foo>::value; }














#include <iostream>

namespace detail {
    enum class enabler {};
}

template <typename Condition>
using EnableIf = typename std::enable_if<Condition::value, detail::enabler>::type;


template <typename T>
struct has_iterator {
    // Types "yes" and "no" are guaranteed to have different sizes,
    // specifically sizeof(yes) == 1 and sizeof(no) == 2.
    typedef char yes[1];
    typedef char no[2];

    template <typename C>
    static yes& test(typename C::iterator*);

    template <typename>
    static no& test(...); // (An ellipsis is used not only because it will accept any argument, but also because its conversion rank is lowest, so a call to the first function will be preferred if it is possible; this removes ambiguity.)

    // If the "sizeof" of the result of calling test<T>(nullptr) is equal to sizeof(yes),
    // the first overload worked and T has a nested type named foobar.
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
};

template <typename T,
          EnableIf<has_iterator<T>>...>
T isLast(T t) { return t.next(); }





void main(void)
{

  std::cout<<

  has_iterator<typename std::vector<int>::const_reverse_iterator>::value

  <<std::endl;
}