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
#include "steam/engine/mock-dispatcher.hpp"
#include "vault/engine/nop-job-functor.hpp"
#include "vault/engine/dummy-job.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/util-tuple.hpp"
#include "lib/util.hpp"
#include "lib/format-cout.hpp"
#include "lib/test/test-helper.hpp"

using test::Test;


///////////////////////////////////////////////////////TODO WIP for investigation
namespace lib {
namespace iter_explorer {
  template<class RES>
  using DecoTraits = _DecoratorTraits<RES>;
}}
///////////////////////////////////////////////////////TODO WIP for investigation
namespace steam {
namespace engine{
namespace test  {
  
  using steam::fixture::Segment;
  using vault::engine::DummyJob;
  using lib::singleValIterator;
  using util::isSameObject;
  using util::seqTuple;

  
  
  /**********************************************************************//**
   * @test validate test support for render job planning and dispatch.
   *       - creating and invoking mock render jobs
   *       - a mocked JobTicket, generating mock render jobs
   *       - configurable test setup for a mocked Segmentation datastructure
   * @see JobPlanningSetup_test
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
            
            JobTicket const& ticket = seg.jobTicket();    ///////////////////////////////////////////////////TICKET #1297 : will need to pass a ModelPort number here (use the first one, i.e. 0)
            
            FrameCoord coord;
            coord.absoluteNominalTime = Time(0,15);
            Job job = ticket.createJobFor(coord);
            CHECK (MockJobTicket::isAssociated (job, ticket));
            
            job.triggerJob();
            CHECK (DummyJob::was_invoked (job));
        }
      
      
      
      
      /** @test document and verify usage of a mock render job */
      void
      verify_MockJob()
        {
          Time nominalTime = lib::test::randTime();
          int additionalKey = rand() % 5000;
          Job mockJob = DummyJob::build (nominalTime, additionalKey);
          CHECK (mockJob.getNominalTime() == nominalTime);
          CHECK (not DummyJob::was_invoked (mockJob));
          
          mockJob.triggerJob();
          CHECK (DummyJob::was_invoked (mockJob));
          CHECK (RealClock::wasRecently (DummyJob::invocationTime (mockJob)));
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
          
          Time prevInvocation   = DummyJob::invocationTime (mockJob);
          mockJob.triggerJob();
          CHECK (prevInvocation < DummyJob::invocationTime (mockJob));                 // invoked again, recorded new invocation time
          CHECK (nominalTime   == DummyJob::invocationNominalTime (mockJob) );         // all other Job parameter recorded again unaltered
          CHECK (additionalKey == DummyJob::invocationAdditionalKey(mockJob));
        }
      
      
      /** @test document and verify usage of a mock JobTicket for frame dispatch */
      void
      verify_MockJobTicket()
        {
          FrameCoord coord;
          coord.absoluteNominalTime = lib::test::randTime();
          
          // build a render job to do nothing....
          Job nopJob = JobTicket::NOP.createJobFor(coord);
          CHECK (INSTANCEOF (vault::engine::NopJobFunctor, static_cast<JobClosure*> (nopJob.jobClosure)));   //////////TICKET #1287 : fix actual interface down to JobFunctor (after removing C structs)
          CHECK (nopJob.parameter.nominalTime == coord.absoluteNominalTime);
          InvocationInstanceID empty; ///////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
          CHECK (lumiera_invokey_eq (&nopJob.parameter.invoKey, &empty));
          
          MockJobTicket mockTicket;
          CHECK (mockTicket.discoverPrerequisites().empty());
          Job mockJob = mockTicket.createJobFor (coord);
          CHECK (    mockTicket.verify_associated (mockJob));                          // proof by invocation hash : is indeed backed by this JobTicket
          CHECK (not mockTicket.verify_associated (nopJob));                           // ...while some random other job is not related
        }
      
      
      /** @test document and verify usage of a complete mocked Segmentation
       *        to back frame dispatch
       */
      void
      verify_MockSegmentation()
        {
          FrameCoord coord;
          Time someTime = lib::test::randTime();
          coord.absoluteNominalTime = someTime;
          //-----------------------------------------------------------------/// Empty default Segmentation
          {
            MockSegmentation mockSeg;
            CHECK (1 == mockSeg.size());
            JobTicket const& ticket = mockSeg[someTime].jobTicket();
            CHECK (util::isSameObject (ticket, JobTicket::NOP));
          }
          //-----------------------------------------------------------------/// Segmentation with one default segment spanning complete timeline
          {
            MockSegmentation mockSegs{MakeRec().genNode()};
            CHECK (1 == mockSegs.size());
            CHECK (Time::MIN == mockSegs[someTime].start());
            CHECK (Time::MAX == mockSegs[someTime].after());
            JobTicket const& ticket = mockSegs[someTime].jobTicket();
            CHECK (not util::isSameObject (ticket, JobTicket::NOP));
            
            Job someJob = ticket.createJobFor(coord);                         // JobTicket uses, but does not check the time given in FrameCoord
            CHECK (someJob.parameter.nominalTime == _raw(coord.absoluteNominalTime));
            CHECK (MockJobTicket::isAssociated (someJob, ticket));            // but the generated Job is linked to the Closure backed by the JobTicket
            CHECK (not DummyJob::was_invoked (someJob));
            
            someJob.triggerJob();
            CHECK (DummyJob::was_invoked (someJob));
            CHECK (RealClock::wasRecently (DummyJob::invocationTime (someJob)));
            CHECK (someTime == DummyJob::invocationNominalTime (someJob));
          }
          //-----------------------------------------------------------------/// Segmentation with a segment spanning part of the timeline > 10s
          {
             // Marker to verify the job calls back into the right segment
            int marker = rand() % 1000;
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
            CHECK (    util::isSameObject (seg1.jobTicket(), JobTicket::NOP));
            CHECK (not util::isSameObject (seg2.jobTicket(), JobTicket::NOP));// this one is the active segment
            
            Job job = seg2.jobTicket().createJobFor(coord);
            CHECK (not MockJobTicket::isAssociated (job, seg1.jobTicket()));
            CHECK (    MockJobTicket::isAssociated (job, seg2.jobTicket()));
            CHECK (marker == job.parameter.invoKey.part.a);
            
            job.triggerJob();
            CHECK (DummyJob::was_invoked (job));
            CHECK (RealClock::wasRecently (DummyJob::invocationTime (job)));
            CHECK (marker == DummyJob::invocationAdditionalKey (job));        // DummyClosure is rigged such as to feed back the seed in `part.a`
                                                                              // and thus we can prove this job really belongs to the marked segment
            // create another job from the (empty) seg1
            job = seg1.jobTicket().createJobFor (coord);
            InvocationInstanceID empty; /////////////////////////////////////////////////////////////////////TICKET #1287 : temporary workaround until we get rid of the C base structs
            CHECK (lumiera_invokey_eq (&job.parameter.invoKey, &empty));      // indicates that it's just a placeholder to mark a "NOP"-Job
            CHECK (seg1.jobTicket().empty());
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
            
            Job job = s2.jobTicket().createJobFor(coord);
            job.triggerJob();
            CHECK (marker == DummyJob::invocationAdditionalKey (job));
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
                                
                                Job job = segment.jobTicket().createJobFor(coord);
                                job.triggerJob();
                                CHECK (DummyJob::was_invoked (job));
                                CHECK (RealClock::wasRecently (DummyJob::invocationTime (job)));
                                
                                return DummyJob::invocationAdditionalKey (job);
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
          FrameCoord coord;
          //-----------------------------------------------------------------/// one Segment with one additional prerequisite
          {
            MockSegmentation mockSegs{MakeRec()
                                     .attrib("mark", 11)
                                     .scope(MakeRec()
                                           .attrib("mark",23)
                                           .genNode())
                                     .genNode()};
            CHECK (1 == mockSegs.size());
            JobTicket const& ticket = mockSegs[Time::ZERO].jobTicket();
            auto prereq = ticket.getPrerequisites();
            CHECK (not isnil (prereq));
            
            JobTicket const& preTicket = *prereq;
            ++prereq;
            CHECK (isnil (prereq));
            
            Job job1 = preTicket.createJobFor (coord);
            Job job2 = ticket.createJobFor (coord);
            
            job1.triggerJob();
            job2.triggerJob();
            CHECK (23 == DummyJob::invocationAdditionalKey (job1));
            CHECK (11 == DummyJob::invocationAdditionalKey (job2));
          }
          //-----------------------------------------------------------------/// deep nested prerequisite
          {
            MockSegmentation mockSegs{MakeRec()
                                     .attrib("mark", 11)
                                     .scope(MakeRec()
                                           .attrib("mark",23)
                                           .genNode())
                                     .genNode()};
            
            using RTick = std::reference_wrapper<JobTicket>;
            auto start = singleValIterator (& util::unConst(mockSegs[Time::ZERO].jobTicket()));
            
            using SrC = lib::iter_explorer::BaseAdapter<lib::SingleValIter<engine::JobTicket*> >;
            
            auto bunny = [](JobTicket* ticket)
                                        {
                                          return lib::transformIterator(ticket->getPrerequisites()
                                                                       ,[](JobTicket const& preq) -> JobTicket*
                                                                          { return unConst(&preq); }
                                                                       );
                                        };
            using ExIt = decltype(bunny(std::declval<JobTicket*>()));
            // ergibt: lib::TransformIter<lib::TransformIter<lib::IterStateWrapper<steam::engine::JobTicket::Prerequisite, lib::LinkedElements<steam::engine::JobTicket::Prerequisite>::IterationState>, const steam::engine::JobTicket&>, steam::engine::JobTicket*>
            
            using Funny = std::function<ExIt(JobTicket*)>;
            Funny funny = bunny;
            
            using ExpandedChildren = typename lib::iter_explorer::_FunTraits<Funny,SrC>::Res;

//            using ResCore = lib::iter_explorer::Expander<SrC, ExpandedChildren>;
            
            using ResIter = typename lib::iter_explorer::DecoTraits<ExpandedChildren>::SrcIter;
            using ResIterVal = typename ResIter::value_type;
            using SrcIterVal = typename SrC::value_type;
//            lib::test::TypeDebugger<SrcIterVal> buggy;
//            lib::test::TypeDebugger<ExIt> bugggy;
            
            
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1294
            auto it = lib::explore(start)
//                          .transform ([](RTick t) -> JobTicket const&
//                                        {
//                                          return t.get();
//                                        })
                          .expand (funny)
                          .expandAll()
                          .transform ([&](JobTicket const& ticket)
                                        {
                                          return ticket.createJobFor(coord).parameter.invoKey.part.a;
                                        });
            cout << util::join(it,"-") <<endl;
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1294
          }
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (MockSupport_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
