/*
  NodeLinkage(Test)  -  verify proper render node operation and calldown

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

/** @file node-linkage-test.cpp
 ** unit test \ref NodeLinkage_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/engine/node-builder.hpp"
#include "steam/engine/test-rand-ontology.hpp" ///////////TODO
#include "lib/test/diagnostic-output.hpp"/////////////////TODO
#include "lib/util.hpp"


using util::isnil;
//using std::string;


namespace steam {
namespace engine{
namespace test  {
  
  
  
  
  /***************************************************************//**
   * @test demonstrate and document how [render nodes](\ref proc-node.hpp)
   *       are connected into a processing network, allowing to _invoke_
   *       a \ref Port on a node to pull-generate a render result.
   *     - the foundation layer is formed by the nodes as linked into a network
   *     - starting from any Port, a TurnoutSystem can be established
   *     - which in turn allows _turn out_ a render result from this port.
   */
  class NodeLinkage_test : public Test
    {
      virtual void
      run (Arg)
        {
          build_connected_nodes();
          generate_turnout_system();
          trigger_node_port_invocation();
        }
      
      
      
      
      /** @test TODO Build render nodes linked into a connectivity network
       * @todo WIP 7/24 🔁 define ⟶ ✔ implement
       */
      void
      build_connected_nodes()
        {
          auto con = prepareNode()
                        .preparePort()
                          .invoke(dummyOp)
                          .completePort()
                        .build();
          CHECK (isnil (con.leads));
          CHECK (1 == con.ports.size());
          
          // can build a ProcNode with this connectivity
          ProcNode n1{move(con)};
          CHECK (watch(n1).isValid());
          CHECK (watch(n1).leads().empty());
          CHECK (watch(n1).ports().size() == 1);
        }
      
      
      /** @test TODO Use existing node connectivity to generate a TurnoutSystem
       * @todo WIP 7/24 🔁 define ⟶ implement
       */
      void
      generate_turnout_system()
        {
          UNIMPLEMENTED ("use existing node connectivity to generate a TurnoutSystem");
        }
      
      
      /** @test TODO Invoke some render nodes as linked together
       * @todo WIP 7/24 🔁 define ⟶ implement
       */
      void
      trigger_node_port_invocation()
        {
          UNIMPLEMENTED ("operate some render nodes as linked together");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeLinkage_test, "unit node");
  
  
  
}}} // namespace steam::engine::test
