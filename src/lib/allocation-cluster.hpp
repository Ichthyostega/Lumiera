/*
  ALLOCATION-CLUSTER.hpp  -  allocating and owning a pile of objects

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file allocation-cluster.hpp
 ** Memory management for the low-level model (render nodes network).
 ** The model is organised into temporal segments, which are considered
 ** to be structurally constant and uniform. The objects within each
 ** segment are strongly interconnected, and thus each segment is created
 ** within a single build process and is replaced or released as a whole.
 ** AllocationCluster implements memory management to support this usage
 ** pattern. Optionally it is even possible to skip invocation of object
 ** destructors, making de-allocation highly efficient (typically the
 ** memory pages are already cache-cold when about to discarded).
 ** \par base allocation
 ** The actual allocation of storage extents uses heap memory expanded in blocks
 ** of AllocationCluster::EXTENT_SIZ. While the idea is to perform allocations
 ** mostly at start and then hold and use the memory, the allocation is never
 ** actually _closed_ — implying that further allocations can be added during
 ** the whole life time, which may possibly even trigger a further base allocation
 ** if storage space in the last Extent is exhausted. In theory, it would be possible
 ** to use a custom allocation (in the AllocationCluster::StorageManager::Extents,
 ** which is a lib::LinkedElements and could be parametrised with a allocator template).
 ** Allocations are never discarded, and thus any alloted memory will be kept until
 ** the whole AllocationCluster is destroyed as a compound.
 ** \par using as STL allocator
 ** AllocationCluster::Allocator is an adapter to expose the interface
 ** expected by std::allocator_traits (and thus usable by all standard compliant
 ** containers). With this usage, the container _manages_ the contained objects,
 ** including the invocation of their destructors, while relying on the allocator
 ** to allot and discard bare memory. However, to avoid invoking any destructors,
 ** the container itself can be created with AllocationCluster::createDisposable.
 ** This causes the container (front-end) to be emplaced itself into the used
 ** AllocationCluster — and since the container's destructor will not be invoked
 ** in this arrangement, the container will not be able to invoke element dtors.
 ** \par dynamic adjustments
 ** Under controlled conditions, it is possible to change the size of the latest
 ** raw allocation handed out, within the limits of the available reserve in the
 ** current memory extent. Obviously, this is a dangerous low-level feature, yet
 ** offers some flexibility for containers and allocation schemes built on top.
 ** @warning deliberately *not threadsafe*.
 ** @remark confine usage to a single thread or use thread-local clusters.
 ** @see allocation-cluster-test.cpp
 ** @see builder::ToolFactory
 ** @see linked-elements.hpp
 */


#ifndef LIB_ALLOCATION_CLUSTER_H
#define LIB_ALLOCATION_CLUSTER_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"

#include <type_traits>
#include <utility>
#include <memory>



