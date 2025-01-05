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

#include <array>

using lib::Symbol;
using std::string;
using std::array;
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
                            .invoke ("fun()", procFun)
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
                            .invoke ("fun()", procFun)
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
      
      
      /** @test build a chain with three connected Nodes
       *      - have two source nodes, which accept a parameter
       *      - but configure them differently: one gets a constant,
       *        while the other draws a random number
       *      - the third node takes two input buffers and and one output;
       *        it retrieves the input values, and sums them together
       *      - use the »simplified 1:1 wiring«, which connects consecutively
       *        each input slot to the next given node on the same port number;
       *        here we only use port#0 on all three nodes.
       * @todo 12/24 ✔ define ⟶ ✔ implement
       */
      void
      build_connectedNodes()
        {
          using SrcBuffs = array<uint*, 2>;
          auto sourceFun = [](uint param, uint* out)  { *out = 1 + param; };
          auto joinerFun = [](SrcBuffs src, uint* out){ *out = *src[0] + *src[1]; };
          
          int peek{-1};
          auto randParam = [&](TurnoutSystem&){ return peek = rani(100); };
          
          
          ProcNode n1{prepareNode("Src1")
                          .preparePort()
                            .invoke ("fix-val()", sourceFun)
                            .setParam (LIFE_AND_UNIVERSE_4EVER)
                            .completePort()
                          .build()};
          
          ProcNode n2{prepareNode("Src2")
                          .preparePort()
                            .invoke ("ran-val()", sourceFun)
                            .attachParamFun (randParam)
                            .completePort()
                          .build()};
          
          ProcNode n3{prepareNode("Join")
                          .preparePort()
                            .invoke ("add()", joinerFun)
                            .connectLead(n1)
                            .connectLead(n2)
                            .completePort()
                          .build()};
          
          uint res = invokeRenderNode(n3);
          CHECK (res == peek+1 + LIFE_AND_UNIVERSE_4EVER+1 );
          CHECK (peek != -1);
        }
      
      
      /** @test demonstrate the setup of a »Param Agent Node«
       *      - perform effectively the same computation as the preceding test
       *      - but use two new custom parameters in the Param Agent Node
       *      - pick them up from the nested source nodes by accessor-functors
       * @todo 12/24 ✔ define ⟶ ✔ implement
       */
      void
      build_ParamNode()
        {
          // Note: using exactly the same functors as in the preceding test
          using SrcBuffs = array<uint*, 2>;
          auto sourceFun = [](uint param, uint* out)  { *out = 1 + param; };
          auto joinerFun = [](SrcBuffs src, uint* out){ *out = *src[0] + *src[1]; };
          
          int peek{-1};
          auto randParam = [&](TurnoutSystem&){ return peek = rani(100); };
          
          // Step-1 : build a ParamSpec
          auto spec = buildParamSpec()
                          .addValSlot (LIFE_AND_UNIVERSE_4EVER)
                          .addSlot (randParam)
                          ;
          auto get0 = spec.makeAccessor<0>();
          auto get1 = spec.makeAccessor<1>();
          
          // Step-2 : build delegate Node tree
          ProcNode n1{prepareNode("Src1")
                          .preparePort()
                            .invoke ("fix-val()", sourceFun)
                            .retrieveParam (get0)
                            .completePort()
                          .build()};
          
          ProcNode n2{prepareNode("Src2")
                          .preparePort()
                            .invoke ("ran-val()", sourceFun)
                            .retrieveParam (get1)
                            .completePort()
                          .build()};
          
          ProcNode n3{prepareNode("Join")
                          .preparePort()
                            .invoke ("add()", joinerFun)
                            .connectLead(n1)
                            .connectLead(n2)
                            .completePort()
                          .build()};
          
          // Step-3 : build Param Agent as entry point
          ProcNode n4{prepareNode("Param")
                          .preparePort()
                            .computeParam(spec)
                            .delegateLead(n3)
                            .completePort()
                          .build()};
          
          uint res = invokeRenderNode(n4);
          CHECK (res == peek+1 + LIFE_AND_UNIVERSE_4EVER+1 );
          CHECK (peek != -1);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeBuilder_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
