/*
  LifeCycle(Test)  -  checking the lifecycle callback hooks provided by AppState

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file life-cycle-test.cpp
 ** unit test \ref LifeCycle_test
 */



#include "lib/test/run.hpp"
#include "lib/util.hpp"

#include "include/lifecycle.h"




namespace lumiera{
namespace test   {
  
  uint basicInit (0);
  uint customCallback (0);
  
  void basicInitHook () { ++basicInit; }
  void myCallback() { ++customCallback; }
  
  const char* MY_DEADLY_EVENT = "dial M for murder";
  
  
  namespace // register them to be invoked by lifecycle event id
    {
    LifecycleHook _schedule1 (ON_BASIC_INIT, &basicInitHook);
    LifecycleHook _schedule2 (MY_DEADLY_EVENT, &myCallback);
  }
  
  
  /** @test the global lifecycle hooks got registered,
   *        the ON_BASIC_INIT hook has been already called,
   *        while our custom callback can be triggered at our will
   */
  class LifeCycle_test : public Test
    {
      virtual void 
      run (Arg)
        {
          CHECK (basicInit, "the basic-init callback hasn't been invoked automatically");
          CHECK (1 == basicInit, "the basic-init callback has been invoked more than once");
          
          CHECK (!customCallback);
          LifecycleHook::trigger  (MY_DEADLY_EVENT);
          CHECK ( 1 == customCallback);
        }
      
    };
  
  LAUNCHER (LifeCycle_test, "function common");
  
  
  
  
}} // namespace util::test

