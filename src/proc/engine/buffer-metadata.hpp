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
 ** These three distinctions are applied in sequence, thus forming a type tree with 3 levels.
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
#include "lib/hash-value.h"
#include "lib/util-foreach.hpp"
#include "include/logging.h"
#include "proc/engine/type-handler.hpp"
#include "proc/engine/buffer-local-key.hpp"

#include <unordered_map>
#include <boost/noncopyable.hpp>


namespace proc {
namespace engine {
  
  using lib::HashVal;
  using lib::Literal;
  using util::for_each; 
  
  namespace error = lumiera::error;
  
  namespace metadata {
    class Key;
    class Entry;
  }
  class BufferMetadata;
  
  
  
  
  /**
   * Buffer states
   * usable within BufferProvider
   * and stored within the metadata
   */
  enum BufferState
    { NIL,        ///< abstract entry, not yet allocated
      FREE,       ///< allocated buffer, no longer in use
      LOCKED,     ///< allocated buffer actively in use
      EMITTED,    ///< allocated buffer, returned from client
      BLOCKED     ///< allocated buffer blocked by protocol failure
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
  
  
  
  /* === Metadata Implementation === */
  
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
    
    
    /**
     * Description of a Buffer-"type".
     * Key elements will be used to generate hash IDs,
     * to be embedded into a BufferDescriptor.
     * Keys are chained hierarchically.
     */
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
        
        
        /** build derived Key for a concrete buffer Entry
         * @param parent type key to subsume this buffer
         * @param bufferAddr pointer to the concrete buffer
         * @return Child key with hashID based on the buffer address.
         *         For NULL buffer a copy of the parent is returned.
         */
        static Key
        forEntry (Key const& parent, const void* bufferAddr, LocalKey const& implID =UNSPECIFIC)
          {
            Key newKey(parent);
            if (bufferAddr)
              {
                newKey.parent_ = HashVal(parent);
                newKey.hashID_ = chainedHash(parent, bufferAddr);
                if (nontrivial(implID))
                  {
                    REQUIRE (!newKey.specifics_.isDefined(),
                             "Implementation defined local key should not be overridden. "
                             "Underlying buffer type already defines a nontrivial LocalKey");
                    newKey.specifics_ = implID;
              }   }
            return newKey; 
          }
        
        void
        useTypeHandlerFrom (Key const& ref)
          {
            if (nontrivial(this->instanceFunc_))
              throw error::Logic ("unable to supersede an already attached TypeHandler"
                                 , LUMIERA_ERROR_LIFECYCLE);
            instanceFunc_ = ref.instanceFunc_;
          }
        
        
        LocalKey const& localKey() const { return specifics_;}
        size_t storageSize() const { return storageSize_; }
        
