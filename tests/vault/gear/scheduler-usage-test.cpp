/*
  SchedulerUsage(Test)  -  verify standard usage patterns of the scheduler service

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file scheduler-usage-test.cpp
 ** unit test \ref SchedulerUsage_test
 */


#include "lib/test/run.hpp"
#include "vault/gear/scheduler.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"

//#include <utility>

using test::Test;
//using std::move;
//using util::isSameObject;


namespace vault{
namespace gear {
namespace test {
  
//  using lib::time::FrameRate;
//  using lib::time::Offset;
//  using lib::time::Time;
  
  
  
  
  
  /*************************************************************************//**
   * @test Ensure expected Scheduler functionality in standard usage situations. 
   * @see SchedulerActivity_test
   * @see SchedulerInvocation_test
   * @see SchedulerCommutator_test
   */
  class SchedulerUsage_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           walkingDeadline();
           setupLalup();
        }
      
      
      /** @test TODO demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
        }
      
      
      
      /** @test TODO
       */
      void
      walkingDeadline()
        {
        }
      
      
      
      /** @test TODO
       */
      void
      setupLalup()
        {
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (SchedulerUsage_test, "unit engine");
  
  
  
}}} // namespace vault::gear::test
