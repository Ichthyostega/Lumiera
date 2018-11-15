/*
  ENGINE-CONFIG.h  -  access point to any kind of engine configuration parameters

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

*/


/** @file engine-config.h
 ** access point to configuration of engine parameters
 */


#ifndef VAULT_ENGINE_ENGINE_CONFIG_H
#define VAULT_ENGINE_ENGINE_CONFIG_H



#include "lib/time.h"


#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "lib/time/timevalue.hpp"
#include "lib/depend.hpp"


namespace backend{
namespace engine {
  
  using lib::time::Duration;
  
  
  /**
   * Point of access for any kind of engine configuration, setup and performance tweaks.
   * Most of these parameters are derived from static system configuration or more focused
   * configuration settings within the session, but some values will be updated \em live
   * as a result of engine self monitoring. The user of this interface remains unaware
   * of this distinction. There is a sub interface (TODO planned 1/2013) for publishing
   * statistics changes and monitoring information.
   * 
   * @note while this interface \em exposes configuration, it is decoupled from
   *       any concerns regarding session and configuration representation.
   * 
   * @todo WIP-WIP as of 1/2013
   * @todo anything regarding configuration and session storage
   * @todo find out about the degree of locking required. Right now (1/13), there
   *       is no locking and all values are hard coded. It is conceivable to implement
   *       the \em access in a lock-free manner (by loosening any guarantee regarding
   *       the actual time point when a changed setting becomes visible)
   */
  class EngineConfig
    {
      EngineConfig();
     ~EngineConfig();
      
      friend class lib::DependencyFactory<EngineConfig>;
      
    public:
      /** access point to the Engine Interface.
       * @internal this is an facade interface for internal use
       *           by the player. Client code should use the Player.
       */
      static lib::Depend<EngineConfig> get;

      
      //////////////////////////////////////////////////////////////////// TODO: find out about required configuration and tweaking values
      
      
      /** reasonable guess of the current engine working delay.
       *  This is the latency to expect when requesting the calculation
       *  of a typical and average data frame, based on self observation
       *  in the recent past
       * @todo hard wired value as of 1/2013
       */
      Duration currentEngineLatency()  const;
      
      
      /** Time interval for ahead planning of render jobs.
       *  Frame calculation is broken down into individual jobs, and these
       *  jobs are prepared and scheduled chunk wise, while they are invoked
       *  as late as possible. This setting defines the time span to prepare
       *  and cover in a single planning chunk.
       */
      Duration currentJobPlanningRhythm()  const;
    };

}} // namespace backend::engine





extern "C" {
#endif /* =========================== CL Interface ===================== */


/** guess of the current effective engine calculation delay */
gavl_time_t lumiera_engine_get_latency  ();



#ifdef __cplusplus
}
#endif
#endif/*VAULT_ENGINE_ENGINE_CONFIG_H*/
