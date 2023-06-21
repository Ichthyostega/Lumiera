/*
  JobPlanning(Test)  -  data evaluation for frame job creation

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

/** @file job-planning-test.cpp
 ** unit test \ref JobPlanning_test
 */


#include "lib/test/run.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <utility>

using test::Test;
using std::move;
using util::isSameObject;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FrameRate;
  using lib::time::Offset;
  using lib::time::Time;
  using play::Timings;
  
  
  
  
  
  /***************************************************************//**
   * @test document and verify the data aggregation and the calculations
   *       necessary to prepare render jobs for scheduling.
   */
  class JobPlanning_test : public Test
    {
      
      virtual void
      run (Arg)
        {
           simpleUsage();
           calculateDeadline();
           setupDependentJob();
        }
      
      
      /** @test demonstrate a simple usage scenario
       */
      void
      simpleUsage()
        {
          MockDispatcher dispatcher;
          play::Timings timings (FrameRate::PAL);
          auto [port,sink] = dispatcher.getDummyConnection(1);
          
          FrameCnt frameNr{5};
          TimeVar nominalTime{Time{200,0}};
          size_t portIDX = dispatcher.resolveModelPort (port);
          JobTicket& ticket = dispatcher.getJobTicketFor(portIDX, nominalTime);
          
          JobPlanning plan{ticket,nominalTime,frameNr};
          Job job = plan.buildJob();
          
          CHECK (dispatcher.verify (job, port, sink));
        }
      
      
      
      /** @test verify the timing calculations to establish
       *        the scheduling deadline of a simple render job
       */
      void
      calculateDeadline()
        {
          MockDispatcher dispatcher;
          play::Timings timings (FrameRate::PAL, Time{0,0,5});
          auto [port,sink] = dispatcher.getDummyConnection(1);
          
          FrameCnt frameNr{5};
          Time nominalTime{200,0};
          size_t portIDX = dispatcher.resolveModelPort (port);
          JobTicket& ticket = dispatcher.getJobTicketFor(portIDX, nominalTime);
          
          JobPlanning plan{ticket,nominalTime,frameNr};

          // the following calculations are expected to happen....
          Duration latency = ticket.getExpectedRuntime()
                           + timings.engineLatency
                           + timings.outputLatency;
          
          Offset nominalOffset (timings.getFrameStartAt(0), timings.getFrameStartAt(frameNr));
          Time expectedDeadline{timings.scheduledDelivery + nominalOffset - latency};
          
          cout << util::_Fmt{"Frame #%d @ %s\n"
                             "real-time-origin : %s\n"
                             "total latency    : %s\n"
                             "deadline         : %s"}
                            % frameNr % nominalOffset
                            % timings.scheduledDelivery
                            % latency
                            % plan.determineDeadline(timings)
               << endl;
          CHECK (plan.determineDeadline(timings) == expectedDeadline);
          CHECK (timings.scheduledDelivery == Time(0,0,5)  );
          CHECK (timings.playbackUrgency == play::TIMEBOUND);
          
          // But when switching form "timebound" to "best effort"...
          timings.playbackUrgency = play::ASAP;
          CHECK (Time::ANYTIME == plan.determineDeadline (timings));
          // ... no deadline is calculated at all
        }
      
      
      
      /** @test verify the setup of a prerequisite job in relation
       *        to the master job depending on this prerequisite
       */
      void
      setupDependentJob()
        {
          MockDispatcher dispatcher{MakeRec()                                       // »master job« for each frame
                                       .attrib("runtime", Duration{Time{30,0}})
                                     .scope(MakeRec()                               // a »prerequisite job« on which the »master job« depends
                                             .attrib("runtime", Duration{Time{50,0}})
                                           .genNode())
                                   .genNode()};
          
          play::Timings timings (FrameRate::PAL, Time{0,0,5});
          auto [port,sink] = dispatcher.getDummyConnection(1);
          
          FrameCnt frameNr{5};
          Time nominalTime{200,0};
          size_t portIDX = dispatcher.resolveModelPort (port);
          JobTicket& ticket = dispatcher.getJobTicketFor(portIDX, nominalTime);
          JobTicket& prereq = *(ticket.getPrerequisites());                         // pick up the (only) prerequisite
          
          JobPlanning masterPlan{ticket,nominalTime,frameNr};                       // the job plan for the master frame calculation
          JobPlanning prereqPlan{move(*(masterPlan.buildDependencyPlanning() ))};   // build a plan for calculating the prerequisite
          
          CHECK (isSameObject(ticket, masterPlan.ticket()));
          CHECK (isSameObject(prereq, prereqPlan.ticket()));
          CHECK (    masterPlan.isTopLevel());
          CHECK (not prereqPlan.isTopLevel());
          
          Time masterDeadline = masterPlan.determineDeadline (timings);
          Time prereqDeadline = prereqPlan.determineDeadline (timings);

          // the following relations are expected to hold for the prerequisite....
          Duration latency = prereq.getExpectedRuntime()
                           + timings.engineLatency;                                 // Note: here only the engine, not the output latency
          
          Time expectedDeadline{masterDeadline - latency};
          
          cout << util::_Fmt{"Prerequisite......\n"
                             "master deadline  : %s\n"
                             "latency          : %s\n"
                             "prereq deadline  : %s"}
                            % masterDeadline
                            % latency
                            % prereqDeadline
               << endl;
          CHECK (prereqDeadline == expectedDeadline);
          
          // However, no deadline established for "best effort" rendering...
          timings.playbackUrgency = play::ASAP;
          CHECK (Time::ANYTIME == masterPlan.determineDeadline (timings));
          CHECK (Time::ANYTIME == prereqPlan.determineDeadline (timings));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (JobPlanning_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
