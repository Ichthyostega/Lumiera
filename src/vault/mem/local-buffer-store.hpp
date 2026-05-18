/*
  LOCAL-BUFFER-STORE.hpp  -  buffer memory handling with thread-local sub-services

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-buffer-store.hpp
 ** Production-grade implementation of engine::BufferProvider memory handling.
 **
 ** @see LocalBufferStore_test 
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see simple-buffer-state-registry.hpp
 */

#ifndef VAULT_MEM_LOCAL_BUFFER_STORE_H
#define VAULT_MEM_LOCAL_BUFFER_STORE_H


#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/local-mem-pool.hpp"
#include "lib/local-slice.hpp"
#include "lib/depend.hpp"


namespace vault {
namespace mem   {
  
  
  
  /**
   * @todo type comment
   */
  class LocalBufferStore
    : public BufferProviderSetup::Store
    {
      struct SetupPool;
      using LocalPool = lib::LocalSlice<LocalMemPool, SetupPool>;
      using GlobalPool = lib::Depend<EngineBufferManager>;
      
      struct SetupPool
        {
          using Service = LocalMemPool;
          
          static LocalMemPool
          init()
            {
              return LocalMemPool{[](LocalMemPool& pool2close)
                                    {
                                      pool2close.purge([](Buff* mem, size_t siz)
                                                        {
                                                          Alloc alloc{mem,siz};
                                                          GlobalPool globalPool;
                                                          globalPool().supply(alloc);
                                                        });
                                    }};
            }
        };
      
      LocalPool localReg_;
      GlobalPool globalReg_;
      
    public:
      LocalBufferStore()
        { }
      
      
      /* ==== Implementation of the BufferStore interface ==== */
      
      uint
      prepareBuffers (HashVal, uint numBuffers, size_t)  override
      {
        UNIMPLEMENTED ("announce");
      }
    
      
      /** API: create a new buffer allocation */
      BuffAlloc
      provideBuffer (HashVal, size_t buffSiz, LocalTag specifics, int64_t)  override
      {
        UNIMPLEMENTED ("retrieve alloc");
      }
      
      
      /** API: mark a buffer as _emitted_ */
      void
      mark_emitted (HashVal, BuffAlloc storageSlot)  override
      {
        UNIMPLEMENTED ("emit");
      }
      
      
      /** API: mark a buffer as officially discarded */
      void
      detachBuffer (HashVal, BuffAlloc storageSlot)  override
      {
        UNIMPLEMENTED ("reclaim alloc");
      }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_BUFFER_STORE_H*/
