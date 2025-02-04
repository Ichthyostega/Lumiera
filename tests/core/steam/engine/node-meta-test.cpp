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
#include "lib/format-util.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/test/diagnostic-output.hpp"/////////////////TODO

#include <cmath>


namespace steam {
namespace engine{
namespace test  {
  
  using std::abs;
  
  
  
  /***************************************************************//**
   * @test Render node metadata and hash identity keys.
   * @todo 2/2025 hash computation is not yet specified...
   */
  class NodeMeta_test : public Test
    {
      virtual void
      run (Arg)
        {
          verify_ID_specification();
          verify_ID_connectivity();
        }
      
      
      /** @test evaluation of processing-spec for a ProcID
       * @todo 1/25 ✔ define ⟶ ✔ implement
       */
      void
      verify_ID_specification()
        {
          auto& p1 = ProcID::describe("N1","(arg)");
          auto& p2 = ProcID::describe("U:N2","+(a1,a2)");
          auto& p3 = ProcID::describe("O:N3","(in/3)(o1,o2/2)");
          
          CHECK (p1.genNodeName()   == "N1"_expect   );
          CHECK (p1.genNodeSymbol() == "N1"_expect   );
          CHECK (p1.genNodeDomain() ==   ""_expect   );
          CHECK (p2.genNodeName()   == "U:N2"_expect );
          CHECK (p2.genNodeSymbol() == "N2"_expect   );
          CHECK (p2.genNodeDomain() ==  "U"_expect   );
          CHECK (p3.genNodeName()   == "O:N3"_expect );
          CHECK (p3.genNodeSymbol() == "N3"_expect   );
          CHECK (p3.genNodeDomain() ==  "O"_expect   );
          
          CHECK (p1.genProcName()   == "N1"_expect   );
          CHECK (p1.genQualifier()  ==   ""_expect   );
          CHECK (p2.genProcName()   == "N2.+"_expect );                   // domain omitted, qualifier joined with '.'
          CHECK (p2.genQualifier()  == ".+"_expect   );                   // qualifier includes leading '.'
          CHECK (p3.genProcName()   == "N3"_expect   );
          CHECK (p2.genProcSpec()   == "N2.+(a1,a2)"_expect       );
          CHECK (p3.genProcSpec()   == "N3(in/3)(o1,o2/2)"_expect );
          
          ProcID::ArgModel arg1 = p1.genArgModel();
          ProcID::ArgModel arg2 = p2.genArgModel();
          ProcID::ArgModel arg3 = p3.genArgModel();
          CHECK (not arg1.hasInArgs());
          CHECK (not arg2.hasInArgs());
          CHECK (arg1.outArity() == 1);
          CHECK (arg2.outArity() == 2);
          CHECK (arg3.outArity() == 3);
          CHECK (arg3.inArity()  == 3);
          CHECK (arg1.iArg ==           "[]"_expect );
          CHECK (arg1.oArg ==        "[arg]"_expect );                    // only one argument list -> used for output
          CHECK (arg2.iArg ==           "[]"_expect );
          CHECK (arg2.oArg ==     "[a1, a2]"_expect );
          CHECK (arg3.iArg == "[in, in, in]"_expect );                    // repetition-abbreviation of arguments unfolded
          CHECK (arg3.oArg == "[o1, o2, o2]"_expect );
          
          // give the spec-parser a rough time....
          string nastySpec = "(\":-)\"/2,std::tuple<short,long>/3,{oh,RLY?}/2,\\\")";
          auto hairyModel = ProcID::describe("○", nastySpec).genArgModel();
          CHECK (hairyModel.outArity() == 8);
          CHECK (hairyModel.inArity()  == 0);
          CHECK (hairyModel.oArg == "[\":-)\", \":-)\", "
                                    "std::tuple<short,long>, "
                                    "std::tuple<short,long>, "
                                    "std::tuple<short,long>, "
                                    "{oh,RLY?}, {oh,RLY?}, \\\"]"_expect);
        }
      
      
      
