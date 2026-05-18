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
 ** @see local-buffer-stage.hpp
 */

#ifndef VAULT_MEM_LOCAL_BUFFER_STORE_H
#define VAULT_MEM_LOCAL_BUFFER_STORE_H


#include "vault/mem/buffer-provider-setup.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/local-mem-pool.hpp"
#include "lib/local-slice.hpp"
#include "lib/depend.hpp"
#include "lib/format-string.hpp"


namespace vault {
namespace mem   {
  
  using util::_Fmt;
  
  
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
      
      /** API: announce demand for some buffer allocations.
       * @return number of allocations that were immediately reserved, locally.
       * @remark This operation is crucial for the interplay between local and global buffer pool.
       *         In case the local pool can not (fully) handle the demand, an asynchronous request
       *         is sent to the global pool, that will be processed there in bulk, on next occasion.
       */
      uint
      prepareBuffers (HashVal, uint numBuffers, size_t sizRequest)  override
      {
        uint cnt = localReg_->reserve(numBuffers, sizRequest);
        for (uint n{cnt}; n < numBuffers; ++n)
          globalReg_().async_requestAllocation (*localReg_, sizRequest);
        // signal back the number of buffers reserved and thus immediately available
        return cnt;
      }
      
      
      /** API: create a new buffer allocation.
       * @return a memory allocation ready to use
       * @throws err::State in case this allocation can not be satisfied.
       * @remark This is the central access point to get buffer memory.
       *         Due to the asynchronous nature of allocation requests, it might be necessary to
       *         process pending global allocations immediately in this thread, thereby acquiring
       *         a global lock on the EngineBufferManager. Since there is no guarantee that an
       *         allocation has actually be announced, we need to perform a direct call to the
       *         central hub, in a second attempt to get the required allocation.
       * @note this is the production-grade implementation, and thus no additional verifications
       *         are performed, other than the lookup of the »buffer type« and the lifecycle state,
       *         that is recorded in the BufferMetadata.
       */
      BuffAlloc
      provideBuffer (HashVal, size_t sizRequest, LocalTag specifics, int64_t)  override
      {
        if (not localReg_->canServe(sizRequest))
          globalReg_().processPendingRequests();
        
        // attempt to satisfy allocation request from the local pool...
        Alloc alloc = localReg_->retrieve (sizRequest);
        auto& [storage,buffSiz] = alloc;
        
        // possibly the request was not announced beforehand...?
        if (not storage)
          alloc = globalReg_().requestAllocation (sizRequest);
        if (not storage)
          throw err::State{_Fmt{"Unable to provide further %d bytes of buffer memory."} % sizRequest
                          ,LUMIERA_ERROR_BUFFER_MANAGEMENT};
        
        return std::make_tuple (storage, buffSiz, specifics);
      }
      
      
      /** API: mark a buffer as _emitted_.
       * @note the production-grade implementation does not need to do anything,
       *       since the buffer state transition was already recorded in the buffer metadata.
       */
      void
      mark_emitted (HashVal, BuffAlloc)  override
      {
        /* nothing to do for a regular buffer allocation -- emit state captured in metadata */
      }
      
      
      /** API: mark a buffer as officially discarded.
       * @remark usually the buffer is only added back into the thread-local pool,
       *         yet some heuristic clean-up of unfrequently-used buffer allocations
       *         might be triggered an that point.
       */
      void
      detachBuffer (HashVal, BuffAlloc storageSlot)  override
      {
        auto& [storage,buffSiz,specifics] = storageSlot;
        REQUIRE (storage);
        localReg_->reSupply (storage);
        /////////////////////////////////////////////////////OOO trigger heuristic clean-up here
      }
    };
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_BUFFER_STORE_H*/
