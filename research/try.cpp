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
 * 
 * The following code lays out the ground structure, while treating Spec as a distinct
 * type, which is then mixed into Draw. This logical separation basically was led me to the
 * final solution: Draw both _is_ a function and _embodies_ the implementation of this function.
 * This somewhat surprising layout is what enables use as a DSL builder, because it allows both
 * to have the _builder use_ and the _converter use_ in the same class, even allowing to _define_
 * a Draw by giving a function which _produces_ a (dynamically parametrised) Draw.
 * 
 * In this prototype, all of the functor adaptation is also part of the Draw template; for the
 * real implementation this will have to be supplied at usage site through a traits template,
 * otherwise it would not be possible to integrate seamlessly with custom data sources (as
 * happens in the intended use case, where actually a Node is the data source)
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include "lib/meta/function.hpp"
#include <functional>
#include <utility>

using lib::meta::_Fun;
using std::function;
using std::forward;
using std::move;

template<typename T, T max>
struct Limited
  {
    static constexpr T minVal() { return T(0); }
    static constexpr T maxVal() { return max;  }
    
    T val;
    
    template<typename X>
    Limited (X raw)
      : val(util::limited (X(minVal()), raw, X(maxVal())))
      { }
  };

template<typename T, T max>
struct Spec
  {
    using Lim = Limited<T,max>;
    static constexpr double CAP_EPSILON = 0.0001;
        
    double probability{0};
    T maxResult{Lim::maxVal()};
    
    Spec() = default;
    
    explicit
    Spec (double p) : probability{p}{ }
    
    Lim
    limited (double val)
      {
        if (probability == 0.0 or val == 0.0)
          return Lim{0};
        double q = (1.0 - probability);
        val -= q;
        val /= probability;
        val *= maxResult;
        val += 1 + CAP_EPSILON;
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
      , Fun{[this](size_t hash){ return Spc::limited (asRand (hash)); }}
      { }
      
    template<class FUN>
    Draw(FUN&& fun)
      : Spc{1.0}
      , Fun{adaptOut(adaptIn(std::forward<FUN> (fun)))}
      { }
    
    
    Draw&&
    probability (double p)
      {
        Spc::probability = p;
        return move (*this);
      }
    
    Draw&&
    maxVal (uint m)
      {
        Spc::maxResult = m;
        return move (*this);
      }

    template<class FUN>
    Draw&&
    mapping (FUN&& fun)
      {
        Fun(*this) = adaptOut(adaptIn(std::forward<FUN> (fun)));
        return move (*this);
      }
    
    
    
    double
    asRand (size_t hash)
      {
        return double(hash % 256)/256;
      }
    
    /**
     * @internal helper to expose the signature `size_t(size_t)`
     *           by wrapping a given lambda or functor.
     */
    template<class SIG, typename SEL=void>
    struct Adaptor
      {
        static_assert (not sizeof(SIG), "Unable to adapt given functor.");
      };
    
    template<typename RES>
    struct Adaptor<RES(size_t)>
      {
        template<typename FUN>
        static decltype(auto)
        build (FUN&& fun)
          {
            return std::forward<FUN>(fun);
          }
      };
    
    template<typename RES>
    struct Adaptor<RES(void)>
      {
        template<typename FUN>
        static auto
        build (FUN&& fun)
          {
            return [functor=std::forward<FUN>(fun)]
                   (size_t)
                      {
                        return functor();
                      };
          }
      };
    
    
    template<class FUN>
    decltype(auto)
    adaptIn (FUN&& fun)
      {
        static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
        static_assert (lib::meta::_Fun<FUN>::ARITY <= 1, "Function with zero or one argument expected.");
        
        using Sig = typename lib::meta::_Fun<FUN>::Sig;
        
        return Adaptor<Sig>::build (forward<FUN> (fun));
      }
    
    template<class FUN>
    decltype(auto)
    adaptOut (FUN&& fun)
      {
        static_assert (lib::meta::_Fun<FUN>(), "Need something function-like.");
        static_assert (lib::meta::_Fun<FUN>::ARITY ==1, "Function with exactly one argument required.");
        
        using Res = typename lib::meta::_Fun<FUN>::Ret;
        
        if constexpr (std::is_same_v<Res, Lim>)
          return std::forward<FUN>(fun);
        else
        if constexpr (std::is_same_v<Res, size_t>)
          return [functor=std::forward<FUN>(fun), this]
                 (size_t rawHash)
                    {
                      size_t hash = functor(rawHash);
                      double randomNum = asRand (hash);
                      return Spc::limited (randomNum);
                    };
        else
        if constexpr (std::is_same_v<Res, double>)
          return [functor=std::forward<FUN>(fun), this]
                 (size_t rawHash)
                    {
                      double randomNum = functor(rawHash);
                      return Spc::limited (randomNum);
                    };
        else
        if constexpr (std::is_same_v<Res, Draw>)
          return [functor=std::forward<FUN>(fun), this]
                 (size_t rawHash)
                    {
                      Draw parametricDraw = functor(rawHash);
                      return parametricDraw (rawHash);
                    };
        else
          static_assert (not sizeof(Res), "unable to adapt / handle result type");
        NOTREACHED("Handle based on return type");
      }
    
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
    
    draw = D{[](size_t i){ return 0.75; }};
SHOW_EXPR(draw(5).val)
    
    draw = D{}.probability(0.25).maxVal(5);
SHOW_EXPR(draw(256-65).val)
SHOW_EXPR(draw(256-64).val)
SHOW_EXPR(draw(256-64+ 1).val)
SHOW_EXPR(draw(256-64+10).val)
SHOW_EXPR(draw(256-64+11).val)
SHOW_EXPR(draw(256-64+12).val)
SHOW_EXPR(draw(256-64+13).val)
SHOW_EXPR(draw(256-64+23).val)
SHOW_EXPR(draw(256-64+24).val)
SHOW_EXPR(draw(256-64+25).val)
SHOW_EXPR(draw(256-64+26).val)
SHOW_EXPR(draw(256-64+36).val)
SHOW_EXPR(draw(256-64+37).val)
SHOW_EXPR(draw(256-64+38).val)
SHOW_EXPR(draw(256-64+39).val)
SHOW_EXPR(draw(256-64+49).val)
SHOW_EXPR(draw(256-64+50).val)
SHOW_EXPR(draw(256-64+51).val)
SHOW_EXPR(draw(256-64+52).val)
SHOW_EXPR(draw(256-64+62).val)
SHOW_EXPR(draw(256-64+63).val)
SHOW_EXPR(draw(256).val)
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
