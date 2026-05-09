/*
  ENGINE-BUFFER-MANAGER.hpp  -  central buffer allocation coordinator

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-buffer-manager.hpp
 ** Central hub of buffer management within the Lumiera Render Engine.
 ** Instantiated as global service and part of the Engine, this global
 ** allocation coordinator is responsible to provide pre-allocated blocks
 ** for render buffers. These allocations are categorised by their size,
 ** and will be sent down to LocalMemPool instances running in each
 ** worker thread. This involves asynchronous messaging to avoid
 ** any blocking of work jobs due to central resource management.
 ** 
 ** Besides the interface of the EngineBufferManager, this header also
 ** defines some common structures, like the \ref mem::Alloc, and the
 ** baseclass \ref AllocReceiver, with a lock-free inbound queue
 ** so that allocations can be supplied to a local memory manager
 ** and picked up from within the worker thread later.
 ** @remark both the LocalMemPool (for each worker thread) and the
 **   EngineBufferManager, as global memory service, are based on
 **   such an input queue, allowing to transfer control over some
 **   allocation from the central allocator to a local memory pool.
 ** 
 ** @see EnginePufferManager_test
 ** @see LocalBufferStore_test
 ** @see BufferProviderProtocol_test
 ** @see buffer-provider.hpp
 */

#ifndef VAULT_MEM_ENGINE_BUFFR_MANAGA_H
#define VAULT_MEM_ENGINE_BUFFR_MANAGA_H


//#include "lib/hash-value.h"
#include "vault/mem/buffhandle.hpp"
//#include "vault/mem/type-handler.hpp"
//#include "vault/mem/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

#include <boost/lockfree/queue.hpp>


namespace vault{
namespace mem {
  
//  using lib::HashVal;
//  using util::unConst;
  
  namespace err = lumiera::error;
  
  
  
  namespace { // config and internal helpers...
    
    const size_t INQUEUE_SIZ = 30;   ///< initial size of the lock-free provision queue
    
  }
  
  struct Alloc
    {
      Buff* mem;
      size_t siz;
    };
  
  
  /**
   * Base building block: a memory handler
   * that can receive memory allocations for further use.
   */
  class AllocReceiver
    : util::NonCopyable
    {
    protected:
      using InQueue = boost::lockfree::queue<Alloc>;
      InQueue inQueue_;
      
      AllocReceiver()
        : inQueue_{INQUEUE_SIZ}
        { }
      
    public:
      /** Transfer control over the given allocation.
       * @remark the \ref Alloc is placed into a lock-free queue;
       *         implementation code from a subclass will have to
       *         receive and dispatch those entries */
      void
      supply (Alloc const& alloc)
        {
          inQueue_.push (alloc);
        }
    };
  
  
  
  
  /*********************************************************//**
   * Core service of the Render Engine : global buffer manager.
   */
  class EngineBufferManager
    : public AllocReceiver
    {
      struct AllocRequest
        {
          AllocReceiver* receiver;
          size_t sizRequest;
        };
      
      using RequestQueue = boost::lockfree::queue<AllocRequest>;
      RequestQueue requestQueue_;
      
    public:
      EngineBufferManager()
        : AllocReceiver{}
        , requestQueue_{INQUEUE_SIZ}
        { }
        
    private:
      /** process all pending requests in the queue */
      void handleAllocRequests();
    };
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_ENGINE_BUFFR_MANAGA_H*/
