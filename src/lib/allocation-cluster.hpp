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
          
          void*
          allot (size_t bytes, size_t alignment)
            {
              void* loc = std::align (alignment, bytes, pos, rest);
              if (loc)
                { // requested allocation indeed fits in space
                  pos = static_cast<std::byte*>(pos) + bytes;
                  rest -= bytes;
                }
              return loc;
            }
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
          
          AllocationCluster* mother_;
        };
      
      template<typename X>
      Allocator<X> getAllocator() { return this; }
      
      
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
  TY&
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
  TY&
  AllocationCluster::create (ARGS&& ...args)
  {
    if constexpr (std::is_trivial_v<TY>)
      return createDisposable<TY> (std::forward<ARGS> (args)...);
    
    using Frame = AllocationWithDestructor<TY>;
    auto& frame = createDisposable<Frame> (std::forward<ARGS> (args)...);
    registerDestructor (frame);
    return frame.payload;
  }
  
  
} // namespace lib
#endif /*LIB_ALLOCATION_CLUSTER_H*/
