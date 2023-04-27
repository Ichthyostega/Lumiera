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
#include "steam/fixture/segmentation.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/engine/dispatcher.hpp"
#include "steam/engine/job-ticket.hpp"
//#include "steam/play/timings.hpp"
#include "lib/time/timevalue.hpp"
////#include "lib/time/timequant.hpp"
////#include "lib/format-cout.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/depend.hpp"
#include "lib/itertools.hpp"
//#include "lib/util-coll.hpp"
#include "vault/real-clock.hpp"
#include "lib/test/test-helper.hpp"
#include "vault/engine/job.h"
#include "vault/engine/dummy-job.hpp"
//#include "lib/util.hpp"

//#include <functional>
//#include <vector>
#include <tuple>
#include <deque>

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
  
  using lib::diff::GenNode;
  using lib::diff::MakeRec;
//  using lib::time::FrameRate;
//  using lib::time::Duration;
//  using lib::time::Offset;
//  using lib::time::TimeVar;
  using lib::time::TimeValue;
//  using lib::time::Time;
//  using mobject::ModelPort;
//  using play::Timings;
  using std::make_tuple;
  using std::deque;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1294 : organisation of namespaces / includes??
  using fixture::Segmentation;
  
  namespace { // used internally
    
//    using play::PlayTestFrames_Strategy;
//    using play::ModelPorts;
    using vault::engine::JobClosure;
    using vault::engine::JobParameter;
    using vault::engine::DummyJob;
    
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
    
    /// @deprecated this setup is confusing and dangerous (instance identity is ambiguous)
    lib::Depend<MockDispatcherTable> mockDispatcher;
    
    
    
    
    
    inline auto
    defineBottomSpec()
    {
      auto emptyPrereq = lib::nilIterator<JobTicket&>();
      using Iter = decltype(emptyPrereq);
      return lib::singleValIterator(
                 std::tuple<JobFunctor&, Iter>(DummyJob::getFunctor()
                                              ,emptyPrereq
                                              ));
    }
    
  }//(End)internal test helpers....
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1221
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1221
  
  /**
   * Mock for...
   * 
   * @see JobPlanningSetup_test
   * @see DispatcherInterface_test
   * 
   */
  class MockJobTicket
    : public JobTicket
    {
      
    public:
      MockJobTicket()
        : JobTicket{defineBottomSpec()}
        { };
      
    private:
    };
  
  
  class MockSegmentation
    : public Segmentation
    {
      std::deque<MockJobTicket> tickets_;
      
    public:
      MockSegmentation()
        : tickets_{}
      { }
      
      MockSegmentation (std::initializer_list<GenNode> specs)
        : MockSegmentation{}
        {
          UNIMPLEMENTED ("populate mock sequence structure");
        }
    };
  
  
  
  /**  */
  
  
  
}}} // namespace steam::engine::test
#endif /*STEAM_ENGINE_TEST_DISPATCHER_MOCK_H*/
