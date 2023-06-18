/*
  MockSupport(Test)  -  verify test support for fixture and job dispatch

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

/** @file mock-support-test.cpp
 ** unit test \ref MockSupport_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "vault/engine/nop-job-functor.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/util-tuple.hpp"
#include "lib/util.hpp"


using test::Test;


namespace steam {
namespace engine{
namespace test  {
  
  using steam::fixture::Segment;
  using lib::singleValIterator;
  using util::isSameObject;
  using util::seqTuple;

  
  
  /**********************************************************************//**
   * @test validate test support for render job planning and dispatch.
   *       - creating and invoking mock render jobs
   *       - a mocked JobTicket, generating mock render jobs
   *       - configurable test setup for a mocked Segmentation datastructure
   *       - configurable setup of a complete frame Dispatcher
   * @see JobPlanningPipeline_test
   * @see Dispatcher
   * @see vault::engine::Job
   * @see steam::fixture::Segmentation
   */
  class MockSupport_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simpleUsage();
          verify_MockJob();
          verify_MockJobTicket();
          verify_MockSegmentation();
          verify_MockPrerequisites();
          verify_MockDispatcherSetup();
        }
      
      
      /** @test simple usage example of the test helpers */
      void
      simpleUsage()
        {
            //  Build a simple Segment at [10s ... 20s[
            MockSegmentation mockSegs{MakeRec()
                                     .attrib ("start", Time{0,10}
                                             ,"after", Time{0,20})
                                     .genNode()};
            CHECK (3 == mockSegs.size());
            fixture::Segment const& seg = mockSegs[Time{0,15}];              // access anywhere 10s <= t < 20s
            
            JobTicket& ticket = seg.jobTicket(0);
            
            Job job = ticket.createJobFor (Time{0,15});
            CHECK (MockJobTicket::isAssociated (job, ticket));
            
            job.triggerJob();
            CHECK (MockJob::was_invoked (job));
        }
      
      
      
      
      /** @test document and verify usage of a mock render job */
      void
      verify_MockJob()
        {
          Time nominalTime = lib::test::randTime();
          int additionalKey = rand() % 5000;
          MockJob mockJob{nominalTime, additionalKey};
          CHECK (mockJob.getNominalTime() == nominalTime);
          CHECK (not MockJob::was_invoked (mockJob));
          
          mockJob.triggerJob();
          CHECK (MockJob::was_invoked (mockJob));
          CHECK (RealClock::wasRecently (MockJob::invocationTime (mockJob)));
          CHECK (nominalTime   == MockJob::invocationNominalTime (mockJob) );
          CHECK (additionalKey == MockJob::invocationAdditionalKey(mockJob));
          
          Time prevInvocation   = MockJob::invocationTime (mockJob);
          mockJob.triggerJob();
          CHECK (prevInvocation < MockJob::invocationTime (mockJob));                  // invoked again, recorded new invocation time
          CHECK (nominalTime   == MockJob::invocationNominalTime (mockJob) );          // all other Job parameter recorded again unaltered
          CHECK (additionalKey == MockJob::invocationAdditionalKey(mockJob));
        }
      
      
      /** @test document and verify usage of a mock JobTicket for frame dispatch */
      void
      verify_MockJobTicket()
        {
          auto frameTime = lib::test::randTime();
          
          // build a render job to do nothing....
          Job nopJob = JobTicket::NOP.createJobFor (frameTime);
          CHECK (INSTANCEOF (vault::engine::NopJobFunctor, static_cast<JobClosure*> (nopJob.jobClosure)));   //////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          CHECK (nopJob.parameter.nominalTime == frameTime);
          InvocationInstanceID empty; ///////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
          CHECK (lumiera_invokey_eq (&nopJob.parameter.invoKey, &empty));
          CHECK (MockJob::isNopJob(nopJob));                                           // this diagnostic helper checks the same conditions as done here explicitly
          
          MockJobTicket mockTicket;
          CHECK (not mockTicket.empty());
          Job mockJob = mockTicket.createJobFor (frameTime);
          CHECK (    mockTicket.verify_associated (mockJob));                          // proof by invocation hash : is indeed backed by this JobTicket
          CHECK (not mockTicket.verify_associated (nopJob));                           // ...while some random other job is not related
          CHECK (not MockJob::isNopJob(mockJob));
        }
      
      
      
      /** @test document and verify usage of a complete mocked Segmentation
       *        to back frame dispatch
       *        - default constructed: empty Segmentation
       *        - cover the whole axis with one segment
       *        - partition axis and verify the association of generated jobs
       *        - a fully defined segment within an otherwise empty axis
       *        - complex partitioning (using the »split-splice« mechanism
       */
      void
      verify_MockSegmentation()
        {
          Time someTime = lib::test::randTime();
          //
          //-----------------------------------------------------------------/// Empty default Segmentation
          {
            MockSegmentation mockSeg;
            CHECK (1 == mockSeg.size());
            JobTicket const& ticket = mockSeg[someTime].jobTicket(0);         // just probe JobTicket generated for Model-Port-Nr.0
            CHECK (util::isSameObject (ticket, JobTicket::NOP));
          }
          //-----------------------------------------------------------------/// Segmentation with one default segment spanning complete timeline
          {
            MockSegmentation mockSegs{MakeRec().genNode()};
            CHECK (1 == mockSegs.size());
            CHECK (Time::MIN == mockSegs[someTime].start());
            CHECK (Time::MAX == mockSegs[someTime].after());
            JobTicket& ticket = mockSegs[someTime].jobTicket(0);
            CHECK (not util::isSameObject (ticket, JobTicket::NOP));
            
            Job someJob = ticket.createJobFor(someTime);                       // JobTicket uses, but does not check the time given
            CHECK (someJob.parameter.nominalTime == someTime);
            CHECK (MockJobTicket::isAssociated (someJob, ticket));            // but the generated Job is linked to the Closure backed by the JobTicket
            CHECK (not MockJob::was_invoked (someJob));
            
            someJob.triggerJob();
            CHECK (MockJob::was_invoked (someJob));
            CHECK (RealClock::wasRecently (MockJob::invocationTime (someJob)));
            CHECK (someTime == MockJob::invocationNominalTime (someJob));
          }
          //-----------------------------------------------------------------/// Segmentation with a segment spanning part of the timeline > 10s
          {
             // Marker to verify the job calls back into the right segment
            int marker = rand() % 1000;
            //
            //  Build a Segmentation partitioned at 10s
            MockSegmentation mockSegs{MakeRec()
                                     .attrib ("start", Time{0,10}
                                             ,"mark",  marker)
                                     .genNode()};
            CHECK (2 == mockSegs.size());
            // since only start-time was given, the SplitSplice-Algo will attach
            // the new Segment starting at 10s and expand towards +∞,
            // while the left part of the axis is marked as NOP / empty
            fixture::Segment const& seg1 = mockSegs[Time::ZERO];              // access anywhere < 10s
            fixture::Segment const& seg2 = mockSegs[Time{0,20}];              // access anywhere >= 10s
            CHECK (    util::isSameObject (seg1.jobTicket(0),JobTicket::NOP));
            CHECK (not util::isSameObject (seg2.jobTicket(0),JobTicket::NOP));// this one is the active segment
            
            Job job = seg2.jobTicket(0).createJobFor(someTime);
            CHECK (not MockJobTicket::isAssociated (job, seg1.jobTicket(0)));
            CHECK (    MockJobTicket::isAssociated (job, seg2.jobTicket(0)));
            CHECK (marker == job.parameter.invoKey.part.a);
            
            job.triggerJob();
            CHECK (MockJob::was_invoked (job));
            CHECK (RealClock::wasRecently (MockJob::invocationTime (job)));
            CHECK (marker == MockJob::invocationAdditionalKey (job));         // DummyClosure is rigged such as to feed back the seed in `part.a`
                                                                              // and thus we can prove this job really belongs to the marked segment
            // create another job from the (empty) seg1
            job = seg1.jobTicket(0).createJobFor (someTime);
            InvocationInstanceID empty; /////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
            CHECK (lumiera_invokey_eq (&job.parameter.invoKey, &empty));      // indicates that it's just a placeholder to mark a "NOP"-Job
            CHECK (seg1.jobTicket(0).empty());
            CHECK (seg1.empty());
            CHECK (not seg2.empty());
          }
          //-----------------------------------------------------------------/// Segmentation with one delineated segment, and otherwise empty
          {
            int marker = rand() % 1000;
            //  Build Segmentation with one fully defined segment
            MockSegmentation mockSegs{MakeRec()
                                     .attrib ("start", Time{0,10}
                                             ,"after", Time{0,20}
                                             ,"mark",  marker)
                                     .genNode()};
            CHECK (3 == mockSegs.size());
            auto const& [s1,s2,s3] = seqTuple<3> (mockSegs.eachSeg());
            CHECK (s1.empty());
            CHECK (not s2.empty());
            CHECK (s3.empty());
            CHECK (isSameObject (s2, mockSegs[Time{0,10}]));
            CHECK (Time::MIN  == s1.start());
            CHECK (Time(0,10) == s1.after());
            CHECK (Time(0,10) == s2.start());
            CHECK (Time(0,20) == s2.after());
            CHECK (Time(0,20) == s3.start());
            CHECK (Time::MAX  == s3.after());
            
            Job job = s2.jobTicket(0).createJobFor(someTime);
            job.triggerJob();
            CHECK (marker == MockJob::invocationAdditionalKey (job));
          }
          //-----------------------------------------------------------------/// Segmentation with several segments built in specific order
          {
            //  Build Segmentation by partitioning in several steps
            MockSegmentation mockSegs{MakeRec()
                                     .attrib ("start", Time{0,20}             // note: inverted segment definition is rectified automatically
                                             ,"after", Time{0,10}
                                             ,"mark",  1)
                                     .genNode()
                                     ,MakeRec()
                                     .attrib ("after", Time::ZERO
                                             ,"mark",  2)
                                     .genNode()
                                     ,MakeRec()
                                     .attrib ("start", Time{FSecs{-5}}
                                             ,"mark",  3)
                                     .genNode()};

            CHECK (5 == mockSegs.size());
            auto const& [s1,s2,s3,s4,s5] = seqTuple<5> (mockSegs.eachSeg());
            CHECK (not s1.empty());
            CHECK (not s2.empty());
            CHECK (    s3.empty());
            CHECK (not s4.empty());
            CHECK (    s5.empty());
            CHECK (Time::MIN  == s1.start());                                 // the second added segment has covered the whole negative axis
            CHECK (-Time(0,5) == s1.after());                                 // ..up to the partitioning point -5
            CHECK (-Time(0,5) == s2.start());                                 // ...while the rest was taken up by the third added segment
            CHECK (Time(0, 0) == s2.after());
            CHECK (Time(0, 0) == s3.start());                                 // an empty gap remains between [0 ... 10[
            CHECK (Time(0,10) == s3.after());
            CHECK (Time(0,10) == s4.start());                                 // here is the first added segment
            CHECK (Time(0,20) == s4.after());
            CHECK (Time(0,20) == s5.start());                                 // and the remaining part of the positive axis is empty
            CHECK (Time::MAX  == s5.after());
            
            auto probeKey = [&](Segment const& segment)
                              {
                                if (segment.empty()) return 0;
                                
                                Job job = segment.jobTicket(0).createJobFor(someTime);
                                job.triggerJob();
                                CHECK (MockJob::was_invoked (job));
                                CHECK (RealClock::wasRecently (MockJob::invocationTime (job)));
                                
                                return MockJob::invocationAdditionalKey (job);
                              };
            CHECK (2 == probeKey(s1));                                        // verify all generated jobs are wired back to the correct segment
            CHECK (3 == probeKey(s2));
            CHECK (0 == probeKey(s3));
            CHECK (1 == probeKey(s4));
            CHECK (0 == probeKey(s5));
          }
        }
      
      
      
      /**
       * @test build a Segment with additional prerequisites,
       *       resulting in additional JobTickets to explore and
       *       additional prerequisite Jobs to build for each frame.
       */
      void
      verify_MockPrerequisites()
        {
          Time someTime = lib::test::randTime();
          //-----------------------------------------------------------------/// one Segment with one additional prerequisite
          {
            MockSegmentation mockSegs{MakeRec()
                                     .attrib("mark", 11)
                                     .scope(MakeRec()
                                           .attrib("mark",23)
                                           .genNode())
                                     .genNode()};
            CHECK (1 == mockSegs.size());
            JobTicket& ticket = mockSegs[Time::ZERO].jobTicket(0);            // Model-PortNr.0
            auto prereq = ticket.getPrerequisites();
            CHECK (not isnil (prereq));
            
            JobTicket& preTicket = *prereq;
            ++prereq;
            CHECK (isnil (prereq));
            
            Job job1 = preTicket.createJobFor (someTime);
            Job job2 = ticket.createJobFor (someTime);
            
            job1.triggerJob();
            job2.triggerJob();
            CHECK (23 == MockJob::invocationAdditionalKey (job1));
            CHECK (11 == MockJob::invocationAdditionalKey (job2));
          }
          //-----------------------------------------------------------------/// a tree of deep nested prerequisites
          {
            MockSegmentation mockSegs{MakeRec()
                                       .attrib("mark", 11)
                                       .scope(MakeRec()
                                               .attrib("mark",33)
                                               .scope(MakeRec()
                                                       .attrib("mark",55)
                                                     .genNode()
                                                     ,MakeRec()
                                                       .attrib("mark",44)
                                                     .genNode()
                                                     )
                                             .genNode()
                                             ,MakeRec()
                                               .attrib("mark",22)
                                             .genNode())
                                     .genNode()};
            
            auto start = singleValIterator (mockSegs[Time::ZERO].jobTicket(0));
            
            auto it = lib::explore(start)
                          .expand ([](JobTicket& ticket)
                                        {
                                          return ticket.getPrerequisites();
                                        })
                          .expandAll()
                          .transform ([&](JobTicket& ticket)
                                        {
                                          return ticket.createJobFor(someTime).parameter.invoKey.part.a;
                                        });
            
            
            CHECK (util::join(it,"-") == "11-22-33-44-55"_expect);
          }                           //  Note: Prerequisites are prepended (LinkedElements)
        }                            //         thus at each level the last ones appear first
      
      
      
      /** @test verify setup of a mocked Dispatcher instance
       *        - by default, MockDispatcher generates a single segment
       *          to span the whole Time-axis and with some random yet valid pipeline-ID,
       *          so that a single job ticket can be generated for each port everywhere
       *        - in addition, it is possible to use the same specification language
       *          as for Segmentation to define a more complex (mock)processing graph
       *  @note lacklustre ModelPort handling: processing graph is just duplicated for
       *        each valid model port — not clear yet if we ever need something better...
       */
      void
      verify_MockDispatcherSetup()
        {
          {
            MockDispatcher dispatcher;
            // automatically generates some fake connection points...
            auto [port0,sink0] = dispatcher.getDummyConnection(0);
            auto [port1,sink1] = dispatcher.getDummyConnection(1);
            CHECK (port0 != port1);
            CHECK (sink0 != sink1);
            CHECK (port0.isValid());
            CHECK (port1.isValid());
            CHECK (sink0.isValid());
            CHECK (sink1.isValid());
            CHECK (not ModelPort().isValid());
            CHECK (not DataSink().isValid());
            
            CHECK (0 == dispatcher.resolveModelPort(port0));
            CHECK (1 == dispatcher.resolveModelPort(port1));
            
            Time frameTime{0,30};
            size_t modelPortIDX = 0;
            Job job0 = dispatcher.createJobFor (modelPortIDX, frameTime);
            modelPortIDX = 1;
            Job job1 = dispatcher.createJobFor (modelPortIDX, frameTime);
            CHECK (dispatcher.verify(job0, port0, sink0));
            CHECK (dispatcher.verify(job1, port1, sink1));
          }
          //-----------------------------------------------------------------/// can define multiple Segments
          {
            MockDispatcher dispatcher{MakeRec()
                                       .attrib("mark", 11)
                                     .genNode()
                                     ,MakeRec()
                                       .attrib("mark", 22)
                                       .attrib("start", Time{0,10})           // second segment covers 10s … +Time::MAX
                                     .genNode()};
            
            size_t modelPortIDX = 1;
            Job job0 = dispatcher.createJobFor (modelPortIDX, Time{0,5});
            Job job1 = dispatcher.createJobFor (modelPortIDX, Time{0,25});
            
            CHECK (11 == job0.parameter.invoKey.part.a);
            CHECK (22 == job1.parameter.invoKey.part.a);
          }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MockSupport_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
