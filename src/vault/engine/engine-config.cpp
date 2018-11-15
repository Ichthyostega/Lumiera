/*
  EngineConfig  -  access point to any kind of engine configuration parameters

  Copyright (C)         Lumiera.org
    2013,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file engine-config.cpp
 ** implementation of engine configuration and parametrisation
 */


#include "backend/engine/engine-config.h"
#include "lib/time/timevalue.hpp"

#include <boost/rational.hpp>


using boost::rational;
using lib::time::FrameRate;
using lib::time::FSecs;


namespace backend{
namespace engine {
  
  namespace { // Hard wired placeholder settings...
    
    const rational<uint> ONE_THIRD(1,3);
    const rational<uint> EIGHTY_PERCENT(8,10);
    
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
  
}} // namespace backend::engine

namespace {
  using backend::engine::EngineConfig;
  
  // any C adapter functions go here...
  
}




extern "C" { /* ==== implementation C interface for engine configuration ======= */
  
gavl_time_t
lumiera_engine_get_latency  ()
{
  return _raw (EngineConfig::get().currentEngineLatency());
}

}//extern "C"
