/*
  TRACKING-HEAP-BLOCK-PROVIDER.hpp  -  plain heap allocating BufferProvider implementation for tests

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

*/

/** @file tracking-heap-block-provider.hpp
 ** Dummy implementation of the BufferProvider interface to support writing unit tests.
 ** This BufferProvider is especially straight forward and brain dead: it just claims
 ** more and more heap blocks and never releases any memory dynamically. This allows
 ** to investigate additional tracking status flags for each allocated block after
 ** the fact.
 ** 
 ** The allocated buffers are numbered with a simple ascending sequence of integers,
 ** used as LocalKey (see BufferMetadata). Clients can just request a Buffer with the
 ** given number, causing that block to be allocated. There is a "backdoor", allowing
 ** to access any allocated block, even if it is considered "released" by the terms
 ** of the usual lifecycle. Only when the provider object itself gets destroyed,
 ** all allocated blocks will be discarded.
 ** 
 ** @see DiagnosticOutputSlot
 ** @see DiagnosticBufferProvider
 ** @see buffer-provider-protocol-test.cpp
 */

#ifndef PROC_ENGINE_TRACKING_HEAP_BLOCK_PROVIDER_H
#define PROC_ENGINE_TRACKING_HEAP_BLOCK_PROVIDER_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/engine/buffer-provider.hpp"
#include "lib/scoped-ptrvect.hpp"

#include <unordered_map>
#include <memory>


namespace proc {
namespace engine {
  
  namespace error = lumiera::error;
  
  using lib::ScopedPtrVect;
  using lib::HashVal;
  
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
    
    typedef std::unordered_map<HashVal,BlockPool> PoolTable;
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
  class TrackingHeapBlockProvider
    : public BufferProvider
    {
      unique_ptr<diagn::PoolTable> pool_;
      ScopedPtrVect<diagn::Block> outSeq_; 
      
    public:
      /* === BufferProvider interface === */
      
      virtual uint prepareBuffers (uint count, HashVal typeID);
      virtual BuffHandle provideLockedBuffer  (HashVal typeID);
      virtual void mark_emitted (HashVal entryID, LocalKey const&);
      virtual void detachBuffer (HashVal entryID, LocalKey const&);
      
    public:
      TrackingHeapBlockProvider();
      virtual ~TrackingHeapBlockProvider();
      
      size_t emittedCnt()  const;
      
      diagn::Block& access_emitted (uint bufferID);
      
      template<typename TY>
      TY&  accessAs (uint bufferID);
      
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
  TrackingHeapBlockProvider::accessAs (uint bufferID)
  {
    if (!withinOutputSequence (bufferID))
      throw error::Invalid ("Buffer with the given ID not yet emitted");
    
    diagn::Block& memoryBlock = access_emitted (bufferID);
    TY* converted = reinterpret_cast<TY*> (memoryBlock.accessMemory());
    
    REQUIRE (converted);
    return *converted;
  }
  
  
  
  
}} // namespace proc::engine
#endif
