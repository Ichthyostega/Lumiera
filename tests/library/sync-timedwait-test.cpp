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

#include <chrono>

using test::Test;
using std::chrono::system_clock;
using std::chrono::milliseconds;


namespace lib {
namespace test{
  
  namespace { // test parameters...
    
    const uint WAIT_mSec = 20;                ///< milliseconds to wait before timeout
    const milliseconds TIMEOUT{WAIT_mSec};
     
    using CLOCK_SCALE = std::milli;           // Results are in ms
    using Dur = std::chrono::duration<double, CLOCK_SCALE>;
    
  }//(End) parameters
  
  
  
  
  
  
  /****************************************************************************//**
   * @test timeout feature on condition wait as provided by the underlying implementation
   *       and accessible via the object monitor based locking/waiting mechanism. Without
   *       creating multiple threads, we engage into a blocking wait, which aborts due to
   *       setting a timeout.
   * @see SyncWaiting_test
   * @see sync.hpp
   */
  class SyncTimedwait_test
    : public Test,
      Sync<NonrecursiveLock_Waitable>
    {
      
      friend class Lock; // allows inheriting privately from Sync
      
      
      virtual void
      run (Arg)
        {
          Lock lock{this};
          
          auto start = system_clock::now();

          auto salvation = []{ return false; };
          bool fulfilled = lock.wait_for (TIMEOUT, salvation);
          
          CHECK (not fulfilled); // condition not fulfilled, but timeout
          
          Dur duration = system_clock::now () - start;
          CHECK (WAIT_mSec <= duration.count());
          CHECK (duration.count() < 2*WAIT_mSec);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncTimedwait_test, "unit common");
  
  
  
}} // namespace lib::test
