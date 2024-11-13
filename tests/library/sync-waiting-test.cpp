/*
  SyncWaiting(Test)  -  check the monitor object based wait/notification

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

/** @file sync-waiting-test.cpp
 ** unit test \ref SyncWaiting_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "lib/thread.hpp"
#include "lib/sync.hpp"

#include <atomic>

using test::Test;
using std::atomic_uint;
using std::atomic_bool;
using std::this_thread::sleep_for;
using std::chrono_literals::operator ""ms;


namespace lib {
namespace test{
  
  namespace { // test subject...
    
    /** demonstrates how to wait on a simple boolean flag */
    class SyncOnBool
      : public Sync<NonrecursiveLock_Waitable>
      {
        atomic_uint sum_{0}, input_{0};
        atomic_bool got_new_data_{false};
        
      public:
        void getIt()
          {
            Lock await{this, [&]{ return bool(got_new_data_); }};
            sum_ += input_;
          }
        
        void provide (uint val)
          {
            Lock sync{this};
            input_ = val;
            got_new_data_ = true;
            sync.notify_all();
          }
        
        /** harvesting the result...*/
        uint result () { return sum_; }
      };
  }//(End) test subject.
  
  
  
  
  
  
  
  /************************************************************************//**
   * @test concurrent waiting and notification, implemented via object monitor.
   * This test covers the second part of the monitor pattern, which builds upon
   * the locking part, additionally using an embedded condition. Two interwoven
   * threads are created, both blocked until a start value is given. Once awakened,
   * each thread should add the start value to a common sum field.
   * @see SyncLocking_test
   * @see sync.hpp
   */
  class SyncWaiting_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          SyncOnBool token;
          
          ThreadJoinable ping ("SyncWaiting ping", [&]{ token.getIt(); });
          ThreadJoinable pong ("SyncWaiting pong", [&]{ token.getIt(); });
          
          CHECK (ping);
          CHECK (pong);
          CHECK (0 == token.result());
          
          sleep_for (100ms);  // if the threads don't block correctly, they've missed their chance by now...
          
          // kick off the notification cascade...
          uint val = rani(1000);
          token.provide (val);
          
          // wait for the two Threads to finish their handshake
          pong.join();
          ping.join();
          
          CHECK (2*val == token.result());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SyncWaiting_test, "unit common");
  
  
  
}} // namespace lib::test
