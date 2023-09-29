/*
  ThreadWrapper(Test)  -  starting threads and passing context

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/

/** @file thread-wrapper-test.cpp
 ** unit test \ref ThreadWrapper_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"///////////TODO
#include "lib/thread.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/scoped-collection.hpp"

#include <atomic>
#include <chrono>

using test::Test;
using lib::explore;
using std::atomic_uint;
using std::this_thread::yield;
using std::this_thread::sleep_for;
using std::chrono::microseconds;


namespace lib {
  namespace test  {
  
    namespace { // private test classes and data...
      
      const uint NUM_THREADS = 200;
      
      
      struct TestThread
        : Thread
        {
          using Thread::Thread;
          
          uint local{0};
          
          void
          doIt (uint a, uint b) ///< the actual operation running in a separate thread
            {
              uint sum = a + b;
              sleep_for (microseconds{sum});
              local = sum;
            }
        };
      
    } // (End) test classes and data....
    
    
    
    
    
    
    
    
    
    
    /**********************************************************************//**
     * @test use the Lumiera Vault to create some new threads, utilising the
     *       lumiera::Thread wrapper for binding to an arbitrary operation
     *       and passing the appropriate context.
     * 
     * @see vault::Thread
     * @see threads.h
     */
    class ThreadWrapper_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            demonstrateSimpleUsage();
            verifyConcurrentExecution();
          }
        
        /**
         * @test demonstrate simple usage of the thread-wrapper
         */ 
        void
        demonstrateSimpleUsage()
          {
            lib::ScopedCollection<Thread> threads{NUM_THREADS};
            
            atomic_uint invocations{0};
            for (uint i=0; i<NUM_THREADS; ++i)
              threads.emplace<Thread> ("counter"
                                      ,[&]{ ++invocations; });
            
            while (explore(threads).has_any())
              yield();
            
            CHECK (invocations == NUM_THREADS);
          }
        
        
        /**
         * @test verify the thread function is actually performed concurrently
         */
        void
        verifyConcurrentExecution()
          {
            lib::ScopedCollection<TestThread> threads{NUM_THREADS};
            
            size_t globalSum = 0;
            for (uint i=1; i<=NUM_THREADS; ++i)
              {
                uint x = rand() % 1000;
                globalSum += (i + x);
                threads.emplace<TestThread> (&TestThread::doIt, uint{i}, uint{x});
              }
            
            while (explore(threads).has_any())
              yield();
            
            size_t checkSum = 0;
            for (auto& t : threads)
              {
                CHECK (not t);
                CHECK (0 < t.local);
                checkSum += t.local;
              }
            CHECK (checkSum == globalSum);
            
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapper_test, "function common");
    
    
    
}} // namespace lib::test
