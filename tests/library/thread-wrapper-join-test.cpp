/*
  ThreadWrapperJoin(Test)  -  wait blocking on termination of a thread

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file thread-wrapper-join-test.cpp
 ** unit test \ref ThreadWrapperJoin_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/thread.hpp"
#include "lib/error.hpp"

#include <chrono>

using test::Test;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;


namespace lib {
namespace test {
  
  using error::LUMIERA_ERROR_LOGIC;
  namespace {
    
    const int DESTRUCTION_CODE = -23;
    
    LUMIERA_ERROR_DEFINE(SPECIAL, "007 exception");
    
    #define Type(_EXPR_) lib::test::showType<decltype(_EXPR_)>()
  }
  
  
  /***********************************************************************//**
   * @test verify the special flavour of the thread-wrapper, allowing to block
   *       waiting on thread end and then pass result or error state.
     * @see thread.hpp
     * @see Result_test
     * @see ThreadWrapper_test
     * @see SyncLocking_test
   */
  class ThreadWrapperJoin_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          
          simpleUse();
          returnValue();
          detectFailure();
          joinOnceOnly();
        }
      
      
      void
      simpleUse()
        {
          ThreadJoinable theThread{"test join-1"
                                  ,[]{ sleep_for (10ms); }};
          CHECK (theThread);
          theThread.join();     // blocks until thread-function has terminated
          CHECK (not theThread);
        }
      
      
      
      int
      theAction (int secretValue)   ///< to be run in a new thread...
        {
          sleep_for (100ms);        // pause 100ms prior to modifying
          
          if (DESTRUCTION_CODE == secretValue)
            throw error::External{"special agent detected"
                                 , LUMIERA_ERROR_SPECIAL};
          else
            return secretValue+42;
        }
      
      
      void
      returnValue()
        {
          int mySecret = rani(1000);
          
          ThreadJoinable theThread{"test join-2"
                                  ,&ThreadWrapperJoin_test::theAction
                                  , this, mySecret};
         
          // Note: join() passes the result value captured in the thread
          CHECK (mySecret+42 == theThread.join());
        }
      
      
      void
      detectFailure()
        {
          ThreadJoinable theThread{"test join-3"
                                  ,&ThreadWrapperJoin_test::theAction
                                  , this, DESTRUCTION_CODE};
         
          // join() actually returns a proxy...
          auto res = theThread.join();
          CHECK (Type(res) == "Result<int>"_expect);
          
          // can detect that the thread was aborted with an exception
          CHECK (not res.isValid());
          VERIFY_ERROR(SPECIAL, res.maybeThrow() );
          VERIFY_ERROR(SPECIAL, (int)res );
        }
      
      
      void
      joinOnceOnly()
        {
          ThreadJoinable theThread{"test join-4"
                                  ,[]{ sleep_for (10ms); }};
          theThread.join();
          
          VERIFY_ERROR(LOGIC, theThread.join() );
          VERIFY_ERROR(LOGIC, theThread.join() );
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ThreadWrapperJoin_test, "function common");
  
  
  
}} // namespace lib::test
