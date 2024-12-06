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
#include "steam/engine/proc-node.hpp"
//#include "steam/engine/nodewiring-obsolete.hpp"       /////////////////////////////////////////////////////TICKET #1367 : sort out dependencies for reworked Node Invocation
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "steam/mobject/session/effect.hpp"
#include "lib/allocation-cluster.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::AllocationCluster;
  using mobject::session::PEffect;
  
  
  namespace { // Test fixture
    
    /**
     * Mock StateClosure/Invocation object.
     * Used as a replacement for the real RenderInvocation,
     * so the test can verify that calculations are actually
     * happening in correct order.
     */
    class TestContext
//      : public StateProxy
      {
        
        //////////////TODO: facility to verify the right access operations get called
        
      };
  
      
    inline PEffect
    createTestEffectMObject()
    {
      UNIMPLEMENTED ("how to create a dummy Effect for tests");
    }
  }
  
  
  /***************************************************************//**
   * @test basic render node properties and behaviour.
   */
  class NodeBase_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("build a simple render node and then activate it");
          
          AllocationCluster alloc;
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
          NodeFactory nodeFab(alloc);
          
          ProcNode * testSource;  ///////////TODO: how to fabricate a test Source-Node????
          
          
          WiringSituation setup(testSource);
          
          PEffect pEffect = createTestEffectMObject();
          ProcNode* pNode = nodeFab (pEffect, setup);
          CHECK (pNode);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
          
          TestContext simulatedInvocation;
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
          BuffHandle result = pNode->pull(simulatedInvocation, 0);
          
          CHECK (result);
          // CHECK we got calculated data in the result buffer
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBase_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
