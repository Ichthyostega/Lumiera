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

#include "lib/thread.hpp"
#include "lib/sync.hpp"
#include "lib/symbol.hpp"

#include <functional>

using std::bind;
using test::Test;


namespace lib {
  namespace test  {
  
    namespace { // private test classes and data...
      
      const uint NUM_THREADS      = 20;
      const uint MAX_RAND_SUMMAND = 100;
      
      
      class Checker
        : public lib::Sync<>
        {
          volatile ulong hot_sum_;
          ulong control_sum_;
          
        public:
          Checker() : hot_sum_(0), control_sum_(0) { }
          
          bool
          verify()    ///< verify test values got handled and accounted
            {
              return 0 < hot_sum_
                  && control_sum_ == hot_sum_;
            }
          
          uint
          createVal() ///< generating test values, remembering the control sum
            {
              uint val(rand() % MAX_RAND_SUMMAND);
              control_sum_ += val;
              return val;
            }
          
          void
          addValues (uint a, uint b)   ///< to be called concurrently
            {
              Lock guard(this);
              
              hot_sum_ *= 2;
              usleep (200);             // force preemption
              hot_sum_ += 2 * (a+b);
              usleep (200);
              hot_sum_ /= 2;
            }
        };
      
      
      Checker checksum; ///< global variable used by multiple threads
      
      
      
      
      struct TestThread
        : Thread
        {
          TestThread()
            : Thread("test Thread creation",
                     bind (&TestThread::theOperation, this, checksum.createVal(), checksum.createVal()))
            { }                         // note the binding (functor object) is passed as anonymous temporary
          
          
          void
          theOperation (uint a, uint b) ///< the actual operation running in a separate thread
            {
              checksum.addValues (a,b);
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
            TestThread instances[NUM_THREADS]    SIDEEFFECT;
            
            usleep (200000);  // pause 200ms for the threads to terminate.....
            
            CHECK (checksum.verify());
          }
      };
    
    
    
    /** Register this test class... */
    LAUNCHER (ThreadWrapper_test, "function common");
    
    
    
}} // namespace lib::test
