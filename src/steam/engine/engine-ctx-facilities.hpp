/*
  ENGINE-CTX-FACILITIES.hpp  -  Implementation of global render engine operational services

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file engine-ctx-facilities.hpp
 ** Implementation of render engine operational configuration and services.
 ** Without further setup, [dependency injection](\ref depend.hpp) will establish a
 ** minimalistic default instantiation of these services, sufficient for demonstration
 ** and tests. For the real render engine however, dedicated service instances will be
 ** created and managed actively.
 ** @todo and we'll have yet to »figure out« what specifically to do for these
 **       services and how to start an active render engine for productive use.
 ** @todo WIP-WIP 10/2024 strive at first complete prototypical engine usage   ////////////////////////////////////////TICKET #1367
 ** 
 ** @see engine-ctx.hpp
 ** @see weaving-pattern-builder.hpp
 */

#ifndef STEAM_ENGINE_ENGINE_CTX_FACILITIES_H
#define STEAM_ENGINE_ENGINE_CTX_FACILITIES_H


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
