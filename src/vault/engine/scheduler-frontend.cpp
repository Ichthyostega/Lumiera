/*
  SchedulerFrontend  -  access point to the scheduler within the renderengine

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file scheduler-frontend.cpp
 ** Scheduler service implementation
 */


#include "lib/error.h"
#include "vault/engine/scheduler-frontend.hpp"

namespace backend{
namespace engine {
  
  
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
  
  
  
}} // namespace backend::engine
