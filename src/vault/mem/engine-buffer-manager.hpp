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


#include "vault/mem/buffhandle.hpp"
#include "lib/live-mark.hpp"
#include "lib/nocopy.hpp"
#include "lib/sync.hpp"

#include <boost/lockfree/queue.hpp>


namespace vault{
namespace mem {
  namespace err = lumiera::error;
  
  
  
  namespace { // config and internal helpers...
    
    const size_t INQUEUE_SIZ = 30;   ///< initial size of the lock-free provision queue
    
  }
  
  using lib::Sync;
  using lib::RecursiveLock_NoWait;
  
  struct Alloc
    {
      Buff* mem;
      size_t siz;
      
      bool
      empty()  const
        {
          return not bool(mem);
        }
      
      void
      discard()
        {
          mem = nullptr;
          siz = 0;
        }
    };
  
  
  /**
   * Base building block: a memory handler
   * that can receive memory allocations for further use.
   */
  class AllocReceiver
    : util::NonCopyable
    {
    protected:
      lib::LiveMark mark_{*this};
      
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
      supply (Alloc& alloc)
        {
          inQueue_.push (alloc);
          alloc.discard();
        }
      
      /** allow safe access through a smart-handle,
       *  to prevent interaction with an deceased partner */
      auto
      getSafeHandle()
        {
          return mark_.makeHandle (*this);
        }
      
      /** accessor to allow verification of liveness state */
      lib::LiveMark const&
      getLiveMark() const
        {
          return mark_;
        }
    };
  
  
  
  
  /*********************************************************//**
   * Core service of the Render Engine : global buffer manager.
   */
  class EngineBufferManager
    : public AllocReceiver
    , public Sync<RecursiveLock_NoWait>
    {
      struct AllocRequest
        {
          using Handle = lib::LiveMark::SafeHandle<AllocReceiver>;
          Handle receiver;
          size_t sizRequest;
        };
      
      using RequestQueue = boost::lockfree::queue<AllocRequest>;
      RequestQueue requestQueue_;
      
      /// diagnostic bookkeeping
      size_t allocated_{0};
      size_t leased_{0};
      size_t cnt_{0};
      
    public:
      EngineBufferManager()
        : AllocReceiver{}
        , requestQueue_{INQUEUE_SIZ}
        { }
      
      
      size_t size()  const;
      bool  empty()  const;
      
      
      /****************************************************************//**
       * Core function: satisfy a request for allocation
       * @param sizRequest minimum required buffer size to allocate, in bytes.
       * @return an \ref Alloc struct representing the desired allocation,
       *         can be empty to signal that the request can not be satisfied.
       * @warning whoever accepts a non-empty result from this call
       *         is **responsible** to [return](\ref AllocReceiver::supply)
       *         this allocation eventually, to prevent memory leakage.
       */
      Alloc requestAllocation (size_t sizRequest);
      
      /**
       * Standard access point: request an allocation
       * that will be dispatched asynchronously to the given receiver.
       * @remark without any temporal guarantees, this request will be
       *         processed eventually, placing an \ref Alloc into the
       *         [receiver's inqueue](\ref AllocReceiver::supply)
       * @warning when the returned Alloc is [non-empty](\ref Alloc::empty),
       *         the receiver is responsible for returning it after use.
       */
      void async_requestAllocation (AllocReceiver&, size_t sizRequest);
      
      /**
       * Handle all enqueued asynchronous requests
       * and perform pending management and allocation work.
       * @remark this function can be invoked as a batch job;
       *         it acquires a lock and then handles the backlog. 
       */
      void processPendingRequests();
      
    private:
      /** performs the actual allocation work */
      Alloc doPerformAllocation (size_t sizRequest);
      void  doRedeemAllocation  (Alloc);
  
      /** a »backdoor« for unit testing */
      friend class BufferManagerDiagnostic;
    };
  
  
  inline void
  EngineBufferManager::async_requestAllocation  (AllocReceiver& receiver, size_t sizRequest)
  {
    REQUIRE (sizRequest);
    requestQueue_.push ({receiver.getSafeHandle(), sizRequest});
  }
  
  
  
  /** wrapper to inspect internals from a unit test */
  class BufferManagerDiagnostic
    : util::MoveOnly
    {
      EngineBufferManager const& globalPool_;
      
    public:
      BufferManagerDiagnostic (EngineBufferManager const& manager)
        : globalPool_{manager}
        { }
      
      bool
      isEmpty()
        {
          return globalPool_.empty();
        }
      
      size_t
      size()
        {
          return globalPool_.size();
        }
      
      size_t
      bytesAllocd()
        {
          return globalPool_.allocated_;
        }
      
      size_t
      bytesLeased()
        {
          return globalPool_.leased_;
        }
      
      size_t
      numAllocs()
        {
          return globalPool_.cnt_;
        }
      
        /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1430 : add pool diagnostics here if we actually switch to pooled allocations
    };
  
  
  /** entrance point to inspection for test */
  inline BufferManagerDiagnostic
  watch (EngineBufferManager const& manager)
  {
    return BufferManagerDiagnostic{manager};
  }
  
}} // namespace vault::mem
#endif /*VAULT_MEM_ENGINE_BUFFR_MANAGA_H*/
