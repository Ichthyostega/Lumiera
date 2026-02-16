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
#include "steam/engine/buffer-provider.hpp"
//#include "steam/engine/engine-ctx.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <memory>


namespace steam {
namespace engine {
  
  using lib::Literal;
  using std::unique_ptr;
  using std::make_unique;
//  using std::forward;
  using std::move;
  
  
  
  
  
  /**
   * Framework for configuring the actual BufferPovider backend implementation.
   * 
   * @todo WIP-WIP-WIP as of 2/2026 -- reworking BufferProvider implementation   ////////////////////////////TICKET #1410
   */
  class BufferProviderSetup
    : public BufferProvider
    {
    public:
      /**
       * Build a new BufferProvider setup to manage a number of buffers.
       * The metadata of these buffers is organised hierarchically based on
       * chained hash values, using an `implementationID` as a seed.
       * The \a CONF configuration record controls the actual implementation
       * of lifecycle transitions and storage management.
       */
      template<class CONF>
      BufferProviderSetup (CONF&& confRec)
        : BufferProvider{}
        {
          bufferStage_ = move (confRec.buildStage());
          bufferStore_ = move (confRec.buildStore());
        }
      
      
      /* ======== Extension points ======== */
      
      class Stage
        : public BufferStage
        { };
      
      class Store
        : public BufferStore
        { };

      
    protected:
    };
  
  
  
  
  /* === Implementation === */
  
  /** */
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFR_PROVIDER_SETUP_H*/
