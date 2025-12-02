/*
  SyncClasslock(Test)  -  validate the type-based Monitor locking

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file sync-classlock-test.cpp
 ** unit test \ref SyncClasslock_test
 */


#include "lib/test/run.hpp"
#include "lib/sync-classlock.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/thread.hpp"

using test::Test;
//using vault::ThreadJoinable;  //////////////////WIP

namespace lib {
namespace test {
  
  namespace { // Parameters for multithreaded contention test
    
    const uint NUM_THREADS = 20;   ///< number of contending threads to create
    const uint NUM_LOOP    = 1000; ///< number of loop iterations per thread
    
  }
  
  
  
  
  /**********************************************************************//**
   * @test check proper handling of class (not instance)-based Monitor locks.
   * Because no instance is available in this case, a hidden storage for the
   * Monitor object needs to be provided in a way safe for use even in the
   * static startup/shutdown phase. This can not directly validate this
   * allocation of a shared monitor object behind the scenes, but it
   * can verify the monitor is indeed shared by all ClassLock instances
   * templated to a specific type.
   * 
   * @see sync.hpp
   */
  class SyncClasslock_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          auto gen = buildCappedSubSequence(defaultGen);
          int contended = 0;
          
          using Threads = lib::ScopedCollection<ThreadJoinable<>>;
          
               // Start a bunch of threads with random access pattern
          Threads threads{NUM_THREADS,
                          [&](Threads::ElementHolder& storage)
                             {
                               storage.create<ThreadJoinable<>> ("Sync-ClassLock stress test"
                                                                ,[&]{
                                                                      for (uint i=0; i<NUM_LOOP; ++i)
                                                                        {
                                                                          uint delay = gen.i(10);
                                                                          usleep (delay);
                                                                          {
                                                                            ClassLock<void> guard;
                                                                            ++contended;
                                                                          }
                                                                        }
                                                                    });
                             }
                         };
          
          for (auto& thread : threads)
            thread.join(); // block until thread terminates   // @suppress("Return value not evaluated")
          
          CHECK (contended == NUM_THREADS * NUM_LOOP,
                 "ALARM: Lock failed, concurrent modification "
                 "during counter increment. Delta == %d"
                ,NUM_THREADS * NUM_LOOP - contended);
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncClasslock_test, "unit common");
  
  
  
}} // namespace lib::test
