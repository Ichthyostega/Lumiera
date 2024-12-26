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
#include "steam/engine/weaving-pattern.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/diagnostic-buffer-provider.hpp"
#include "lib/several-builder.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////////TODO
//#include "lib/util.hpp"


//using std::string;
using lib::Several;
using lib::makeSeveral;


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
          feedParam();
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
      
      /** @test create extended parameter data for use in recursive Node invocation */
      void
      feedParamNode()
        {
          TODO ("implement the logic for the TurnoutSystem --> node-base-test.cpp");
          TODO ("implement a simple Builder for ParamAgent-Node");
          TODO ("then use both together to demonstrate a param data feed here");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeFeed_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
