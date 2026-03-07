/*
  SIMPLE-BUFFER-STATE-REGISTRY.hpp  -  naively tracking buffer states in a central hash table

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file simple-buffer-state-registry.hpp
 ** Demo implementation of engine::BufferProvider state coordination, suitable for unit tests.
 ** Based on a shared metadata table, accessed directly, disregarding concurrency.
 ** 
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see naive-buffer-setup.hpp
 */

#ifndef VAULT_MEM_SIMPLE_BUFFER_STATE_REGISTRY_H
#define VAULT_MEM_SIMPLE_BUFFER_STATE_REGISTRY_H


#include "vault/mem/buffer-metadata.hpp"
#include "vault/mem/buffer-provider-setup.hpp"

#include <memory>


namespace vault {
namespace mem   {
  
  
  
  /**
   * Simple Buffer type and state tracking registry, for test and demonstration.
   * Relies on a central hashtable, without considering any concurrency concerns.
   */
  class SimpleBufferStateRegistry
    : public BufferProviderSetup::Stage
    {
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
          return lookup (metadata_.key (buffSiz, move (handlerFunctions), localTag));
        }     // deliberately: create storage, and return reference to it
      
      ID
      mark_locked (ID typeKey, Buff* storage, LocalTag implMark)  override
        {
          return metadata_.markLocked (typeKey, storage, implMark);
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
      SimpleBufferStateRegistry (Literal implementationID)
        : metadata_{implementationID}
        { }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_SIMPLE_BUFFER_STATE_REGISTRY_H*/
