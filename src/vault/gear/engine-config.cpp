/*
  EngineConfig  -  access point to any kind of engine configuration parameters

   Copyright (C)
     2013,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-config.cpp
 ** implementation of engine configuration and parametrisation
 ** @todo 6/2023 not clear if this is placeholder code or something substantial;
 **       however, it seems not plausible; rather I'd expect some data collection
 **       and information service to be connected with the RenderEnvironmentClosure
 */


#include "vault/gear/engine-config.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/rational.hpp"


using lib::time::FrameRate;
using lib::time::FSecs;
using util::Rat;


namespace vault{
namespace gear {
  
  namespace { // Hard wired placeholder settings...
    
    const auto EIGHTY_PERCENT = 8_r/10;
    
    const Duration DEFAULT_ENGINE_LATENCY  = EIGHTY_PERCENT * Duration(1, FrameRate{25}); ///////TODO: shouldn't be hard wired and shouldn't be calculated in static/global init
    const Duration DEFAULT_JOB_PLANNING_TURNOVER(FSecs(3,2));
    
  }//(End)hard wired settings
  
  
  EngineConfig::~EngineConfig() { }
  
  
  /** storage for the Singleton accessor */
  lib::Depend<EngineConfig> EngineConfig::get;
  
  
  /** build up a new engine configuration set,
   *  based on reasonable default values
   *  
   * @warning using hard wired values as of 1/2013
   */
  EngineConfig::EngineConfig()
    { }
  
  
  
  
  Duration
  EngineConfig::currentEngineLatency()  const
  {
    return DEFAULT_ENGINE_LATENCY;
  }
  
  
  Duration
  EngineConfig::currentJobPlanningRhythm()  const
  {
    return DEFAULT_JOB_PLANNING_TURNOVER;
  }
  
}} // namespace vault::gear
