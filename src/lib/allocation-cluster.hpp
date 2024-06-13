/*
  ALLOCATION-CLUSTER.hpp  -  allocating and owning a pile of objects

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
 ** \par using as STL allocator
 ** AllocationCluster::Allocator is an adapter to expose the interface
 ** expected by std::allocator_traits (and thus usable by all standard compliant
 ** containers). With this usage, the container _manages_ the contained objects,
 ** including the invocation of their destructors, while relying on the allocator
 ** to allot and discard bare memory. However, to avoid invoking any destructors,
 ** the container itself can be created with AllocationCluster::createDisposable.
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
          ENSURE (_is_within_limits (bytes, alignment));
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
      bool _is_within_limits (size_t,size_t);
      
      friend class test::AllocationCluster_test;
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
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
    REQUIRE (hasReserve (rest));
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
  
  
} // namespace lib
#endif /*LIB_ALLOCATION_CLUSTER_H*/
