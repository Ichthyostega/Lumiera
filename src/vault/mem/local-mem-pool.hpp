/*
  LOCAL-MEM-POOL.hpp  -  working pool of buffer allocations for local reuse

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file local-mem-pool.hpp
 ** A local cache of buffer allocations to facilitate reuse of memory in a worker.
 ** 
 ** @see local-mem-pool-test.cpp
 ** @see local-buffer-store.hpp
 ** @see buffer-provider.hpp
 ** @see buffer-provider-protocol-test.cpp
 ** @see simple-buffer-state-registry.hpp
 */

#ifndef VAULT_MEM_LOCAL_MEM_POOL_H
#define VAULT_MEM_LOCAL_MEM_POOL_H


#include "lib/error.hpp"
//#include "vault/mem/buffer-provider-setup.hpp"
//#include "vault/mem/engine-buffer-metadata.hpp"
//#include "vault/mem/buffer-metadata.hpp"
#include "vault/mem/buffhandle.hpp" //////////////TODO
//#include "lib/local-slice.hpp"
//#include "lib/depend.hpp"
#include "lib/util.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/trait.hpp"//////////////TODO

#include <boost/lockfree/queue.hpp>
#include <algorithm>
//#include <tuple>
#include <utility>
#include <list>


namespace vault {
namespace mem   {
  namespace err = lumiera::error;
  
  namespace { // Configuration tuning parameters
    
    const size_t INQUEUE_SIZ = 30; ///< initial size of the lock-free provision queue
    
  }//(End) config parameters and internals
  
  using std::move;
  using std::forward;
  using util::unConst;
  
  class PoolDiagnostic;
  
  
  /**
   * Low-level Building block for Render Engine memory management.
   */
  class LocalMemPool
    {
      struct Alloc
        {
          Buff* mem;
          size_t siz;
        };
      static_assert (std::is_trivially_copy_assignable_v<Alloc>);
      
      struct Block
        {
          Alloc   alloc{nullptr,0};
          int32_t score{0};
          bool used :1 {false};
          bool resd :1 {false};
          
          Block() = default;
          Block(Alloc&& alloc)
            : alloc{move(alloc)}
            { }
        };
      static_assert (std::is_trivially_copy_assignable_v<Block>);
      
      using InQueue = boost::lockfree::queue<Alloc>;
      using BlockList = std::list<Block>;
      
      InQueue inQueue_;
      BlockList blocks_;
      
    public:
      LocalMemPool()
        : inQueue_{INQUEUE_SIZ}
        , blocks_{}
        { }
      
      
      /* ===== Pool Access API ===== */
      
      bool
      empty()  const
        {
          return blocks_.empty()
             and inQueue_.empty();
        }
      
      /**
       * @return overall number of descriptors in the pool, including used ones
       * @warning this operation has linear complexity
       */
      size_t
      size()  const
        {
          return blocks_.size();
        }
      
      
      /** Query if this pool can provide some chunk of memory right away. */
      bool
      canServe (size_t sizRequest)  const
        {
          unConst(this)->ingest();
          return bool(anyFree (sizRequest));
        }
      
      /**
       * Determine to what extent the indicated allocation request
       * could be handled, given the current memory available in the pool.
       * An allocation promised hereby will be marked as reserved internally,
       * and will thus not be considered for further `reserve()` requests.
       * @return number of allocations of the indicated size,
       *         that are available immediately and have been reserved now.
       * @note irrespective of any reservation, an immediate request to
       *         \ref retrieve() an allocation will use available memory.
       *         There is no _handle_ to refer to some specific reservation.
       */
      size_t
      reserve (uint cnt, size_t sizRequest)
        {
          ingest();
          return 0; ///////////////////////////////////////OOO
        }
      
      /**
       * Transfer discretionary power over the given allocation to this pool.
       * @note this call is thread-safe and asynchronous; the pool will pick up
       *       the given information on the next request to serve or reserve memory.
       * @remark the \a mem _must not be touched_ by any other part of the system,
       *       unless it is [yielded](\ref yield()) or disposed as part of clean-up.
       */
      void
      add (Buff* mem, size_t siz)
        {
          inQueue_.push ({mem,siz});
        }
      
