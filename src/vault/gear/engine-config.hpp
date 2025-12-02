/*
  ENGINE-CONFIG.h  -  access point to any kind of engine configuration parameters

   Copyright (C)
     2013,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file engine-config.hpp
 ** access point to configuration of engine parameters
 ** @todo 6/2023 not clear if this is placeholder code or something substantial;
 **       however, it seems not plausible; rather I'd expect some data collection
 **       and information service to be connected with the RenderEnvironmentClosure
 */


#ifndef VAULT_GEAR_ENGINE_CONFIG_H
#define VAULT_GEAR_ENGINE_CONFIG_H



#include "lib/time/timevalue.hpp"
#include "lib/depend.hpp"


namespace vault{
namespace gear {
  
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
   * 
   * @deprecated 5/23 singleton access looks questionable; should be part of RenderEnvironmentClosure rather
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

}} // namespace vault::gear
#endif/*VAULT_GEAR_ENGINE_CONFIG_H*/
