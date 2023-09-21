/*
  SyncTimedwait(Test)  -  check the monitor object based timed condition wait

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

/** @file sync-timedwait-test.cpp
 ** unit test \ref SyncTimedwait_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "lib/sync.hpp"

#include <iostream>

using std::cout;
using test::Test;


namespace lib {
namespace test{
  
  namespace { // private test classes and data...
    
    const uint WAIT_mSec = 200;   ///< milliseconds to wait before timeout
    
  } // (End) test classes and data....
  
  
  
  
  
  
  
  
  
  
  /****************************************************************************//**
   * @test timeout feature on condition wait as provided by pthread and accessible
   *       via the object monitor based locking/waiting mechanism. Without creating
   *       multiple threads, we engage into a blocking wait, which aborts due to
   *       setting a timeout. Our waiting facility is written such as to invoke
   *       the condition prior to entering wait state (and consecutively whenever
   *       awakened). This test switches into wait-with-timeout mode right from
   *       within this condition check and thus works even while there is no
   *       other thread and thus an unconditional wait would stall forever.
   *       
   * @note it is discouraged to use the timed wait feature for "timing";
   *       when possible you should prefer relying on the Lumiera scheduler
   * 
   * @see SyncWaiting_test
   * @see sync::Timeout
   * @see sync.hpp
   */
  class SyncTimedwait_test
    : public Test,
      Sync<RecursiveLock_Waitable>
    {
      
      friend class Lock; // allows inheriting privately from Sync
      
      
      virtual void
      run (Arg)
        {
          checkTimeoutStruct();
          
          Lock block(this, &SyncTimedwait_test::neverHappens);
          
          cout << "back from LaLaLand, alive and thriving!\n";
          CHECK (block.isTimedWait());
        }
      
      
      bool
      neverHappens()                              ///< the "condition test" used for waiting....
        {
          Lock currentLock(this);                 // get the Lock recursively
          if (!currentLock.isTimedWait())         // right from within the condition check:
            currentLock.setTimeout(WAIT_mSec);    // switch waiting mode to timed wait and set timeout
          
          return false;
        }
      
      
      
      void
      checkTimeoutStruct()
        {
          sync::Timeout tout;
          
          CHECK (!tout);
          CHECK (0 == tout.tv_sec);
          CHECK (0 == tout.tv_nsec);
          
          tout.setOffset (0);
          CHECK (!tout);
          CHECK (0 == tout.tv_sec);
          CHECK (0 == tout.tv_nsec);
          
          timespec ref;
          clock_gettime(CLOCK_REALTIME, &ref);
          tout.setOffset (1);
          CHECK (tout);
          CHECK (0 < tout.tv_sec);
          CHECK (ref.tv_sec <= tout.tv_sec);
          CHECK (ref.tv_nsec <= 1000000 + tout.tv_nsec || ref.tv_nsec > 1000000000-100000);
          
          clock_gettime(CLOCK_REALTIME, &ref);
          tout.setOffset (1000);
          CHECK (tout);
          if (ref.tv_nsec!=0) // should have gotten an overflow to the seconds part
            {
              CHECK (ref.tv_sec <= 2 + tout.tv_sec );
              CHECK ((ref.tv_nsec +  1000000 * 999) % 1000000000
                                 <= tout.tv_nsec);
            }
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncTimedwait_test, "unit common");
  
  
  
}} // namespace lib::test
