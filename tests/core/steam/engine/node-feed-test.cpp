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


using lib::Several;
using lib::makeSeveral;
using lib::time::Time;
using lib::time::FrameNr;
using lib::test::showType;
using std::make_tuple;
using std::get;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  /************************************************************************//**
   * @test demonstrate how to feed data into, through and out of render nodes.
   * @todo 2/2025 unfinished -- add cases to cover media data feeds
   */
  class NodeFeed_test : public Test
    {
      virtual void
      run (Arg)
        {
          seedRand();
          feedParam();
          feedParamNode();
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
          // Assuming that somewhere in the system a 1-seconds time grid was predefined...
          steam::asset::meta::TimeGrid::build ("grid_sec", 1);
          
            //_______________________________________________
           // Demo-1: demonstrate the access mechanism directly;
          //          create and link an extended parameter block.
          
          // This test will create an extension data block with two parameters,
          // one of these is generated from time-quantisation into a 1-seconds-grid
          auto createParmFun = [](TurnoutSystem& turnoutSys) -> long
                                  {
                                    return FrameNr::quant (turnoutSys.getNomTime(), "grid_sec");
                                  };
          
          // The »Param-Spec« is used to coordinate type-safe access
          // and also is used as a blueprint for building a Param(Agent)Node
          // Note the builder syntax to add several parameter »slots«...
          auto spec = buildParamSpec()
                        .addValSlot (LIFE_AND_UNIVERSE_4EVER)
                        .addSlot (createParmFun)
                        ;
          
          // Implied type of the parameter-tuple to generate
          using ParamTup = decltype(spec)::ParamTup;
          CHECK (showType<ParamTup>() == "tuple<uint, long>"_expect);
          
          auto acc0 = spec.makeAccessor<0>();                          // can now store accessor-functors for later use....
          auto acc1 = spec.makeAccessor<1>();
          
          // Prepare for invocation....
          Time nomTime{rani(10'000),0};                                // drive test with a random »nominal Time« <10s with ms granularity
          TurnoutSystem turnoutSys{nomTime};                           // build minimal TurnoutSystem for invocation, just with this time parameter
          auto v0 = spec.invokeParamFun<0> (turnoutSys);               // can now immediately invoke the embedded parameter-functors
          auto v1 = spec.invokeParamFun<1> (turnoutSys);
          CHECK (v0 == LIFE_AND_UNIVERSE_4EVER);                       // ◁————————— the first paramFun yields the configured fixed value
          CHECK (v1 == FrameNr::quant (nomTime, "grid_sec"));          // ◁————————— the second paramFun accesses the time via TurnoutSystem
          
          
          // after all setup of further accessor functors is done
          // finally transform the ParamSpec into a storage-block-builder
          auto blockBuilder = spec.clone().makeBlockBuilder();         // (use clone() since we're re-using the same spec in Demo-2 below)
          
          {  //  Now build an actual storage block in local scope,
            //   thereby invoking the embedded parameter-functors...
            auto paramBlock = blockBuilder.buildParamDataBlock (turnoutSys);
            CHECK (v0 == paramBlock.get<0>());                         // Values are now materialised into paramBlock
            CHECK (v1 == paramBlock.get<1>());
            
            turnoutSys.attachChainBlock(paramBlock);                   // link this extension block into the parameter-chain in TurnoutSystem;
            CHECK (v0 == acc0.getParamVal (turnoutSys));               // Can now access the parameter values through the TurnoutSystem as front-End
            CHECK (v1 == acc1.getParamVal (turnoutSys));               // ...using the pre-configured accessor-functors stored above
            
            turnoutSys.detachChainBlock(paramBlock);                   // should detach extension block before leaving scope
          }// extension block is gone...
          
          {  // Demonstrate the same access mechanism
            //  but integrated into a Weaving-Pattern
            using Spec = decltype(spec);
            using WeavingPattern = ParamWeavingPattern<Spec>;
            using Feed = WeavingPattern::Feed;
            
            Feed feed;
            feed.emplaceParamDataBlock (blockBuilder, turnoutSys);
            // note that the param-data-block is embedded into the feed,
            // so that it can be easily placed into the current stack frame
            CHECK (v0 == feed.block().get<0>());
            CHECK (v1 == feed.block().get<1>());
          }
          
          
          
            //_________________________________________________
           // Demo-2: perform exactly the same access scheme,
          //          but now embedded into a Render Node graph.
          using Param = tuple<int,int>;
          
          // The processing function uses two parameter values
          auto processFun = [](Param par, long* buff)
                                {
                                  *buff = get<0>(par) + get<1>(par);
                                };
          // These parameter values are picked up from the extended TurnoutSystem,
          // relying on the accessor objects, which were created from the ParamSpec
          auto accessParam = [acc0,acc1]
                             (TurnoutSystem& turnoutSys) -> Param
                                {
                                  return make_tuple (turnoutSys.get(acc0)
                                                    ,turnoutSys.get(acc1));
                                };
          
          ProcNode delegate{prepareNode("Delegate")
                                .preparePort()
                                  .invoke("proc()", processFun)
                                  .attachParamFun (accessParam)
                                  .completePort()
                                .build()};
          
          ProcNode paramAgent{prepareNode("Param")
                                .preparePort()
                                  .computeParam (move(spec))
                                  .delegateLead (delegate)             // ◁————————— linked to the Delegate-Node
                                  .completePort()
                                .build()};

          // Prepare result buffer for invocation
          BufferProvider& provider = DiagnosticBufferProvider::build();
          BuffHandle buff = provider.lockBufferFor<long> (-55);
          CHECK (-55 == buff.accessAs<long>());
          
          // Invoke Port#0 on the top-level Node (≙ the ParamAgent)
          buff = paramAgent.getPort(0).weave(turnoutSys, buff);        // ◁————————— generate Param-Values, link into TurnoutSystem, invoke Delegate
          CHECK (v0+v1 == buff.accessAs<long>());
          
          buff.release();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeFeed_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
