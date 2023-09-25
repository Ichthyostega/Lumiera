/*
  ThreadWrapperJoin(Test)  -  wait blocking on termination of a thread

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

/** @file thread-wrapper-join-test.cpp
 ** unit test \ref ThreadWrapperJoin_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "lib/thread.hpp"
#include "lib/error.hpp"

#include <functional>

using std::bind;
using test::Test;



namespace lib {
namespace test {
  
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  namespace {
    
    const int DESTRUCTION_CODE = 23;
    
    LUMIERA_ERROR_DEFINE(SPECIAL, "grandiose exception");
  }
  
  
  /***********************************************************************//**
   * @test use the Lumiera Vault to create some new threads, additionally
   *       synchronising with these child threads and waiting for termination.
   * 
   * @see vault::Thread
   * @see threads.h
   */
  class ThreadWrapperJoin_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUse ();
          wrongUse ();
        }
      
      
      volatile int aValue_;         ///< state to be modified by the other thread
      
      void
      theAction (int secretValue)   ///< to be run in a new thread...
        {
          usleep (100000);          // pause 100ms prior to modifying
          
          if (DESTRUCTION_CODE == secretValue)
            LUMIERA_ERROR_SET(test, SPECIAL, 0);
          else
            aValue_ =  secretValue+42;
          
          
        }
      
      
      void
      simpleUse ()
        {
          aValue_=0;
          int mySecret = (rand() % 1000) - 500;
          
          ThreadJoinable newThread("test Thread joining-1"
                                  , bind (&ThreadWrapperJoin_test::theAction, this, mySecret)
                                  );
          newThread.join();      // blocks until theAction() is done
          
          CHECK (aValue_ == mySecret+42);
        }
      
      
      void
      wrongUse ()
        {
          ThreadJoinable newThread("test Thread joining-2"
                                  , bind (&ThreadWrapperJoin_test::theAction, this, 1234)
                                  );
          newThread.join();      // blocks until theAction() is done
          
          VERIFY_ERROR(LOGIC, newThread.join() );
          VERIFY_ERROR(LOGIC, newThread.join() );
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ThreadWrapperJoin_test, "function common");
  
  
  
}} // namespace lib::test
