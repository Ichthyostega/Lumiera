/*
  ThreadWrapperSelfRecognitionTest(Test)  -  detect when code is running within a thread

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "backend/thread-wrapper.hpp"
#include "lib/error.hpp"

using test::Test;



namespace backend {
namespace test {
  
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  namespace {
    
    class TestThread
      : Thread
      {
        void
        doIt()
          {
            CHECK (invocation_happens_within_this_thread());
          }
        
        public:
          TestThread()
            : Thread{"test Thread self recognition", [&]() { doIt(); }}
            { }
          
          bool
          invocation_happens_within_this_thread()
            {
              return invokedWithinThread();
            }
      };
    
  }
  
  
  /******************************************************//**
   * @test verify the ability of a thread to detect code
   *       executing within the thread itself.
   * 
   * @see Thread::invokedWithinThread()
   * @see proc::control::DispatcherLoop::stateIsSynched()
   */
  class ThreadWrapperSelfRecognitionTest_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          TestThread testThread;
          
          CHECK (not testThread.invocation_happens_within_this_thread());
          
          usleep(10); // give the thread a chance to run before shutdown
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ThreadWrapperSelfRecognitionTest_test, "function common");
  
  
  
}} // namespace backend::test
