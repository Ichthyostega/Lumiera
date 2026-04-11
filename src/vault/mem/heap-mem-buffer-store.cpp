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
#include "lib/integral.hpp"
#include "include/logging.h"
#include "lib/scoped-ptrvect.hpp"
#include "vault/mem/buffer-metadata.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/util-foreach.hpp"
#include "lib/nocopy.hpp"

#include "vault/mem/heap-mem-buffer-store.hpp"

#include <algorithm>
#include <vector>
#include <array>

using util::and_all;
using std::vector;
using std::array;
using lib::ScopedPtrVect;
using util::contains;
using util::_Fmt;



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
    
    using StorageWord = uint64_t;
    constexpr inline size_t WORD_SIZ = sizeof(StorageWord);
    
    constexpr size_t
    ceilDiv (size_t num, size_t den) noexcept
    {
      return (num + den - 1u) / den;
    }
    
    constexpr size_t
    wordCnt (size_t sizeRequest) noexcept
    {
      return ceilDiv (sizeRequest, WORD_SIZ);
    }
  
    static_assert (0 == wordCnt(0));
    static_assert (1 == wordCnt(1));
    static_assert (1 == wordCnt(WORD_SIZ));
    
    inline void
    zeroFill (void* buf, size_t cnt)
    {
      auto begin = static_cast<StorageWord*> (buf);
      std::fill (begin, begin+cnt, StorageWord(0));
    }
    
  } // (END) Details of allocation and accounting
  
  
  /**
   * @internal smart-handle and administrative record
   *  to manage a single buffer allocation and verify
   *  the proper sequence of lifecycle steps.
   * @remark since HeapMemBufferStore is the default for tests,
   *         it seems prudent to perform additional sanity checks.
   */
  class HeapMemBufferStore::Alloc
    : util::MoveOnly
    {
      Buff* mem_{nullptr};
      size_t siz_{0};
      BufferState state_{NIL};

      void
      __requireState (BufferState expected)
        {
          if (state_ != expected)
                throw err::Logic{_Fmt{"HeapMemBufferStore: Lifecycle broken: "
                                      "expected_state:%d actual_state:%d"}
                                     % expected % state_
                                ,LERR_(LIFECYCLE)
                                };
        }

      void
      allocate (size_t cnt)
        {
          __requireState (NIL);
          REQUIRE (!mem_,"Attempt to re-allocate / double-allocate");
          REQUIRE (cnt,  "Attempt to create a zero-sized buffer");
          siz_ = cnt;
          size_t words = wordCnt(cnt);
          mem_ = static_cast<Buff*> (std::aligned_alloc (std::alignment_of<StorageWord>(), words ));
          ENSURE (mem_);
          state_ = LOCKED;
        }
      
      void
      discard()
        {
          if (mem_)
            std::free (mem_);
          mem_ = nullptr;
          siz_ = 0;
          state_ = BLOCKED;
        }
      
      
      Alloc (size_t size) { allocate(size); }
      
    public:
     ~Alloc() { discard(); }
      Alloc() = default;
      
      Alloc (Alloc&& rr)
        : Alloc{}
        {
          std::swap (state_, rr.state_);
          std::swap (siz_,   rr.siz_);
          std::swap (mem_,   rr.mem_);
        }
      
      
      using IdxEntry = Index::value_type;
      
      /**
       * Create new allocation and map a new entry for the
       * internal allocation index of HeapMemBufferStore.
       * @remark this is the only way to allocate.
       */
      static Buff*
      makeNewAllocEntry (Index& idx, size_t buffSize)
        {
          Alloc newAlloc{buffSize};
          Buff* memLocation = newAlloc.mem_;
          if (contains (idx, memLocation))
            throw err::State{"HeapMemBufferStore allocated already "
                             "registered memory address again."};
          idx.emplace (memLocation, move(newAlloc));
          ENSURE (contains (idx, memLocation));
          return memLocation;
        }
      
      void
      verify (Buff* address, size_t size)
        {
          if (mem_ != address or siz_ != size)
            throw err::State{_Fmt{"HeapMemBufferStore allocation identity corrupted: "
                                  "Expect (size=%d|%s) yet invoked at state:%d with (%d|%s)."}
                                 % siz_ % util::showAdr(mem_) % state_ % size % util::showAdr(address)};
        }
      
      
      void
      emit()
        {
          __requireState (LOCKED);
          state_ = EMITTED;
        }
      
      void
      release()
        {
          if (state_ != LOCKED)
            __requireState (EMITTED);
          state_ = FREE;
        } // NOTE: Allocation retained (for diagnostics)
    };
  
  
  
  /**
   * @internal create a memory tracking BufferProvider,
   */
  HeapMemBufferStore::HeapMemBufferStore()
    : allocIdx_{}
    , pool_(new PoolTable)
    , outSeq_()
    { }
  
  HeapMemBufferStore::~HeapMemBufferStore() { /* dtor of index table -> de-allocation here */ }
  
  
  
  /* ==== Implementation of the BufferProvider interface ==== */
  
  uint
  HeapMemBufferStore::prepareBuffers (HashVal, uint numBuffers, size_t)
  {
    return numBuffers;
  }

  
  BuffAlloc
  HeapMemBufferStore::provideBuffer (HashVal, size_t buffSiz, LocalTag specifics, int64_t)
  {
    Buff* storage = Alloc::makeNewAllocEntry (allocIdx_, buffSiz);
    return std::make_tuple (storage, buffSiz, specifics);
  }
  
  
  void
  HeapMemBufferStore::mark_emitted (HashVal, BuffAlloc storageSlot)
  {
    auto& [storage,buffSiz,specifics] = storageSlot;
    if (not contains (allocIdx_, storage))
      throw err::Invalid{_Fmt{"Passed buffer (size=%d|%s) for emit() call "
                              "that is not recognised by this HeapMemBufferStore"}
                             % buffSiz % util::showAdr(storage)};
    
    auto& entry = allocIdx_[storage];
    entry.verify (storage,buffSiz);
    entry.emit();
  }
  
  
  /** mark a buffer as officially discarded */
  void
  HeapMemBufferStore::detachBuffer (HashVal, BuffAlloc storageSlot)
  {
    auto& [storage,buffSiz,specifics] = storageSlot;
    if (not contains (allocIdx_, storage))
      throw err::Invalid{_Fmt{"Passed buffer (size=%d|%s) for release() call "
                              "that is not recognised by this HeapMemBufferStore"}
                             % buffSiz % util::showAdr(storage)};
    
    auto& entry = allocIdx_[storage];
    entry.verify (storage,buffSiz);
    entry.release();
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
