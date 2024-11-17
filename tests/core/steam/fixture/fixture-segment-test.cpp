/*
  FixtureSegment(Test)  -  verify properties of a single segment in the fixture

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file fixture-segment-test.cpp
 ** unit test \ref FixtureSegment_test
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
  using lib::diff::MakeRec;
  
  using vault::gear::Job;
  using engine::JobTicket;
  using engine::test::MockSegmentation;
  
  
  /*****************************************************************************//**
   * @test Verify properties and behaviour of a single Segment in the Segmentation
   *       - construction of a mocked Segment
   *       - on-demand allocation of a JobTicket for a ModelPort(index)
   * @see steam::fixture::Segment
   * @see JobPlanningPipeline_test
   * @see MockSupport_test
   */
  class FixtureSegment_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          seedRand();
          fabricate_MockSegment();
          retrieve_JobTicket();
        }
      
      
      /** @test setup a properly structured ExitNode graph using the
       *        specification scheme supported by MockSegmentation
       *  @see MockSupport_test::verify_MockSegmentation
       */
      void
      fabricate_MockSegment()
        {
          //  Build a Segmentation partitioned at 10s
          MockSegmentation segmentation{MakeRec()
                                       .attrib ("start", Time{0,10}
                                               ,"mark",  101010)
                                       .genNode()};
          CHECK (2 == segmentation.size());
          Segment const& seg = segmentation[Time{0,20}];     // access anywhere >= 10s
          CHECK (Time(0,10)  == seg.start());
          CHECK (Time::NEVER == seg.after());
          CHECK (101010 == seg.exitNode[0].getPipelineIdentity());
        }
      
      
      /** @test on-demand generate a JobTicket from an existing NodeGraphAttachment
       */
      void
      retrieve_JobTicket()
        {
          MockSegmentation segmentation{MakeRec()
                                         .attrib("mark", 13)         // top-level: marked with hash/id = 13
                                         .scope(MakeRec()            //        ... defines two nested prerequisites
                                                 .attrib("mark",23)  //          + Prerequisite-1 hash/id = 23
                                               .genNode()
                                               ,MakeRec()
                                                 .attrib("mark",55)  //          + Prerequisite-2 hash/id = 55
                                               .genNode()
                                               )
                                       .genNode()};
          CHECK (1 == segmentation.size());                      // whole time axis covered by one segment
          Segment const& seg = segmentation[Time::ANYTIME];     //  thus accessed time point is irrelevant
          
          // verify mapped JobTicket is assembled according to above spec...
          auto getMarker = [](JobTicket& ticket)
                              {
                                Job job = ticket.createJobFor(Time::ANYTIME);
                                return job.parameter.invoKey.part.a;
                              };
          
          JobTicket& ticket = seg.jobTicket(0);
          CHECK (13 == getMarker (ticket));
          auto prereq = ticket.getPrerequisites();
          CHECK (not isnil(prereq));
          CHECK (55 == getMarker (*prereq));      // Note: order of prerequisites is flipped (by LinkedElements)
          ++prereq;
          CHECK (23 == getMarker (*prereq));
          ++prereq;
          CHECK (isnil(prereq));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (FixtureSegment_test, "unit fixture");
  
  
  
}}} // namespace steam::fixture::test
