/*
  LocalSlice(Test)  -  verify access to thread-local services

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file local-slice-test.cpp
 ** unit test \ref LocalSlice_test
 */

#include "test/run.hpp"
#include "test/test-helper.hpp"
#include "lib/scoped-collection.hpp"
#include "test/microbenchmark.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/format-cout.hpp"
#include "lib/thread.hpp"

#include "lib/local-slice.hpp"

using lib::Thread;
using lib::explore;
using test::threadBenchmark;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""us;


namespace lib {
namespace test {
  
  namespace { // test fixture...
    
    const uint NUM_THREADS = 200;
    
    constexpr size_t CONCURR_BENCH = 4;
    const size_t REPETITIONS_BENCH = 1'000'000;
    
    /**
     * the "service" to be instantiated thread-local
     */
    class DummyBackend
      {
        uint step{0};
      public:
        uint
        getStep()
          {
            return ++step;
          }
      };
    
    using DummySlice = LocalSlice<DummyBackend>;
    
    
    /**
     * A common shared service instance
     * that dispatches through LocalSlice
     * into the thread-local service back-ends
     */
    class DummyFrontend
      {
        DummySlice dummySrv_;
        
      public:
        uint
        invoke()
          {
            return dummySrv_->getStep()
                 + dummySrv_->getStep()
                 + dummySrv_->getStep()
                 + dummySrv_->getStep()
                 ;
          }
      };
     //
  } //(End) test fixture
  
  
  /***********************************************************************//**
   * @test demonstrate access to thread-local service instances.
   *     - setup a common front-end service instance
   *     - its functionality is implemented by delegating
   *       through the LocalSlice into the thread-local back-ends
   *     - launch threads and invoke the front-end concurrently
   * @see local-slice.hpp
   */
  class LocalSlice_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          investigatePerformance();
        }
      
      
      /** @test access thread-local service back-ends
       *        through a shared front-end instance;
       *        verify that each thread invokes a
       *        separate instance without interference
       * @see ThreadWrapper_test
       */
      void
      simpleUsage ()
        {
          // Σi ≡ N/2·(N+1)
          auto expectSum = [](uint n){ return n/2 * (n+1); };
          
          // the front-end invokes the back-end four times in row...
          uint i1 = expectSum(4);
          uint i2 = expectSum(8) - expectSum(4);
          uint i3 = expectSum(12) - expectSum(8);
          
          // Note: all threads use this common service front-end instance
          DummyFrontend myDummy;
          
          // Now invoke through this front-end
          CHECK (i1 == myDummy.invoke());
          CHECK (i2 == myDummy.invoke());
          
          // launch a significant number of threads,
          // each verifying the same invocation sequence
          using Threads = lib::ScopedCollection<Thread>;
          Threads threads{NUM_THREADS
                         ,Threads::fill ([&]{
                                              CHECK (i1 == myDummy.invoke());
                                              
                                              sleep_for (100us);
                                              
                                              CHECK (i2 == myDummy.invoke());
                                              
                                              sleep_for (100us);
                                              
                                              CHECK (i3 == myDummy.invoke());
                                            })};
          
          while (explore(threads).has_any())
            yield();          // wait for all threads to finish
          
          
          // ...without any interference with the main thread's instance
          CHECK (i3 == myDummy.invoke());
        }
      
      
      
      /** @test investigate performance of thread-local dispatch
       *      - subject-1 accesses the increment through the common thread-local dispatch
       *      - subject-2 increments just a local variable in each thread
       *      - run a multithreaded benchmark with 1 million repetitions
       *      - but only small number of threads to avoid OS scheduler congestion
       * @see test::threadBenchmark()
       */
      void
      investigatePerformance()
        {
          DummySlice threadLocalBackend;
          auto testThreadLocal = [&]{
                                      auto volatile result = threadLocalBackend->getStep();
                                      return result;
                                    };
          
          auto testLocalInc    = [cnt=0] mutable
                                    {
                                      auto volatile result = ++cnt;
                                      return result;
                                    };
          
          cout << "µ-benchmark :: thread-local" <<endl;
          
          auto [t1, s1] = ::test::threadBenchmark<CONCURR_BENCH> (testThreadLocal, REPETITIONS_BENCH);
          cout << "thread-local dispatch: "<<t1<<"µs" <<endl;
          
          auto [t2, s2] = ::test::threadBenchmark<CONCURR_BENCH> (testLocalInc, REPETITIONS_BENCH);
          cout << "increment local var  : "<<t2<<"µs" <<endl;
          
          CHECK (s1 == s2);
          CHECK (t1  > t2);
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER(LocalSlice_test, "unit common");
  
  
}} // namespace lib::test
