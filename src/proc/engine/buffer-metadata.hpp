/*
  BUFFER-METADATA.hpp  -  internal metadata for data buffer providers

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

/** @file buffer-metadate.hpp
 ** Metadata for managing and accessing buffers. The Lumiera Engine uses the
 ** Abstraction of an BufferProvider to handle various kinds of buffer organisation
 ** and access in a uniform way. Actually, buffers can be exposed and provided by several
 ** facilities, which might even be implemented through an external library. Thus the engine
 ** and the abstraction placed in between needs a common set of control data, to be able to
 ** expose the correct buffer for each request. Typically -- and independent of the actual
 ** implementation -- the following properties need to be tracked
 ** - that overall storage size available within the buffer
 ** - a pair of custom \em creator and \em destructor functions to use together with this buffer
 ** - an additional client key to distinguish otherwise otherwise identical client requests
 ** These three distinctions are applied in sequence, thus forming a tree with 3 levels.
 ** Only the first distinguishing level (the size) is mandatory. The others are provided,
 ** because some of the foreseeable buffer providers allow to re-access the data placed
 ** into the buffer, by assigning an internally managed ID to the buffer. The most
 ** prominent example is the frame cache, which obviously needs to keep track of
 ** the buffers after the render engine is finished, while the engine code
 ** just accesses yet another buffer to place the results of calculations.
 ** 
 ** These additional distinctions and properties are associated with the help of the
 ** BufferDescriptor, embedded into each BuffHandle. While the engine just uses these
 ** handles in the way of a pointer, the buffer descriptor acts as an additional tag
 ** attached to the buffer access, allowing to re-access a context within the
 ** buffer provider implementation.
 ** 
 ** @see buffer-provider.hpp
 ** @see BufferMetadata_test
 ** @see BufferProviderProtocol_test
 */

#ifndef PROC_ENGINE_BUFFR_METADATA_H
#define PROC_ENGINE_BUFFR_METADATA_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"

//#include <boost/noncopyable.hpp>


namespace engine {
  
  using lib::Literal;
  
  
  typedef uint64_t LocalKey;
  typedef size_t HashVal;
  
  enum BufferState
    { NIL,
      FREE,
      LOCKED,
      EMITTED,
      BLOCKED
    };
  
  const LocalKey UNSPECIFIC = 0;
  
  struct TypeHandler
    {
      typedef void (*Ctor) (void*);
      typedef void (*Dtor) (void*);
      
      Ctor createAttached;
      Dtor destroyAttached;
      
      TypeHandler()
        : createAttached (0)
        , destroyAttached (0)
        { }
      
      template<class X>
      TypeHandler()
        : createAttached (0)    /////////TODO: how to attach the ctor function??? mabye better use a class with virtual functions?
        , destroyAttached (0)
        { }
    };
  
  const TypeHandler RAW_BUFFER;
  
  
  class Metadata
    {
    public:
      struct Entry
        {
          BufferState state ()  const;
          Entry& mark (BufferState newState);
        };
      
      
      Metadata (Literal implementationID)
        { }
      
      HashVal
      key ( size_t storageSize
          , TypeHandler instanceFunc =RAW_BUFFER
          , LocalKey specifics =UNSPECIFIC)
        {
          UNIMPLEMENTED ("combine the distinguishing properties into a single hash");
        }
      
      Entry&
      get (HashVal key)
        {
          UNIMPLEMENTED ("access, possibly create metadata records");
        }
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #834
    };
    
    
    
    
  /* === Implementation === */
  
  /** */
  BufferState
  Metadata::Entry::state ()  const
    {
      UNIMPLEMENTED ("buffer state accounting");
    }
  
  Metadata::Entry&
  Metadata::Entry::mark (BufferState newState)
    {
      UNIMPLEMENTED ("buffer state transitions");
      return *this;
    }
  
  
  
  
} // namespace engine
#endif
