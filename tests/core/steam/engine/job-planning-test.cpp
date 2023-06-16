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
 ** 
 ** @warning as of 4/2023 a complete rework of the Dispatcher is underway ///////////////////////////////////////////TICKET #1275
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

//#include "steam/engine/procnode.hpp"
//#include "steam/play/dummy-play-connection.hpp"
//#include "steam/mobject/model-port.hpp"
#include "steam/engine/mock-dispatcher.hpp"
#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/time/timequant.hpp"
#include "lib/format-cout.hpp" ///////////////TODO
//#include "lib/depend.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util-coll.hpp"
//#include "lib/util.hpp"

//#include <functional>
//#include <vector>

using test::Test;
//using util::isnil;
//using util::last;
//using std::vector;
//using std::function;
//using std::rand;


namespace steam {
namespace engine{
namespace test  {
  
  using lib::time::FrameRate;
//  using lib::time::Duration;
//  using lib::time::Offset;
//  using lib::time::TimeVar;
//  using lib::time::Time;
//  using mobject::ModelPort;
//  using play::Timings;
  
  namespace { // used internally
    
    
//    ModelPort
//    getTestPort()
//    {
//      return mockDispatcher().provideMockModelPort();
//    }
    
    
  } // (End) internal defs
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1301
  
  
  
  
  
  
  
  /***************************************************************//**
   * @test document and verify the data aggregation and the calculations
   *       necessary to prepare render jobs for scheduling.
   * @todo WIP-WIP 6/2023
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
          
          Time nominalTime{200,0};
          size_t portIDX = dispatcher.resolveModelPort (port);
          FrameCoord frame{nominalTime, portIDX};
          JobTicket& ticket = dispatcher.getJobTicketFor(frame);
          
          JobPlanning plan{frame,ticket,sink};
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
          
          Time nominalTime{200,0};
          size_t portIDX = dispatcher.resolveModelPort (port);
          FrameCoord frame{nominalTime, portIDX};
          JobTicket& ticket = dispatcher.getJobTicketFor(frame);
          
          JobPlanning plan{frame,ticket,sink};
          
          timings.playbackUrgency = play::ASAP;
          cout << plan.determineDeadline(timings) <<endl;
          CHECK (Time::ANYTIME == plan.determineDeadline (timings));
          
          timings.playbackUrgency = play::TIMEBOUND;
          ////////TODO where to set the anchor point realTime <-> nominalTime ??
          cout << plan.determineDeadline(timings) <<endl;
        }
      
      
      
      /** @test verify the setup of a prerequisite job in relation
       *        to the master job depending on this prerequisite
       */
      void
      setupDependentJob()
        {
          UNIMPLEMENTED ("chained deadlines");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (JobPlanning_test, "unit engine");
  
  
  
}}} // namespace steam::engine::test
