/*
  SyncLocking(Test)  -  check the monitor object based locking

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

/** @file sync-locking-test.cpp
 ** unit test \ref SyncLocking_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "lib/sync.hpp"
#include "backend/thread-wrapper.hpp"

#include <iostream>
#include <functional>

using std::bind;

using std::cout;
using test::Test;


namespace lib {
namespace test{
  
  namespace { // private test classes and data...
  
    const uint NUM_COUNTERS = 20;       ///< number of independent counters to increment in parallel
    const uint NUM_THREADS  = 10;       ///< number of threads trying to increment these counters
    const uint MAX_PAUSE    = 10000;    ///< maximum delay implemented as empty counting loop
    const uint MAX_SUM      = 1000;     ///< trigger when to finish incrementing
    const uint MAX_INC      = 10;       ///< maximum increment on each step
    
    
    
    class Victim
      : public Sync<RecursiveLock_NoWait>
      {
        volatile uint cnt_[NUM_COUNTERS];
        volatile uint step_;         ///< @note stored as instance variable
        
        void
        pause ()
          {
            Lock guard (this); // note recursive lock
            
            for ( uint i=0, lim=(rand() % MAX_PAUSE); i<lim; ++i)
              ;
          }
        
        void 
        incrementAll ()
          {
            for (uint i=0; i<NUM_COUNTERS; ++i)
              {
                pause();
                cnt_[i] += step_;
              }
          }
        
        
      public:
        Victim ()
          {
            for (uint i=0; i<NUM_COUNTERS; ++i)
              cnt_[i] = 0;
          }
        
        void 
        inc (uint newStep)
          {
            Lock guard (this);
            step_ = newStep;
            incrementAll();
          }
        
        bool
        belowLimit ()
          {
            Lock guard (this);
            return cnt_[0] < MAX_SUM;
          }
        
        bool
        checkAllEqual ()
          {
            for (uint i=1; i<NUM_COUNTERS; ++i)
              if (cnt_[i-1] != cnt_[i])
                return false;
            return true;
          }
        
        void 
        report ()
          {
            for (uint i=0; i<NUM_COUNTERS; ++i)
              cout << "Counter-#" << i << " = " << cnt_[i] << "\n";
          }
      }
    ourVictim;
    
    
    
    /**
     * A Thread trying to increment all victim counters in sync...
     */
    class HavocThread
      {
        backend::ThreadJoinable thread_;
        
        void
        doIt ()
          {
            while (ourVictim.belowLimit())
              ourVictim.inc (rand() % MAX_INC);
          }
        
      public:
        
        HavocThread ()
          : thread_("HavocThread"
                   , bind (&HavocThread::doIt, this)
                   )
          {
            CHECK (thread_.isValid());
          }
        
        ~HavocThread ()
          {
            if (thread_.isValid())
              thread_.join();
          }
      };
    
  } // (End) test classes and data....
  
  
  
  
  
  
  
  
  
  
  /******************************************************************//**
   * @test create multiple threads, all concurrently trying to increment
   * a number of counters with random steps and random pauses. Without
   * locking, the likely result will be differing counters.
   * But because the class Victim uses an object level monitor to
   * guard the mutations, the state should remain consistent.
   * 
   * @see SyncWaiting_test condition based wait/notify
   * @see SyncClasslock_test locking a type, not an instance 
   * @see sync.hpp
   */
  class SyncLocking_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          CHECK (ourVictim.checkAllEqual());
          {
            HavocThread threads[NUM_THREADS]   SIDEEFFECT;
          } 
          // all finished and joined here...
          
          if (!ourVictim.checkAllEqual())
            {
              cout << "Thread locking is broken; internal state got messed up\n"
                      "NOTE: all counters should be equal and >=" << MAX_SUM << "\n";
              ourVictim.report();
            }
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncLocking_test, "unit common");
  
  
  
}} // namespace lib::test
