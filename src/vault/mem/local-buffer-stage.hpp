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


#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-metadata.hpp"
#include "vault/mem/buffer-metadata.hpp"
#include "lib/local-slice.hpp"
#include "lib/depend.hpp"


namespace vault {
namespace mem   {
  
  
  
  /**
   * Buffer type and state tracking registry, specialised for running
   * in a worker thread and within a massively concurrent environment.
   * Requests are first and foremost handled by a thread-local registry.
   * Metadata is synchronised with a global registry only when types are
   * defined anew in the worker, or when a worker refers for the first
   * time to an type predefined from the Builder. In those (rare) cases
   * however, a mutex synchronisation is necessary to protect the global
   * metadata table against corruption and ensure synchronous response.
   */
  class LocalBufferStage
    : public BufferProviderSetup::Stage
    {
      using LocalRegistry = lib::LocalSlice<BufferMetadata>;
      using EngineRegistry = lib::Depend<EngineBufferMetadata>;
      
      HashVal familyID_;
      LocalRegistry localReg_;
      EngineRegistry globalReg_;
      
      
      /* === BufferStage interface === */
      
      ID
      lookup (HashVal key)  override
        {
          if (not localReg_->isKnown(key))
            globalReg_().propagateDown(key, *localReg_);
          // try it again after sync with central registry
          return localReg_->isKnown(key)? localReg_->get (key)
                                        : metadata::Key::INVALID;
        }
      
      bool
      isAllotted (HashVal stateKey)  const override
        {
          return localReg_->isLocked (stateKey);
        }
      
      bool
      isAccessible (HashVal stateKey)  const override
        {
          return localReg_->isAccessible (stateKey);
        }
      
      ID
      defineBufferType (size_t buffSiz, TypeHandler handlerFunctions, LocalTag localTag)
        {
          ID typeKey = lookup (localReg_->key (familyID_, buffSiz, move (handlerFunctions), localTag));
                           //  possibly create new entry, and retrieve (stable) reference
          globalReg_().propagateUp (typeKey, *localReg_);
          return typeKey;
        }
      
      ID
      mark_locked (ID typeKey, BuffAlloc alloc)  override
        {
          auto& [storage,size,implMark] = alloc;
          return localReg_->markLocked (typeKey, storage, size, implMark);
        }
      
      ID
      mark_emitted (HashVal stateKey)  override
        {
          metadata::Entry& metaEntry = localReg_->get (stateKey);
          metaEntry.mark(EMITTED);
          return metaEntry;   // contains also the key
        }
      
      ID
      mark_released (HashVal stateKey)  override
        {
          metadata::Entry& metaEntry = localReg_->get (stateKey);
          metaEntry.mark(FREE);   // might invoke embedded dtor function
          return metaEntry;
        }
      
      ID
      abandon (HashVal stateKey, bool invokeDtor)  override
        {
          metadata::Entry& metaEntry = localReg_->get (stateKey);
          metaEntry.invalidate (invokeDtor);
          return metaEntry;
        }
      
      void
      discard (HashVal stateKey)  override
        {
          localReg_->release (stateKey);
        }
      
      
    public:
      LocalBufferStage (Literal implementationID)
        : familyID_{hash_value (implementationID)}
        { }
      
      HashVal getFamilyID() const override { return familyID_; }
      size_t  cntEntries()  const          { return localReg_->cntEntries(); }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_BUFFER_STAGE_H*/
