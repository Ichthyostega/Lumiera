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

#include <boost/noncopyable.hpp>


namespace engine {
  
  using lib::Literal;
  
  namespace error = lumiera::error;
  
  
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
        : createAttached (0)    /////////TODO: how to attach the ctor function??? Maybe better use a class with virtual functions?
        , destroyAttached (0)
        { }
    };
  
  const TypeHandler RAW_BUFFER;
  
  
  class Metadata
    : boost::noncopyable
    {
    public:
      class Entry
        {
          HashVal parent_;
          
        protected:
          Entry (HashVal parent) : parent_(parent) { }
         ~Entry ()                                 { }
          
        public:
          HashVal parentKey()  const { return parent_;}
          
          virtual BufferState state()  const              =0;
          virtual const void* access() const              =0;
          virtual Entry& mark (BufferState newState)      =0;
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
      
      HashVal
      key (HashVal parentKey, TypeHandler instanceFunc)
        {
          UNIMPLEMENTED ("create sub-type key");
        }
      
      HashVal
      key (HashVal parentKey, LocalKey specifics)
        {
          UNIMPLEMENTED ("create sub-type key");
        }
      
      HashVal
      key (HashVal parentKey, const void* concreteBuffer)
        {
          UNIMPLEMENTED ("create sub-object key for concrete buffer");
        }
      
      Entry&
      get (HashVal key)
        {
          UNIMPLEMENTED ("access, possibly create metadata records");
        }
      
      bool
      isKnown (HashVal key)  const
        {
          UNIMPLEMENTED ("diagnostics: known record?");
        }
      
      bool
      isLocked (HashVal key)  const
        {
          UNIMPLEMENTED ("diagnostics: actually locked buffer instance record?");
        }
      
      
      /* == memory management == */
      
      Entry& markLocked (HashVal parentKey, const void* buffer);
      void release (HashVal key);
      
    };
    
    
    
    
  /* === Implementation === */
  
  namespace {
    
    using error::LUMIERA_ERROR_LIFECYCLE;
    using error::LUMIERA_ERROR_BOTTOM_VALUE;
    
    
    class TypeEntry
      : public Metadata::Entry
      {
        virtual BufferState
        state()  const
          {
            return NIL;
          }
        
        virtual const void*
        access()  const
          {
            throw error::Logic ("This metadata entry is still abstract. "
                                "It can't be associated with a concrete buffer. "
                                "You need to invoke markLocked (buffer)."
                               , LUMIERA_ERROR_LIFECYCLE
                               );
          }
        
        virtual Entry&
        mark (BufferState newState)
          {
            if (newState != NIL)
              throw error::Logic ("This metadata entry is still abstract. "
                                  "The only possible state transition is to markLocked (buffer)."
                                 , LUMIERA_ERROR_LIFECYCLE
                                 );
            return *this;
          }
        
      };
    
    
    class BufferEntry
      : public Metadata::Entry
      {
        BufferState state_;
        const void* buffer_;
        
        virtual BufferState
        state()  const
          {
            __must_not_be_NIL();
            return state_;
          }
        
        virtual const void*
        access()  const
          {
            __must_not_be_NIL();
            __must_not_be_FREE();
            
            ENSURE (buffer_);
            return buffer_;
          }
        
        virtual Entry&
        mark (BufferState newState)
          {
            switch (this->state_)
              {
              case NIL:  __must_not_be_NIL();
              case FREE: __must_not_be_FREE();
                
              case LOCKED:
                if (newState == EMITTED) break; // allow transition
                
              case EMITTED:
                if (newState == BLOCKED) break; // allow transition
                
              case BLOCKED:
                if (newState == FREE)           // note fall through for LOCKED and EMITTED too
                  {
                    buffer_ = 0;
                    break; // allow transition
                  }
              default:
                throw error::Fatal ("Invalid buffer state encountered.");
              }
            state_ = newState;
            return *this;
          }
        
        
        void
        __must_not_be_NIL()  const
          {
            if (NIL == state_)
              throw error::Fatal ("Concrete buffer entry with state==NIL encountered."
                                  "State transition logic broken (programming error)");
          }
        
        void
        __must_not_be_FREE()  const
          {
            if (FREE == state_)
                throw error::Logic ("Buffer is inaccessible (marked as free). "
                                    "Need a new buffer pointer in order to lock an entry. "
                                    "You should invoke markLocked(buffer) prior to access."
                                   , LUMIERA_ERROR_LIFECYCLE );
          }
      };
  }
  
  
  
  
  /** */
  Metadata::Entry&
  Metadata::markLocked (HashVal parentKey, const void* buffer)
  {
    UNIMPLEMENTED ("transition to locked state");
    if (!buffer)
      throw error::Fatal ("Attempt to lock for a NULL buffer. Allocation floundered?"
                         , LUMIERA_ERROR_BOTTOM_VALUE);
    
    HashVal newKey = this->key (parentKey, buffer);
    if (isLocked(newKey))
      throw error::Logic ("Attempt to lock a slot for a new buffer, "
                          "while actually the old buffer is still locked."
                         , LUMIERA_ERROR_LIFECYCLE );
    
    return this->get(newKey);
  }
  
  void
  Metadata::release (HashVal key)
  {
    UNIMPLEMENTED ("metadata memory management");
  }
  
  
  
} // namespace engine
#endif
