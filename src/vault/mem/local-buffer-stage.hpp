/*
  SIMPLE-BUFFER-STATE-REGISTRY.hpp  -  buffer state management with thread-local sub-services

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-buffer-stage.hpp
 ** Production-grade implementation of engine::BufferProvider state coordination.
 ** Requests to the BufferProvider will be issued from all threads concurrently,
 ** which requires to distinguish between known common state and particular state
 ** relevant only for the current worker. Whenever a _buffer type_ is registered,
 ** this information could become relevant later, when actually a buffer is locked
 ** with this type, and such a request for an actual buffer can be encountered
 ** in any thread. While, on the other hand, the state transitions for actual
 ** buffer usage will happen within a single job invocation, and can thus be
 ** handled locally within the corresponding worker thread. An allocated buffer
 ** that is passed into the frame cache is yet another story, and will be handled
 ** by a dedicated _cache provider_.
 ** 
 ** Together these observations lead to a structure with locally replicated copies
 ** of the buffer metadata store, where additional type registration information
 ** is requested from the central metadata hub on-demand. Yet the front-end for
 ** accessing the \ref BufferProvider is a common service of the Render Engine,
 ** and thus the switch to the thread-local metadata management has to happen
 ** somewhere in-between. This can be achieved by connecting the BufferMetadata
 ** as a \ref lib::LocalSlice as part of the \ref BufferProvider::BufferStage
 ** implementation — as provided by this class, LocalBufferStage.
 ** 
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see simple-buffer-state-registry.hpp
 */

#ifndef VAULT_MEM_LOCAL_BUFFER_STAGE_H
#define VAULT_MEM_LOCAL_BUFFER_STAGE_H


#include "vault/mem/buffer-metadata.hpp"
#include "vault/mem/buffer-provider-setup.hpp"

#include <memory>


namespace vault {
namespace mem   {
  
  
  
  /**
   * Simple Buffer type and state tracking registry, for test and demonstration.
   * Relies on a central hashtable, without considering any concurrency concerns.
   */
  class LocalBufferStage
    : public BufferProviderSetup::Stage
    {
      HashVal familyID_;
      BufferMetadata metadata_;

      /* === BufferStage interface === */

      ID
      lookup (HashVal key)  override
        {
          return metadata_.isKnown(key)? metadata_.get (key)
                                       : metadata::Key::INVALID;
        }
      
      bool
      isAllotted (HashVal stateKey)  const override
        {
          return metadata_.isLocked (stateKey);
        }
      
      bool
      isAccessible (HashVal stateKey)  const override
        {
          return metadata_.isAccessible (stateKey);
        }
      
      ID
      defineBufferType (size_t buffSiz, TypeHandler handlerFunctions, LocalTag localTag)
        {
          return lookup (metadata_.key (familyID_, buffSiz, move (handlerFunctions), localTag));
        }     // deliberately: maybe create storage, and return reference to it
      
      ID
      mark_locked (ID typeKey, BuffAlloc alloc)  override
        {
          auto& [storage,size,implMark] = alloc;
          return metadata_.markLocked (typeKey, storage, size, implMark);
        }
      
      ID
      mark_emitted (HashVal stateKey)  override
        {
          metadata::Entry& metaEntry = metadata_.get (stateKey);
          metaEntry.mark(EMITTED);
          return metaEntry;   // contains also the key
        }
      
      ID
      mark_released (HashVal stateKey)  override
        {
          metadata::Entry& metaEntry = metadata_.get (stateKey);
          metaEntry.mark(FREE);   // might invoke embedded dtor function
          return metaEntry;
        }
      
      ID
      abandon (HashVal stateKey, bool invokeDtor)  override
        {
          metadata::Entry& metaEntry = metadata_.get (stateKey);
          metaEntry.invalidate (invokeDtor);
          return metaEntry;
        }
      
      void
      discard (HashVal stateKey)  override
        {
          metadata_.release (stateKey);
        }
      
      
    public:
      LocalBufferStage (Literal implementationID)
        : familyID_{hash_value (implementationID)}
        , metadata_{}
        { }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_BUFFER_STAGE_H*/
