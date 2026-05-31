/*
  BUFFER-METADATA.hpp  -  internal metadata for data buffer providers

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file buffer-metadata.hpp
 ** Metadata for managing and accessing buffers. The Lumiera Engine uses the
 ** Abstraction of an BufferProvider to handle various kinds of buffer organisation
 ** and access in a uniform way. Actually, buffers can be exposed and provided by several
 ** facilities, which might even be implemented through an external library. Thus the engine
 ** and the abstraction placed in between needs a common set of control data, to be able to
 ** expose the correct buffer for each request. Typically -- and independent of the actual
 ** implementation -- the following properties need to be tracked
 ** - that overall storage size available within the buffer
 ** - a pair of custom _creator_ and _destructor_ functions to use together with this buffer
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
 ** [Buffer Descriptor](\ref BuffDescr), embedded into each BuffHandle. While the engine
 ** mostly uses these handles in the way of a pointer, the buffer descriptor acts as a
 ** configuration tag attached to the buffer access, allowing to re-access a context
 ** within the buffer provider implementation. Notably the Render Engine employs
 ** several BufferProvider services (Memory, Cache, Output) that share the metadata
 ** table, albeit with thread-local caches. For this reason, the registered types
 ** are anchored at a root-hash, allowing to keep those several clients apart.
 ** 
 ** @todo 2/2026 After refactoring the BufferProvider and splitting up the implementation
 **       in two realms (state and storage), the purpose of this code her became much
 **       clearer. The problem remains however, that the BufferMetadata directly
 **       maintains its metadata in an embedded hashtable. This is problematic
 **       when operating within a massively concurrent environment.
 ** @todo 4/2026 After resolving the problems related to BufferProvidder and OutputSlot,
 **       the next task is to build a first draft implementation for a production-grade
 **       buffer allocator and manager. I decided to employ thread-local »satellite«
 **       tables, and a central EngineBufferMetadata hub. It remains to be seen
 **       if this design is sustainable.
 ** 
 ** @warning BufferMetadata entails an intricate setup of chained hash keys, which relies
 **       on several implicit assumptions regarding consistency. These are largely unchecked
 **       (beyond the unit test) and guaranteed only through the call sequence in proper use.
 **       - there are Key entries that represent a _buffer type_ — these must not be mutated.
 **       - the hash value for these typeKeys is totally determined by their properties
 **       - a child entry can only vary _one_ of these properties, and the change should
 **         be applied in the fixed sequence as defined in BufferMetadata::key()
 **       - any violation to this scheme might lead to *duplicate entries* -> buffer corruption
 **       - active state entries should only added as leaves, and should be removed after use.
 **       - the hash of an active entry must reflect its associated memory address.
 **       - Warning: this is only ensured by invoking Key::forEntry, but not persisted in the
 **         Key itself. A directly computed hash of such a Key differs from the explicitly
 **         associated hashID_. If the associated buffer* is changed, the entry can not be
 **         found anymore, which might lead to double allocation and memory corruption.
 ** 
 ** @see buffer-provider.hpp
 ** @see BufferMetadata_test
 ** @see BufferProviderProtocol_test
 */

#ifndef VAULT_MEM_BUFFR_METADATA_H
#define VAULT_MEM_BUFFR_METADATA_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "include/logging.h"
#include "vault/mem/buffhandle.hpp"
#include "vault/mem/type-handler.hpp"
#include "vault/mem/buffer-local-tag.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <unordered_map>
#include <utility>


namespace vault{
namespace mem {
  
  using std::move;
  using std::forward;
  using lib::HashVal;
  using util::unConst;
  
  namespace err = lumiera::error;
  
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
  
  inline bool
  isValidBufferTransition (BufferState oldState, BufferState newState)
  {
    return (  (oldState == FREE    and newState == LOCKED)
           or (oldState == LOCKED  and newState == EMITTED)
           or (oldState == LOCKED  and newState == BLOCKED)
           or (oldState == LOCKED  and newState == FREE)
           or (oldState == EMITTED and newState == BLOCKED)
           or (oldState == EMITTED and newState == FREE)
           or (oldState == BLOCKED and newState == FREE)
           );
  }
  
  
  
  
  
