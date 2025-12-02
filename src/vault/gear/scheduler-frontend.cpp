/*
  SchedulerFrontend  -  access point to the scheduler within the renderengine

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file scheduler-frontend.cpp
 ** Scheduler service implementation
 ** @warning as of 4/2023 Render-Engine integration work is underway ////////////////////////////////////////TICKET #1280
 ** 
 */


#include "lib/error.h"
#include "vault/gear/scheduler-frontend.hpp"

namespace vault{
namespace gear {
  
  
  /** storage for the (singleton) scheduler access frontend */
  lib::Depend<SchedulerFrontend> SchedulerFrontend::instance;
  
  
  
  
  /**
   * Switch the complete engine into diagnostics mode.
   * This activates additional logging and reporting facilities,
   * allowing to verify some specific operations within the engine
   * did indeed happen. Activating this mode incurs a performance hit.
   */
  void
  SchedulerFrontend::activateTracing()
  {
    UNIMPLEMENTED ("tracing/diagnostics mode of the render engine");
  }
  
  void
  SchedulerFrontend::disableTracing()
  {
    UNIMPLEMENTED ("tracing/diagnostics mode of the render engine");
    ///////////TODO ensure this is EX_FREE
  }
  
  
  
}} // namespace vault::gear
