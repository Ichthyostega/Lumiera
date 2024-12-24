/*
  NodeBuilder(Test)  -  creation and setup of render nodes

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-builder-test.cpp
 ** Unit test \ref NodeBuilder_test demonstrates how to build render nodes.
 */


#include "lib/test/run.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "lib/test/diagnostic-output.hpp"
//#include "lib/util.hpp"


using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test creating and configuring various kinds of Render Nodes.
   */
  class NodeBuilder_test : public Test
    {
      virtual void
      run (Arg)
        {
          build_simpleNode();
          build_Node_fixedParam();
          build_Node_dynamicParam();
          build_connectedNodes();
          build_ParamNode();
        }
      
      
      /** @test build a simple output-only Render Node
       * @todo 12/24 ✔ define ⟶ ✔ implement
       */
      void
      build_simpleNode()
        {
          auto fun = [](uint* buff){ *buff = LIFE_AND_UNIVERSE_4EVER; };
          
          ProcNode node{prepareNode("Test")
                          .preparePort()
                            .invoke("fun()", fun)
                            .completePort()
                          .build()};
          
          CHECK (watch(node).isSrc());
          CHECK (watch(node).ports().size() == 1);
          
          // Prepare setup to invoke such a Render Node...
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<long> (-55);
          Time nomTime = Time::ZERO;
          ProcessKey key{0};
          uint port{0};
          
          CHECK (-55 == buff.accessAs<long>());
          
          // Trigger Node invocation...
          buff = node.pull (port, buff, nomTime, key);
          
          CHECK (LIFE_AND_UNIVERSE_4EVER == buff.accessAs<uint>());
        }
      
      
      /** @test TODO build a Node with a fixed invocation parameter
       * @todo WIP 12/24 🔁 define ⟶ implement
       */
      void
      build_Node_fixedParam()
        {
          UNIMPLEMENTED ("build node with fixed param");
        }
      
      
      /** @test TODO build a Node with dynamically generated parameter
       * @todo WIP 12/24 define ⟶ implement
       */
      void
      build_Node_dynamicParam()
        {
          UNIMPLEMENTED ("build node with param-functor");
        }
      
      
      /** @test TODO build a chain with two connected Nodes
       * @todo WIP 12/24 define ⟶ implement
       */
      void
      build_connectedNodes()
        {
          UNIMPLEMENTED ("build two linked nodes");
        }
      
      
      /** @test TODO 
       * @todo WIP 12/24 define ⟶ implement
       */
      void
      build_ParamNode()
        {
          UNIMPLEMENTED ("build ParamNode + follow-up-Node");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBuilder_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