      /** @test validate the interplay of node connectivity
       *        with reported properties at the ProcID and
       *        demonstrate tools to check connectivity.
       *      - Build a 3-node network with dummy operations,
       *        which however are built to mimic the very common
       *        situation where two sources are mixed
       *      - at exit side, three different »flavours« can be
       *        produced, which implies that there are three Ports.
       *      - the source at the »A-side« provided only two flavours,
       *        and thus an explicit wiring has to be made for the
       *        A-side connection of the third chain
       *      - In real usage, the node specification strings will be
       *        provided from the Media-Lib adapter plug-in. Here it is
       *        hard wired, and defined in a way to reflect structure.
       *      - various ways to drill-down into the structure are explored
       *        by verifying the ProcID specification visible at each point.
       *      - then the tools for verifying connectivity are demonstrated
       *        and covered with relevant positive and negative combinations.
       * @todo 2/25 🔁 define ⟶ ✔ implement
       */
      void
      verify_ID_connectivity()
        {
          // These operations emulate data sources
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
          
           // Drill down into each node...
          //  investigate spec and precursor connectivity
          CHECK (watch(nA).getNodeName()              == "srcA"_expect         );
          CHECK (watch(nA).getNodeSpec()              == "srcA-◎"_expect       );  // includes shortened rendering of lead nodes
          CHECK (watch(nA).isSrc()                    ==  true                 );  // ...but this one has no leads ==> it is a source
          CHECK (watch(nA).ports().size()             ==  2                    );
          CHECK (watch(nA).watchPort(0).getProcName() == "srcA.a"_expect       );
          CHECK (watch(nA).watchPort(0).getProcSpec() == "srcA.a(int)"_expect  );
          CHECK (watch(nA).watchPort(1).getProcSpec() == "srcA.b(int)"_expect  );
          
          VERIFY_FAIL ("Port-idx 2 >= 2 (available Ports)"
                      , watch(nA).watchPort(2) );
          
          CHECK (watch(nB).getNodeSpec()              == "srcB-◎"_expect       );
          CHECK (watch(nB).isSrc()                    ==  true                 );
          CHECK (watch(nB).ports().size()             ==  3                    );
          CHECK (watch(nB).watchPort(0).getProcSpec() == "srcB.a(ulong)"_expect);
          CHECK (watch(nB).watchPort(1).getProcSpec() == "srcB.b(ulong)"_expect);
          CHECK (watch(nB).watchPort(2).getProcSpec() == "srcB.c(ulong)"_expect);
          
          CHECK (watch(nM).getNodeName()                           == "fade"_expect         );
          CHECK (watch(nM).getNodeSpec()                           == "fade┉┉{srcA, srcB}"_expect );  // the spec shows the set of source nodes
          CHECK (watch(nM).ports().size()                          ==  3                    );
          CHECK (watch(nM).watchPort(0).getProcName()              == "fade.A_mix"_expect   );
          CHECK (watch(nM).watchPort(1).getProcName()              == "fade.B_mix"_expect   );
          CHECK (watch(nM).watchPort(2).getProcName()              == "fade.C_mix"_expect   );
          CHECK (watch(nM).watchPort(2).getProcSpec()              == "fade.C_mix(int,ulong)(uint64_t)"_expect );
          CHECK (watch(nM).watchPort(0).srcPorts().size()          ==  2                    );
          CHECK (watch(nM).watchPort(0).watchLead(0).getProcName() == "srcA.a"_expect       );        // watchLead(#) navigates to source port
          CHECK (watch(nM).watchPort(0).watchLead(1).getProcName() == "srcB.a"_expect       );
          CHECK (watch(nM).watchPort(1).srcPorts().size()          ==  2                    );
          CHECK (watch(nM).watchPort(1).watchLead(0).getProcName() == "srcA.b"_expect       );
          CHECK (watch(nM).watchPort(1).watchLead(1).getProcName() == "srcB.b"_expect       );
          CHECK (watch(nM).watchPort(2).srcPorts().size()          ==  2                    );
          CHECK (watch(nM).watchPort(2).watchLead(0).getProcName() == "srcA.b"_expect       );
          CHECK (watch(nM).watchPort(2).watchLead(1).getProcName() == "srcB.c"_expect       );
          CHECK (watch(nM).watchPort(2).watchLead(1).getProcSpec() == "srcB.c(ulong)"_expect);
          CHECK (watch(nM).watchPort(2).watchLead(1).isSrc()       ==  true );                        // the lead port itself is a source
          CHECK (watch(nM).watchPort(2).watchLead(1).srcPorts().size() == 0 );                        // ...and thus has an empty source-port-collection
          
          
          // Helper predicate to verify connectedness to a specific Port given by reference
          CHECK (watch(nM).watchPort(2).verify_connected(   watch(nA).ports()[0]) == false  );
          CHECK (watch(nM).watchPort(2).verify_connected(   watch(nA).ports()[1]) == true   );        // Node-nM.port#2 is somehow connected to Node-nA.port#1
          CHECK (watch(nM).watchPort(2).verify_connected(   watch(nB).ports()[0]) == false  );
          CHECK (watch(nM).watchPort(2).verify_connected(   watch(nB).ports()[1]) == false  );
          CHECK (watch(nM).watchPort(2).verify_connected(   watch(nB).ports()[2]) == true   );
          CHECK (watch(nM).watchPort(2).verify_connected(0, watch(nA).ports()[1]) == true   );        // Node-nM.port#2 connects via source#0 to Node-nA.port#1
          CHECK (watch(nM).watchPort(2).verify_connected(1, watch(nB).ports()[2]) == true   );
          CHECK (watch(nM).watchPort(2).verify_connected(0, watch(nB).ports()[2]) == false  );
          CHECK (watch(nM).watchPort(2).verify_connected(1, watch(nA).ports()[1]) == false  );        // Node-nM.port#2 doesn't connect via source#1 to Node-nA.port#1
          
          
           //__________________________________
          // Inspect Node and Port connectivity
          
          // High-level case: connections between nodes
          CHECK (is_linked(nM).to(nA) == true );
          CHECK (is_linked(nM).to(nB) == true );
          CHECK (is_linked(nA).to(nB) == false);
          
          // additionally qualify the index position
          // of the source node in the sequence of »Lead nodes«
          CHECK (is_linked(nM).to(nA).asLead(0)  == true );                              // Node-nA is Lead-#0
          CHECK (is_linked(nM).to(nA).asLead(1)  == false);
          CHECK (is_linked(nM).to(nB).asLead(0)  == false);
          CHECK (is_linked(nM).to(nB).asLead(1)  == true );                              // Node-nB is Lead-#1
          
          // Check if a specific Port is connected to a source node
          CHECK (is_linked(nM).port(0).to(nA)    == true );
          CHECK (is_linked(nM).port(0).to(nB)    == true );
          CHECK (is_linked(nM).port(0).to(nM)    == false);                              // never connected to itself
          
          // Similar, but now pick the source node from the »Leads«
          CHECK (is_linked(nM).port(0).toLead(0) == true );
          CHECK (is_linked(nM).port(0).toLead(1) == true );
          CHECK (is_linked(nA).port(0).toLead(0) == false);                              // nA is a source node and thus has no further source-connections
          
          // Verify detailed port-to-port connectivity
          CHECK (is_linked(nM).port(0).to(watch(nA).ports()[0]) == true );               // Node-nM connected within Port-0 to Port-0 of Node-nA
          CHECK (is_linked(nM).port(0).to(watch(nA).ports()[1]) == false);               //         ......but not connected to Port-1 of Node-nA 
          CHECK (is_linked(nM).port(0).to(watch(nB).ports()[0]) == true );
          CHECK (is_linked(nM).port(0).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(0).to(watch(nB).ports()[2]) == false);
          CHECK (is_linked(nM).port(2).to(watch(nA).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).to(watch(nA).ports()[1]) == true );               // this is the connection routed from port-2 to Node-nA, Port-1
          CHECK (is_linked(nM).port(2).to(watch(nB).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(2).to(watch(nB).ports()[2]) == true );
          CHECK (is_linked(nM).port(2).to(watch(nM).ports()[2]) == false);               // a nonsensical check, nodes are never connected to themselves
          
          CHECK (is_linked(nM).port(0).to(nA).atPort(0)         == true );
          CHECK (is_linked(nM).port(0).to(nA).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).to(nB).atPort(0)         == true );
          CHECK (is_linked(nM).port(0).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).to(nB).atPort(2)         == false);
          CHECK (is_linked(nM).port(2).to(nA).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).to(nA).atPort(1)         == true );
          CHECK (is_linked(nM).port(2).to(nB).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(2).to(nB).atPort(2)         == true );
          CHECK (is_linked(nM).port(2).to(nM).atPort(2)         == false);
          
