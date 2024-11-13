/*
  DispatcherInterface(Test)  -  document and verify dispatcher for frame job creation

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file dispatcher-interface-test.cpp
 ** unit test \ref DispatcherInterface_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/util.hpp"

#include <utility>

using test::Test;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FrameRate;
  using lib::time::Offset;
  using lib::time::Time;
  using play::Timings;
  
  using asset::Pipe;
  using PID = asset::ID<Pipe>;
  using mobject::ModelPort;
  using lumiera::error::LUMIERA_ERROR_LOGIC;

  
  
  
  
  
  /***************************************************************//**
   * @test document and verify the engine::Dispatcher interface, used
   *       to translate a CalcStream into individual node jobs.
   *       This test covers the definition of the interface itself,
   *       together with the supporting types and the default
   *       implementation of the basic operations.
   *       It uses a mock Dispatcher implementation.
   * @see JobPlanningPipeline_test
   */
  class DispatcherInterface_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           seedRand();
           resolveModelPort();
           accessJobTicket();
           pipelineBuilder();
        }
      
      
      /** @test the dispatcher can resolve a known ModelPort
       *        into the internal index number used on the Segmentation
       *        for the corresponding timeline (which exposes this ModelPort)
       */
      void
      resolveModelPort()
        {
          MockDispatcher dispatcher;
          auto [port,sink] = dispatcher.getDummyConnection(1);
          CHECK (1 == dispatcher.resolveModelPort (port));
          
          // but when using some arbitrary unrelated ModelPort...
          PID dazedPipe = Pipe::query ("id(dazed)");
          ModelPort evil = reinterpret_cast<ModelPort&> (dazedPipe);
          VERIFY_ERROR (LOGIC, dispatcher.resolveModelPort(evil));
        }
      
      
      
      /** @test the dispatcher knows how to pick the right JobTicket
       *        for each point on the timeline, and thus how to access
       *        the proper part of the render nodes responsible for
       *        rendering this part of the timeline
       */
      void
      accessJobTicket()
        {
          MockDispatcher dispatcher{MakeRec()                                       // a first active segment
                                     .attrib("start", Time{0,10})                   // covering the time [10s ... 20s[
                                     .attrib("after", Time{0,20})
                                     .attrib("mark", 23)                            // pipeline-Hash used as marker to verify proper access
                                   .genNode()
                                   ,MakeRec()                                       // add a second Segment
                                     .attrib("start", Time{0,20})                   // covering the rest of the timeline from 20s on
                                     .attrib("mark", 45)
                                   .genNode()};
          size_t portIDX = 1;
          // Dispatcher-Interface: access JobTicket
          JobTicket& ticket0 = dispatcher.getJobTicketFor (portIDX, -Time{0,5});
          JobTicket& ticket1 = dispatcher.getJobTicketFor (portIDX, Time{0,15});
          JobTicket& ticket2 = dispatcher.getJobTicketFor (portIDX, Time{0,25});
          
          CHECK (    ticket0.empty());                                              // this ticket was drawn from an undefined part of the timeline
          CHECK (not ticket1.empty());                                              // while this ticket belongs to the first segment
          CHECK (not ticket2.empty());                                              // and this to the second segment

          Job job0 = ticket0.createJobFor(-Time{0,5});
          Job job1 = ticket1.createJobFor(Time{0,15});
          Job job2 = ticket2.createJobFor(Time{0,25});
          
          CHECK (MockJob::isNopJob(job0));
          
          CHECK (Time(0,15) == job1.parameter.nominalTime);
          CHECK (23 == job1.parameter.invoKey.part.a);                              // proof that this job is connected to segment #1
          
          CHECK (Time(0,25) == job2.parameter.nominalTime);
          CHECK (45 == job2.parameter.invoKey.part.a);                              // and this one to segment #2
        }
      
      
      
      /** @test for the actual use case, the dispatcher acts as entrance point
       *        to a job-planning pipeline builder, which in the end is an iterator
       *        to pull render jobs from
       *  @see JobPlanningPipeline_test for in-depth coverage of this complex topic
       */
      void
      pipelineBuilder()
        {
          MockDispatcher dispatcher{MakeRec()                                       // a single segment covering the complete time-axis
                                     .attrib("mark", 555)                           // marker to demonstrate proper connectivity
                                   .genNode()};
          
          play::Timings timings (FrameRate::PAL);
          auto [port,sink] = dispatcher.getDummyConnection(1);
          
          // Dispatcher-Interface: pipeline builder...
          auto pipeline = dispatcher.forCalcStream (timings)
                                    .timeRange(Time{200,0}, Time{300,0})
                                    .pullFrom (port)
                                    .feedTo (sink);
          
          CHECK (not isnil (pipeline));
          CHECK (5 == pipeline.currFrameNr());                                      // 5 * 1/25sec = 200ms
          
          Job job = pipeline.buildJob();                                            // invoke the JobPlanning to build a Job for the first frame
          CHECK (Time(200,0) == job.parameter.nominalTime);
          CHECK (555 == job.parameter.invoKey.part.a);                              // the marker shows that this job is connected properly
          
          ++pipeline;                                                               // iterate to advance to the next frame
          CHECK (not isnil (pipeline));
          CHECK (6 == pipeline.currFrameNr());
          job = pipeline.buildJob();                                                // build job for the next frame
          CHECK (Time(240,0) == job.parameter.nominalTime);
          CHECK (555 == job.parameter.invoKey.part.a);
          
          ++pipeline;
          CHECK (7 == pipeline.currFrameNr());
          job = pipeline.buildJob();
          CHECK (Time(280,0) == job.parameter.nominalTime);
          
          ++pipeline;                                                               // iterate beyond end point
          CHECK (isnil (pipeline));                                                 // pipeline exhausted
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherInterface_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
