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
 ** Implementation of render engine operational configuration and services.
 ** Without further setup, [dependency injection](\ref depend.hpp) will establish a
 ** minimalistic default instantiation of these services, sufficient for demonstration
 ** and tests. For the real render engine however, dedicated service instances will be
 ** created and managed actively.
 ** @todo and we'll have yet to »figure out« what specifically to do for these
 **       services and how to start an active render engine for productive use.
 ** @deprecated 1/2026 I'm unhappy with this convoluted structure;
 **             my intention is to fuse that with the engine-ctx.cpp implementation.
 ** 
 ** @see engine-ctx.hpp
 ** @see weaving-pattern-builder.hpp
 */

#ifndef STEAM_ENGINE_ENGINE_FACILITIES_H
#define STEAM_ENGINE_ENGINE_FACILITIES_H


#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/buffer-provider.hpp"
#include "lib/nocopy.hpp"

//#include <utility>
#include <memory>


namespace steam {
namespace engine {
  
//  using lib::Literal;
  using std::unique_ptr;
//  using std::forward;
  
  class EngineCtx::Facilities
    : util::NonCopyable
    {
      unique_ptr<BufferProvider> memProvider_;
      unique_ptr<BufferProvider> cacheProvider_;
      
    public:
      Facilities();
      
      BufferProvider&
      getMemProvider()
        {
          REQUIRE (memProvider_);
          return *memProvider_;
        }
      
      BufferProvider&
      getCacheProvider()
        {
          return cacheProvider_? *cacheProvider_
                               : *memProvider_;
        }
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_ENGINE_CTX_FACILITIES_H*/