      /**
       * Return an used memory block back to the pool
       * @param mem a chunk of memory previously handed out by \ref retrieve()
       * @throw err::Logic in case the indicated allocation is not known
       * @warning **not threadsafe** — invoke from worker thread only
       */
      void
      reAdd (Buff* mem)
        {
          ingest();
          Block* block = UN_CONST (anyMatch (mem));
          if (not block)
            throw err::Logic{"returning unknown allocation to LocalMemPool"};
          if (not block->used)
            throw err::Logic{"returning known allocation that is not marked as used"};
          block->used = false;
          ENSURE (not block->resd);
        }
      
      /**
       * Acquire a block of memory for active use
       * @param sizRequest minimum size of storage expected
       * @return a chunk of memory of indicated size, which may be larger
       *         than requested. The answer can be `(nullptr, 0)` in case
       *         the request can not be satisfied
       * @note the allocation is marked as used internally,
       *         and must be returned to this pool after use.
       *         Memory can only be disposed after \ref yield()
       * @warning **not threadsafe** — invoke from worker thread only
       */
      Alloc
      retrieve (size_t sizRequest)
        {
          ingest();
          Block* found = selectBestMatch (sizRequest);
          if (not found)
              return {nullptr, 0};
          else
            {
              ENSURE (not found->used);
              found->score += 1; ///////////////////TODO
              found->resd = false;
              found->used = true;
              return found->alloc;
            }
        }
      
      /**
       * Yield control over the indicated number of allocations.
       * @tparam FUN a consumer to receive the allocations removed from the pool
       * @param siz precise size of the memory block(s) to yield
       * @return actual number of allocations removed from the pool.
       */
      template<class FUN> ////////TODO requires std::invocable<FUN,Buff*,size_t>
      size_t
      yield (uint cnt, size_t siz, FUN&& consumer)
        {
          ingest();
          return 0;///////////////////////OOO
        }
      
      /**
       * Perform a heuristic clean-up of the pool, based on usage statistics.
       * @tparam FUN a consumer to receive the allocations removed from the pool
       * @param degree a measure of how aggressively the clean-up should be performed;
       *               1.0 indicates that every allocation must be released, whereas
       *               \a degree ≡ 0.5 retains all matches that were frequently successful.
       * @return actual number of allocations removed from the pool.
       */
      template<class FUN> ////////TODO requires std::invocable<FUN,Buff*,size_t>
      size_t
      cleanup (double degree, FUN&& consumer)
        {
          ingest();
          return 0;///////////////////////OOO
        }
      
    private:
      void
      ingest()
        {
          inQueue_.consume_all ([this](Alloc alloc)
                                      {
                                        blocks_.emplace_front (move(alloc));
                                      });
        }
      
      template<class PRED>
      Block const*
      findMatch (PRED&& predicate)  const
        {
          auto found = std::ranges::find_if (blocks_, forward<PRED> (predicate));
          return found!=blocks_.end()? & *found
                                     : nullptr;
        }
      
      Block const*
      anyFree (size_t sizRequest)  const
        {
          return findMatch ([sizRequest](Block const& b)
                                        {
                                          return not b.used
                                             and sizRequest <= b.alloc.siz;
                                        });
        }
      
      Block const*
      anyMatch (Buff* mem)  const
        {
          return findMatch ([mem](Block const& b){ return mem == b.alloc.mem; });
        }
      
      Block*
      selectBestMatch (size_t sizRequest)
        {
          Block* match{nullptr};
          size_t waste(-1);
          for (Block& block : blocks_)
            if (not block.used
                and sizRequest <= block.alloc.siz)
              {
                size_t wasted = block.alloc.siz - sizRequest;
                if (wasted < waste)
                  {
                    waste = wasted;
                    match = &block;
                  }
              }
          return match;
        }
      
      friend class PoolDiagnostic;
    };
  
  
  class PoolDiagnostic
    : util::MoveOnly
    {
      using Block = LocalMemPool::Block;
      
      LocalMemPool& memPool_;
    public:
      PoolDiagnostic (LocalMemPool& lmp)
        : memPool_{lmp}
        { }
      
      bool
      isEmpty()
        {
          return memPool_.empty();
        }
      
      size_t
      cnt (size_t siz)
        {
          return std::ranges::count_if (memPool_.blocks_
                                       ,[siz](Block const& b){ return siz == b.alloc.siz; });
        }
    };
  
  
  inline PoolDiagnostic
  watch (LocalMemPool& lmp)
  {
    return PoolDiagnostic{lmp};
  }
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_MEM_POOL_H*/
