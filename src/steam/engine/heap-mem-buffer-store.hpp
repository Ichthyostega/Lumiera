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
 ** Dummy implementation engine::BufferProvider storage, for sake of conceptual
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
  
  using lib::ScopedPtrVect;
  using lib::HashVal;
  
  namespace diagn {
    
    using std::unique_ptr;

    
  }
  
  
  /**
   * simple BufferProvider implementation with additional allocation tracking.
   * @internal used as PImpl by DiagnosticBufferProvider and DiagnosticOutputSlot.
   * 
   * This dummy implementation of the BufferProvider interface uses a linearly growing
   * table of heap allocated buffer blocks, which will never be discarded, unless the object
   * is discarded as a whole. There is an additional testing/diagnostics API to access the
   * tracked usage information, even when blocks are already marked as "released".
   */
  class HeapMemProvider
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
      
      virtual uint prepareBuffers (uint count, HashVal typeID)    override;
      virtual BuffHandle provideLockedBuffer  (HashVal typeID)    override;
      virtual void mark_emitted (HashVal, LocalTag const&)        override;
      virtual void detachBuffer (HashVal, LocalTag const&, Buff&) override;
      
    public:
     ~HeapMemProvider();
      HeapMemProvider();
      
      size_t emittedCnt()  const;
      
      Block& access_emitted (uint bufferID);
      
      template<typename TY>
      TY&  accessAs (uint bufferID);
      
      void markAllEmitted();
      
    private:
      bool withinOutputSequence (uint bufferID)  const;
      BlockPool& getBlockPoolFor (HashVal typeID);
      Block* locateBlock (HashVal typeID, void*);
      Block* searchInOutSeqeuence (void* storage);
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
    
    Block& memoryBlock = access_emitted (bufferID);
    NOTREACHED ("TICKET #1410 : tracking implementation -> DiagnosticBufferProvider");
//  TY* converted = std::launder (reinterpret_cast<TY*> (memoryBlock.accessMemory()));  /////////////////////TICKET 1410 : my assumption is that Block will be implemented differently -- at least not used this way...
    
//  REQUIRE (converted);
//  return *converted;
  }
  
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_HEAP_MEM_BUFFER_STORE_H*/
