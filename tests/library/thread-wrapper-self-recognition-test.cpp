/*
  ThreadWrapperSelfRecognitionTest(Test)  -  detect when code is running within a thread

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file thread-wrapper-self-recognition-test.cpp
 ** unit test \ref ThreadWrapperSelfRecognitionTest_test
 */


#include "lib/test/run.hpp"
#include "lib/thread.hpp"

using test::Test;



namespace lib {
namespace test {
  
  
  /******************************************************//**
   * @test verify the ability of a thread to detect code
   *       executing within the thread itself.
   * @see Thread::invokedWithinThread()
   * @see thread::ThreadWrapper::invokedWithinThread()
   * @see steam::control::DispatcherLoop::stateIsSynched()
   */
  class ThreadWrapperSelfRecognitionTest_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          ThreadJoinable<bool> testThread{"Thread self detection"
                                         ,[&]{
                                               return testThread.invokedWithinThread();
                                             }};
          
          CHECK (not testThread.invokedWithinThread());
          CHECK (testThread.join().get<bool>());
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ThreadWrapperSelfRecognitionTest_test, "function common");
  
  
  
}} // namespace lib::test
