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
 */

typedef unsigned int uint;

#include "lib/format-cout.hpp"
#include "lib/depend2.hpp"
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
#include "backend/thread-wrapper.hpp"
#include <chrono>
#include <vector>
  
  namespace {
    constexpr size_t NUM_MEASUREMENTS = 10000000;
    constexpr double SCALE = 1e6;                  // Results are in µ sec
  }
  
  
  /** perform a multithreaded microbenchmark.
   * This function fires up a number of threads
   * and invokes the given test subject repeatedly.
   * @tparam number of threads to run in parallel
   * @param subject `void(void)` function to be timed
   * @return the averaged invocation time in _mircroseconds_
   * @remarks - the subject function will be _copied_ into each thread
   *          - so `nThreads` copies of this function will run in parallel
   *          - consider locking if this function accesses a shared closure.
   *          - if you pass a lambda, it is eligible for inlining followed
   *            by loop optimisation -- be sure to include some action, like
   *            e.g. accessing a volatile variable, to prevent the compiler
   *            from optimising it away entirely.
   */
  template<size_t nThreads, class FUN>
  double
  microbenchmark(FUN const& subject)
  {
    using backend::ThreadJoinable;
    using std::chrono::system_clock;
    
    using Dur = std::chrono::duration<double>;
    
    struct Thread
      : ThreadJoinable
      {
        Thread(FUN const& subject)
          : ThreadJoinable("Micro-Benchmark"
                          ,[subject, this]()       // local copy of the test-subject-Functor
                                    {
                                      syncPoint(); // block until all threads are ready
                                      auto start = system_clock::now();
                                      for (size_t i=0; i < NUM_MEASUREMENTS; ++i)
                                        subject();
                                      duration = system_clock::now () - start;
                                    })
          { }
        /** measured time within thread */
        Dur duration{};
      };
    
    std::vector<Thread> threads;
    threads.reserve(nThreads);
    for (size_t n=0; n<nThreads; ++n)              // create test threads
      threads.emplace_back (subject);

    for (auto& thread : threads)
      thread.sync();                               // start timing measurement
    
    Dur sumDuration{0.0};
    for (auto& thread : threads)
      {
        thread.join();                             // block on measurement end
        sumDuration += thread.duration;
      }
    
    return sumDuration.count() / (nThreads * NUM_MEASUREMENTS) * SCALE;
  }
//////////////////////////////////////////////////////////////////////////(End)Microbenchmark

#include "include/lifecycle.h"
#include "lib/test/testoption.hpp"
#include "lib/test/suite.hpp"

using lumiera::LifecycleHook;
using lumiera::ON_GLOBAL_INIT;
using lumiera::ON_GLOBAL_SHUTDOWN;

///////////////////////////////////////////////////////Usage

int
main (int, char**)
  {
    std::srand(std::time(nullptr));
    LifecycleHook::trigger (ON_GLOBAL_INIT);
    
//  DependInject<long>::useSingleton ([&] { return "long{rand() % 100}"; });
//  DependInject<long>::Local<std::string> dummy ([&]{ return new long{rand() % 100}; });
    
    volatile int blackHole{0};
    
    cout << "pling..."<<endl;
    cout << "plong..."<< microbenchmark<8> ([&]()
                                              {
                                                //volatile int dummy =0;
                                                //dummy == 0;
                                                //++dummy;
                                                blackHole == 0;
                                                //++blackHole;
                                              })
                      << endl;
    cout << "........"<< blackHole/8<<endl;
    
    LifecycleHook::trigger (ON_GLOBAL_SHUTDOWN);
    cout <<  "\n.gulp.\n";
    
    return 0;
  }
