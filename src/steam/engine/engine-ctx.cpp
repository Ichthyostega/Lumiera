/*
  EngineCtx  -  Base implementation of global render engine operational services

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-ctx.cpp
 ** Base implementation of render engine operational service access.
 ** This singleton implementation provides and configures the access point,
 ** while the actual [»facilities«](engine-facilities.hpp), which are retrieved
 ** through interface methods, can be configured with the help of the
 ** [Dependency-Injection framework](depend-inject.hpp)
 ** @warning as of 1/2026 a preliminary integration of Render-Engine underway ////////////////////////////////////////TICKET #1367
 */


#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/engine-facilities.hpp"


namespace steam {
namespace engine{
  
  
  
  /** storage for the EngineService interface object */
  lib::Depend<EngineCtx> EngineCtx::access;
  
  /** configurable access point to the »Engine Facilities« */
  lib::Depend<EngineFacilities> accessEngineFacilities;
  
  
  
  /**
   * Bootstrap the EngineCtx access front-end.
   * The EngineCtx object instance is created on-demand through
   * lib::Depend<EngineCtx>; at that point, a service implementation
   * of the EngineFacilities is accessed; when used from a test setup,
   * this will be a "naive" implementation, based on heap allocations.
   * For the actual Render Engine, the EngineEnvironment must been
   * bootstrapped prior to this first access, to make the full set
   * of Render Engine services available through this interface.
   */
  EngineCtx::EngineCtx()
    : mem  {accessEngineFacilities().setupBufferProvider()}
    , cache{accessEngineFacilities().setupCacheProvider()}
    { }
  
  
  
}} // namespace steam::engine