        HashVal parentKey()  const { return parent_;}
        operator HashVal()   const { return hashID_;}
      };
    
    
    /**
     * A complete metadata Entry, based on a Key.
     * This special Key element usually describes an actual Buffer.
     * Entries are to be managed in a hashtable, which is "the metadata table".
     * As a special case, an entry without a concrete buffer storage pointer
     * can be created. This corresponds to a (plain) key and describes just
     * a buffer type. Such type-only entries are fixed to the NIL state.
     * All other entries allow for state transitions.
     * 
     * The "metadata table" with its entries is maintained by an engine::BufferMetadata
     * instance. For the latter, Entry serves as representation and access point
     * to the individual metadata; this includes using the TypeHandler for
     * building and destroying buffer structures.
     */
    class Entry
      : public Key
      {
        BufferState state_;
        void*       buffer_;
        
      protected:
        Entry (Key const& parent, void* bufferPtr =0, LocalKey const& implID =UNSPECIFIC)
          : Key (Key::forEntry (parent, bufferPtr, implID))
          , state_(bufferPtr? LOCKED:NIL)
          , buffer_(bufferPtr)
          { }
        
        /// BufferMetadata is allowed to create 
        friend class engine::BufferMetadata;
        
        // standard copy operations permitted
        
      public:
        /** is this Entry currently associated to a
         *  concrete buffer? Is this buffer in use? */ 
        bool
        isLocked()  const
          {
            ASSERT (!buffer_ || (NIL != state_ && FREE != state_));
            return bool(buffer_);
          }
        
        /** is this Entry just an (abstract) placeholder for a type?
         * @return false if it's a real entry corresponding to a concrete buffer
         */ 
        bool
        isTypeKey()  const
          {
            return NIL == state_ && !buffer_;
          }
        
        
        BufferState
        state()  const
          {
            return state_;
          }
        
        void*
        access()
          {
            __must_not_be_NIL();
            __must_not_be_FREE();
            
            ENSURE (buffer_);
            return buffer_;
          }
        
        /** Buffer state machine */
        Entry&
        mark (BufferState newState)
          {
            __must_not_be_NIL();
            
            if ( (state_ == FREE    && newState == LOCKED)
               ||(state_ == LOCKED  && newState == EMITTED)
               ||(state_ == LOCKED  && newState == BLOCKED)
               ||(state_ == LOCKED  && newState == FREE)
               ||(state_ == EMITTED && newState == BLOCKED)
               ||(state_ == EMITTED && newState == FREE)
               ||(state_ == BLOCKED && newState == FREE))
              {
                // allowed transition
                if (newState == FREE)
                  invokeEmbeddedDtor_and_clear();
                if (newState == LOCKED)
                  invokeEmbeddedCtor();
                state_ = newState;
                return *this;
              }
            
            throw error::Fatal ("Invalid buffer state transition.");
          }
        
        Entry&
        lock (void* newBuffer)
          {
            __must_be_FREE();
            buffer_ = newBuffer;
            return mark (LOCKED);
          }
        
        Entry&
        invalidate (bool invokeDtor =true)
          {
            if (buffer_ && invokeDtor)
              invokeEmbeddedDtor_and_clear();
            buffer_ = 0;
            state_ = FREE;
            return *this;
          }
        
        
      protected:
        /** @internal maybe invoke a registered TypeHandler's
         * constructor function, which typically builds some
         * content object into the buffer by placement new. */
        void
        invokeEmbeddedCtor()
          {
            __buffer_required();
            if (nontrivial (instanceFunc_))
              instanceFunc_.createAttached (buffer_);
          }
        
        /** @internal maybe invoke a registered TypeHandler's
         * destructor function, which typically clears up some
         * content object living within the buffer */
        void
        invokeEmbeddedDtor_and_clear()
          {
            __buffer_required();
            if (nontrivial (instanceFunc_))
              instanceFunc_.destroyAttached (buffer_);
            buffer_ = 0;
          }
        
      private:
        void
        __must_not_be_NIL()  const
          {
            if (NIL == state_)
              throw error::Fatal ("Buffer metadata entry with state==NIL encountered."
                                  "State transition logic broken (programming error)"
                                 , LUMIERA_ERROR_LIFECYCLE);
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
        
        void
        __must_be_FREE()  const
          {
            if (FREE != state_)
                throw error::Logic ("Buffer already in use"
                                   , LUMIERA_ERROR_LIFECYCLE );
            REQUIRE (!buffer_, "Buffer marked as free, "
                               "but buffer pointer is set.");
          }
        
        void
        __buffer_required()  const
          {
            if (!buffer_)
                throw error::Fatal ("Need concrete buffer for any further operations");
          }
      };
    
    
      
    /**
     * (Hash)Table to store and manage buffer metadata.
     * Buffer metadata entries are comprised of a Key part and an extended
     * Entry, holding the actual management and housekeeping metadata. The
     * Keys are organised hierarchically and denote the "kind" of buffer.
     * The hash values for lookup are based on the key part, chained with
     * the actual memory location of the concrete buffer corresponding
     * to the metadata entry to be retrieved.
     */
    class Table
      {
        typedef std::unordered_map<HashVal,Entry> MetadataStore;
        
        MetadataStore entries_;

      public:
       ~Table() { verify_all_buffers_freed(); }
        
        /** fetch metadata record, if any
         * @param hashID for the Key part of the metadata entry
         * @return pointer to the entry in the table or NULL
         */
        Entry*
        fetch (HashVal hashID)
          {
            MetadataStore::iterator pos = entries_.find (hashID);
            if (pos != entries_.end())
              return &(pos->second);
            else
              return NULL;
          }
        
        const Entry*
        fetch (HashVal hashID)  const
          {
            MetadataStore::const_iterator pos = entries_.find (hashID);
            if (pos != entries_.end())
              return &(pos->second);
            else
              return NULL;
          }
        
        /** store a copy of the given new metadata entry.
         *  The hash key for lookup is retrieved from the given Entry, by conversion to HashVal.
         *  Consequently, this will be the hashID of the parent Key (type), when the entry holds
         *  a NULL buffer (i.e a "pseudo entry"). Otherwise, it will be this parent Key hash,
         *  extended by hashing the actual buffer address.
         * @return reference to relevant entry for this Key. This might be a copy
         *         of the new entry, or an already existing entry with the same Key
         */
        Entry&
        store (Entry const& newEntry)
          {
            using std::make_pair;
            REQUIRE (!fetch (newEntry), "duplicate buffer metadata entry");
            MetadataStore::iterator pos = entries_.insert (make_pair (HashVal(newEntry), newEntry))
                                                  .first;
            
            ENSURE (pos != entries_.end());
            return pos->second;
          }
        
        void
        remove (HashVal hashID)
          {
            uint cnt = entries_.erase (hashID);
            ENSURE (cnt, "entry to remove didn't exist");
          }
        
      private:
        void
        verify_all_buffers_freed()
          try
            {
              for_each (entries_, verify_is_free);
            }
          ERROR_LOG_AND_IGNORE (engine,"Shutdown of BufferProvider metadata store")
          
        static void
        verify_is_free (std::pair<HashVal, Entry> const& e)
          {
            WARN_IF (e.second.isLocked(), engine,
                     "Buffer still in use while shutting down BufferProvider? ");
          }
      };
    
  }//namespace metadata
  
  
  
  
  
