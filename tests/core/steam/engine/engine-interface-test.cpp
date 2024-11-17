/*
  EngineInterface(Test)  -  verify basics of the engine (scheduling) service

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file engine-interface-test.cpp
 ** unit test \ref EngineInterface_test
 */


#include "lib/test/run.hpp"
#include "lib/error.hpp"

#include "steam/engine/calc-stream.hpp"
#include "steam/engine/engine-service.hpp"
#include "steam/engine/engine-diagnostics.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/play/diagnostic-output-slot.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/asset/pipe.hpp"
#include "lib/time/timevalue.hpp"

//#include <ctime>

using test::Test;
//using std::rand;


namespace steam  {
namespace engine{
namespace test  {
  
  using asset::Pipe;
  using asset::PPipe;
  using mobject::ModelPort;
  using steam::play::OutputSlot;
  using steam::play::DiagnosticOutputSlot;
  using lib::time::FrameRate;
  
  typedef asset::ID<Pipe> PID;
  typedef OutputSlot::Allocation Allocation;

  
  namespace { // test fixture...
    
  } // (End) test fixture
  
  
  
  /***************************************************************//**
   * @test cover the basic service exposed at the engine interface:
   *       create a calculation stream and verify the translation
   *       into individual jobs.
   * 
   * This test relies on the engine's diagnostic facilities, allowing
   * to log and verify the generated jobs without needing to execute
   * them. So this test doesn't actually run the engine. There are
   * \link OutputSlotProtocol_test other tests \endlink covering
   * the output generation separate from the engine.
   * 
   * @see CalcStream_test more in-depth coverage of the various
   *      flavours of calculation streams supported by the engine
   * @see EngineService
   * @see CalcStream
   * @see OutputSlotProtocol_test
   */
  class EngineInterface_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("simple standard case of Engine interface usage");
          
          EngineService& engine = EngineService::instance();
          EngineDiagnostics monitor(engine);
          
          PID pipe = Pipe::query("id(dummy)");
          ModelPort port(pipe);
          
          OutputSlot& oSlot = DiagnosticOutputSlot::build();
          Allocation& output = oSlot.allocate();
          Timings timings (FrameRate::PAL); /////////TODO
          
          // Invoke test subject...
          CalcStreams calc = engine.calculate(port, timings, output);
          
          ////TODO some direct checks on the calculation stream??
          
          CHECK (monitor.has_scheduled_jobs_for(timings));
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (EngineInterface_test, "function engine");
  
  
  
}}} // namespace steam::engine::test
