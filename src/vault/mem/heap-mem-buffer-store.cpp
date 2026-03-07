/*
  HeapMemBufferStore  -  plain heap allocating BufferProvider storage implementation for tests

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file heap-mem-buffer-store.cpp
 ** Implementation details of simple heap based engine::BufferProvider storage.
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/scoped-ptrvect.hpp"
#include "lib/util-foreach.hpp"

#include "vault/mem/heap-mem-buffer-store.hpp"

#include <algorithm>
#include <vector>

using util::and_all;
using std::vector;
using lib::ScopedPtrVect;



namespace vault {
namespace mem   {
  namespace err = lumiera::error;
  
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 1410 : need to question what implementation structures are needed, after »tracking« was extracted...
    /**
     * Simplistic implementation of buffer storage.
     * Allocates a block of heap memory for buffers and never deallocates.
     * This strange behaviour can be used to investigate and demonstrate
     * buffer usage from a unit test setup.
     * @see DiagnosticBufferProvider
     */
    class HeapMemBufferStore::Block
      : util::NonCopyable
      {
        unique_ptr<char[]> storage_;
        
        bool was_released_;
        
      public:
        explicit
        Block(size_t bufferSize)
          : storage_(bufferSize? new char[bufferSize] : NULL)
          , was_released_(false)
          { }
        
        bool
        was_used()  const
          {
            return bool(storage_);
          }
        
        bool
        was_closed()  const
          {
            return was_released_;
          }
        
        void*
        accessMemory()  const
          {
            REQUIRE (storage_, "Block was never prepared for use");
            return storage_.get();
          }
        
        void
        markReleased()
          {
            was_released_ = true;
          }
      };
  namespace { // implementation helpers...
    
    inline Buff*
    asBuffer(void* mem)
      {// type tag to mark memory address as Buffer
        return static_cast<Buff*> (mem);
      }
    
    
    using Block = HeapMemBufferStore::Block;
    
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
  
  

      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 1410 : need to question what implementation structures are needed, after »tracking« was extracted...
    
    using PoolBlocks = ScopedPtrVect<HeapMemBufferStore::Block>;
    
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
    class HeapMemBufferStore::BlockPool
      {
        uint maxAllocCount_;
        size_t memBlockSize_;
        using PoolBlocks = ScopedPtrVect<Block>;
        PoolBlocks blockList_;
        
      public:
        BlockPool()
          : maxAllocCount_(0) // unlimited by default
          , memBlockSize_(0)
          , blockList_{}
          { }
        
        void
        initialise (size_t blockSize)
          {
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
        
        /** mark all managed blocks as disposed */
        void
        discard()
          {
            for (Block& block : blockList_)
              block.markReleased();
          }
        
        uint
        prepare_for (uint number_of_expected_buffers)
          {
            if (maxAllocCount_ &&
                maxAllocCount_ < blockList_.size() + number_of_expected_buffers)
              {
                ASSERT (maxAllocCount_ >= blockList_.size());
                return maxAllocCount_ - blockList_.size();
              }
            // currently no hard limit imposed
            return number_of_expected_buffers;
          }
        
        
        Block&
        createBlock()
          {
            return blockList_.manage (new Block(memBlockSize_));
          }
        
        
        Block*
        find (void* blockLocation)
          {
            return pick_Block_by_storage (blockList_, blockLocation);
          }
        
        
        Block*
        transferResponsibility (Block* allocatedBlock)
          {
            return blockList_.detach (allocatedBlock);
          }
        
        
        size_t
        size()  const
          {
            return blockList_.size();
          }
        
        bool
        isValid()  const
          {
            return not blockList_.empty();
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
                  return and_all (blockList_, is_in_sane_state);
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 1410 : need to question what implementation structures are needed, after »tracking« was extracted...
  
  
  
  namespace { // Details of allocation and accounting
    
    const uint MAX_BUFFERS = 50;
    
    Block emptyPlaceholder(0);
  
  } // (END) Details of allocation and accounting
  
  
  
  /**
   * @internal create a memory tracking BufferProvider,
   */
  HeapMemBufferStore::HeapMemBufferStore()
    : pool_(new PoolTable)
    , outSeq_()
    { }
  
  
  HeapMemBufferStore::~HeapMemBufferStore() { /* emit dtor of BlockPool here */ }
  
  
  /* ==== Implementation of the BufferProvider interface ==== */
  
  uint
  HeapMemBufferStore::prepareBuffers (uint numBuffers, size_t buffSiz, HashVal typeID)
  {
    BlockPool& responsiblePool = getBlockPoolFor (buffSiz, typeID);
    return responsiblePool.prepare_for (numBuffers);
  }

  
  BufferProviderSetup::Store::Slot
  HeapMemBufferStore::provideBuffer (size_t buffSiz, HashVal typeID, LocalTag)
  {
    BlockPool& blocks = getBlockPoolFor (buffSiz, typeID);
    Block& newBlock = blocks.createBlock();
    LocalTag specifics{&newBlock}; // used by this implementation to find the storage to release later
    return std::make_tuple (asBuffer(newBlock.accessMemory()), specifics);
  }
  
  
  void
  HeapMemBufferStore::mark_emitted (size_t buffSiz, HashVal typeID, LocalTag const& specifics)
  {
    Block* block4buffer = locateBlock (buffSiz, typeID, specifics);
    if (!block4buffer)
      throw err::Logic ("Attempt to emit a buffer not known to this BufferProvider"
                       , LUMIERA_ERROR_BUFFER_MANAGEMENT);
    BlockPool& pool = getBlockPoolFor (buffSiz, typeID);
    Block* active = pool.transferResponsibility (block4buffer);
    if (active)
      outSeq_.manage (active);
    else
    if (block4buffer->was_closed())
      WARN (proc_mem, "Attempt to emit() an already closed buffer.");
    else
      WARN (proc_mem, "Attempt to emit() a buffer not found in active pool. "
                      "Maybe duplicate call to emit()?");
  }
  
  
  /** mark a buffer as officially discarded */
  void
  HeapMemBufferStore::detachBuffer (size_t buffSiz, HashVal typeID, Slot alloc)
  {
    auto& [storage, specifics] = alloc;
    Block* block4buffer = locateBlock (buffSiz, typeID, specifics);
    REQUIRE (block4buffer, "releasing a buffer not allocated through this provider");
    REQUIRE (util::isSameAdr (storage, block4buffer->accessMemory()));
    block4buffer->markReleased();
  }
  
  
  
  /* ==== Implementation details ==== */
  
#if false    ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : this additional tracking API is obsolete and need to be removed
  size_t
  HeapMemBufferStore::emittedCnt()  const
  {
    return outSeq_.size();
  }
  
  void
  HeapMemBufferStore::markAllEmitted()
  {
    for (auto& [_, blockPool] : *pool_)
         blockPool.discard();
  }
  
  HeapMemBufferStore::Block&
  HeapMemBufferStore::access_emitted (uint bufferID)
  {
    if (!withinOutputSequence (bufferID))
      return emptyPlaceholder;                                                ////////////////////////////////TICKET #856
    else
      return outSeq_[bufferID];
  }
#endif       ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (End) obsoleted API
  
  bool
  HeapMemBufferStore::withinOutputSequence (uint bufferID)  const
  {
    if (bufferID >= MAX_BUFFERS)
      throw err::Fatal ("hardwired internal limit for test buffers exceeded");
    
    return bufferID < outSeq_.size();
  }
  
  HeapMemBufferStore::BlockPool&
  HeapMemBufferStore::getBlockPoolFor (size_t buffSiz, HashVal typeID)
  {
    BlockPool& pool = (*pool_)[typeID];
    if (not pool)
      pool.initialise (buffSiz);
    return pool;
  }
  
  HeapMemBufferStore::Block*
  HeapMemBufferStore::locateBlock (size_t buffSiz, HashVal typeID, void* storage)
  {
    BlockPool& pool = getBlockPoolFor (buffSiz, typeID);
    Block* block4buffer = pool.find (storage);                                ////////////////////////////////TICKET #856
    return block4buffer? block4buffer
                       : searchInOutSeqeuence (storage);
  }
  
  
  
  HeapMemBufferStore::Block*
  HeapMemBufferStore::searchInOutSeqeuence (void* blockLocation)
  {
    return pick_Block_by_storage (outSeq_, blockLocation);                    ////////////////////////////////TICKET #856
  }
  
  
  
}} // namespace vault::mem
