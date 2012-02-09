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


#include "lib/test/run.hpp"
#include "lib/error.hpp"

//#include "proc/engine/procnode.hpp"
#include "proc/engine/dispatcher.hpp"
#include "proc/play/timings.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/singleton.hpp"

//#include <boost/scoped_ptr.hpp>
//#include <iostream>

using test::Test;
//using std::cout;
//using std::rand;


namespace proc {
namespace engine{
namespace test  {
  
  using lib::time::QuTime;
  using lib::time::FrameRate;
  using lib::time::Duration;
  using proc::play::Timings;
  
  namespace { // used internally
    
    
    class MockDispatcherTable
      : public Dispatcher
      {
        
        FrameCoord
        locateFrameNext (uint frameCountOffset)
          {
            UNIMPLEMENTED ("dummy implementation of the core dispatch operation");
          }

      public:
      };
    
    lib::Singleton<MockDispatcherTable> mockDispatcher;
    
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #890
      
  } // (End) internal defs
  
  
  
  /*******************************************************************
   * @test document and verify the engine::Dispatcher interface, used
   *       to translate a CalcStream into individual node jobs.
   *       This test covers the definition of the interface itself,
   *       together with the supporting types and the default
   *       implementation of the basic operations.
   *       It creates and uses  a mock Dispatcher implementation.
   */
  class DispatcherInterface_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
           verify_basicDispatch();
           verify_standardDispatcherUsage();
        }
      
      
      /** @test perform the basic dispatch step
       * and verify the generated frame coordinates
       */
      void
      verify_basicDispatch()
        {
          Dispatcher& dispatcher = mockDispatcher();
          Timings timings (FrameRate::PAL);
          uint startFrame(10);
          
          TimeAnchor refPoint = TimeAnchor::build (timings, startFrame);
          CHECK (refPoint == Time::ZERO + Duration(10, FrameRate::PAL));
          
          FrameCoord coordinates = dispatcher.locateFrameNext (15);
          CHECK (coordinates.absoluteNominalTime == Time(0,1));
          CHECK (coordinates.absoluteFrameNumber == 25);
          CHECK (coordinates.remainingRealTime() >= Time(FSecs(24,25)));
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
        }
      
      
      /** @test the standard invocation sequence
       * used within the engine for planning new jobs.
       * The actual implementation is mocked.
       */
      void
      verify_standardDispatcherUsage()
        {
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #880
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (DispatcherInterface_test, "unit engine");
  
  
  
}}} // namespace proc::engine::test
