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
//#include "lib/scoped-ptrvect.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/util-foreach.hpp"

#include "proc/engine/tracking-heap-block-provider.hpp"

#include <boost/noncopyable.hpp>
#include <vector>

using util::for_each;
using std::vector;
using std::auto_ptr;
using lib::ScopedHolder;



namespace engine {
  
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
            memBlockSize_(blockSize);
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
                newBlock = new Block();  ////////////////////////////TODO pass size as ctor param  
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
        
        
        auto_ptr<Block>
        transferResponsibility (Block* allocatedBlock)
          {
            auto_ptr<Block> extracted;
            PoolVec& vec = *blockList_;
            PoolVec::iterator pos = find (vec.begin(),vec.end(), allocatedBlock);
            if (pos != vec.end())
              {
                extracted.reset (allocatedBlock);
                vec.erase(pos);
              }
            return extracted;
          }
        
        
        size_t
        size()  const
          {
            return blockList_->size();
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
    , pool_()
    { }
  
  TrackingHeapBlockProvider::~TrackingHeapBlockProvider()
    {
      INFO (proc_mem, "discarding %zu diagnostic buffer entries", TrackingHeapBlockProvider::size());
    }
  
  
  /* ==== Implementation of the BufferProvider interface ==== */
  
  uint
  TrackingHeapBlockProvider::announce (uint count, BufferDescriptor const& type)
  {
    UNIMPLEMENTED ("pre-register storage for buffers of a specific kind");   
  }

  
  BuffHandle
  TrackingHeapBlockProvider::lockBufferFor (BufferDescriptor const& type)
  {
    UNIMPLEMENTED ("lock buffer for exclusive use");
    diagn::BlockPool& blocks = getBlockPoolFor (type);
    diagn::Block* newBlock = blocks.createBlock();
    return buildHandle (type, newBlock);
  }
  
  
  void
  TrackingHeapBlockProvider::mark_emitted (BuffHandle const& handle)
  {
    UNIMPLEMENTED ("commit a buffer to the protocol section");
  }
  
  
  void
  TrackingHeapBlockProvider::releaseBuffer (BuffHandle const& handle)
  {
    UNIMPLEMENTED ("mark a buffer as officially discarded");
  }
  
  
  
  /* ==== Implementation details ==== */
  
  diagn::Block&
  TrackingHeapBlockProvider::access_or_create (uint bufferID)
  {
    while (!withinStorageSize (bufferID))
      this->manage (new diagn::Block);
    
    ENSURE (withinStorageSize (bufferID));
    return (*this)[bufferID];
  }
  
  bool
  TrackingHeapBlockProvider::withinStorageSize (uint bufferID)  const
  {
    if (bufferID >= MAX_BUFFERS)
      throw error::Fatal ("hardwired internal limit for test buffers exceeded");
    
    return bufferID < this->size();
  }
  
  
  
  
} // namespace engine
