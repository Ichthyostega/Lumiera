/*
  SyncLocking(Test)  -  check the monitor object based locking

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file sync-locking-test.cpp
 ** unit test \ref SyncLocking_test
 */


#include "lib/test/run.hpp"

#include "lib/sync.hpp"
#include "lib/thread.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/scoped-collection.hpp"

using test::Test;
using lib::explore;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""us;


namespace lib {
namespace test{
    
    namespace { // private test classes and data...
      
      const uint NUM_THREADS      = 200;
      const uint MAX_RAND_SUMMAND = 100;
      
      
      
      /** Helper to verify a contended chain calculation */
      template<class POLICY>
      class Checker
        : public Sync<POLICY>
        {
          size_t hot_sum_{0};
          size_t control_sum_{0};
          
          using Lock = typename Sync<POLICY>::Lock;
          
        public:
          bool
          verify()    ///< verify test values got handled and accounted
            {
              Lock guard{this};
              return 0 < hot_sum_
                 and control_sum_ == hot_sum_;
            }
          
          uint
          createVal() ///< generating test values, remembering the control sum
            {
              uint val = rani (MAX_RAND_SUMMAND);
              control_sum_ += val;
              return val;
            }
          
          void
          addValues (uint a, uint b)   ///< to be called concurrently
            {
              Lock guard{this};
              
              hot_sum_ *= 2;
              sleep_for (200us);       // force preemption
              hot_sum_ += 2 * (a+b);
              sleep_for (200us);
              hot_sum_ /= 2;
            }
        };
    }// (End) test classes and data....
    
    
    
    
    
    
    /******************************************************************//**
     * @test verify the object monitor provides locking and to prevent
     *       data corruption on concurrent modification of shared storage.
     *       - use a chained calculation with deliberate sleep state
     *         while holding onto an intermediary result
     *       - run this calculation contended by a huge number of threads
     *       - either use locking or no locking
     * @see sync.happ
     * @see thread.hpp
     */
    class SyncLocking_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            seedRand();
            CHECK (can_calc_without_Error<NonrecursiveLock_NoWait>());
            CHECK (can_calc_without_Error<RecursiveLock_NoWait>());
            CHECK (not can_calc_without_Error<sync::NoLocking>());
          }
        
        
        template<class POLICY>
        bool
        can_calc_without_Error()
          {
            Checker<POLICY> checksum;  // shared variable used by multiple threads
            
            lib::ScopedCollection<Thread> threads{NUM_THREADS};
            for (uint i=1; i<=NUM_THREADS; ++i)
              threads.emplace ([&checksum,   // Note: added values prepared in main thread
                                a = checksum.createVal(),
                                b = checksum.createVal()]
                                {
                                  checksum.addValues (a,b);
                                });
            
            while (explore(threads).has_any())
              yield();  // wait for all threads to terminate
            
            return checksum.verify();
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (SyncLocking_test, "function common");
    
    
    
}} // namespace lib::test
