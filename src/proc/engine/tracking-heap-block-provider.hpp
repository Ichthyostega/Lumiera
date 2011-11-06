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
#include "proc/engine/buffer-provider.hpp"
#include "lib/scoped-ptrvect.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>


namespace engine {
  
  namespace error = lumiera::error;
  
  
  namespace diagn {
    
    using boost::scoped_array;

    
    /**
     * Helper for a diagnostic BufferProvider:
     * A block of heap allocated storage, with the capability
     * to store some additional tracking information.
     */
    class Block
      : boost::noncopyable
      {
        size_t size_;
        scoped_array<char> storage_;
        
        bool was_locked_;
        
      public:
        Block()
          : size_(0)
          , storage_()
          , was_locked_(false)
          { }
        
        bool
        was_used()  const
          {
            return was_locked_;
          }
        
        bool
        was_closed()  const
          {
            return was_locked_;
          }
        
        void*
        accessMemory()  const
          {
            return storage_.get();
          }
      };
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
    , public lib::ScopedPtrVect<diagn::Block>
    {
      
      virtual uint announce (uint count, BufferDescriptor const& type);
      virtual BuffHandle lockBufferFor (BufferDescriptor const& descriptor);
      virtual void releaseBuffer (BuffHandle const& handle);
      
    public:
      TrackingHeapBlockProvider();
      virtual ~TrackingHeapBlockProvider();
      
      diagn::Block& access_or_create (uint bufferID);
      
    private:
      bool withinStorageSize (uint bufferID)  const;
    };
  
  
  
  
  
} // namespace engine
#endif
