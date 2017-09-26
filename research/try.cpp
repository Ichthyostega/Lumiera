/* try.cpp  -  for trying out some language features....
 *             scons will create the binary bin/try
 *
 */

// 8/07  - how to control NOBUG??
//         execute with   NOBUG_LOG='ttt:TRACE' bin/try
// 1/08  - working out a static initialisation problem for Visitor (Tag creation)
// 1/08  - check 64bit longs
// 4/08  - comparison operators on shared_ptr<Asset>
// 4/08  - conversions on the value_type used for boost::any
// 5/08  - how to guard a downcasting access, so it is compiled in only if the involved types are convertible
// 7/08  - combining partial specialisation and subclasses 
// 10/8  - abusing the STL containers to hold noncopyable values
// 6/09  - investigating how to build a mixin template providing an operator bool()
// 12/9  - tracking down a strange "warning: type qualifiers ignored on function return type"
// 1/10  - can we determine at compile time the presence of a certain function (for duck-typing)?
// 4/10  - pretty printing STL containers with python enabled GDB?
// 1/11  - exploring numeric limits
// 1/11  - integer floor and wrap operation(s)
// 1/11  - how to fetch the path of the own executable -- at least under Linux?
// 10/11 - simple demo using a pointer and a struct
// 11/11 - using the boost random number generator(s)
// 12/11 - how to detect if string conversion is possible?
// 1/12  - is partial application of member functions possible?
// 5/14  - c++11 transition: detect empty function object
// 7/14  - c++11 transition: std hash function vs. boost hash
// 9/14  - variadic templates and perfect forwarding
// 11/14 - pointer to member functions and name mangling
// 8/15  - Segfault when loading into GDB (on Debian/Jessie 64bit
// 8/15  - generalising the Variant::Visitor
// 1/16  - generic to-string conversion for ostream
// 1/16  - build tuple from runtime-typed variant container
// 3/17  - generic function signature traits, including support for Lambdas
// 9/17  - manipulate variadic templates to treat varargs in several chunks


/** @file try.cpp
 ** Metaprogramming: manipulations on variadic template argument packs.
 ** Investigation how to transform a parameter pack such as to forward a fixed chunk
 ** to one sub-ctor and delegate the rest to a tail recursive call.
 ** 
 ** This investigation was spurred by an attempt to create an inline storage layout
 ** with possible heap-based extension. Being able to build such an object layout
 ** would enable several possibly interesting optimisations when most usage scenarios
 ** of a class will only use a small element count, while some usages still require
 ** an essentially unlimited open number of elements.
 ** 
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
//#include "lib/meta/function.hpp"
#include "lib/meta/tuple-helper.hpp"

#include <functional>
#include <utility>
#include <string>

using lib::meta::_Fun;

using std::function;
using std::forward;
using std::move;
using std::string;




#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;


void
fun1 (int i, int j, int k)
  {
    cout << "FUN-1: "<<i<<" "<<j<<" "<<k<<" " <<endl;
  }

void
fun2 (int const& i, int const& j, int const& k)
  {
    cout << "FUN-2: "<<i<<" "<<j<<" "<<k<<" " <<endl;
  }


  /** Hold a sequence of index numbers as template parameters */
  template<size_t...idx>
  struct IndexSeq
    {
      template<size_t i>
      using AppendElm = IndexSeq<idx..., i>;
    };
  
  /** build an `IndexSeq<0, 1, 2, ..., n-1>` */
  template<size_t n>
  struct BuildIndexSeq
    {
      using Ascending = typename BuildIndexSeq<n-1>::Ascending::template AppendElm<n-1>;
      
      template<size_t i>
      using FilledWith = typename BuildIndexSeq<n-1>::template FilledWith<i>::template AppendElm<i>;
    };
  
  template<>
  struct BuildIndexSeq<0>
    {
      using Ascending = IndexSeq<>;
      
      template<size_t>
      using FilledWith = IndexSeq<>;;
    };


template<size_t i, typename...ARGS>
struct Pick;

template<size_t i, typename ARG, typename...ARGS>
struct Pick<i, ARG,ARGS...>
  : Pick<i-1, ARGS...>
  { 
    static auto
    get (ARG, ARGS ...args)
      {
        return Pick<i-1, ARGS...>::get (args...);
      }
  };

template<typename ARG, typename...ARGS>
struct Pick<0, ARG,ARGS...>
  { 
    static ARG
    get (ARG a, ARGS...)
      {
        return a;
      }
  };

template<size_t...idx, typename...ARGS>
void
dispatch_ (IndexSeq<idx...>, ARGS...args)
  {
    
    fun1 (Pick<idx,ARGS...>::get(args...) ...);
  }

template<typename...ARGS>
void
dispatch (ARGS...args)
  {
    enum {SIZ = sizeof...(args)};
    
    using First = typename BuildIndexSeq<3>::Ascending;
    
    dispatch_ (First{}, args...);
  }


int
main (int, char**)
  {
    fun1 (1,2,3);
    fun2 (4,5,6);
    
    dispatch (2,3,4,5,6,7,8);
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
