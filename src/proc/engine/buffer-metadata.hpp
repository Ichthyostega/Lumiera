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
#include "lib/functor-util.hpp"

#include <tr1/functional>
#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>


namespace engine {
  
  using lib::HashVal;
  using lib::Literal;
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
  namespace error = lumiera::error;
  
  namespace metadata {
    class Key;
    class Entry;
  }
  
  
  
  enum BufferState
    { NIL,
      FREE,
      LOCKED,
      EMITTED,
      BLOCKED
    };
  
  
  /**
   * an opaque ID to be used by the BufferProvider implementation.
   * Typically this will be used, to set apart some pre-registered
   * kinds of buffers. It is treated as being part of the buffer type.
   * LocalKey objects may be copied but not re-assigned or changed.
   */
  class LocalKey
    {
      uint64_t privateID_;
      
    public:
      LocalKey (uint64_t opaqueValue=0)
        : privateID_(opaqueValue)
        { }
      
      operator uint64_t()  const { return privateID_; }
      
      friend size_t
      hash_value (LocalKey const& lkey)
      {
        boost::hash<uint64_t> hashFunction;
        return hashFunction(lkey.privateID_);
      }
      
    private:
      /** assignment usually prohibited */
      LocalKey& operator= (LocalKey const& o)
        {
          privateID_ = o.privateID_;
          return *this;
        }
      
      /** but Key assignments are acceptable */
      friend class metadata::Key;
    };
  
  
  namespace { // Helpers for construction within the buffer...
    
    template<class X>
    inline void
    buildIntoBuffer (void* storageBuffer)
    {
      new(storageBuffer) X();
    }
    
    template<class X, typename A1>
    inline void
    buildIntoBuffer_A1 (void* storageBuffer, A1 arg1)
    {
      new(storageBuffer) X(arg1);
    }
    
    template<class X>
    inline void
    destroyInBuffer (void* storageBuffer)
    {
      X* embedded = static_cast<X*> (storageBuffer);
      embedded->~X();
    }
  }//(End)placement-new helpers
  
  
  /**
   * A pair of functors to maintain a datastructure within the buffer.
   * TypeHandler describes how to outfit the buffer in a specific way.
   * When defined, the buffer will be prepared when locking and cleanup
   * will be invoked automatically when releasing. Especially, this
   * can be used to \em attach an object to the buffer (placement-new) 
   */
  struct TypeHandler
    {
      typedef function<void(void*)> DoInBuffer;
      
      DoInBuffer createAttached;
      DoInBuffer destroyAttached;
      
      /** build an invalid NIL TypeHandler */
      TypeHandler()
        : createAttached()
        , destroyAttached()
        { }
      
      /** build a TypeHandler
       *  binding to arbitrary constructor and destructor functions.
       *  On invocation, these functions get a void* to the buffer.
       * @note the functor objects created from these operations
       *       might be shared for handling multiple buffers.
       *       Be careful with any state or arguments.
       */
      template<typename CTOR, typename DTOR>
      TypeHandler(CTOR ctor, DTOR dtor)
        : createAttached (ctor)
        , destroyAttached (dtor)
        { }
      
      /** builder function defining a TypeHandler
       *  to place a default-constructed object
       *  into the buffer. */
      template<class X>
      static TypeHandler
      create ()
        {
          return TypeHandler (buildIntoBuffer<X>, destroyInBuffer<X>);
        }
      
      template<class X, typename A1>
      static TypeHandler
      create (A1 a1)
        {
          return TypeHandler ( bind (buildIntoBuffer_A1<X,A1>, _1, a1)
                             , destroyInBuffer<X>);
        }
      
      bool
      isValid()  const
        {
          return bool(createAttached)
              && bool(destroyAttached);
        }
      
      friend HashVal
      hash_value (TypeHandler const& handler)
      {
        HashVal hash(0);
        if (handler.isValid())
          {
            boost::hash_combine(hash, handler.createAttached);
            boost::hash_combine(hash, handler.destroyAttached);
          }
        return hash;
      }
      
      friend bool
      operator== (TypeHandler const& left, TypeHandler const& right)
      {
        return (!left.isValid() && !right.isValid())
            || (  util::rawComparison(left.createAttached, right.createAttached)
               && util::rawComparison(left.destroyAttached, right.destroyAttached)
               );
      }
      friend bool
      operator!= (TypeHandler const& left, TypeHandler const& right)
      {
        return !(left == right);       
      }
    };
  
  
  namespace { // internal constants to mark the default case
    
    const LocalKey UNSPECIFIC;
    const TypeHandler RAW_BUFFER;
    
    inline bool
    nontrivial (TypeHandler const& toVerify)
    {
      return RAW_BUFFER != toVerify;
    }
    
    inline bool
    nontrivial (LocalKey const& toVerify)
    {
      return UNSPECIFIC != toVerify;
    }
  }
  
  
  
  /* === Implementation === */
  
  namespace metadata {
    
    using error::LUMIERA_ERROR_LIFECYCLE;
    using error::LUMIERA_ERROR_BOTTOM_VALUE;
    
    namespace { // details of hash calculation
        template<typename VAL>
        HashVal
        chainedHash(HashVal accumulatedHash, VAL changedValue)
        {
          boost::hash_combine (accumulatedHash, changedValue);
          return accumulatedHash;
        }
    }
    
    class Key
      {
        HashVal parent_;
        HashVal hashID_;
        
