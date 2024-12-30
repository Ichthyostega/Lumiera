/*
  NodeFeed(Test)  -  verify render node data feeds

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-feed-test.cpp
 ** Feeding into and retrieving data from render nodes is covered by \ref NodeFeed_test.
 */


#include "lib/test/run.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/engine/media-weaving-pattern.hpp"
#include "steam/engine/param-weaving-pattern.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "lib/several-builder.hpp"
#include "lib/time/timecode.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////////TODO
//#include "lib/util.hpp"


//using std::string;
using lib::Several;
using lib::makeSeveral;
using lib::time::Time;
using lib::time::FSecs;
using lib::time::FrameNr;
using lib::test::showType;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  /************************************************************************//**
   * @test demonstrate how to feed data into, through and out of render nodes.
   */
  class NodeFeed_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          feedParam();
          feedParamNode();
          UNIMPLEMENTED ("render node pulling source data from vault");
        }
      
      
      /** @test demonstrate internal setup to invoke a simple output-only function,
       *        passing an additional invocation parameter generated from a parameter-functor
       *      - embed the processing-functor and parameter-functor into a FeedPrototype
       *      - construct the type of the »Weaving Pattern« to use for invocation
       *      - setup an empty wiring (output-only, thus no predecessor ports)
       *      - setup a single BuffDesrc for a result puffer to pass to the processing-functor
       *      - create a Turnout, which implements the Port interface, using the Weaving-Pattern
       *      - for the actual invocation, setup a TurnoutSystem, initialised with a nominal time
       *      - invoke the Port::weave() function and retrieve the result from the buffer.
       * @remark this is a semi-integrated setup to demonstrate the interplay of the
       *         internal components within a Render Node, without the _outer shell_
       *         provided by the NodeBuilder and the ProcNode itself
       */
      void
      feedParam()
        {
          auto procFun =  [](ushort param, uint* buff){ *buff = param; };
          auto paramFun = [](TurnoutSystem&){ return LIFE_AND_UNIVERSE_4EVER; };
          
          auto feedPrototype = FeedPrototype{move(procFun), move(paramFun)};
          using Prototype = decltype(feedPrototype);
          using WeavingPattern = MediaWeavingPattern<Prototype>;
          using TurnoutWeaving = Turnout<WeavingPattern>;

          BufferProvider& provider = DiagnosticBufferProvider::build();
          
          Several<PortRef> noLeadPorts;                               // ◁————————— empty predecessor-port-sequence
          Several<BuffDescr> outBuffDescr = makeSeveral({provider.getDescriptor<uint>()})
                                              .build();             //   ◁————————— a single output buffer to hold an `uint`
          uint resultSlot{0};
          
          TurnoutWeaving port{ProcID::describe ("SimpleNode","procFun()")
                             , move (noLeadPorts)
                             , move (outBuffDescr)
                             , resultSlot
                             , move (feedPrototype)
                             };
          
          // setup for invocation...
          Time nomTime =Time::ZERO;
          TurnoutSystem turnoutSys{nomTime};
          BuffHandle result = port.weave (turnoutSys);                // ◁————————— paramFun invoked here, then procFun
          CHECK (LIFE_AND_UNIVERSE_4EVER == result.accessAs<uint>());//             and procFun wrote param-value into result buffer
          result.release();
        }
      
      
      /** @test create extended parameter data for use in recursive Node invocation.
       *      - demonstrate the mechanism of param-functor invocation,
       *        and how a Param-Spec is built to create and hold those functors
       *      - then instantiate an actual TurnoutSystem, as is done for a Node invocation,
       *        with an embedded »absolute nominal time« parameter value
       *      - can then invoke the param-functors and materialise results into a Param-Data-Block
       *      - which then can be linked internally to be reachable through the TurnoutSystem
       *      - other code further down the call-stack can thus access those parameter values.
       *      - The second part of the test uses the same scheme embedded into a Param(Agent)Node
       */
      void
      feedParamNode()
        {
          steam::asset::meta::TimeGrid::build("grid_sec", 1);
          
          // Parameter-functor based on time-quantisation into a 1-seconds-grid
          auto fun1 = [](TurnoutSystem& turSys)
                        {
                          return FrameNr::quant (turSys.getNomTime(), "grid_sec");
                        };
          
          // The Param-Spec is used to coordinate type-safe access
          // and also is used as a blueprint for building a Param(Agent)Node
          auto spec = buildParamSpec()
                        .addValSlot (LIFE_AND_UNIVERSE_4EVER)
                        .addSlot (move (fun1))
                        ;
          
          // The implied type of the parameter-tuple to generate
          using ParamTup = decltype(spec)::ParamTup;
          CHECK (showType<ParamTup>() == "tuple<uint, long>"_expect);
          
          // can now store accessor-functors for later use....
          auto acc0 = spec.slot<0>().makeAccessor();
          auto acc1 = spec.slot<1>().makeAccessor();
          
          // drive test with a random »nominal Time« <10s with ms granularity
          Time nomTime{rani(10'000),0};
          TurnoutSystem turnoutSys{nomTime};
          // can now immediately invoke the embedded parameter-functors
          auto v0 = spec.slot<0>().invokeParamFun (turnoutSys);
          auto v1 = spec.slot<1>().invokeParamFun (turnoutSys);
          CHECK (v0 == LIFE_AND_UNIVERSE_4EVER);                      // ◁————————— the first paramFun yields the configured fixed value
          CHECK (v1 == FrameNr::quant (nomTime, "grid_sec"));         // ◁————————— the second paramFun accesses the time in TurnoutSystem

          {  //  Now build an actual storage block in local scope,
            //   thereby invoking the embedded parameter-functors...
            auto paramBlock = spec.buildParamDataBlock (turnoutSys);
            // Values are now materialised into paramBlock
            CHECK (v0 == paramBlock.get<0>());
            CHECK (v1 == paramBlock.get<1>());
            
            // link this extension block into the parameter-chain in TurnoutSystem
            turnoutSys.attachChainBlock(paramBlock);
            
            // can now access the parameter values through the TurnoutSystem as front-End
            CHECK (v0 == spec.slot<0>().getParamVal (turnoutSys));
            CHECK (v1 == spec.slot<1>().getParamVal (turnoutSys));
            // and can also use the accessor-functors stored above
            CHECK (v0 == turnoutSys.get(acc0));
            CHECK (v1 == turnoutSys.get(acc1));
            
            // should detach extension block before leaving scope
            turnoutSys.detachChainBlock(paramBlock);
          }

          TODO ("implement a simple Builder for ParamAgent-Node");
          TODO ("then use both together to demonstrate a param data feed here");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeFeed_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
