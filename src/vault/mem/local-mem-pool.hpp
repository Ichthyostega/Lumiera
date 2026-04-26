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
 ** Since the Lumiera Render Engine relies on scheduled jobs running concurrently,
 ** it is essential to avoid any kind of locking that might prevent the calculations
 ** from moving past each other. One crucial functionality that might lead to such
 ** a subtle cross-wise obstruction is the handling of buffer memory allocations.
 ** Each worker thread in the engine thus needs a (small) thread-local pool of
 ** buffer allocations to handle all foreseeable allocation requests. Whenever
 ** a render job starts, the information regarding a _maximal allocation footprint_
 ** can be retrieved from the ProcNode, since this information is determined solely
 ** by the expected call topology. Based on this information, additional memory
 ** can be requested from a global buffer memory allocator; and while any new
 ** allocation will take some time, the alloted buffer entries can be sent down
 ** to the worker's local allocation pool through a lock-free queue, so that —
 ** hopefully — these entries have already arrived at the point when the memory
 ** is actually required from within the recursive Render Node `pull()` call.
 ** 
 ** LocalMemPool is responsible for receiving those memory provisions and for
 ** managing and assigning available buffer blocks, drawing from the pool of
 ** locally known allocations. For every new allocation request, an attempt
 ** is made to find a best match within the available allotments of memory.
 ** In the exceptional case however when no suitable prepared allocation
 ** can be found locally, new memory must be acquired right away, since
 ** render jobs are assumed to run through without blocking waits.
 ** 
 ** To avoid the hazard of the pool accumulating an increasing number of surplus
 ** allocations over time, less frequently used blocks are culled using a heuristic.  
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
    
    const size_t INQUEUE_SIZ = 30;   ///< initial size of the lock-free provision queue
    
    const uint MATCH_SCORE    = 10;  ///< score to add when a buffer can be used to satisfy a request 
    const uint UNUSABLE_MALUS = 2;   ///< reduce score whenever a buffer is too small to be useful
    const double USAGE_WEIGHT = 0.9; ///< degree to which very frequent usage counteracts waste of memory
    const double CLOSE_MATCH  = 0.2; ///< fraction of wasted memory that still counts as /good match/
    
  }//(End) config parameters and internals
  
  using std::move;
  using std::forward;
  using util::unConst;
  using util::isLimited;
  
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
      
      int32_t maxScore_{1};
      
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
       *   that are available immediately and have been reserved now.
       * @note only allocations larger yet comparatively close to the desired
       *   size are considered; instead of reserving an over-allocation, it seems
       *   preferable to request a fitting new one asynchronously from the global
       *   memory pool, given that there is still some time left until it's actually
       *   needed. However, irrespective of any reservation, an immediate request to
       *   \ref retrieve() an allocation will use any locally available memory.
       *   There is no _handle_ to refer to some specific reservation.
       */
      uint
      reserve (uint cnt, size_t sizRequest)
        {
          ingest();
          uint reserved{0};
          for (Block& block : blocks_)
            if (reserved >= cnt)
              break;
            else
            if (not (block.used or block.resd)
                and isLimited(0.0, 1 - double(sizRequest)/block.alloc.siz, CLOSE_MATCH))
              { // good match, reserve this allocation
                block.resd = true;
                ++reserved;
              }
          /*  Note we did not consider to use much larger blocks
           *  even while they might be used to satisfy the request.
           *  In theory, we could even sort matches by amount of waste,
           *  but it's doubtful this would be better than requesting
           *  a really fitting new allocation from the global manager.
           */
          return reserved;
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
      uint
      yield (uint cnt, size_t siz, FUN&& consumer)
        {
          ingest();
          uint removed{0};
          for (auto pos = blocks_.begin()
              ; pos != blocks_.end()
              ; )
            if (cnt == 0)
              break;
            else
            if (not pos->used
                and siz == pos->alloc.siz)
              {
                consumer (pos->alloc.mem, pos->alloc.siz);
                pos = blocks_.erase (pos);
                ++removed;
                --cnt;
              }
            else
              ++pos;
            
          return removed;
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
      uint
      cleanup (double degree, FUN&& consumer)
        {
          ingest();
          uint removed{0};
          int32_t killLevel = degree * maxScore_;
          for (auto pos = blocks_.begin()
              ; pos != blocks_.end()
              ; )
            if (not (pos->used or pos->resd)
                and pos->score <= killLevel)
              { // send this allocation away and remove it from the pool
                consumer (pos->alloc.mem, pos->alloc.siz);
                pos = blocks_.erase (pos);
                ++removed;
              }
            else
              {
                pos->score -= killLevel;
                ++pos;
              }
           // recalibrate all score levels
          maxScore_ -= killLevel;
          return removed;
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
          double bestSelector{0.0};
          for (Block& block : blocks_)
            if (not block.used)
              {
                if (sizRequest > block.alloc.siz)
                  block.score -= UNUSABLE_MALUS;
                else
                  {
                    double wasted = block.alloc.siz - sizRequest;
                    double wasteScore = wasted / block.alloc.siz;
                    double usageFact = block.score / maxScore_; 
                    double selector = 1.0 - wasteScore * (1.0 - usageFact * USAGE_WEIGHT);
                    if (selector > bestSelector)
                      {
                        bestSelector = selector;
                        match = &block;
                      }
                  }
              }
          if (match)
            {
              ENSURE (match->alloc.siz >= sizRequest);
              match->score += int(MATCH_SCORE * (1 - (1 - double(sizRequest) / match->alloc.siz)));
              if (match->score > maxScore_)       // reduce score by waste factor as malus
                maxScore_ = match->score;
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
      size()
        {
          return memPool_.size();
        }

      size_t
      cnt (size_t siz)
        {
          return std::ranges::count_if (memPool_.blocks_
                                       ,[siz](Block const& b){ return siz == b.alloc.siz; });
        }
      
      size_t
      cntFree()
        {
          return std::ranges::count_if (memPool_.blocks_
                                       ,[](Block const& b){ return not b.used; });
        }
    };
  
  
  inline PoolDiagnostic
  watch (LocalMemPool& lmp)
  {
    return PoolDiagnostic{lmp};
  }
  
}} // namespace vault::mem
#endif /*VAULT_MEM_LOCAL_MEM_POOL_H*/
