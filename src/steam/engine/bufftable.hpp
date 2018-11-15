/*
  BUFFTABLE.hpp  -  Table of buffer pointers to be used by the render nodes

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file bufftable.hpp
 ** helper for organisation of render data buffers
 ** Used during the process of _"pulling"_ a render node, which recursively might
 ** pull further nodes. Any node has a _render calculation function,_ which in turn
 ** needs input and output buffers
 ** @see buffer-provider.hpp
 */


#ifndef ENGINE_BUFFHTABLE_H
#define ENGINE_BUFFHTABLE_H


#include "lib/error.hpp"
#include "steam/engine/buffhandle.hpp"
#include "steam/engine/procnode.hpp"
#include "lib/iter-adapter.hpp"

#include <vector>
#include <utility>



namespace steam {
namespace engine {
  
  using std::pair;
  using std::vector;
  
  
    /** 
     * Tables of buffer handles and corresponding dereferenced buffer pointers.
     * Used within the invocation of a processing node to calculate data.
     * The tables are further differentiated into input data buffers and output
     * data buffers. The tables are supposed to be implemented as bare "C" arrays,
     * thus the array of real buffer pointers can be fed directly to the
     * processing function of the respective node.
     * 
     * @todo this whole design is a first attempt and rather clumsy. It should be reworked
     *       to use a single contiguous memory area and just layer the object structure on top
     *       (by using placement new). Yet the idea of an stack-like organisation should be retained
     */
  struct BuffTable
    {
      typedef BuffHandle        * PHa;
      typedef BuffHandle::PBuff * PBu;
      
      struct StorageChunk
        { };
      
      template<uint count>
      struct Storage
        {
          enum{size = count * sizeof(StorageChunk)};
        };
      
      class Builder
        {
        public:
          Builder& announce (uint count, BufferDescriptor const& type);
          BuffTable& build();
        };
      
      static Builder& prepare (const size_t STORAGE_SIZE, void* storage);
      
      void lockBuffers();
      void releaseBuffers();
      
      typedef vector<BuffHandle> BuffHandleTable;
      typedef lib::RangeIter<BuffHandleTable::iterator> iterator;
      
      iterator buffers();
      iterator inBuffers();
      iterator outBuffers();
    };
  
  
  
  
  /* === Implementation === */
  
  inline BuffTable::Builder&
  BuffTable::prepare (const size_t STORAGE_SIZE, void* storage)
  {
    UNIMPLEMENTED ("expose a builder object for outfitting a buffer pointer table");
  }
  
  
  inline BuffTable::Builder&
  BuffTable::Builder::announce (uint count, BufferDescriptor const& type)
  {
    UNIMPLEMENTED ("accept announcement of additional buffer table entries required");
  }
  
  
  inline BuffTable&
  BuffTable::Builder::build()
  {
    UNIMPLEMENTED ("finally drop off the newly configured buffer pointer table");
  }
  
  
  inline void
  BuffTable::lockBuffers()
  {
    UNIMPLEMENTED ("convenience shortcut: lock all preconfigured buffers within this table through the underlying buffer provider");
  }
  
  
  inline void
  BuffTable::releaseBuffers()
  {
    UNIMPLEMENTED ("convenience shortcut: release all the buffers managed through this buffer table, by forwarding to the underlying buffer provider");
  }
  
  
  
  inline BuffTable::iterator
  BuffTable::buffers()
  {
    UNIMPLEMENTED ("expose an iterator to yield all prepared buffers within this buffer table");
  }
  
  
  inline BuffTable::iterator
  BuffTable::inBuffers()
  {
    UNIMPLEMENTED ("expose an iterator to access all the input buffer slots of this buffer table");
  }
  
  
  inline BuffTable::iterator
  BuffTable::outBuffers()
  {
    UNIMPLEMENTED ("expose an iterator to access all the output buffer slots of this buffer table");
  }



  
  
  
}} // namespace steam::engine
#endif
