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
#include "lib/meta/variadic-helper.hpp"
#include "lib/test/test-helper.hpp"

#include <functional>
#include <utility>
#include <string>
#include <array>

using lib::meta::_Fun;
using lib::test::showSizeof;

using std::function;
using std::forward;
using std::move;
using std::string;




#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;


template<typename...ARGS>
void
fun2 (ARGS... is)
  {
    cout << "FUN-"<<sizeof...(is)<<": "<<util::join({is...}, " ") <<endl;
  }

void
fun2 ()
  {
    cout << "NO FUN" <<endl;
  }


using lib::meta::IndexSeq;
using lib::meta::BuildIndexSeq;
using lib::meta::BuildIdxIter;


template<size_t i, typename INIT=void>
struct SelectVararg
  { 
    template<typename ARG, typename...ARGS>
    static auto
    get (ARG, ARGS ...args)
      {
        return SelectVararg<i-1,INIT>::get (args...);
      }
    
    static INIT get() { return INIT{}; }
  };

template<typename INIT>
struct SelectVararg<0, INIT>
  { 
    template<typename ARG, typename...ARGS>
    static ARG
    get (ARG a, ARGS...)
      {
        return a;
      }
    
    static INIT get() { return INIT{}; }
  };

template<size_t idx, typename...ARGS>
constexpr inline auto
pickArg (ARGS... args)
  {
    return SelectVararg<idx>::get (args...);
  }

template<size_t idx, typename INIT, typename...ARGS>
constexpr inline auto
pickInit (ARGS... args)
  {
    return SelectVararg<idx, INIT>::get (args...);
  }



using Arr = std::array<int, 3>;


template<size_t...idx1, size_t...idx2, typename...ARGS>
Arr
dispatch_ (IndexSeq<idx1...>,IndexSeq<idx2...>, ARGS...args)
  {
    Arr arr{pickInit<idx1,int> (args...) ...};
    fun2 (pickArg<idx2> (args...) ...);
    return arr;
  }

template<typename...ARGS>
Arr
dispatch (ARGS...args)
  {
    enum {SIZ = sizeof...(args)};
    
    using First = typename BuildIndexSeq<3>::Ascending;
    using Next  = typename BuildIdxIter<ARGS...>::template After<3>;
    
    return dispatch_ (First(),Next(), args...);
  }


int
main (int, char**)
  {
    fun2 (1,2,3,4);
    fun2 (5,6);
    fun2 ();
    
    auto arr = dispatch (2,3,4,5,6,7,8);
    cout << util::join(arr) << "| " << showSizeof(arr) <<endl;
    
    arr = dispatch (7,8);
    cout << util::join(arr);
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