      protected:
        size_t storageSize_;
        TypeHandler instanceFunc_;
        LocalKey specifics_;
        
        
      public:
        /** build a standard basic key describing a kind of Buffer. 
         * @param familyID basic hash seed value to distinguish
         *                 families of buffer types managed by
         *                 different BufferProvider instances
         * @param storageSize fundamental info: buffer size
         */
        Key (HashVal familyID, size_t storageSize)
          : parent_(familyID)
          , hashID_(chainedHash (familyID, storageSize))
          , storageSize_(storageSize)
          , instanceFunc_(RAW_BUFFER)
          , specifics_(UNSPECIFIC)
          { }
        
        // standard copy operations permitted
        
        /** create a derived buffer type description.
         *  Using a different storage size than the parent type,
         *  all else remaining the same
         */
        Key (Key const& parent, size_t differingStorageSize) 
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, differingStorageSize))
          , storageSize_(differingStorageSize)  // differing from parent
          , instanceFunc_(parent.instanceFunc_)
          , specifics_(parent.specifics_)
          { }
        
        
        /** create a derived buffer type description.
         *  Using different ctor and dtor functions,
         *  all else remaining the same as with parent
         */
        Key (Key const& parent, TypeHandler const& differingTypeHandlerFunctions) 
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, differingTypeHandlerFunctions))
          , storageSize_(parent.storageSize_)
          , instanceFunc_(differingTypeHandlerFunctions)  // differing from parent
          , specifics_(parent.specifics_)
          { }
        
        
        /** create a derived buffer type description.
         *  Using a different private ID than the parent type,
         *  all else remaining the same
         */
        Key (Key const& parent, LocalKey anotherTypeSpecificInternalID) 
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, anotherTypeSpecificInternalID))
          , storageSize_(parent.storageSize_)
          , instanceFunc_(parent.instanceFunc_)
          , specifics_(anotherTypeSpecificInternalID)  // differing from parent
          { }
        
        
        HashVal parentKey()  const { return parent_;}
        operator HashVal()   const { return hashID_;}
      };
    
    
    class Entry
      : public Key
      {
        BufferState state_;
        const void* buffer_;
        
      public:
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
  
  
  
  
  
  class Metadata
    : boost::noncopyable
    {
      Literal id_;
      HashVal family_;
      
    public:
      
      typedef metadata::Key Key;
      typedef metadata::Entry Entry;
      
      /** establish a metadata registry.
       *  Such will maintain a family of buffer type entries
       *  and provide a service for storing and retrieving metadata
       *  for concrete buffer entries associated with these types.
       * @param implementationID to distinguish families
       *        of type keys belonging to different registries.
       */
      Metadata (Literal implementationID)
        : id_(implementationID)
        , family_(hash_value(id_))
        { }
      
      /** combine the distinguishing properties
       *  into a single type key, which will be known/remembered
       *  from that point on. Properties are combined according to
       *  a fixed type specialisation order, with the buffer size
       *  forming the base level, possible TypeHandler functors the
       *  second level, and implementation defined LocalKey entries
       *  the third level. All these levels describe abstract type
       *  keys, not entries for concrete buffers. The latter are
       *  always created as children of a known type key.  
       */
      Key
      key ( size_t storageSize
          , TypeHandler instanceFunc =RAW_BUFFER
          , LocalKey specifics =UNSPECIFIC)
        {
          REQUIRE (storageSize);
          Key typeKey = trackKey (family_, storageSize);
          
          if (nontrivial(instanceFunc))
            {
              typeKey = trackKey (typeKey, instanceFunc);
            }
          
          if (nontrivial(specifics))
            {
              typeKey = trackKey (typeKey, specifics);
            }
          
          return typeKey;
        }
      
      /** create a sub-type, using a different type/handler functor */
      Key
      key (Key const& parentKey, TypeHandler const& instanceFunc)
        {
          return trackKey (parentKey, instanceFunc);
        }
      
      /** create a sub-type, using a different private-ID (implementation defined) */
      Key
      key (Key const& parentKey, LocalKey specifics)
        {
          return trackKey (parentKey, specifics);
        }
      
      Key
      key (Key const& parentKey, const void* concreteBuffer)
        {
          UNIMPLEMENTED ("create sub-object key for concrete buffer");
        }
      
      Key const&
      get (HashVal hashID)
        {
          UNIMPLEMENTED ("access the plain key entry");
        }
      
      Entry&
      get (Key key)
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
      
      Entry& markLocked (Key const& parentKey, const void* buffer);
      void release (HashVal key);
      
    private:
            
      template<typename PAR, typename DEF>
      Key
      trackKey (PAR parent, DEF specialisation)
        {
          Key newKey (parent,specialisation);
          maybeStore (newKey);
          return newKey;
        }
      
      void
      maybeStore (Key const& key)
        {
          if (isKnown (key)) return;
          UNIMPLEMENTED ("registry for type keys");
        }

    };
    
    
    
    
  
  
  
  
  /** */
  inline Metadata::Entry&
  Metadata::markLocked (Key const& parentKey, const void* buffer)
  {
    UNIMPLEMENTED ("transition to locked state");
    if (!buffer)
      throw error::Fatal ("Attempt to lock for a NULL buffer. Allocation floundered?"
                         , error::LUMIERA_ERROR_BOTTOM_VALUE);
    
    Key newKey = this->key (parentKey, buffer);
    if (isLocked(newKey))
      throw error::Logic ("Attempt to lock a slot for a new buffer, "
                          "while actually the old buffer is still locked."
                         , error::LUMIERA_ERROR_LIFECYCLE );
    
    return this->get(newKey);
  }
  
  inline void
  Metadata::release (HashVal key)
  {
    UNIMPLEMENTED ("metadata memory management");
  }
  
  
  
} // namespace engine
#endif
