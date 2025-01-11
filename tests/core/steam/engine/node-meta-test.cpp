/*
  NodeMeta(Test)  -  verify render node data feeds

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file node-meta-test.cpp
 ** Naming and hash-key identification of render nodes is covered by \ref NodeMeta_test.
 */


#include "lib/test/run.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/node-builder.hpp"
//#include "steam/engine/test-rand-ontology.hpp" ///////////TODO
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
//#include "lib/util.hpp"

#include <cmath>

//using std::string;
using std::abs;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test Render node metadata and hash identity keys.
   */
  class NodeMeta_test : public Test
    {
      virtual void
      run (Arg)
        {
          verify_ID_specification();
          verify_ID_properties();
        }
      
      
      /** @test TODO evaluation of processing-spec for a ProcID
       * @todo WIP 1/25 🔁 define ⟶ 🔁 implement
       */
      void
      verify_ID_specification()
        {
          auto& p1 = ProcID::describe("N1","(arg)");
          auto& p2 = ProcID::describe("U:N2","+(a1,a2)");
          auto& p3 = ProcID::describe("O:N3","(in/3)(o1,o2/2)");
          
          CHECK (p1.genNodeName()   == "N1"_expect );
          CHECK (p1.genNodeSymbol() == "N1"_expect );
          CHECK (p1.genNodeDomain() ==   ""_expect );
          CHECK (p2.genNodeName()   == "U:N2"_expect );
          CHECK (p2.genNodeSymbol() == "N2"_expect );
          CHECK (p2.genNodeDomain() ==  "U"_expect );
          CHECK (p3.genNodeName()   == "O:N3"_expect );
          CHECK (p3.genNodeSymbol() == "N3"_expect );
          CHECK (p3.genNodeDomain() ==  "O"_expect );
          
          CHECK (p1.genProcName()   == "N1"_expect );
          CHECK (p1.genQualifier()  ==   ""_expect );
          CHECK (p2.genProcName()   == "N2.+"_expect );                   // domain omitted, qualifier joined with '.'
          CHECK (p2.genQualifier()  == ".+"_expect );                     // qualifier includes leading '.'
          CHECK (p3.genProcName()   == "N3"_expect );
          CHECK (p2.genProcSpec()   == "U:N2.+(a1,a2)"_expect );
          CHECK (p3.genProcSpec()   == "O:N3(in/3)(o1,o2/2)"_expect );
          UNIMPLEMENTED ("parse and evaluate");
        }
      
      
      /** @test TODO aspects of node definition relevant for the ProcID
       * @todo WIP 1/25 🔁 define ⟶ implement
       */
      void
      verify_ID_properties()
        {
          // This operation emulates a data source
          auto src_opA = [](int param, int* res)    { *res = param; };
          auto src_opB = [](ulong param, ulong* res){ *res = param; };
          
          // A Node with two (source) ports
          ProcNode nA{prepareNode("srcA")
                        .preparePort()
                          .invoke("a(int)", src_opA)
                          .setParam(5)
                          .completePort()
                        .preparePort()
                          .invoke("b(int)", src_opA)
                          .setParam(23)
                          .completePort()
                        .build()};
          
          // A different Node with three ports
          ProcNode nB{prepareNode("srcB")
                        .preparePort()
                          .invoke("a(ulong)", src_opB)
                          .setParam(7)
                          .completePort()
                        .preparePort()
                          .invoke("b(ulong)", src_opB)
                          .setParam(13)
                          .completePort()
                        .preparePort()
                          .invoke("c(ulong)", src_opB)
                          .setParam(17)
                          .completePort()
                        .build()};
          
          // This operation emulates fading of two source chains
          auto fade_op = [](double mix, tuple<int*,ulong*> src, uint64_t* res)
                          {
                            auto [srcA,srcB] = src;
                            *res = uint64_t(abs(*srcA * mix + (1-mix) * int64_t(*srcB))); 
                          };
          
          // Wiring for the Mix, building up three ports
          // Since the first source-chain has only two ports,
          // for the third result port we'll re-use the second source
          ProcNode nM{prepareNode("fade")
                        .preparePort()
                          .invoke("A_mix(int,ulong)(uint64_t)", fade_op)
                          .connectLead(nA)
                          .connectLead(nB)
                          .completePort()
                        .preparePort()
                          .invoke("B_mix(int,ulong)(uint64_t)", fade_op)
                          .connectLead(nA)
                          .connectLead(nB)
                          .completePort()
                        .preparePort()
                          .invoke("C_mix(int,ulong)(uint64_t)", fade_op)
                          .connectLeadPort(nA,1)
                          .connectLead(nB)
                          .setParam(0.5)
                          .completePort()
                        .build()};
          UNIMPLEMENTED ("verify connectivity");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeMeta_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
