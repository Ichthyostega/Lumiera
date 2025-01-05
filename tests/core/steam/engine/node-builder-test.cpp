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
#include "steam/asset/meta/time-grid.hpp"
#include "lib/test/diagnostic-output.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/timecode.hpp"
#include "lib/symbol.hpp"
//#include "lib/util.hpp"

using lib::Symbol;
using std::string;
using lib::time::Time;
using lib::time::QuTime;
using lib::time::FrameNr;
using lib::time::SmpteTC;


namespace steam {
namespace engine{
namespace test  {
  
  namespace {
    Symbol SECONDS_GRID = "grid_sec";
  }
  
  
  
  /***************************************************************//**
   * @test creating and configuring various kinds of Render Nodes.
   */
  class NodeBuilder_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();                // used for simple time-based „automation“
          steam::asset::meta::TimeGrid::build (SECONDS_GRID, 1);
          
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
          
          CHECK (LIFE_AND_UNIVERSE_4EVER == invokeRenderNode (node));
        }
      
      /**
       * @internal Helper for Render Node invocation
       *  - use a DiagnosticBufferProvider to allocate a result buffer
       *  - assuming that the Node internally does not allocate further buffers
       *  - pull from Port #0 of the given node, passing the \a nomTime as argument
       *  - expect the buffer to hold a single `uint` value after invocation
       */
      uint
      invokeRenderNode (ProcNode& theNode, Time nomTime =Time::ZERO)
        {
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<long> (-55);
          ProcessKey key{0};
          uint port{0};
          
          CHECK (-55 == buff.accessAs<long>());
          
          // Trigger Node invocation...
          buff = theNode.pull (port, buff, nomTime, key);
          
          uint result = buff.accessAs<uint>();
          buff.release();
          return result;
        }
      
      
      /** @test build a Node with a fixed invocation parameter
       * @todo 12/24 ✔ define ⟶ ✔ implement
       */
      void
      build_Node_fixedParam()
        {
          auto procFun =  [](ushort param, uint* buff){ *buff = param; };
          
          ProcNode node{prepareNode("Test")
                          .preparePort()
                            .invoke("fun()", procFun)
                            .setParam (LIFE_AND_UNIVERSE_4EVER)
                            .completePort()
                          .build()};
          
          CHECK (LIFE_AND_UNIVERSE_4EVER == invokeRenderNode (node));
        }
      
      
      /** @test build a Node with dynamically generated parameter
       *      - use a processing function which takes a parameter
       *      - use an _automation functor,_ which just quantises
       *        the time into an implicitly defined grid
       *      - install both into a render node
       *      - set a random _nominal time_ for invocation
       * @todo 12/24 ✔ define ⟶ ✔ implement
       */
      void
      build_Node_dynamicParam()
        {
          auto procFun = [](long param, int* buff){ *buff = int(param); };
          auto autoFun = [](Time nomTime){ return FrameNr::quant (nomTime, SECONDS_GRID); };
          
          ProcNode node{prepareNode("Test")
                          .preparePort()
                            .invoke("fun()", procFun)
                            .attachAutomation (autoFun)
                            .completePort()
                          .build()};
          
          // invoke with a random »nominal Time« <10s with ms granularity
          Time theTime{rani(10'000),0};
          int res = invokeRenderNode (node, theTime);
          
          // for verification: quantise the given Time into SMPTE timecode;
          QuTime qantTime (theTime, SECONDS_GRID);
          CHECK (res == SmpteTC(qantTime).secs);
          // Explanation: since the param-functor quantises into a 1-second grid
          //              and the given time is below 1 minute, the seconds field
          //              of SMPTE Timecode should match the parameter value
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
       * @todo WIP 12/24 🔁 define ⟶ implement
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
