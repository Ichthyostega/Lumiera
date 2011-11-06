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
#include "lib/scoped-ptrvect.hpp"

#include "proc/engine/tracking-heap-block-provider.hpp"

#include <boost/noncopyable.hpp>
//#include <vector>

using lib::ScopedPtrVect;



namespace engine {
  
  
  
    
    
  namespace { // Details of allocation and accounting
    
    const uint MAX_BUFFERS = 50;
  
  } // (END) Details of allocation and accounting
  
  
  
  /**
   * @internal create a memory tracking BufferProvider,
   */
  TrackingHeapBlockProvider::TrackingHeapBlockProvider()
    : BufferProvider ("Diagnostic_HeapAllocated")
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
  TrackingHeapBlockProvider::lockBufferFor (BufferDescriptor const& descriptor)
  {
    UNIMPLEMENTED ("lock buffer for exclusive use");
  }
  
  
  void
  TrackingHeapBlockProvider::releaseBuffer (BuffHandle const& handle)
  {
    UNIMPLEMENTED ("release a buffer and invalidate the handle");
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
