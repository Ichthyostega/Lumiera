/*
  RenderEnvironment  -  setup of the Lumiera Render Engine

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file render-environment.cpp
 ** Base implementation of render engine operational service access.
 ** This singleton implementation provides and configures the access point,
 ** while the actual [»facilities«](engine-facilities.hpp), which are retrieved
 ** through interface methods, can be configured with the help of the
 ** [Dependency-Injection framework](depend-inject.hpp)
 ** @warning as of 1/2026 a preliminary integration of Render-Engine underway ////////////////////////////////////////TICKET #1367
 */


#include "steam/engine/render-environment.hpp"
#include "steam/engine/engine-facilities.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "steam/engine/heap-mem-provider.hpp"


namespace steam {
namespace engine{
  
  
  namespace {
    /** A simplistic buffer manager for tests
     * (Meyer's Singleton) */
    BufferProvider&
    getNaiveBufferManager()
    {
      static HeapMemProvider buffProvider;               ////////////////////////////////////////////////////TICKET #1367 : only suitable for first prototype; should implement a bare bone memory manager instead
      return buffProvider;
    }
  }
  
  /* ===== Render Environment Implementation ===== */
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1409 : actually build that! right now (1/2026) it is a conceptual placeholder
  /// The real Render Engine needs a well defined life cycle;
  /// services must be booted and shut down.
  /// Notably a dedicated serivce-like subclass of EngineCtx
  /// must be created here and installed for lib::Depend<EngineCtx>
  ///
  
  
  /* ===== Access to Render Engine Facilities ===== */
  
  BufferProvider&
  EngineFacilities::setupBufferProvider()
  {
    return getNaiveBufferManager();
  }
  
  BufferProvider&
  EngineFacilities::setupCacheProvider()     ////////////////////////////////////////////////////////////////TICKET #1223 : in what ways does client code need to interact with the Frame Cache / Management?
  {
    return getNaiveBufferManager();
  }
  
  
  
}} // namespace steam::engine