  namespace { // internal constants to mark the default case
    
    inline bool
    nontrivial (TypeHandler const& toVerify)
    {
      return TypeHandler::RAW != toVerify;
    }
    
    inline bool
    nontrivial (LocalTag const& toVerify)
    {
      return LocalTag::UNKNOWN != toVerify;
    }
  }
  
  
  
  /* === Metadata Implementation === */
  
  namespace metadata {
    
    namespace { // details of hash calculation
        template<typename VAL>
        HashVal
        chainedHash(HashVal accumulatedHash, VAL const& changedValue)
        {
          boost::hash_combine (accumulatedHash, changedValue);
          return accumulatedHash;
        }
    }
    
    
    /**
     * Description of a Buffer-"type".
     * Key elements will be used to generate hash IDs,
     * to be embedded into a [Buffer Descriptor](\ref BuffDescr).
     * Keys are chained hierarchically.
     */
    class Key
      {
        HashVal parent_;
        HashVal hashID_;
        
      protected:
        size_t storageSize_;
        TypeHandler instanceFunc_;
        LocalTag specifics_;
        
        
      public:
        /** Generic marker for an impossible / rejected type */
        static const Key INVALID;
        
        
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
          , instanceFunc_(TypeHandler::RAW)
          , specifics_(LocalTag::UNKNOWN)
          { }
        
        // standard copy operations permitted
        
