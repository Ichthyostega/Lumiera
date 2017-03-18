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


/** @file try.cpp
 ** Metaprogramming: unified treatment of functors, function references and lambdas.
 ** 
 ** This investigation is a partial step towards #994 and became necessary to support
 ** Command definition by Lambda
 ** 
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/meta/function.hpp"

#include <functional>
#include <string>

using lib::meta::_Fun;

using std::function;
using std::placeholders::_1;
using std::bind;
using std::string;
using std::tuple;

int
funny (uint i)
{
  return -i+1;
}

struct Funky
  {
    int ii = 2;
    
    int
    fun (uint i2)
      {
        return ii + funny(i2);
      }
    
    int
    operator() (uint i2)
      {
        return 2*ii - fun(i2);
      }
    
    static int
    notfunny (uint i)
      {
        return 2*funny (i);
      }
  };



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;

#define EVAL_PREDICATE(_PRED_) \
    cout << STRINGIFY(_PRED_) << "\t : " << _PRED_ <<endl;


template<typename F>
void
showType (F)
  {
    using Sig = typename _Fun<F>::Sig;
    
    SHOW_TYPE (F);
    SHOW_TYPE (Sig);
  }


using Fun = function<int(uint)>;
using Fuk = function<int(Funky&, uint)>;

int
main (int, char**)
  {
    Fun f1{funny};
    Fun f2{&funny};
    
    Fun f3{Funky::notfunny};
    Fun f4{&Funky::notfunny};
    
    auto memfunP = &Funky::fun;
    
    Fuk f5{memfunP};
    
    Funky funk;
    
    Fun f6{bind (f5, funk, _1)};
    
    
    showType (funny);
    showType (&funny);
    showType (Funky::notfunny);
    
    showType (memfunP);
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
