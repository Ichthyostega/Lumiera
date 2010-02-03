/*
  ThreadWrapper(Test)  -  starting threads and passing context
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "lib/test/run.hpp"

#include "lib/symbol.hpp"
#include "backend/thread-wrapper.hpp"
#include "lib/sync.hpp"

#include <tr1/functional>

using std::tr1::bind;
using test::Test;
using lib::Sync;
using lib::NonrecursiveLock_NoWait;

namespace backend {
  namespace test  {
  
    namespace { // private test classes and data...
      
      volatile ulong sum;
      ulong checksum;
      
      Sync<NonrecursiveLock_NoWait> lockme;
      
      const uint NUM_THREADS      = 20;
      const uint MAX_RAND_SUMMAND = 100;
      
      uint
      createVal()  ///< generating test values, remembering the sum
      {
        uint val(rand() % MAX_RAND_SUMMAND);
        checksum += val;
        return val;
      }
      
      
      struct TestThread
        : Sync<NonrecursiveLock_NoWait>  // using dedicated locking for this test TODO: needs classlock
        , Thread
        {
          TestThread()
            : Thread("test Thread creation",
                     bind (&TestThread::theOperation, this, createVal(), createVal()))
            { }                         // note the binding (functor object) is passed as anonymous temporary
          
          
          void
          theOperation (uint a, uint b) ///< the actual operation running in a separate thread
            {
              //Lock(this);     << broken we need a classlock, using sync-classlock is left as excercise for the reader
              Sync<NonrecursiveLock_NoWait>::Lock gotit(&lockme);
              sum *= 2;
              usleep (200);             // force preemption
              sum += (2*(a+b));
              usleep (200);
              sum /= 2;
            }
        };
      
    } // (End) test classes and data....
    
    
    
    
    
    
    
    
    
    
    /**************************************************************************
     * @test use the Lumiera backend to create some new threads, utilising the
     *       lumiera::Thread wrapper for binding to an arbitrary operation
     *       and passing the appropriate context.
     * 
     * @see backend::Thread
     * @see threads.h
     */
    class ThreadWrapper_test : public Test
      {
        
        virtual void
        run (Arg)
          {
            sum = checksum = 0;
            TestThread instances[NUM_THREADS]    SIDEEFFECT;
            
            usleep (200000);  // pause 200ms for the threads to terminate.....
            
            ASSERT (0 < sum);
            ASSERT (sum==checksum);
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapper_test, "function common");
    
    
    
  } // namespace test

} // namespace backend
