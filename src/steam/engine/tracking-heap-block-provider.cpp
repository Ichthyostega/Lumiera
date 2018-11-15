/*
  TrackingHeapBlockProvider  -  plain heap allocating BufferProvider implementation for tests

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


/** @file tracking-heap-block-provider.cpp
 ** Implementation details of a mock engine::BufferProvider for unit testing
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/scoped-ptrvect.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/util-foreach.hpp"

#include "steam/engine/tracking-heap-block-provider.hpp"

#include <algorithm>
#include <vector>

using util::and_all;
using std::vector;
using lib::ScopedHolder;
using lib::ScopedPtrVect;



namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  
  namespace { // implementation helpers... 
    
    using diagn::Block;
    
    /** helper to find Block entries
     *  based on their raw memory address */
    inline bool
    identifyBlock (Block const& inQuestion, void* storage)
    {
      return storage == &inQuestion;
    }
    
    /** build a searching predicate */
    inline function<bool(Block const&)>
    search_for_block_using_this_storage (void* storage)
    {
      return bind (identifyBlock, _1, storage);
    }
    
    template<class VEC>
    inline Block*
    pick_Block_by_storage (VEC& vec, void* blockLocation)
    {
      typename VEC::iterator pos
        = std::find_if (vec.begin(),vec.end()
                       ,search_for_block_using_this_storage(blockLocation));
      if (pos!=vec.end())
        return &(*pos);
      else
        return NULL;
    }
  }
  
  
  
  namespace diagn {
    
    typedef ScopedPtrVect<Block>  PoolVec;
    typedef ScopedHolder<PoolVec> PoolHolder;
    
    /**
     * @internal Pool of allocated buffer Blocks of a specific size.
     * Helper for implementing a Diagnostic BufferProvider; actually does
     * just heap allocations for the Blocks, but keeps a collection of
     * allocated Blocks around. Individual entries can be retrieved
     * and thus removed from the responsibility of BlockPool.
     * 
     * The idea is that each buffer starts its lifecycle within some pool
     * and later gets "emitted" to an output sequence, where it remains for
     * later investigation and diagnostics.
     */
    class BlockPool
      {
        uint maxAllocCount_;
        size_t memBlockSize_;
        PoolHolder blockList_;
        
      public:
        BlockPool()
          : maxAllocCount_(0) // unlimited by default
          , memBlockSize_(0)
          , blockList_()
          { }
        
        void
        initialise (size_t blockSize)
          {
            blockList_.create();
            memBlockSize_ = blockSize;
          }
         // standard copy operations are valid, but will
        //  raise an runtime error, once BlockPool is initialised.
        
       ~BlockPool()
         {
           if (!verify_all_children_idle())
             ERROR (test, "Block actively in use while shutting down BufferProvider "
               "allocation pool. This might lead to Segfault and memory leaks.");
         }
        
        
        uint 
        prepare_for (uint number_of_expected_buffers)
          {
            if (maxAllocCount_ && 
                maxAllocCount_ < blockList_->size() + number_of_expected_buffers)
              {
                ASSERT (maxAllocCount_ >= blockList_->size());
                return maxAllocCount_ - blockList_->size();
              }
            // currently no hard limit imposed
            return number_of_expected_buffers;
          }
        
        
        Block&
        createBlock()
          {
            return blockList_->manage (new Block(memBlockSize_));
          }
        
        
        Block*
        find (void* blockLocation)
          {
            return pick_Block_by_storage (*blockList_, blockLocation);            
          }
        
        
        Block*
        transferResponsibility (Block* allocatedBlock)
          {
            return blockList_->detach (allocatedBlock);
          }
        
        
        size_t
        size()  const
          {
            return blockList_->size();
          }
        
        bool
        isValid()  const
          {
            return bool(blockList_);
          }
      
        explicit
        operator bool()  const
          {
            return isValid();
          }
        
      private:
          bool
          verify_all_children_idle()
            {
            try {
                if (blockList_)
                  return and_all (*blockList_, is_in_sane_state);
                }
              ERROR_LOG_AND_IGNORE (test, "State verification of diagnostic BufferProvider allocation pool");
              return true;
            }
          
          
          static bool
          is_in_sane_state (Block const& block)
            {
              return not block.was_used() or block.was_closed();
            }
      };
  }
  
    
    
  namespace { // Details of allocation and accounting
    
    const uint MAX_BUFFERS = 50;
    
    diagn::Block emptyPlaceholder(0);
  
  } // (END) Details of allocation and accounting
  
  
  
  /**
   * @internal create a memory tracking BufferProvider,
   */
  TrackingHeapBlockProvider::TrackingHeapBlockProvider()
    : BufferProvider ("Diagnostic_HeapAllocated")
    , pool_(new diagn::PoolTable)
    , outSeq_()
    { }
  
  TrackingHeapBlockProvider::~TrackingHeapBlockProvider()
    {
      INFO (proc_mem, "discarding %zu diagnostic buffer entries", outSeq_.size());
    }
  
  
  /* ==== Implementation of the BufferProvider interface ==== */
  
  uint
  TrackingHeapBlockProvider::prepareBuffers(uint requestedAmount, HashVal typeID)
  {
    diagn::BlockPool& responsiblePool = getBlockPoolFor (typeID);
    return responsiblePool.prepare_for (requestedAmount);
  }

  
  BuffHandle
  TrackingHeapBlockProvider::provideLockedBuffer(HashVal typeID)
  {
    diagn::BlockPool& blocks = getBlockPoolFor (typeID);
    diagn::Block& newBlock = blocks.createBlock();
    return buildHandle (typeID, newBlock.accessMemory(), &newBlock);
  }
  
  
  void
  TrackingHeapBlockProvider::mark_emitted (HashVal typeID, LocalKey const& implID)
  {
    diagn::Block* block4buffer = locateBlock (typeID, implID);
    if (!block4buffer)
      throw error::Logic ("Attempt to emit a buffer not known to this BufferProvider"
                         , LUMIERA_ERROR_BUFFER_MANAGEMENT);
    diagn::BlockPool& pool = getBlockPoolFor (typeID);
    outSeq_.manage (pool.transferResponsibility (block4buffer));
  }
  
  
  /** mark a buffer as officially discarded */
  void
  TrackingHeapBlockProvider::detachBuffer (HashVal typeID, LocalKey const& implID)
  {
    diagn::Block* block4buffer = locateBlock (typeID, implID);
    REQUIRE (block4buffer, "releasing a buffer not allocated through this provider");
    block4buffer->markReleased();
  }
  
  
  
  /* ==== Implementation details ==== */
  
  size_t
  TrackingHeapBlockProvider::emittedCnt()  const
  {
    return outSeq_.size();
  }

  diagn::Block&
  TrackingHeapBlockProvider::access_emitted (uint bufferID)
  {
    if (!withinOutputSequence (bufferID))
      return emptyPlaceholder;                                                ////////////////////////////////TICKET #856
    else
      return outSeq_[bufferID];
  }
  
  bool
  TrackingHeapBlockProvider::withinOutputSequence (uint bufferID)  const
  {
    if (bufferID >= MAX_BUFFERS)
      throw error::Fatal ("hardwired internal limit for test buffers exceeded");
    
    return bufferID < outSeq_.size();
  }
  
  diagn::BlockPool&
  TrackingHeapBlockProvider::getBlockPoolFor (HashVal typeID)
  {
    diagn::BlockPool& pool = (*pool_)[typeID];
    if (!pool)
        pool.initialise(getBufferSize(typeID));
    return pool;
  }
  
  diagn::Block*
  TrackingHeapBlockProvider::locateBlock (HashVal typeID, void* storage)
  {
    diagn::BlockPool& pool = getBlockPoolFor (typeID);
    diagn::Block* block4buffer = pool.find (storage);                         ////////////////////////////////TICKET #856
    return block4buffer? block4buffer
                       : searchInOutSeqeuence (storage);
  }
  
  diagn::Block*
  TrackingHeapBlockProvider::searchInOutSeqeuence (void* blockLocation)
  {
    return pick_Block_by_storage (outSeq_, blockLocation);                    ////////////////////////////////TICKET #856
  }
  
  
  
  
}} // namespace engine