  /* ===== Buffer Metadata Frontend ===== */
  
  /**
   * Registry for managing buffer metadata.
   * This is an implementation level service,
   * used by the standard BufferProvider implementation.
   * Each metadata registry (instance) defines and maintains
   * a family of "buffer types"; beyond the buffer storage size,
   * the concrete meaning of those types is tied to the corresponding
   * BufferProvider implementation and remains opaque. These types are
   * represented as hierarchically linked hash keys. The implementation
   * may bind a TypeHandler to a specific type, allowing automatic invocation
   * of a "constructor" and "destructor" function on each buffer of this type,
   * when \em locking or \em freeing the corresponding buffer.
   */
  class BufferMetadata
    : boost::noncopyable
    {
      Literal id_;
      HashVal family_;
      
      metadata::Table table_;
                                          ///////////////////////////TICKET #854 : ensure proper locking happens "somewhere" when mutating metadata
      
    public:
      typedef metadata::Key Key;
      typedef metadata::Entry Entry;
      
      /** establish a metadata registry.
       *  Such will maintain a family of buffer type entries
       *  and provide a service for storing and retrieving metadata
       *  for concrete buffer entries associated with these types.
       * @param implementationID to distinguish families of
       *        type keys belonging to different registries.
       */
      BufferMetadata (Literal implementationID)
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
          , LocalKey specifics       =UNSPECIFIC)
        {
          REQUIRE (storageSize);
          Key typeKey = trackKey (family_, storageSize);
          
          if (nontrivial(instanceFunc))
              typeKey = trackKey (typeKey, instanceFunc);
          
          if (nontrivial(specifics))
              typeKey = trackKey (typeKey, specifics);
          
          return typeKey;
        }
      
      /** create a sub-type, using a different type/handler functor */
      Key
      key (Key const& parentKey, TypeHandler const& instanceFunc)
        {
          return trackKey (parentKey, instanceFunc);
        }
      
      /** create a sub-type,
       *  using a different private-ID (implementation defined) */
      Key
      key (Key const& parentKey, LocalKey specifics)
        {
          return trackKey (parentKey, specifics);
        }
      
      /** shortcut to access the Key part of a (probably new) Entry
       *  describing a concrete buffer at the given address
       * @note might create/register a new Entry as a side-effect 
       */ 
      Key const&
      key (Key const& parentKey, void* concreteBuffer, LocalKey const& implID =UNSPECIFIC)
        {
          Key derivedKey = Key::forEntry (parentKey, concreteBuffer);
          Entry* existing = table_.fetch (derivedKey);
          
          return existing? *existing
                         : markLocked (parentKey,concreteBuffer,implID);
        }
      
