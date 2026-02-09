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
   * Framework for configuring the actual BufferPovider backeind implementation.
   * 
   * @todo WIP-WIP-WIP as of 2/2026 -- reworking BufferProvider implementation   ////////////////////////////TICKET #1410
   */
  class BufferProviderSetup
    : util::NonCopyable
    {
    public:
      BufferProviderSetup (Literal implementationID);
      
      
      uint announce (uint count, BuffDescr const&);
      
      BuffHandle lockBuffer (BuffDescr  const&);
      void       emitBuffer (BuffHandle const&);
      void    releaseBuffer (BuffHandle const&);
      
      template<typename BU, typename...ARGS>
      BuffHandle lockBufferFor (ARGS ...args);
      
      /** allow for attaching and owing an object within an already created buffer */
      void attachTypeHandler (BuffHandle const& target, BuffDescr const& reference);
      
      void emergencyCleanup (BuffHandle const& target, bool invokeDtor =false);
      
      
      /** describe the kind of buffer managed by this provider */
      BuffDescr getDescriptorFor(size_t storageSize=0);
      BuffDescr getDescriptorFor(size_t storageSize, TypeHandler specialTreatment);
      
      template<typename BU, typename...ARGS>
      BuffDescr getDescriptor (ARGS ...args);
      
      
      
      /* === API for BuffHandle internal access === */
      
      bool verifyValidity (BuffDescr const&)  const;
      size_t getBufferSize (HashVal typeID)   const;
      
    protected:
      BuffHandle buildHandle (HashVal typeID, Buff* storage, LocalTag const& =LocalTag::UNKNOWN);
      
      bool was_created_by_this_provider (BuffDescr const&)  const;
    };
  
  
  
  
  /* === Implementation === */
  
  /** */
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_BUFFR_PROVIDER_SETUP_H*/
