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


/** @file try.cpp
 ** Rework of the template lib::Depend for singleton and service access.
 ** The (now for the third time rewritten) dependency factory can be configured to yield
 ** a subclass singleton, or to bind to an external service. Lazy initialisation relies on
 ** Double Checked Locking, which we need switch to C++11 Atomics in order to be correct
 ** (in theory). The impact of this initialisation guard should be investigated by benchmark.
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/depend.hpp"
#include "lib/depend-inject.hpp"
//#include "lib/meta/util.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/util.hpp"



#define SHOW_TYPE(_TY_) \
    cout << "typeof( " << STRINGIFY(_TY_) << " )= " << lib::meta::typeStr<_TY_>() <<endl;
#define SHOW_EXPR(_XX_) \
    cout << "Probe " << STRINGIFY(_XX_) << " ? = " << _XX_ <<endl;


  namespace error = lumiera::error;

  using lib::Depend;
  using lib::DependInject;
  
//////////////////////////////////////////////////////////////////////////Microbenchmark
#include "lib/test/microbenchmark.hpp"
using lib::test::microbenchmark;
//////////////////////////////////////////////////////////////////////////(End)Microbenchmark

#include "include/lifecycle.h"
#include "lib/test/testoption.hpp"
#include "lib/test/suite.hpp"

using lumiera::LifecycleHook;
using lumiera::ON_GLOBAL_INIT;
using lumiera::ON_GLOBAL_SHUTDOWN;

///////////////////////////////////////////////////////Usage

class BlackHoleService
  : util::NonCopyable
  {
    volatile int theHole_ = rand() % 1000;
    
    public:
      int readMe() { return theHole_; }
  };


int
main (int, char**)
  {
    std::srand(std::time(nullptr));
    LifecycleHook::trigger (ON_GLOBAL_INIT);
    
    Depend<BlackHoleService> mystery;
    
    thread_local int64_t cnt = 0;
    
    cout << microbenchmark<8> ([&]()
                                 {
                                   cnt += mystery().readMe();
                                 }
                              ,50000000)
         << endl;
    
    LifecycleHook::trigger (ON_GLOBAL_SHUTDOWN);
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