        /** create a derived buffer type description.
         *  Using a different storage size than the parent type,
         *  all else remaining the same
         */
        Key (Key const& parent, size_t differingStorageSize)
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, differingStorageSize))
          , storageSize_(differingStorageSize)  //  ◁────────────────────────────────┨ differing from the parent
          , instanceFunc_(parent.instanceFunc_)
          , specifics_(parent.specifics_)
          { }
        
        
        /** create a derived buffer type description.
         *  Using different ctor and dtor functions,
         *  all else remaining the same as with parent
         */
        Key (Key const& parent, TypeHandler differingTypeHandlerFunctions)
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, differingTypeHandlerFunctions))
          , storageSize_(parent.storageSize_)
          , instanceFunc_(move(differingTypeHandlerFunctions)) //  ◁─────────────────┨ differing from the parent
          , specifics_(parent.specifics_)
          { }
        
        
        /** create a derived buffer type description.
         *  Using a different private ID than the parent type,
         *  all else remaining the same
         */
        Key (Key const& parent, LocalTag anotherTypeSpecificInternalTag)
          : parent_(parent.hashID_)
          , hashID_(chainedHash (parent_, anotherTypeSpecificInternalTag))
          , storageSize_(parent.storageSize_)
          , instanceFunc_(parent.instanceFunc_)
          , specifics_(anotherTypeSpecificInternalTag)   //  ◁───────────────────────┨ differing from the parent
          { }
        
        
        /** build derived Key for a concrete buffer Entry
         * @param parent type key to subsume this buffer
         * @param bufferAddr pointer to the concrete buffer
         * @return Child key with hashID based on the buffer address.
         *         For NULL buffer a copy of the parent is returned.
         */
        static Key
        forEntry (Key const& parent, const Buff* bufferAddr, LocalTag const& localTag =LocalTag::UNKNOWN)
          {
            Key newKey{parent};  // copy of parent as baseline
            if (nontrivial(localTag))
              {
                if (nontrivial(parent.specifics_)
                    and localTag != parent.specifics_)
                  throw err::Logic{"Implementation defined local key should not be overridden. "
                                   "Underlying buffer type already defines a nontrivial LocalTag"};
                newKey.parent_ = HashVal(parent);
                newKey.hashID_ = chainedHash(newKey.hashID_, localTag);
                newKey.specifics_ = localTag;
              }
            if (bufferAddr)
              {
                newKey.parent_ = HashVal(parent);
                newKey.hashID_ = chainedHash(newKey.hashID_, bufferAddr);
              }
            return newKey;
          }
        
        
        LocalTag const& localTag() const { return specifics_;}
        size_t storageSize() const { return storageSize_; }
        
        HashVal parentKey()  const { return parent_;}
        operator HashVal()   const { return hashID_;}
        
        friend bool
        operator== (Key const& k1, Key const& k2)
          {
            return k1.hashID_ == k2.hashID_
               and k1.parent_ == k2.parent_;
          }
        
        explicit
        operator bool()  const
          {
            return *this != INVALID;
          }
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
     * 
     * @remark 2/2026 the plan is to use this entry beyond metadata handling
     *         and especially to describe a buffer within a layered allocator
     */
    class Entry
      : public Key
      {
        BufferState state_;
        Buff*       buffer_;
        
      protected:
        Entry (Key const& parent
              ,Buff* bufferPtr            =nullptr
              ,size_t actualSize          =0
              ,LocalTag const& specialTag =LocalTag::UNKNOWN
              )
          : Key{Key::forEntry (parent, bufferPtr, specialTag)}
          , state_{bufferPtr? LOCKED:NIL}
          , buffer_{bufferPtr}
          {
            if (bufferPtr and actualSize)
                Key::storageSize_ = actualSize;
            ENSURE (Key::storageSize() >= parent.storageSize());
          }
        
        /// BufferMetadata is allowed to create
        friend class vault::mem::BufferMetadata;
        
        // standard copy operations permitted
        
      public:
        /** is this Entry currently associated to a
         *  concrete buffer? Is this buffer in use? */
        bool
        isLocked()  const
          {
            ASSERT (!buffer_ or (NIL != state_ and FREE != state_));
            return bool(buffer_);
          }
        
        /** is the client currently allowed to access the buffer memory?
         * @remark implies #isLocked() but is more narrow */
        bool
        isAccessible()  const
          {
            ENSURE (isLocked());
            return bool(buffer_)
               and LOCKED == state_;
          }
        
        /** is this Entry just an (abstract) placeholder for a type?
         * @return false if it's a real entry corresponding to a concrete buffer
         */
        bool
        isTypeKey()  const
          {
            return NIL == state_ and not buffer_;
          }
        
        
        BufferState
        state()  const
          {
            return state_;
          }
        
        Buff*
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
            
            if (isValidBufferTransition (state_, newState))
              { // perform transition
                if (newState == FREE)
                  invokeEmbeddedDtor_and_clear();
                if (newState == LOCKED)
                  invokeEmbeddedCtor();
                state_ = newState;
                return *this;
              }
            
            throw err::Fatal ("Invalid buffer state transition.");
          }
        
        Entry&
        lock (Buff* newBuffer, size_t actualSize =0)
          {
            __must_be_FREE();
            buffer_ = newBuffer;
            storageSize_ = std::max (actualSize, storageSize_);
            return mark (LOCKED);
          }
        
        Entry&
        invalidate (bool invokeDtor =true)
          {
            if (buffer_ and invokeDtor)
              invokeEmbeddedDtor_and_clear();
            buffer_ = nullptr;
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
            buffer_ = nullptr;
          }
        
      private:
        void
        __must_not_be_NIL()  const
          {
            if (NIL == state_)
              throw err::Fatal ("Buffer metadata entry with state==NIL encountered."
                                "State transition logic broken (programming error)"
                               , LERR_(LIFECYCLE));
          }
        
        void
        __must_not_be_FREE()  const
          {
            if (FREE == state_)
                throw err::Logic ("Buffer is inaccessible (marked as free). "
                                  "Need a new buffer pointer in order to lock an entry. "
                                  "You should invoke markLocked(buffer) prior to access."
                                 , LERR_(LIFECYCLE));
          }
        
        void
        __must_be_FREE()  const
          {
            if (FREE != state_)
                throw err::Logic ("Buffer already in use"
                                 , LERR_(LIFECYCLE));
            REQUIRE (!buffer_, "Buffer marked as free, "
                               "but buffer pointer is set.");
          }
        
        void
        __buffer_required()  const
          {
            if (!buffer_)
                throw err::Fatal ("Need concrete buffer for any further operations");
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
        using MetadataStore = std::unordered_map<HashVal,Entry>;
        
        MetadataStore entries_;

      public:
       ~Table() { verify_all_buffers_freed(); }
        
        size_t size() const { return entries_.size(); }
        
        
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
            REQUIRE (!fetch (newEntry), "duplicate buffer metadata entry");
            MetadataStore::iterator pos = entries_.emplace (HashVal(newEntry), newEntry)
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
              for (auto const& entry : entries_)
                verify_is_free(entry);
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
   * A metadata registry defines and maintains several families
   * of "buffer types"; beyond the buffer storage size, the concrete
   * meaning of those types is tied to the corresponding BufferProvider
   * implementation and remains opaque. These types are represented as
   * hierarchically linked hash keys. The implementation may bind a
   * TypeHandler to a specific type, allowing automatic invocation
   * of a "constructor" and "destructor" function on each buffer
   * of this type, when _locking_ or _freeing_ the corresponding
   * buffer. When creating or accessing a top-level (type)-Key,
   * a hash-ID of the "family" must be provided; this is a
   * root anchor and allows several clients to use a common
   * metadata table
   * @remark in the Render Engine there can be several
   *         BufferProvider services, while the BufferMetadata
   *         uses a common setup with thread-local caches.
   * @note BufferMetadata is *not threadsafe*, thus the workers
   *         rely on thread-local instances, while the central
   *         EngineBufferMetadata is protected by a monitor.
   */
  class BufferMetadata
    : util::NonCopyable
    {
      metadata::Table table_;
      
    public:
      using Key   = metadata::Key;
      using Entry = metadata::Entry;
      
      /** establish a metadata registry.
       *  Such will maintain families of chained buffer type entries
       *  and provide a service for storing and retrieving metadata
       *  for concrete buffer entries associated with these types.
       */
      BufferMetadata()
        : table_{}
        { }
      
      /** combine the distinguishing properties
       *  into a single type key, which will be known/remembered
       *  from that point on. Properties are combined according to
       *  a fixed type specialisation order, with the buffer size
       *  forming the base level, possible TypeHandler functors the
       *  second level, and implementation defined LocalTag entries
       *  the third level. All these levels describe abstract type
       *  keys, not entries for concrete buffers. The latter are
       *  always created as children of a known type key.
       * @param familyID root anchor to keep the mandators apart
       */
      Key
      key (HashVal familyID
          ,size_t storageSize
          ,TypeHandler instanceFunc =TypeHandler::RAW
          ,LocalTag specifics       =LocalTag::UNKNOWN)
        {
          REQUIRE (storageSize);
          Key typeKey = trackKey (familyID, storageSize);
          
          if (nontrivial(instanceFunc))
              typeKey = trackKey (typeKey, move(instanceFunc));
          
          if (nontrivial(specifics))
              typeKey = trackKey (typeKey, specifics);
          
          return typeKey;
        }
      
      /** create a sub-type, using a different type/handler functor */
      Key
      key (Key const& parentKey, TypeHandler instanceFunc)
        {
          return trackKey (parentKey, move(instanceFunc));
        }
      
      /** create a sub-type,
       *  using a different private-ID (implementation defined) */
      Key
      key (Key const& parentKey, LocalTag specifics)
        {
          return trackKey (parentKey, specifics);
        }
      
      /** shortcut to access the Key part of a (probably new) Entry
       *  describing a concrete buffer at the given address
       * @note might create/register a new Entry as a side-effect
       */
      Key const&
      key (Key const& parentKey, Buff* concreteBuffer, LocalTag const& specifics =LocalTag::UNKNOWN)
        {
          Key derivedKey = Key::forEntry (parentKey, concreteBuffer, specifics);
          Entry* existing = table_.fetch (derivedKey);
          
          return existing? *existing
                         : markLocked (parentKey,concreteBuffer,specifics);
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
            throw err::Invalid ("Attempt to access an unknown buffer metadata entry");
          
          return *entry;
        }
      
      Entry const&
      get (HashVal hashID)  const
        {
          return unConst(this)->get (hashID);
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
             and entry->isLocked();
        }
      
      bool
      isAccessible (HashVal key)  const
        {
          const Entry* entry = table_.fetch (key);
          return entry
             and entry->isAccessible();
        }
      
      size_t
      cntEntries()  const
        {
          return table_.size();
        }
      
      
      
      /* == memory management operations == */
      
      /** combine the type (Key) with a concrete buffer,
       *  thereby marking this buffer as locked. Store a concrete
       *  metadata Entry to account for this fact. This might include
       *  invoking a constructor function, in case the type (Key)
       *  defines a (nontrivial) TypeHandler.
       * @param parentKey designates the _buffer type_ to base on
       * @param buffer an actual memory address, which defines a distinct Entry
       * @param size the size of the actual allocation. When size ≡ 0,
       *        the nominal size is assumed, as defined through parentKey.
       *        Otherwise, the actual size must be _larger_.
       * @param specifics an optional qualification marker, which is managed
       *        and evaluated by the BufferStore implementation and treated as
       *        opaque data, as far as BufferMetadata is concerned. However,
       *        the LocalTag becomes part of the identity of the metadata record.
       *        Thus, either the parentKey must not include a LocalTag, or the
       *        LocalTag given here must be identical to what was associate
       *        with the _buffer type_ represented by parentKey.
       * @throw error::Fatal when locking a NULL buffer
       * @throw exceptions which might be raised by a TypeHandler's
       *        constructor function. In this case, the Entry remains
       *        created, but is marked as FREE
       */
      Entry&
      markLocked (Key const& parentKey
                 ,Buff* buffer
                 ,size_t actualSize  =0
                 ,LocalTag const& specifics =LocalTag::UNKNOWN)
        {
          if (not buffer)
            throw err::Fatal{"Attempt to lock for a NULL buffer. Allocation floundered?"
                            , LERR_(BOTTOM_VALUE)};
          if (not actualSize)
            actualSize = parentKey.storageSize();
          REQUIRE (parentKey.storageSize() <= actualSize);
          
          return BufferMetadata::lock (parentKey, buffer, actualSize, specifics);
        }
      
      /** variation to use when a LocalTag is given, yet size is implicitly taken from parent */
      Entry&
      markLocked (Key const& parentKey, Buff* buffer, LocalTag const& specifics)
        {
          return markLocked (parentKey, buffer, size_t(0), specifics);
        }
      
      /** purge the bare metadata Entry from the metadata tables.
       * @throw error::Logic if the entry isn't marked FREE already
       */
      void
      release (HashVal key)
        {
          Entry* entry = table_.fetch (key);
          if (!entry) return;
          
          ASSERT (entry and (key == HashVal(*entry)));
          release (*entry);
        }
      
      void
      release (Entry const& entry)
        {
          if (FREE != entry.state())
            throw err::Logic{"Attempt to release a buffer still in use"
                            , LERR_(LIFECYCLE)};
          
          table_.remove (HashVal(entry));
        }
      
      
      /** Synchronise metadata, including the key path up to root
       * @param key start point of data exchange
       * @param srcReg another BufferMetadata registry from which
       *        metadata shall be retrieved and imported
       */
      void
      import (HashVal key, BufferMetadata const& srcReg)
        {
          if (not srcReg.isKnown (key))
            return;
          Entry const& entry{srcReg.get(key)};
          if (doSynchronise (entry))
            import (entry.parentKey(), srcReg);
        }
      
      
    private:
      /** Core low-level operation to access or create a buffer metadata entry.
       *  The hashID of the entry in question is built, based on the parentKey,
       *  which designates a buffer type, optionally an implementation defined
       *  LocalTag, together with the concrete buffer address and storage size.
       *  If yet unknown, a new concrete buffer metadata Entry is created and
       *  initialised immediately to LOCKED state. Otherwise just the matching
       *  Entry found in the metadata registry is fetched and locked.
       * @note  This function does the actual work to _activate_ the buffer.
       *        In case the type (Key) involves a TypeHandler (functor),
       *        its constructor function will be invoked, if actually a new
       *        entry gets created. Typically this mechanism will be used
       *        to placement-create an object into the buffer.
       * @param parentKey a key describing the _type_ of the buffer
       * @param concreteBuffer storage pointer, must not be NULL
       * @param specifics an implementation defined tag
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
           ,Buff* concreteBuffer
           ,size_t actualSize
           ,LocalTag const& specifics =LocalTag::UNKNOWN)
        {
          if (not concreteBuffer)
            throw err::Invalid{"Attempt to lock a slot for a NULL buffer"
                              , LERR_(BOTTOM_VALUE)};
          
          Entry newEntry{parentKey, concreteBuffer, actualSize, specifics};
          Entry* existing = table_.fetch (newEntry);
          
          if (existing and existing->isLocked())
            throw err::Logic{"Attempt to re-lock a buffer still in use"
                            , LERR_(LIFECYCLE)};
          
          if (not existing)
            return store_as_locked (newEntry); // store new Entry marked as locked
          else
            return existing->lock (concreteBuffer, actualSize);
        }
      
      
      template<typename PAR, typename DEF>
      Key
      trackKey (PAR parent, DEF&& specialisation)
        {
          Key newKey{parent, forward<DEF>(specialisation)};
          maybeStore (newKey);
          return newKey;
        }
      
      void
      maybeStore (Key const& key)
        {
          if (isKnown (key)) return;
          table_.store (Entry{key, nullptr});
          // stored a type marker entry with this key
          ENSURE (HashVal(key) == HashVal(get(key)));
          ENSURE (get(key).isTypeKey());
        }
      
      /** store a fully populated entry immediately starting with locked state
       * @return a persisted copy that will be used to track the state henceforth.
       * @remark the (optional) constructor function for a type embedded into the
       *         buffer is invoked when a _persistent_ entry transitions to _locked_ state;
       *         since a new buffer created with storage location is already marked as _locked,_
       *         for sake of consistency the embedded constructor must now be invoked; if this
       *         fails, the state has to be transitioned back to FREE before re-throwing.
       */
      Entry&
      store_as_locked (Entry const& metadata)
        {
          REQUIRE (metadata.isLocked());
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
      
      
      /** import data from another metadata entry,
       *  yet reject violating sanity rules.
       * @remark usually this is used to import new type-keys;
       *  an active buffer entry can only be handled if it is new,
       *  or implies a valid state transition.
       * @note if the buffer is not NULL, it must be the same address,
       *  otherwise the keys would differ, since they are generated
       *  with \ref Key::forEntry()
       * @return `true` if new data was stored, which implies
       *  that synchronisation should proceed up to the parent
       */
      bool
      doSynchronise (Entry const& otherEntry)
        {
          Entry* entry = table_.fetch (otherEntry);
          if (not entry)
              table_.store (otherEntry);
          else
          if (not otherEntry.isTypeKey()
              and otherEntry.state() != entry->state())
            {
              ASSERT (not entry->isTypeKey(), "Metadata State-Engine corrupted");
              if (   otherEntry.state() == FREE
                  or otherEntry.state() == LOCKED)
                throw err::Logic ("Metadata synchronisation can not imply allocation/deallocation.");
              else
                entry->mark(otherEntry.state());
            }
          else
            return false;
          // something to synchronise => continue up the path
          return true;
        }
    };
  
  
  
  
}} // namespace vault::mem
#endif /*VAULT_MEM_BUFFR_METADATA_H*/
