/*
  EnginePufferManager  -  central buffer allocation coordinator

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file engine-buffer-manager.cpp
 ** Implementation of render buffer allocation for the Lumiera Engine.
 ** Instantiated as global service and part of the Engine, this global
 ** buffer coordinator uses the [actual allocator](\ref engine-buffer-allocator.hpp)
 ** internally to maintain a pool of usage buffer memory blocks. Furthermore,
 ** both a synchronous and an asynchronous interface is provided to request
 ** allocations, that will be sent as \ref Alloc entries to some local memory
 ** pool, that is an \ref AllocReceiver, thereby passing control over these
 ** blocks of memory. The contract is that the local memory user has to
 ** return those blocks reliably to the EngineBuffrerManager, which
 ** itself is also an \ref AllocReceiver.
 ** @todo 5/2026 the plan is to use pooling allocators for the render buffers  //////////////////////////////TICKET #1430
 ** 
 ** @see EnginePufferManager_test
 ** @see LocalBufferStore_test
 */


#include "lib/error.hpp"
#include "lib/util-quant.hpp"
#include "lib/allocator-handle.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/engine-buffer-manager.hpp"
#include "vault/mem/engine-buffer-allocator.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <array>


namespace vault{
namespace mem {
  namespace err = lumiera::error;
  
  using std::byte;
  
  
  using BufferAllocator = HeapBufferAllocator;
  
  
  
  
  /* === EngineBufferManager implementation === */
  
  /**
   * @note 5/2026 I prefer a small number of locks at API level,
   *       based on the assumption that this processing environment
   *       does not exhibit much contention. This should be [validated] however.
   * [validated]: https://issues.lumiera.org/ticket/1429#comment:1
   */
  void
  EngineBufferManager::processPendingRequests()
  {
    Lock sync{this};
    inQueue_     .consume_all ([this](Alloc alloc){ doRedeemAllocation (alloc); });
    requestQueue_.consume_all ([this](AllocRequest request)
                                {
                                  if (not request.receiver) return;
                                  Alloc alloc = doPerformAllocation (request.sizRequest);
                                  ENSURE (not alloc.empty());
                                  if (request.receiver)
                                    request.receiver->supply (alloc);
                                });
  }
  
  
  Alloc
  EngineBufferManager::requestAllocation (size_t sizRequest)
  {
    Lock sync{this};
    processPendingRequests();
    return doPerformAllocation (sizRequest);
  }
  
  
  /**
   * @todo 5/2026 this is a preliminary placeholder implementation,
   *       since any requests will just be passed-through to the heap allocator.
   *       The plan is to use a system of buffer pools eventually...  ///////////////////////////////////////TICKET #1430
   */
  Alloc
  EngineBufferManager::doPerformAllocation (size_t sizRequest)
  {
    BufferAllocator allocator;
    Alloc alloc = allocator.allocate (sizRequest);
    if (not alloc.empty())
      {
        allocated_ += alloc.siz;
        leased_    += alloc.siz;
        ++cnt_;
      }
    return alloc;
  }
  
  
  void
  EngineBufferManager::doRedeemAllocation  (Alloc alloc)
  {
    REQUIRE (not alloc.empty());
    ENSURE (allocated_ >= alloc.siz);
    ENSURE (leased_    >= alloc.siz);
    ENSURE (cnt_       >= 1        );
    
    BufferAllocator allocator;
    allocated_ -= alloc.siz;
    leased_    -= alloc.siz;
    --cnt_;
    allocator.deallocate (alloc);
  }
  
  
  /** @remark this API becomes relevant if we actually use a pool */
  size_t
  EngineBufferManager::size()  const
  {
    return cnt_;
  }
  
  bool
  EngineBufferManager::empty()  const
  {
    return 0 == cnt_;
  }
  
  
}} // namespace vault::mem
