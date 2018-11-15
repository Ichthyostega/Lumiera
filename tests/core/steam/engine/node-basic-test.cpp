/*
  NodeBasic(Test)  -  unit test covering render node basic lifecycle and usage

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-basic-test.cpp
 ** unit test \ref NodeBasic_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/nodefactory.hpp"
#include "steam/engine/nodewiring.hpp"
#include "steam/engine/stateproxy.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "steam/mobject/session/effect.hpp"
#include "lib/allocation-cluster.hpp"
//#include "lib/format-cout.hpp"
//#include "lib/util.hpp"


//using std::string;


namespace proc {
namespace engine{
namespace test  {
  
  using lib::AllocationCluster;
  using mobject::session::PEffect;
  
  
  namespace { // Test fixture
    
    /**
     * Mock State/Invocation object.
     * Used as a replacement for the real RenderInvocation,
     * so the test can verify that calculations are actually
     * happening in correct order.
     */
    class TestContext
      : public StateProxy
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
  class NodeBasic_test : public Test
    {
      virtual void run(Arg) 
        {
          UNIMPLEMENTED ("build a simple render node and then activate it");
          
          AllocationCluster alloc;
          NodeFactory nodeFab(alloc);
          
          ProcNode * testSource;  ///////////TODO: how to fabricate a test-Node???
          
          
          WiringSituation setup(testSource);
          
          PEffect pEffect = createTestEffectMObject();
          ProcNode* pNode = nodeFab (pEffect, setup);
          CHECK (pNode);
          
          TestContext simulatedInvocation;
          BuffHandle result = pNode->pull(simulatedInvocation, 0);
          
          CHECK (result);
          // CHECK we got calculated data in the result buffer
        } 
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBasic_test, "unit engine");
  
  
  
}}} // namespace proc::engine::test
