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


#include "lib/error.hpp"
#include "include/logging.h"
//#include "lib/meta/function.hpp"
#include "lib/bool-checkable.hpp"
//#include "lib/scoped-ptrvect.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/util-foreach.hpp"

#include "proc/engine/tracking-heap-block-provider.hpp"

#include <boost/noncopyable.hpp>
#include <algorithm>
#include <vector>

using util::for_each;
using std::vector;
using lib::ScopedHolder;



namespace engine {
  
  namespace error = lumiera::error;
  
  namespace diagn {
    
    typedef vector<Block*> PoolVec;
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
      : public lib::BoolCheckable<BlockPool>
      {
        size_t memBlockSize_;
        PoolHolder blockList_;
        
      public:
        BlockPool()
          : memBlockSize_(0)
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
          try {
            if (blockList_)
              for_each (*blockList_, discardBuffer);
            }
          ERROR_LOG_AND_IGNORE (test, "Shutdown of diagnostic BufferProvider allocation pool");
        
          
        Block&
        createBlock()
          {
            Block* newBlock(0);
            try
              {
                newBlock = new Block(memBlockSize_);  
                blockList_->push_back (newBlock);
              }
            catch(...)
              {
                if (newBlock)
                  delete newBlock;
                throw;
              }
            ENSURE (newBlock);
            return *newBlock;
          }
        
        
        Block*
        find (void* bufferStorage)
          {
            UNIMPLEMENTED ("find a block based on storage location");
          }
        
        
        Block*
        transferResponsibility (Block* allocatedBlock)
          {
            Block* extracted;
            PoolVec& vec = *blockList_;
            PoolVec::iterator pos = std::find (vec.begin(),vec.end(), allocatedBlock);
            if (pos != vec.end())
              {
                extracted = *pos;
                vec.erase(pos);
              }
            return extracted;
          }
        
        
        size_t
        size()  const
          {
            return blockList_->size();
          }
        
        bool
        isValid()  const
          {
            return blockList_;
          }
        
      private:
          static void
          discardBuffer (Block* block)
            {
              if (!block) return;
              
              if (block->was_used() && !block->was_closed())
                ERROR (test, "Block actively in use while shutting down BufferProvider "
                             "allocation pool. This might lead to Segfault and memory leaks.");
              
              delete block;
            }
      };

  }
  
    
    
  namespace { // Details of allocation and accounting
    
    const uint MAX_BUFFERS = 50;
  
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
  TrackingHeapBlockProvider::prepareBuffers(uint, lib::HashVal)
  {
    UNIMPLEMENTED ("pre-register storage for buffers of a specific kind");   
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
  TrackingHeapBlockProvider::access_or_create (uint bufferID)
  {
    while (!withinOutputSequence (bufferID))
      outSeq_.manage (new diagn::Block(0));   /////////TICKET #856 really need a better way of returning a fallback
    
    ENSURE (withinOutputSequence (bufferID));
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
    diagn::Block* block4buffer = pool.find (storage);
    return block4buffer? block4buffer
                       : searchInOutSeqeuence (storage);
  }
  
  diagn::Block*
  TrackingHeapBlockProvider::searchInOutSeqeuence (void* storage)
  {
    UNIMPLEMENTED ("find block by storage location");
  }
  
  
  
  
} // namespace engine