      /** core operation to access or create a concrete buffer metadata entry.
       *  The hashID of the entry in question is built, based on the parentKey,
       *  which denotes a buffer type, and the concrete buffer address. If yet
       *  unknown, a new concrete buffer metadata Entry is created and initialised
       *  to LOCKED state. Otherwise just the existing Entry is fetched.
       * @note  this function really \em activates the buffer.
       *        In case the type (Key) involves a TypeHandler (functor),
       *        its constructor function will be invoked, if actually a new
       *        entry gets created. Typically this mechanism will be used
       *        to placement-create an object into the buffer.
       * @param parentKey a key describing the \em type of the buffer
       * @param concreteBuffer storage pointer, must not be NULL
       * @param onlyNew disallow fetching an existing entry
       * @throw error::Logic when #onlyNew is set, but an equivalent entry
       *        was registered previously. This indicates a serious error
       *        in buffer lifecycle management.
       * @throw error::Invalid when invoked with NULL buffer. Use the #key
       *        functions instead to register and track type keys.
       * @return reference to the entry stored in the metadata table.
       * @warning the exposed reference might become invalid when the
       *        buffer is released or re-used later.
       */
      Entry&
      lock (Key const& parentKey
           ,void* concreteBuffer
           ,LocalKey const& implID =UNSPECIFIC
           ,bool onlyNew =false)
        {
          if (!concreteBuffer)
            throw error::Invalid ("Attempt to lock a slot for a NULL buffer"
                                 , error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          Entry newEntry(parentKey, concreteBuffer, implID);
          Entry* existing = table_.fetch (newEntry);
          
          if (existing && onlyNew)
            throw error::Logic ("Attempt to lock a slot for a new buffer, "
                                "while actually the old buffer is still locked"
                               , error::LUMIERA_ERROR_LIFECYCLE );
          if (existing && existing->isLocked())
            throw error::Logic ("Attempt to re-lock a buffer still in use"
                               , error::LUMIERA_ERROR_LIFECYCLE );
          
          if (!existing)
            return store_and_lock (newEntry); // actual creation
          else
            return existing->lock (concreteBuffer);
        }
      
      /** access the metadata record registered with the given hash key.
       *  This might be a pseudo entry in case of a Key describing a buffer type.
       *  Otherwise, the entry associated with a concrete buffer pointer is returned
       *  by reference, an can be modified (e.g. state change) 
       * @param hashID which can be calculated from the Key
       * @throw error::Invalid when there is no such entry
       * @note use #isKnown to check existence
       */
      Entry&
      get (HashVal hashID)
        {
          Entry* entry = table_.fetch (hashID);
          if (!entry)
            throw error::Invalid ("Attempt to access an unknown buffer metadata entry");
          
          return *entry;
        }
      
      bool
      isKnown (HashVal key)  const
        {
          return bool(table_.fetch (key));
        }
      
      bool
      isLocked (HashVal key)  const
        {
          const Entry* entry = table_.fetch (key);
          return entry
              && entry->isLocked();
        }
      
      
      
      /* == memory management operations == */
      
      /** combine the type (Key) with a concrete buffer,
       *  thereby marking this buffer as locked. Store a concrete
       *  metadata Entry to account for this fact. This might include
       *  invoking a constructor function, in case the type (Key)
       *  defines a (nontrivial) TypeHandler.
       * @throw error::Fatal when locking a NULL buffer
       * @throw exceptions which might be raised by a TypeHandler's
       *        constructor function. In this case, the Entry remains
       *        created, but is marked as FREE
       */
      Entry&
      markLocked (Key const& parentKey, void* buffer, LocalKey const& implID =UNSPECIFIC)
        {
          if (!buffer)
            throw error::Fatal ("Attempt to lock for a NULL buffer. Allocation floundered?"
                               , error::LUMIERA_ERROR_BOTTOM_VALUE);
          
          return this->lock(parentKey, buffer, implID, true); // force creation of a new entry
        }
      
      /** purge the bare metadata Entry from the metadata tables.
       * @throw error::Logic if the entry isn't marked FREE already
       */
      void
      release (HashVal key)
        {
          Entry* entry = table_.fetch (key);
          if (!entry) return;
          
          ASSERT (entry && (key == HashVal(*entry)));
          release (*entry);
        }
      
      void
      release (Entry const& entry)
        {
          if (FREE != entry.state())
            throw error::Logic ("Attempt to release a buffer still in use"
                               , error::LUMIERA_ERROR_LIFECYCLE);
          
          table_.remove (HashVal(entry));
        }
      
      
      
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
          table_.store (Entry (key, NULL));
        }
      
      Entry&
      store_and_lock (Entry const& metadata)
        {
          Entry& newEntry = table_.store (metadata);
          try
            {
              newEntry.invokeEmbeddedCtor();
              ENSURE (LOCKED == newEntry.state());
              ENSURE (newEntry.access());
            }
          catch(...)
            {
              newEntry.mark(FREE);
              throw;
            }
          return newEntry;
        }
    };
  
  
  
  
  
}} // namespace proc::engine
#endif