namespace lib {
  namespace test { class AllocationCluster_test; } // declared friend for low-level-checks
  namespace err = lumiera::error;
  
  
  /**
   * A pile of objects sharing common allocation and lifecycle.
   * AllocationCluster owns a heterogeneous collection of objects of various types.
   * Typically, allocation happens during a short time span when building a new segment,
   * and objects are used together until the segment is discarded. The primary leverage
   * is to bulk-allocate memory, and to avoid invoking destructors (and thus to access
   * a lot of _cache-cold memory pages_ on clean-up). A Stdlib compliant #Allocator
   * is provided for use with STL containers. The actual allocation uses heap memory
   * in _extents_ of hard-wired size, maintained by the accompanying StorageManager.
   * @warning use #createDisposable whenever possible, but be sure to understand
   *          the ramifications of _not invoking_ an object's destructor.
   */
  class AllocationCluster
    : util::MoveOnly
    {
      class StorageManager;
      
      /** maintaining the Allocation */
      struct Storage
        {
          void*  pos{nullptr};
          size_t rest{0};
          
          auto bytePos() { return static_cast<std::byte*> (pos); }
          
          void*
          allot (size_t bytes, size_t alignment)
            {
              void* loc = std::align (alignment, bytes, pos, rest);
              if (loc)
                { // requested allocation indeed fits in space
                  pos = bytePos() + bytes;
                  rest -= bytes;
                }
              return loc;
            }
          
          void adjustPos (int offset);
          bool hasReserve (int offset)  const;
          bool matches_last_allocation (void const* loc, size_t siz) const;
        };
      Storage storage_;
      
      
    public:
      AllocationCluster ();
     ~AllocationCluster ()  noexcept;
      
      /** hard wired size of storage extents */
      static size_t constexpr EXTENT_SIZ = 256;
      static size_t constexpr max_size();
      
      
      /* === diagnostics === */
      size_t numExtents() const;
      size_t numBytes()   const;
      
      
      template<class TY, typename...ARGS>
      TY& create (ARGS&& ...);
      
      template<class TY, typename...ARGS>
      TY& createDisposable (ARGS&& ...);
      
      
      template<typename X>
      struct Allocator
        {
          using value_type = X;
          
          [[nodiscard]] X* allocate (size_t n)   { return mother_->allot<X>(n); }
          void deallocate (X*, size_t)  noexcept { /* rejoice */ }
          
          Allocator(AllocationCluster* m) : mother_{m} { }
          // standard copy acceptable
          template<typename T>
          Allocator(Allocator<T> const& o) : mother_{o.mother_} { }
          
          template<typename T>
          bool operator== (Allocator<T> const& o) const { return mother_ == o.mother_; }
          
          AllocationCluster* mother_;
        };
      
      template<typename X>
      Allocator<X> getAllocator() { return this; }
      
      
      bool canAdjust (void* loc, size_t oldSiz, size_t newSiz)  const;
      void doAdjust  (void* loc, size_t oldSiz, size_t newSiz);
      
    private:
      /**
       * portion out the requested amount of memory,
       * possibly claiming a new pool block.
       */
      void*
      allotMemory (size_t bytes, size_t alignment)
        {
          __enforce_limits (bytes, alignment);
          void* loc = storage_.allot(bytes, alignment);
          if (loc) return loc;
          expandStorage (bytes);
          return allotMemory (bytes, alignment);
        }
      
      template<typename X>
      X*
      allot (size_t cnt =1)
        {
          return static_cast<X*> (allotMemory (cnt * sizeof(X), alignof(X)));
        }
      
      
      class Destructor
        : util::NonCopyable
        {
        public:
          virtual ~Destructor();    ///< this is an interface
          Destructor* next{nullptr};//   intrusive linked list...
        };
      
      /** @internal storage frame with the actual payload object,
       *   which can be attached to a list of destructors to invoke
       */
      template<typename X>
      struct AllocationWithDestructor
        : Destructor
        {
          X payload;
          
          template<typename...ARGS>
          AllocationWithDestructor (ARGS&& ...args)
            : payload(std::forward<ARGS> (args)...)
            { }
        };
      
      void expandStorage (size_t);
      void registerDestructor (Destructor&);
      void __enforce_limits (size_t,size_t);
      
      friend class test::AllocationCluster_test;
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
  /**
   * Maximum individual allocation size that can be handled.
   * @remark AllocationCluser expands its storage buffer in steps
   *         of fixed sized _tiles_ or _extents._ Doing so can be beneficial
   *         when clusters are frequently created and thrown away (which is the
   *         intended usage pattern). However, using such extents is inherently
   *         wasteful, and thus the size must be rather tightly limited.
   */
  size_t constexpr
  AllocationCluster::max_size()
  {
    size_t ADMIN_OVERHEAD = 2 * sizeof(void*);
    return EXTENT_SIZ - ADMIN_OVERHEAD;
  }

  /**
   * Factory function: place a new instance into this AllocationCluster,
   * but *without invoking its destructor* on clean-up (for performance reasons).
   */
  template<class TY, typename...ARGS>
  inline TY&
  AllocationCluster::createDisposable (ARGS&& ...args)
  {
    return * new(allot<TY>()) TY (std::forward<ARGS> (args)...);
  }

  /**
   * Factory function: place a new instance into this AllocationCluster;
   * the object will be properly destroyed when the cluster goes out of scope.
   * @note whenever possible, the #createDisposable variant should be preferred
   */
  template<class TY, typename...ARGS>
  inline TY&
  AllocationCluster::create (ARGS&& ...args)
  {
    if constexpr (std::is_trivial_v<TY>)
      return createDisposable<TY> (std::forward<ARGS> (args)...);
    
    using Frame = AllocationWithDestructor<TY>;
    auto& frame = createDisposable<Frame> (std::forward<ARGS> (args)...);
    registerDestructor (frame);
    return frame.payload;
  }
  
  
  /**
   * Adjust the size of the latest raw memory allocation dynamically.
   * @param loc an allocation provided by this AllocationCluster
   * @param oldSiz the size requested for the allocation \a loc
   * @param newSiz desired new size for this allocation
   * @remarks since AllocationCluster must be used in a single threaded environment,
   *        the invoking code can sometimes arrange to adapt the latest allocation
   *        to a dynamically changing situation, like e.g. populating a container
   *        with a previously unknown number of elements. Obviously, the overall
   *        allocation must stay within the confines of the current extent; it
   *        is thus mandatory to [check](\ref canAdjust) the ability beforehand.
   */
  inline void
  AllocationCluster::doAdjust(void* loc, size_t oldSiz, size_t newSiz)
  {
    if (not canAdjust (loc,oldSiz,newSiz))
      throw err::Invalid {"AllocationCluster: unable to perform this allocation adjustment."};
    storage_.adjustPos (int(newSiz) - int(oldSiz));
  }
  
  inline bool
  AllocationCluster::canAdjust(void* loc, size_t oldSiz, size_t newSiz)  const
  {
    int offset{int(newSiz) - int(oldSiz)};               // is properly limited iff oldSiz is correct
    return storage_.matches_last_allocation (loc, oldSiz)
       and storage_.hasReserve (offset);
  }
  
  inline void
  AllocationCluster::Storage::adjustPos (int offset)  ///< @warning be sure a negative offset is properly limited
  {
    REQUIRE (pos);
    REQUIRE (hasReserve (offset));
    pos = bytePos() + offset;
    rest -= offset;
  }
  
  inline bool
  AllocationCluster::Storage::hasReserve (int offset)  const
  {
    return offset <= int(rest);
  }
  
  inline bool
  AllocationCluster::Storage::matches_last_allocation (void const* loc, size_t siz)  const
  {
    return loc == static_cast<std::byte const*> (pos) - siz;
  }
  
  
  
  
  
  //-----Policies-to-use-AllocationCluster------------------------
  
  namespace allo { // Setup for custom allocator policies
    
    // Forward declaration: configuration policy for lib::SeveralBuilder
    template<class I, class E, template<typename> class ALO>
    struct AllocationPolicy;
    
    template<template<typename> class ALO, typename...ARGS>
    struct SetupSeveral;
    
    /**
     * Specialisation to use lib::Several with storage managed by an
     * AllocationCluster instance, which must be provided as argument.
     * \code
     * AllocationCluster clu;
     * using Data = ....
     * Several<Data> elms = makeSeveral<Data>()
     *                        .withAllocator(clu)
     *                        .fillElm(5)
     *                        .build();
     * \endcode
     */
    template<>
    struct SetupSeveral<std::void_t, lib::AllocationCluster&>
      {
        template<typename X>
        using Adapter = typename AllocationCluster::template Allocator<X>;
        
        template<class I, class E>
        struct Policy
          : AllocationPolicy<I,E,Adapter>
          {
            using Base = AllocationPolicy<I,E,Adapter>;
            using Bucket = typename Base::Bucket;
            
            /** @warning allocation size is severely limited in AllocationCluster. */
            size_t static constexpr ALLOC_LIMIT = AllocationCluster::max_size();
            
            
            Policy (AllocationCluster& clu)
              : AllocationPolicy<I,E,Adapter> (clu.getAllocator<std::byte>())
              { }
            
            bool
            canExpand (Bucket* bucket, size_t request)
              {
                if (not bucket) return false;
                size_t currSize = bucket->getAllocSize();
                long delta = long(request) - long(bucket->buffSiz);
                return this->mother_->canAdjust (bucket,currSize, currSize+delta);
              }
            
            Bucket*
            realloc (Bucket* bucket, size_t cnt, size_t spread)
              {
                size_t request = cnt*spread;
                REQUIRE (request);
                if (not canExpand (bucket,request))
                  return Base::realloc (bucket,cnt,spread);
                
                size_t currSize = bucket->getAllocSize();
                size_t delta = request - bucket->buffSiz;
                this->mother_->doAdjust (bucket, currSize, currSize+delta);
                bucket->buffSiz += delta;
                ENSURE (bucket->buffSiz == request);
                return bucket;
              }
          };
      };
    //
  }//(End)Allocator configuration
  
  
} // namespace lib
#endif /*LIB_ALLOCATION_CLUSTER_H*/
