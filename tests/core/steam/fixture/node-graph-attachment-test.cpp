/*
  NodeGraphAttachment(Test)  -  verify link from fixture to low-level-model

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file node-graph-attachment-test.cpp
 ** unit test \ref NodeGraphAttachment_test
 */


#include "lib/test/run.hpp"
#include "steam/fixture/node-graph-attachment.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "steam/engine/exit-node.hpp"
#include "lib/util.hpp"

#include <utility>


namespace steam {
namespace fixture {
namespace test  {
  
  using std::move;
  using util::isnil;
  using util::isSameObject;
  using engine::ExitNode;
  
  
  /*****************************************************************************//**
   * @test Verify the facade object used to connect from the Segments in the Fixture
   *       into the actual render nodes network
   *       - construction of ExitNode
   *       - composition of the NodeGraphAttachment including prerequisites
   *       - generation of a complete setup of fake ExitNodes from a test spec.
   * @see steam::fixture::Segment
   * @see JobPlanningPipeline_test
   * @see MockSupport_test
   */
  class NodeGraphAttachment_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          access_ExitNodeTree();
          fabricate_MockExitNode();
        }
      
      
      /** @test access to ExitNode and fall back on ExitNode::NIL when undefined
       *        - verify ExitNode::NIL
       *        - build a simple ExitNode without prerequisites
       *        - use this as prerequisite(s) for another ExitNode
       *        - put a third ExitNode(id=13) also into the same collection
       *        - use this to construct a NodeGraphAttachment
       *        - access existing and non-existing index positions
       */
      void
      access_ExitNodeTree()
        {
          CHECK (0 == ExitNode::NIL.getPipelineIdentity());
          CHECK (isnil (ExitNode::NIL.getPrerequisites()));
          
          ExitNodes subDead;
          subDead.emplace_back(55);
          CHECK (55 == subDead[0].getPipelineIdentity());
          CHECK (isnil (subDead[0].getPrerequisites()));
          
          ExitNodes superDead;
          superDead.emplace_back(23, move (subDead));
          superDead.emplace_front(13);
          CHECK (13 == superDead[0].getPipelineIdentity());
          CHECK (23 == superDead[1].getPipelineIdentity());
          CHECK (not isnil (superDead[1].getPrerequisites()));
          CHECK (55 == superDead[1].getPrerequisites()->getPipelineIdentity());
          
          NodeGraphAttachment succubus{move (superDead)};
          CHECK (13 == succubus[0].getPipelineIdentity());
          CHECK (23 == succubus[1].getPipelineIdentity());
          CHECK (55 == succubus[1].getPrerequisites()->getPipelineIdentity());
          
          CHECK (isSameObject (succubus[5], ExitNode::NIL));           // out-of-index access falls back to ExitNode::NIL
        }
      
      
      /** @test setup a properly structured ExitNode graph using the
       *        specification scheme supported by MockSegmentation
       *  @see MockSupport_test::verify_MockSegmentation
       */
      void
      fabricate_MockExitNode()
        {
          using lib::diff::MakeRec;
          
          engine::test::MockSegmentation builder;
          ExitNode node =
            builder.buildExitNodeFromSpec(MakeRec()
                                           .attrib("mark", 13)         // top-level: marked with hash/id = 13
                                           .scope(MakeRec()            //        ... defines two nested prerequisites
                                                   .attrib("mark",23)  //          + Prerequisite-1 hash/id = 23
                                                 .genNode()
                                                 ,MakeRec()
                                                   .attrib("mark",55)  //          + Prerequisite-2 hash/id = 55
                                                 .genNode()
                                                 )
                                         .genNode());
          
          // verify generated Node is assembled according to above spec...
          CHECK (13 == node.getPipelineIdentity());
          auto feed = node.getPrerequisites();
          CHECK (not isnil (feed));
          CHECK (23 == feed->getPipelineIdentity());
          ++feed;
          CHECK (55 == feed->getPipelineIdentity());
          ++feed;
          CHECK (isnil (feed));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (NodeGraphAttachment_test, "unit fixture");
  
  
  
}}} // namespace steam::fixture::test
