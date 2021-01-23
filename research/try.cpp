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


/** @file try.cpp
 * Verify a way to detect the presence of a specific implementation function,
 * even when it is just inherited and thus not part of the concrete class definition.
 * The trick is to _emulate the use_ of the object method in question within a `decltype( )`
 * statement, which in turn is used to build the concrete template signature.
 */

typedef unsigned int uint;


#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/meta/duck-detector.hpp"

#include <utility>
#include <string>

using std::string;

using lib::idi::EntryID;
using lib::idi::BareEntryID;
using lib::meta::Yes_t;
using lib::meta::No_t;


#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


META_DETECT_FUNCTION(BareEntryID const&, getID,(void) const);
META_DETECT_FUNCTION_ARGLESS(getID);

template<typename TY>
    class Can_retrieve_and_compare_ID
      {
        template<typename X,
                 typename SEL = decltype(std::declval<BareEntryID>() == std::declval<X>().getID())>
        struct Probe
          { };                                                    \
                                                                   \
        template<class X>                                           \
        static Yes_t check(Probe<X> * );                             \
        template<class>                                               \
        static No_t  check(...);                                       \
                                                                        \
      public:                                                            \
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));  \
      };


class Base
  {
    EntryID<Base> idi;
  public:
    BareEntryID const&
    getID() const
      {
        return idi;
      }
  };

class Derived
  : public Base
  { };

class Derailed
  { };


int
main (int, char**)
  {
    Base b1;
    Derived d1;
    Derailed r1;
    SHOW_EXPR( b1 );
    SHOW_EXPR( b1.getID() );
    SHOW_EXPR( HasFunSig_getID<Base>::value );
    SHOW_EXPR( HasArglessFun_getID<Base>::value );
    SHOW_EXPR( Can_retrieve_and_compare_ID<Base>::value );
    
    SHOW_EXPR( d1 );
    SHOW_EXPR( d1.getID() );
    SHOW_EXPR( HasFunSig_getID<Derived>::value );
    SHOW_EXPR( HasArglessFun_getID<Derived>::value );
    SHOW_EXPR( Can_retrieve_and_compare_ID<Derived>::value );
    
    SHOW_EXPR( r1 );
    SHOW_EXPR( HasFunSig_getID<Derailed>::value );
    SHOW_EXPR( HasArglessFun_getID<Derailed>::value );
    SHOW_EXPR( Can_retrieve_and_compare_ID<Derailed>::value );
    
//    SHOW_TYPE( decltype( d1.getID() ))
    
    cout <<  "\n.gulp.\n";
    return 0;
  }
