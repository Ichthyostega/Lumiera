/*
  JobPlanningSetup(Test)  -  structure and setup of the job-planning pipeline

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

/** @file job-planning-setup-test.cpp
 ** unit test \ref CalcStream_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/engine/mock-dispatcher.hpp"

#include "lib/format-cout.hpp"///////////////////////TODO
#include "lib/iter-tree-explorer.hpp"
#include "lib/format-string.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"


using test::Test;
using lib::eachNum;
using lib::treeExplore;
using lib::time::PQuant;
using lib::time::FrameRate;
using util::isnil;
using util::_Fmt;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FixedFrameQuantiser;

  namespace { // test fixture...
    
    /** Diagnostic helper: join all the elements from some given container or iterable */
    template<class II>
    inline string
    materialise (II&& ii)
    {
      return util::join (std::forward<II> (ii), "-");
    }
    
    inline PQuant
    frameGrid (FrameRate fps)
    {
      return PQuant (new FixedFrameQuantiser (fps));
    }
    
  } // (End) test fixture
  
  
  
  /****************************************************************************//**
   * @test demonstrate interface, structure and setup of the job-planning pipeline.
   *       - using a frame step as base tick
   *       - invoke the dispatcher to retrieve the top-level JobTicket
   *       - expander function to explore prerequisite JobTickets
   *       - integration: generate a complete sequence of (dummy)Jobs
   *       - scaffolding and mocking used for this test
   * @remark the »pipeline« is implemented as »Lumiera Forward Iterator«
   *       and thus forms a chain of on-demand processing. At the output side,
   *       fully defined render Jobs can be retrieved, ready for scheduling.
   * @todo WIP-WIP 4/2023
   * 
   * @see DispatcherInterface_test
   * @see MockSupport_test
   * @see Dispatcher
   * @see CalcStream
   * @see RenderDriveS
   */
  class JobPlanningSetup_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          demonstrateScaffolding();
          buildBaseTickGenerator();
          accessTopLevelJobTicket();
          exploreJobTickets();
          integration();
        }
      
      
      /** @test document and verify the mock setup used for this test */
      void
      demonstrateScaffolding()
        {
          Time nominalTime = lib::test::randTime();
          int additionalKey = rand() % 5000;
          
          // (1) mocked render Job
          MockJob mockJob{nominalTime, additionalKey};
          mockJob.triggerJob();
          CHECK (MockJob::was_invoked (mockJob));
          CHECK (RealClock::wasRecently (MockJob::invocationTime (mockJob)));
          CHECK (nominalTime   == MockJob::invocationNominalTime (mockJob) );
          CHECK (additionalKey == MockJob::invocationAdditionalKey(mockJob));
          
          // (2) Build a mocked Segment at [10s ... 20s[
          MockSegmentation mockSegs{MakeRec()
                                     .attrib ("start", Time{0,10}          // start time (inclusive) of the Segment at 10sec
                                             ,"after", Time{0,20}          // the Segment ends *before* 20sec
                                             ,"mark",  123)                // marker-ID 123 (can be verified from Job invocation)
                                     .scope(MakeRec()                      // this JobTicket also defines a prerequisite ticket
                                             .attrib("mark",555)           // using a different marker-ID 555
                                           .genNode()
                                           )
                                   .genNode()};
          fixture::Segment const& seg = mockSegs[Time{0,15}];              // access anywhere 10s <= t < 20s
          JobTicket& ticket = seg.jobTicket(0);                            // get the master-JobTicket from this segment
          JobTicket& prereq = *(ticket.getPrerequisites());                // pull a prerequisite JobTicket
          
          FrameCoord coord;                                                // Frame coordinates for invocation (placeholder)
          Job jobP = prereq.createJobFor(coord);                           // create an instance of the prerequisites for this coordinates
          Job jobM = ticket.createJobFor(coord);                           // ...and an instance of the master job for the same coordinates
          CHECK (MockJobTicket::isAssociated (jobP, prereq));
          CHECK (MockJobTicket::isAssociated (jobM, ticket));
          CHECK (not MockJobTicket::isAssociated (jobP, ticket));
          CHECK (not MockJobTicket::isAssociated (jobM, prereq));
          
          jobP.triggerJob();
          jobM.triggerJob();
          CHECK (123 == MockJob::invocationAdditionalKey (jobM));          // verify each job was invoked and linked to the correct spec,
          CHECK (555 == MockJob::invocationAdditionalKey (jobP));          // indicating that in practice it will activate the proper render node
          
          // (3) demonstrate mocked frame dispatcher...
          coord.modelPortIDX = 1;
          coord.absoluteNominalTime = Time{0,30};
          MockDispatcher dispatcher;                                       // a complete dispatcher backed by a mock Segment for the whole timeline
          auto [port1,sink1] = dispatcher.getDummyConnection(1);           // also some fake ModelPort and DataSink entries are registered
          Job jobD = dispatcher.getJobTicketFor(coord).createJobFor(coord);
          CHECK (dispatcher.verify(jobD, port1, sink1));                   // the generated job uses the associated ModelPort and DataSink and JobTicket
        }
      
      
      
      /** @test use the Dispatcher interface (mocked) to generate a frame »beat«
       *        - demonstrate explicitly the mapping of a (frame) number sequence
       *          onto a sequence of time points with the help of time quantisation
       *        - use the Dispatcher API to produce the same frame time sequence
       *  @remark this is the foundation to generate top-level frame render jobs
       */
      void
      buildBaseTickGenerator()
        {
          auto grid = frameGrid(FrameRate::PAL);   // one frame ≙ 40ms
          
          CHECK (materialise (
                    treeExplore (eachNum(5,13))
                      .transform([&](FrameCnt frameNr) -> TimeVar          //////////////////////////////////TICKET #1261 : transform-iterator unable to handle immutable time
                                     {
                                       return grid->timeOf (frameNr);
                                     })
                    )
                 == "200ms-240ms-280ms-320ms-360ms-400ms-440ms-480ms"_expect);
          
          
          MockDispatcher dispatcher;
          play::Timings timings (FrameRate::PAL);
          
          CHECK (materialise (
                    treeExplore (
                      dispatcher.forCalcStream(timings)
                                .timeRange(Time{200,0}, Time{500,0})       // Note: end point is exclusive
                    ))
                 == "200ms-240ms-280ms-320ms-360ms-400ms-440ms-480ms"_expect);
        }
      
      
      /** @test use the base tick to access the corresponding JobTicket
       *        through the Dispatcher interface (mocked here).
       */
      void
      accessTopLevelJobTicket()
        {
          MockDispatcher dispatcher;
          
          play::Timings timings (FrameRate::PAL);
          auto [port,sink] = dispatcher.getDummyConnection(0);
          auto pipeline = dispatcher.forCalcStream (timings)
                                    .timeRange(Time{200,0}, Time{300,0})
                                    .pullFrom (port);
          
          CHECK (not isnil (pipeline));
          CHECK (nullptr == pipeline->first);       // is a top-level ticket
          JobTicket const& ticket = *pipeline->second;
          
          FrameCoord dummy{Time::ZERO};          // actual time point is irrelevant here
          Job job = ticket.createJobFor(dummy);
          CHECK (dispatcher.verify(job, port, sink));
        }
      
      
      /** @test build and verify the exploration function to discover job prerequisites
       *        - use a setup where the master ExitNode requires a prerequisite ExitNode to be pulled
       *        - mark the pipeline-IDs, so that both nodes can be distinguished in the resulting Jobs
       *        - the `expandPrerequisites()` builder function uses JobTicket::getPrerequisites()
       *        - and this »expander« function is unfolded recursively such that first the source
       *          appears in the iterator, and as next step the child prerequisites, possibly to
       *          be unfolded further recursively
       *        - by design of the iterator pipeline, it is always possible to access the `PipeFrameTick`
       *        - this corresponds to the top-level JobTicket, which will produce the final frame
       *        - putting all these information together, proper working can be visualised.
       */
      void
      exploreJobTickets()
        {
          MockDispatcher dispatcher{MakeRec()                                       // define a single segment for the complete time axis
                                     .attrib("mark", 11)                            // the »master job« for each frame has pipeline-ID ≔ 11
                                     .scope(MakeRec()
                                             .attrib("mark",22)                     // add a »prerequisite job« marked with pipeline-ID ≔ 22
                                           .genNode())
                                   .genNode()};
          
          play::Timings timings (FrameRate::PAL);
          auto [port,sink] = dispatcher.getDummyConnection(0);
          auto pipeline = dispatcher.forCalcStream (timings)
                                    .timeRange(Time{200,0}, Time{300,0})
                                    .pullFrom (port)
                                    .expandPrerequisites();
          
          // the first element is identical to previous test
          CHECK (not isnil (pipeline));
          CHECK (nullptr == pipeline->first);
          Job job = pipeline->second->createJobFor(FrameCoord{});
          CHECK (11 == job.parameter.invoKey.part.a);
          
          auto visualise = [](auto& pipeline) -> string
                              {
                                FrameCoord frame{pipeline.currPoint};               // can access the embedded PipeFrameTick core to get "currPoint" (nominal time)
                                Job job = pipeline->second->createJobFor(frame);    // looking always at the second element, which is the current JobTicket
                                TimeValue nominalTime{job.parameter.nominalTime};   // job parameter holds the microseconds (gavl_time_t)
                                int32_t mark = job.parameter.invoKey.part.a;        // the MockDispatcher places the given "mark" here
                                return _Fmt{"J(%d|%s)"} % mark % nominalTime;
                              };
          CHECK (visualise(pipeline) == "J(11|200ms)"_expect);                      // first job in pipeline is at t=200ms and has mark=11 (it's the master Job for this frame)
          
          CHECK (materialise (pipeline.transform (visualise))
                 == "J(11|200ms)-J(22|200ms)-J(11|240ms)-J(22|240ms)-J(11|280ms)-J(22|280ms)"_expect);
        }
      
      
      /** @test Job-planning pipeline integration test
       *  @remark generating dummy jobs for verification
       */
      void
      integration()
        {
          UNIMPLEMENTED ("integration incl. generation of dummy jobs");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (JobPlanningSetup_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
