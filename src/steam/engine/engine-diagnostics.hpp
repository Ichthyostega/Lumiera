/*
  ENGINE-DIAGNOSTICS.hpp  -  diagnostic facility to investigate engine operation

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-diagnostics.hpp
 ** An facility to check and monitor engine operations.
 ** Once created, an EngineDiagnostics object connects to the EngineService
 ** interface to activate additional tracing facilities within the engine,
 ** allowing to watch and verify the creation of individual jobs and further
 ** engine state parameters.
 ** 
 ** @see EngineInterface
 ** @see engine-interface-test.cpp
 ** @see calc-stream-test.cpp
 */


#ifndef STEAM_ENGINE_ENGINE_DIAGNOSTICS_H
#define STEAM_ENGINE_ENGINE_DIAGNOSTICS_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/engine/engine-service.hpp"
//#include "include/dummy-player-facade.h"
//#include "include/display-facade.h"
//#include "steam/engine/calc-stream.hpp"
//#include "steam/mobject/model-port.hpp"
#include "steam/play/timings.hpp"
//#include "steam/play/output-slot.hpp"
//#include "common/instancehandle.hpp"
//#include "lib/singleton-ref.hpp"
//#include "lib/polymorphic-value.hpp"
//#include "lib/depend.hpp"
//
//#include <string>


namespace steam  {
namespace engine{

//    using std::string;
//    using lumiera::Subsys;
//    using lumiera::Display;
//    using lumiera::DummyPlayer;
  using steam::play::Timings;
  
  
  
  
  
  /**************************************************//**
   * Render engine diagnostic facility. Creating an instance
   * will activate additional tracing facilities within the
   * render engine; results may be investigated through
   * EngineDiagnostics public functions. The object acts
   * like a smart handle, i.e. the tracing facilities will
   * be disabled and disconnected when going out of scope.
   * @warning not reentrant, no reference-counting.
   *          At any given time, at most a single instance
   *          of EngineDiagnostics may be used.  
   */
  class EngineDiagnostics
    : util::NonCopyable
    {
      EngineService& engine_;
      
    public:
      EngineDiagnostics (EngineService& e)
        : engine_(e)
        {
          UNIMPLEMENTED ("attach tracing connector");
          engine_.activateTracing();
        }
      
     ~EngineDiagnostics()
        {
          TODO ("detach tracing connector");
          engine_.disableTracing();
        }
      
      /** */
      bool
      has_scheduled_jobs_for (Timings const& timings)
        {
          UNIMPLEMENTED ("Engine Diagnostics: query scheduled jobs");
        }
    };
  
  
  
  
  
  
} // namespace engine
} // namespace steam
#endif
