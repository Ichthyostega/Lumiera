/*
  NAIVE-BUFFER_SETUP.hpp  -  plain heap allocating BufferProvider implementation for tests

   Copyright (C)
     2026,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file naive-buffer-setup.hpp
 ** Dummy implementation of the BufferProvider interface to support writing unit tests.
 ** This setup for a BufferProvider is notably straight forward and basically allocates
 ** a new heap block for any buffer, without deallocating memory ever. This allows to
 ** attach additional tracking status flags for each allocated block and validate
 ** allocator behaviour after the fact.
 ** 
 ** @todo 1/2026 for sake of implementation clarity, the following instrumentation functionality
 **       must be relocated into the DiagnosticOutputProvider: Allocated buffers are numbered
 **       with a simple ascending sequence of integers, used as LocalTag (see BufferMetadata).
 **       Clients can request a Buffer with the given number, causing that block to be allocated.
 **       There is a "backdoor", allowing  to access any allocated block, even if it is considered
 **       "released" by the terms of the usual lifecycle. Only when the provider object itself
 **       gets destroyed, all allocated blocks will be discarded.
 ** 
 ** @see DiagnosticOutputSlot
 ** @see DiagnosticBufferProvider
 ** @see buffer-provider-protocol-test.cpp
 */

#ifndef STEAM_ENGINE_NAIVE_BUFFER_STEUP_H
#define STEAM_ENGINE_NAIVE_BUFFER_STEUP_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffer-provider-setup.hpp"
#include "steam/engine/heap-mem-buffer-store.hpp"
#include "lib/scoped-ptrvect.hpp"

#include <unordered_map>
#include <memory>


namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
//  using lib::HashVal;
  
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
  namespace diagn {
    
    using std::unique_ptr;

    
    /**
     * Helper for implementing a diagnostic BufferProvider:
     * A block of heap allocated storage, with the capability
     * to store some additional tracking information.
     */
    class Block
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
      
    class BlockPool;
    
    using PoolTable = std::unordered_map<HashVal,BlockPool>;
  }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
  
  
  /**
   * simple BufferProvider implementation with additional allocation tracking.
   * @internal used as PImpl by DiagnosticBufferProvider and DiagnosticOutputSlot.
   * 
   * This dummy implementation of the BufferProvider interface uses a linearly growing
   * table of heap allocated buffer blocks, which will never be discarded, unless the object
   * is discarded as a whole. There is an additional testing/diagnostics API to access the
   * tracked usage information, even when blocks are already marked as "released".
   */
  class NaiveBufferSetup
    : public BufferProviderSetup
    {
    public:
      NaiveBufferSetup()
        : BufferProviderSetup{*this}
        { }
        
        auto buildStage() { return std::make_unique<Stage> ("Naive_HeapAllocated"); }
        auto buildStore() { return std::make_unique<HeapMemProvider>(); }   /////////////////////////////////TICKET #1410 : turn into a subclass of BufferSetup::Store and push down implementation
    };
  
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : disabled code to disentangle BufferProvider implementation
  class HeapMemProvider
    : public BufferProvider
    {
      unique_ptr<diagn::PoolTable> pool_;
      ScopedPtrVect<diagn::Block> outSeq_;
      
    public:
      /* === BufferProvider interface === */
      
      virtual uint prepareBuffers (uint count, HashVal typeID)    override;
      virtual BuffHandle provideLockedBuffer  (HashVal typeID)    override;
      virtual void mark_emitted (HashVal, LocalTag const&)        override;
      virtual void detachBuffer (HashVal, LocalTag const&, Buff&) override;
      
    public:
     ~HeapMemProvider();
      HeapMemProvider();
      
      size_t emittedCnt()  const;
      
      diagn::Block& access_emitted (uint bufferID);
      
      template<typename TY>
      TY&  accessAs (uint bufferID);
      
      void markAllEmitted();
      
    private:
      bool withinOutputSequence (uint bufferID)  const;
      diagn::BlockPool& getBlockPoolFor (HashVal typeID);
      diagn::Block* locateBlock (HashVal typeID, void*);
      diagn::Block* searchInOutSeqeuence (void* storage);
    };
  
  
  
  /** convenience shortcut: access the buffer with the given number,
   *  then try to convert the raw memory to the templated type.
   * @throw error::Invalid if the required fame number is beyond
   *        the number of buffers marked as "emitted"
   * @throw error::Fatal if conversion is not possible or the
   *        conversion path chosen doesn't work (which might
   *        be due to RTTI indicating an incompatible type).
   */
  template<typename TY>
  TY&
  HeapMemProvider::accessAs (uint bufferID)
  {
    if (!withinOutputSequence (bufferID))
      throw error::Invalid ("Buffer with the given ID not yet emitted");
    
    diagn::Block& memoryBlock = access_emitted (bufferID);
    TY* converted = std::launder (reinterpret_cast<TY*> (memoryBlock.accessMemory()));
    
    REQUIRE (converted);
    return *converted;
  }
#endif  /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (end) disabled code
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_NAIVE_BUFFER_STEUP_H*/
