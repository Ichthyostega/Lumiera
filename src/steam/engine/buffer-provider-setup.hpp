/*
  BUFFER-PROVIDER-SETUP.hpp  -  configuration of the actual BufferProvider implementation

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-provider-setup.hpp
 ** Helper template for installing a suitable BufferProvider setup.
 ** 
 ** @see engine-facilities.cpp
 ** @see buffer-provider.hpp
 */

#ifndef STEAM_ENGINE_BUFFR_PROVIDER_SETUP_H
#define STEAM_ENGINE_BUFFR_PROVIDER_SETUP_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffhandle.hpp"
//#include "steam/engine/engine-ctx.hpp"
#include "steam/engine/type-handler.hpp"
#include "steam/engine/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <memory>


namespace steam {
namespace engine {
  
  using lib::Literal;
  using std::unique_ptr;
  using std::forward;
  
  
  class BufferMetadata;
  
  
  LUMIERA_ERROR_DECLARE (BUFFER_MANAGEMENT); ///< Problem providing working buffers
  
  
  /**
   * Framework for configuring the actual BufferPovider backend implementation.
   * 
   * @todo WIP-WIP-WIP as of 2/2026 -- reworking BufferProvider implementation   ////////////////////////////TICKET #1410
   */
  class BufferProviderSetup
    : public BufferProvider
    {
    public:
      BufferProviderSetup (Literal implementationID);

      class Stage
        : protected BufferStage
        {
          
        };

      class Store
        : protected BufferStore
        {
          
        };

      
    protected:
    };
  
  
  
  
  /* === Implementation === */
  
  /** */
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFR_PROVIDER_SETUP_H*/