          CHECK (is_linked(nM).port(0).toLead(0).atPort(0)      == true );
          CHECK (is_linked(nM).port(0).toLead(0).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).toLead(1).atPort(0)      == true );
          CHECK (is_linked(nM).port(0).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).toLead(1).atPort(2)      == false);
          CHECK (is_linked(nM).port(2).toLead(0).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).toLead(0).atPort(1)      == true );
          CHECK (is_linked(nM).port(2).toLead(1).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(2).toLead(1).atPort(2)      == true );
          
          // additionally also qualify the «source slot«
          // at which the connection is used as input for the processing-function
          CHECK (is_linked(nM).port(0).asSrc(0).to(nA)    == true );                     // Node-nM, Port-0 uses as source-slot-0 a connection to Node-nA
          CHECK (is_linked(nM).port(0).asSrc(1).to(nA)    == false );
          CHECK (is_linked(nM).port(0).asSrc(0).to(nB)    == false );
          CHECK (is_linked(nM).port(0).asSrc(1).to(nB)    == true );
          CHECK (is_linked(nM).port(2).asSrc(0).to(nA)    == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(nA)    == false );
          CHECK (is_linked(nM).port(2).asSrc(0).to(nB)    == false );
          CHECK (is_linked(nM).port(2).asSrc(1).to(nB)    == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(nM)    == false );                    // never connected to itself
          
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(0) == true );                     // Lead-#0 is Node-nA
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(0) == false );
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(1) == false );
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(1) == true );
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(0) == true );
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(0) == false );
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(1) == false );
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(1) == true );
          
          // Again detailed port-to-port connections, this time limited by «source slot»
          CHECK (is_linked(nM).port(0).asSrc(0).to(watch(nA).ports()[0]) == true );      // Node-nA, Port-0 connects as src-#0 to node-nA at Port-0
          CHECK (is_linked(nM).port(0).asSrc(0).to(watch(nA).ports()[1]) == false);      // ...and can thus not be connected to any other Port there
          CHECK (is_linked(nM).port(0).asSrc(1).to(watch(nA).ports()[0]) == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(watch(nA).ports()[1]) == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(watch(nB).ports()[0]) == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(watch(nB).ports()[2]) == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(watch(nB).ports()[0]) == true );
          CHECK (is_linked(nM).port(0).asSrc(1).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(watch(nB).ports()[2]) == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(watch(nA).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(watch(nA).ports()[1]) == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nA).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nA).ports()[1]) == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(watch(nB).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(watch(nB).ports()[2]) == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nB).ports()[0]) == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nB).ports()[1]) == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nB).ports()[2]) == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(watch(nM).ports()[2]) == false);      // never connected to itself
          
          CHECK (is_linked(nM).port(0).asSrc(0).to(nA).atPort(0)         == true );
          CHECK (is_linked(nM).port(0).asSrc(0).to(nA).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(nA).atPort(0)         == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(nA).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(nB).atPort(0)         == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).asSrc(0).to(nB).atPort(2)         == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(nB).atPort(0)         == true );
          CHECK (is_linked(nM).port(0).asSrc(1).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(0).asSrc(1).to(nB).atPort(2)         == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(nA).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(nA).atPort(1)         == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(nA).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(nA).atPort(1)         == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(nB).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(2).asSrc(0).to(nB).atPort(2)         == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(nB).atPort(0)         == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(nB).atPort(1)         == false);
          CHECK (is_linked(nM).port(2).asSrc(1).to(nB).atPort(2)         == true );
          CHECK (is_linked(nM).port(2).asSrc(1).to(nM).atPort(2)         == false);
          
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(0).atPort(0)      == true );
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(0).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(0).atPort(0)      == false);
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(0).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(1).atPort(0)      == false);
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).asSrc(0).toLead(1).atPort(2)      == false);
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(1).atPort(0)      == true );
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(0).asSrc(1).toLead(1).atPort(2)      == false);
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(0).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(0).atPort(1)      == true );
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(0).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(0).atPort(1)      == false);
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(1).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(2).asSrc(0).toLead(1).atPort(2)      == false);
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(1).atPort(0)      == false);
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(1).atPort(1)      == false);
          CHECK (is_linked(nM).port(2).asSrc(1).toLead(1).atPort(2)      == true );
          
          // Some fallback-cases tested when given an incomplete chain:
          // Specifying only a Port an source-slot just checks for valid index
          CHECK (is_linked(nM).port(0).asSrc(0) == true );
          CHECK (is_linked(nM).port(0).asSrc(1) == true );
          CHECK (is_linked(nM).port(0).asSrc(2) == false);  // has only 2 source-slots
          CHECK (is_linked(nA).port(0).asSrc(0) == false);  // node-nA is a source-node and thus has no source-slot at all
          
          // A port allone is checked for any incoming connections
          CHECK (is_linked(nM).port(0) == true );
          CHECK (is_linked(nM).port(1) == true );
          CHECK (is_linked(nM).port(2) == true );
          CHECK (is_linked(nM).port(3) == false);           // node-nM has only 3 ports, i.e. index [0...2]
          CHECK (is_linked(nA).port(0) == false);           // node-nA is a source node and thus no port can have an incoming connection
          CHECK (is_linked(nB).port(0) == false);           // same for node-nB
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeMeta_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
