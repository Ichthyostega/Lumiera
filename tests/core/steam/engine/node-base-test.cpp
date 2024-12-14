/*
  NodeBase(Test)  -  unit test to cover the render node base elements

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-base-test.cpp
 ** Unit test \ref NodeBase_test covers elementary components of render nodes.
 */


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
#include "lib/meta/function.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  namespace { // Test fixture
    /**
     */
  }
  
  
  /***************************************************************//**
   * @test basic render node properties and behaviour.
   */
  class NodeBase_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          verify_TurnoutSystem();
          UNIMPLEMENTED ("build a simple render node and then activate it");
        }
      
      /** @test the TurnoutSystem as transient coordinator for node invocation
       */
      void
      verify_TurnoutSystem()
        {
          Time nomTime{rani(10'000),0};                    // drive test with a random »nominal Time« <10s with ms granularity
          TurnoutSystem invoker{nomTime};                  // a time spec is mandatory, all further parameters are optional
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBase_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
