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
// 11/17 - metaprogramming to detect the presence of extension points
// 11/17 - detect generic lambda


/** @file try.cpp
 ** Metaprogramming: is it possible to distinguish a generic lambda from something not a function at all?
 ** Answer: not really. We can only ever check for the function call operator.
 ** Even worse: if we instantiate a templated function call operator with unsuitable parameter types,
 ** the compilation as such fails. Whereas SFINAE is only limited to substituting a type signature.  
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/format-util.hpp"
#include "lib/meta/function.hpp"
#include "lib/test/test-helper.hpp"

//#include <functional>
#include <utility>
#include <string>

using lib::meta::No_t;
using lib::meta::Yes_t;
using lib::meta::_Fun;
using lib::test::showSizeof;

using std::function;
using std::forward;
using std::move;
using std::string;


namespace lib {
namespace meta{
  
  
  template<class FUN, typename...ARGS>
  struct ProbeFunctionInvocation
    {
      using Ret  = decltype(std::declval<FUN>() (std::declval<ARGS>()...));
      using Args = Types<ARGS...>;
      using Sig  = Ret(ARGS...);
    };
  
}}

using lib::meta::enable_if;


#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


long
funny(int i)
  {
    return i+1;
  }


template<typename FUN, typename SEL =void>
struct FunTrait
  {
    using XXX = decltype(std::declval<FUN>() (0));
    
    static string doIt() { return "Uh OH:" + lib::meta::typeStr<XXX>(); }
  };

template<typename FUN>
struct FunTrait<FUN,  enable_if<_Fun<FUN>> >
  {
    static string doIt() { return "Yeah FUN:" + lib::meta::typeStr<typename _Fun<FUN>::Sig>(); }
  };

int
main (int, char**)
  {
    auto lamb1 = [](int i)  { return double(i) / (i*i); };
    auto lamb2 = [](auto i) { return double(i) / (i*i); };
    
    SHOW_TYPE (decltype(lamb1));
    SHOW_TYPE (decltype(lamb2));
    
    SHOW_EXPR ((_Fun<decltype(lamb1)>::value));
    SHOW_EXPR ((_Fun<decltype(lamb2)>::value));
    SHOW_EXPR ((_Fun<decltype(funny)>::value));
    SHOW_EXPR ((_Fun<decltype(&funny)>::value));
    
    auto funky = function<double(float)> (lamb2);
    SHOW_EXPR ((_Fun<decltype(funky)>::value));
    
    cout << FunTrait<decltype(lamb1)>::doIt() <<endl;
    cout << FunTrait<decltype(lamb2)>::doIt() <<endl;
    cout << FunTrait<decltype(funny)>::doIt() <<endl;
    cout << FunTrait<decltype(funky)>::doIt() <<endl;
    
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
