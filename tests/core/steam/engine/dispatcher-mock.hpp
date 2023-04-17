/*
  DISPATCHER-MOCK.hpp  -  test scaffolding to verify render job planning and dispatch

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

*/

/** @file testframe.hpp
 ** Unit test helper to generate fake test data frames
 */


#ifndef STEAM_ENGINE_TEST_DISPATCHER_MOCK_H
#define STEAM_ENGINE_TEST_DISPATCHER_MOCK_H


////#include "steam/engine/procnode.hpp"
//#include "steam/play/dummy-play-connection.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
//#include "steam/play/timings.hpp"
//#include "lib/time/timevalue.hpp"
////#include "lib/time/timequant.hpp"
////#include "lib/format-cout.hpp"
#include "lib/depend.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util-coll.hpp"
#include "lib/test/test-helper.hpp"
//#include "lib/util.hpp"

//#include <functional>
//#include <vector>

//using test::Test;
//using util::isnil;
//using util::last;
//using std::vector;
//using std::function;
//using std::rand;

//#include <cstdlib>
//#include <stdint.h>


namespace steam  {
namespace engine {
namespace test   {
  
//  using lib::time::FrameRate;
//  using lib::time::Duration;
//  using lib::time::Offset;
//  using lib::time::TimeVar;
//  using lib::time::Time;
//  using mobject::ModelPort;
//  using play::Timings;
  
  namespace { // used internally
    
//    using play::PlayTestFrames_Strategy;
//    using play::ModelPorts;
    
//    typedef play::DummyPlayConnection<play::PlayTestFrames_Strategy> DummyPlaybackSetup;
    
    
    class MockDispatcherTable
      : public Dispatcher
      {
        
//        DummyPlaybackSetup dummySetup_;
        
        
        /* == mock Dispatcher implementation == */
        
        FrameCoord
        locateRelative (FrameCoord const&, FrameCnt frameOffset)
          {
            UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
          }
        
        bool
        isEndOfChunk (FrameCnt, ModelPort port)
          {
            UNIMPLEMENTED ("determine when to finish a planning chunk");
          }

        JobTicket&
        accessJobTicket (ModelPort, TimeValue nominalTime)
          {
            UNIMPLEMENTED ("dummy implementation of the model backbone / segmentation");
          }
        
      public:
        
        ModelPort
        provideMockModelPort()
          {
//            ModelPorts mockModelPorts = dummySetup_.provide_testModelPorts();
//            return *mockModelPorts;  // using just the first dummy port
          }
      };
    
    lib::Depend<MockDispatcherTable> mockDispatcher;
    
  }//(End)internal test helpers....
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #890
  
  /**
   * Mock for...
   * 
   * @see JobPlanningsetup_test
   * @see DispatcherInterface_test
   * 
   */
  class MockJobTicket
    {
      
    public:
     ~MockJobTicket();
      
    private:
    };
  
  
  
  /**  */
  
  
  
}}} // namespace steam::engine::test
#endif /*STEAM_ENGINE_TEST_DISPATCHER_MOCK_H*/
