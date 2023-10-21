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


/** @file try.cpp
 * Investigate how to detect the signature of a _function-like member,_ irrespective
 * if referring to a static function, a member function or a functor member. Turns out this
 * can be achieved in a syntactically uniform way by passing either a pointer or member pointer.
 * @see vault::gear::_verify_usable_as_ExecutionContext
 * @see lib::meta::isFunMember
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/util.hpp"

#include "lib/meta/function.hpp"

struct Stat
  {
    static long fun (double, char*) {return 42; }
  };

struct Funi
  {
    std::function<long(double, char*)> fun;
    short gun;
  };

struct Dyna
  {
    long fun (double, char*) const {return 42; }
  };


using lib::meta::_Fun;


  /** @deprecated this is effectively the same than using decltype */
  template<typename P>
  struct Probe
    : _Fun<P>
    {
      Probe(P&&){}
    };

  template<typename FUN, typename SIG, bool =_Fun<FUN>()>
  struct has_SIGx
    : std::is_same<SIG, typename _Fun<FUN>::Sig>
    {
//      has_SIGx() = default;
//      has_SIGx(FUN, _Fun<SIG>){ }
    };

  template<typename FUN, typename X>
  struct has_SIGx<FUN,X,false>
    : std::false_type
    {
//      has_SIGx() = default;
//      has_SIGx(FUN, _Fun<X>){ }
    };
  
  
  
  template<typename SIG, typename FUN>
  constexpr inline auto
  isFunMember (FUN)
  {
    return has_SIGx<FUN,SIG>{};
  }
  
#define ARSERT_MEMBER_FUNCTOR(_EXPR_, _SIG_) \
        static_assert (isFunMember<_SIG_>(_EXPR_), \
                       "Member " STRINGIFY(_EXPR_) " unsuitable, expect function signature: " STRINGIFY(_SIG_));

int
main (int, char**)
  {
    using F1 = decltype(Stat::fun);
    using F2 = decltype(Funi::fun);
    using F3 = decltype(&Dyna::fun);
    
    SHOW_TYPE(F1)
    SHOW_TYPE(F2)
    SHOW_TYPE(F3)
    
    using F1a = decltype(&Stat::fun);
    using F2a = decltype(&Funi::fun);
    using F2b = decltype(&Funi::gun);
    SHOW_TYPE(F1a)
    SHOW_TYPE(F2a)
    SHOW_TYPE(F2b)
    
    SHOW_TYPE(_Fun<F1>::Sig)
    SHOW_TYPE(_Fun<F2>::Sig)
    SHOW_TYPE(_Fun<F3>::Sig)

    SHOW_TYPE(_Fun<F1a>::Sig)
    SHOW_TYPE(_Fun<F2a>::Sig)
    
    SHOW_EXPR(_Fun<F2a>::value)
    SHOW_EXPR(_Fun<F2b>::value)
    cout <<  "\n--------\n";
    
    SHOW_EXPR(bool(isFunMember<long(double,char*)>(&Stat::fun)))
    SHOW_EXPR(bool(isFunMember<long(double,char*)>(&Funi::fun)))
    SHOW_EXPR(bool(isFunMember<long(double,char*)>(&Funi::gun)))
    SHOW_EXPR(bool(isFunMember<long(double,char*)>(&Dyna::fun)))
    
    ARSERT_MEMBER_FUNCTOR (&Stat::fun, long(double,char*));
    ARSERT_MEMBER_FUNCTOR (&Dyna::fun, long(double,char*));
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
