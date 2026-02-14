/*
  HEAP-MEM-BUFFER_STORE.hpp  -  plain heap allocating BufferProvider implementation for tests

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file heap-mem-provider.hpp
 ** Dummy engine::BufferProvider storage implementation, for sake of conceptual
 ** demonstration and unit testing. This BufferProvider implementation is notably straight forward,
 ** if not outright silly: it happily claims more and more heap blocks and never releases any memory
 ** dynamically. This both demonstrates the simplest possible implementation of storage handling, and
 ** allows to investigate additional tracking status flags for each allocated block after the fact.
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

#ifndef STEAM_ENGINE_HEAP_MEM_BUFFER_STORE_H
#define STEAM_ENGINE_HEAP_MEM_BUFFER_STORE_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffer-provider-setup.hpp"
#include "lib/scoped-ptrvect.hpp"

#include <unordered_map>
#include <memory>


namespace steam {
namespace engine {
  
  namespace error = lumiera::error;
  
  using std::unique_ptr;
  using lib::ScopedPtrVect;
  using lib::HashVal;
  
  
  /**
   * Naive demo implementation of the BufferProvider::BufferStore.
   * This is kind of a faked implementation of the BufferProvider backend and uses a
   * linearly growing table of heap allocated buffer blocks, which will never be discarded,
   * unless the object is discarded as a whole. Paired with the instrumentation and diagnostics
   * API of DiagnosticBufferProvider, this can be used to confirm assumptions regarding
   * buffer usage from a unit test setup. Even blocks already marked as "released"
   * are actually retained and can be investigated after the fact.
   */
  class HeapMemBufferStore
    : public BufferProviderSetup::Store
    {
    public: /////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : looks like I'll re-implement the storage from scratch, since »tracking« is no longer required here
      class Block;
      class BlockPool;
    private:
      using PoolTable = std::unordered_map<HashVal,BlockPool>;
      unique_ptr<PoolTable> pool_;
      ScopedPtrVect<Block> outSeq_;
      
    public:
      /* === BufferStore interface === */
      
      virtual uint prepareBuffers (uint cnt, size_t,HashVal typeID) override;
      virtual Slot provideBuffer (size_t,HashVal, LocalTag)         override;
      virtual void mark_emitted  (size_t,HashVal, LocalTag const&)  override;
      virtual void detachBuffer  (size_t,HashVal, Slot alloc)       override;
      
    public:
     ~HeapMemBufferStore();
      HeapMemBufferStore();
      
#if false    ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : this additional tracking API is obsolete and need to be removed
      size_t emittedCnt()  const;
      
      Block& access_emitted (uint bufferID);
      
      template<typename TY>
      TY&  accessAs (uint bufferID);
      
      void markAllEmitted();
#endif       ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1410 : (End) obsoleted API
      
    private:
      bool withinOutputSequence (uint bufferID)  const;
      BlockPool& getBlockPoolFor (size_t,HashVal typeID);
      Block* locateBlock (size_t,HashVal typeID, void*);
      Block* searchInOutSeqeuence (void* storage);
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_HEAP_MEM_BUFFER_STORE_H*/
