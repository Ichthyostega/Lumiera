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
 ** This BufferStore implementation is configured by default and thus used primarily
 ** from the unit tests; for that reasons, some additional sanity checks were added,
 ** and the implementation is wasteful and not performance oriented. Furthermore,
 ** aspects of concurrency safety were not considered.
 ** 
 ** For each allocation, a new vault::mem::HeapMemBufferStore::Alloc entry is attached
 ** to the index table, which allows follow-up calls to rediscover this management entry
 ** just based on the memory address of the allocation. These entries manage the actual
 ** allocations, and **will be retained** for the lifespan of the BufferStore instance.
 ** This allows diagnostic code to investigate the contents of the buffers
 ** after invoking the code subject to testing.
 */


#include "lib/error.hpp"
#include "lib/integral.hpp"
#include "vault/mem/buffer-metadata.hpp"
#include "lib/format-string.hpp"
#include "lib/format-obj.hpp"
#include "lib/nocopy.hpp"

#include "vault/mem/heap-mem-buffer-store.hpp"

using util::contains;
using util::_Fmt;


namespace vault {
namespace mem   {
  namespace err = lumiera::error;
  
  namespace { // internal config and helpers...
    
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
  } //(END) Internals
  
  
  
  
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
  
  
  
  
  
  HeapMemBufferStore::HeapMemBufferStore()
    : allocIdx_{}
    { }
  
  HeapMemBufferStore::~HeapMemBufferStore() { /* dtor of index table -> de-allocation here */ }
  
  
  
  /* ==== Implementation of the BufferProvider interface ==== */
  
  /** @note pre-anouncement is ignored */
  uint
  HeapMemBufferStore::prepareBuffers (HashVal, uint numBuffers, size_t)
  {
    return numBuffers;
  }

  
  /** API: create a new buffer allocation */
  BuffAlloc
  HeapMemBufferStore::provideBuffer (HashVal, size_t buffSiz, LocalTag specifics, int64_t)
  {
    Buff* storage = Alloc::makeNewAllocEntry (allocIdx_, buffSiz);
    return std::make_tuple (storage, buffSiz, specifics);
  }
  
  
  /** API: mark a buffer as _emitted_ */
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
  
  
  /** API: mark a buffer as officially discarded */
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
  
  
}} // namespace vault::mem
