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
// 12/17 - investigate SFINAE failure. Reason was indirect use while in template instantiation
// 03/18 - Dependency Injection / Singleton initialisation / double checked locking
// 04/18 - investigate construction of static template members
// 08/18 - Segfault when compiling some regular expressions for EventLog search
// 10/18 - investigate insidious reinterpret cast
// 12/18 - investigate the trinomial random number algorithm from the C standard lib
// 04/19 - forwarding tuple element(s) to function invocation
// 06/19 - use a statefull counting filter in a treeExplorer pipeline
// 03/20 - investigate type deduction bug with PtrDerefIter
// 01/21 - look for ways to detect the presence of an (possibly inherited) getID() function
// 08/22 - techniques to supply additional feature selectors to a constructor call
// 10/23 - search for ways to detect signatures of member functions and functors uniformly
// 11/23 - prototype for a builder-DSL to configure a functor to draw and map random values


/** @file try.cpp
 * Prototyping to find a suitable DSL to configure drawing of random numbers and mapping results.
 * The underlying implementation shall be extracted from (and later used by) TestChainLoad; the
 * random numbers will be derived from node hash values and must be mapped to yield parameters
 * limited to a very small value range. While numerically simple, this turns out to be rather
 * error-prone, hence the desire to put a DSL in front. The challenge however arises from
 * the additional requirement to support various usage patters, all with minimal specs.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include "lib/meta/function.hpp"
#include <functional>

using std::function;

template<typename T, T max>
struct Limited
  {
    static constexpr T min() { return T(0); }
    static constexpr T max() { return max;  }
    
    T val;
    
    template<typename X>
    Limited (X raw)
      : val(util::limited (X(min()), raw, X(max())))
      { }
  };

template<typename T, T max>
struct Spec
  {
    using Lim = Limited<T,max>;
    static constexpr double CAP_EPSILON = 0.001;
        
    double lower{0};
    double upper{max};
    
    Spec() = default;
    
    Lim
    limited (double val)
      {
        if (val==lower)
          return Lim{0};
        val -= lower;
        val /= upper-lower;
        val *= max;
        val += CAP_EPSILON;
        return Lim{val};
      }
      
  };

template<typename T, T max>
struct Draw
  : Spec<T,max>
  , function<Limited<T,max>(size_t)>
  {
    using Spc = Spec<T,max>;
    using Lim = typename Spc::Lim;
    using Fun = function<Lim(size_t)>;
    
    Draw()
      : Spc{}
      , Fun{[](size_t){ return Spc{}.limited(0); }}
      { }
      
    template<class FUN>
    Draw(FUN fun)
      : Fun{fun}
      { }
  };

int
main (int, char**)
  {
    using D = Draw<uint,16>;
    using L = typename D::Lim;
    using S = typename D::Spc;
    D draw;
SHOW_EXPR(draw)
SHOW_EXPR(draw(5).val)
    draw = D{[](size_t i){ return S{}.limited(i); }};
    
SHOW_EXPR(draw(5).val)
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
