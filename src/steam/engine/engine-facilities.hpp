/*
  ENGINE-FACILITIES.hpp  -  Access and coordination of global render engine operational services

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-facilities.hpp
 ** Interface to setup and access the render engine configuration and services.
 ** Without further setup, [dependency injection](\ref depend.hpp) will establish a
 ** minimalistic default instantiation of these services, sufficient for demonstration
 ** and tests. For the real render engine however, dedicated service instances will be
 ** created and managed actively, by the [»Render Environment«](\ref RenderEnvironment).
 ** @todo and we'll have yet to »figure out« what specifically to do for these
 **       services and how to start an active render engine for productive use.
 ** @remark this seemingly convoluted setup with several layers of abstraction is
 **       necessary to allow for "simple use without much ado" in a test scenario,
 **       while having a fully controlled lifecycle for the actual Render Engine.
 ** 
 ** @see engine-ctx.hpp
 ** @see engine-service.hpp
 ** @see weaving-pattern-builder.hpp
 */

#ifndef STEAM_ENGINE_ENGINE_FACILITIES_H
#define STEAM_ENGINE_ENGINE_FACILITIES_H


#include "steam/engine/engine-ctx.hpp"
#include "lib/nocopy.hpp"


namespace steam {
namespace engine{
  
  class EngineFacilities
    : util::NonCopyable
    {
      
    public:
      virtual ~EngineFacilities() { }  ///< this is an interface
      
      virtual BufferProvider& setupBufferProvider();
      virtual BufferProvider& setupCacheProvider();   ///////////////////////////////////////////////////////TICKET #1223 : very likely super interface required for the CacheService
    };
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_ENGINE_CTX_FACILITIES_H*/
