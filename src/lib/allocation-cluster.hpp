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
 ** segment are strongly interconnected, and thus each segment is 
 ** being built in a single build process and is replaced or released
 ** as a whole. AllocationCluster implements memory management to
 ** support this usage pattern.
 ** 
 ** @warning in rework 5/2024 — with the goal to simplify the logic,
 **          remove all thread safety and make the implementation
 **          usable as standard conformant allocator for STL.
 **
 ** @see allocation-cluster-test.cpp
 ** @see builder::ToolFactory
 ** @see frameid.hpp
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
   * AllocationCluster owns a number of object families of various types.
   * Each of those contains a initially undetermined (but rather large)
   * number of individual objects, which can be expected to be allocated
   * within a short timespan and which are to be released cleanly on
   * destruction of the AllocationCluster. We provide a service creating
   * individual objects with arbitrary ctor parameters.
   * @warning make sure the objects dtors aren't called and object references
   *          aren't used after shutting down a given AllocationCluster.
   * @todo    implement a facility to control the oder in which
   *          the object families are to be discarded. Currently
   *          they are just purged in reverse order defined by
   *          the first request for allocating a certain type.
   * @todo    should we use an per-instance lock? We can't avoid
   *          the class-wide lock, unless also the type-ID registration
   *          is done on a per-instance base. AllocationCluster is intended
   *          to be used within the builder, which executes in a dedicated
   *          thread. Thus I doubt lock contention could be a problem and
   *          we can avoid using a mutex per instance. Re-evaluate this!
   * @todo    currently all AllocationCluster instances share the same type-IDs.
   *          When used within different usage contexts this leads to some slots
   *          remaining empty, because not every situation uses any type encountered.
   *          wouldn't it be desirable to have multiple distinct contexts, each with
   *          its own set of Type-IDs and maybe also separate locking?
   *          Is this issue worth the hassle?            //////////////////////////////TICKET #169
   */
  class AllocationCluster
    : util::MoveOnly
    {
      
      template<typename X>
      struct Allocator
        {
          using value_type = X;
          
          [[nodiscard]] X* allocate (size_t n)   { return mother_->allot<X>(n); }
          void deallocate (X*, size_t)  noexcept { /* rejoice */ }
          
          Allocator(AllocationCluster* m) : mother_{m} { }
        private:
          AllocationCluster* mother_;
        };
      
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
      
      
      template<class TY, typename...ARGS>
      TY& create (ARGS&& ...);
      
      template<class TY, typename...ARGS>
      TY& createDisposable (ARGS&& ...);
      
      template<typename X>
      Allocator<X>
      getAllocator()
        {
          return Allocator<X>{this};
        }
      
      
      /* === diagnostics === */
      
      size_t numExtents() const;
      size_t numBytes()   const;
      
      
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
      
      typedef void (*DtorInvoker) (void*);
      
      template<typename X>
      void* allotWithDeleter();
      
      void expandStorage (size_t);
      void registerDestructor (void*);
      void discardLastDestructor();
      bool _is_within_limits (size_t,size_t);
      
      friend class test::AllocationCluster_test;
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
  template<class TY, typename...ARGS>
  TY&
  AllocationCluster::createDisposable (ARGS&& ...args)
  {
    return * new(allot<TY>()) TY (std::forward<ARGS> (args)...);
  }
  
  template<class TY, typename...ARGS>
  TY&
  AllocationCluster::create (ARGS&& ...args)
  {
    if constexpr (std::is_trivial_v<TY>)
      return createDisposable<TY> (std::forward<ARGS> (args)...);
    
    void* storage = allotWithDeleter<TY>();
    try {
        return * new(storage) TY (std::forward<ARGS> (args)...);
      }
    catch(...)
      {
        discardLastDestructor();
        throw;
      }
  }
  
  /**
   * Establish a storage arrangement with a callback to invoke the destructor.
   * @remark the purpose of AllocationCluster is to avoid deallocation of individual objects;
   *         thus the position and type of allocated payload objects is discarded. However,
   *         sometimes it is desirable to ensure invocation of object destructors; in this case,
   *         a linked list of destructor callbacks is hooked up in the storage extent. These
   *         callback records are always allocated directly before the actual payload object,
   *         and use a special per-type trampoline function to invoke the destructor, passing
   *         a properly adjusted self-pointer.
   */
  template<typename X>
  void*
  AllocationCluster::allotWithDeleter()
  {
    /**
     * Memory layout frame to place a payload object
     * and store a destructor callback as intrusive linked list.
     * @note this object is never constructed, but it is used to
     *       reinterpret the StorageManager::Destructor record,
     *       causing invocation of the destructor of the payload object,
     *       which is always placed immediately behind.
     */
    struct TypedDtorInvoker
      {
        void* next;
        DtorInvoker dtor;
        X payload;
        
        /** trampoline function: invoke the destructor of the payload type */
        static void
        invokePayloadDtor (void* self)
          {
            REQUIRE (self);
            TypedDtorInvoker* instance = static_cast<TypedDtorInvoker*> (self);
            instance->payload.~X();
          }
      };
    
    TypedDtorInvoker* allocation = allot<TypedDtorInvoker>();
    allocation->dtor = &TypedDtorInvoker::invokePayloadDtor;
    registerDestructor (allocation);
    return & allocation->payload;
  }
  
  
} // namespace lib
#endif /*LIB_ALLOCATION_CLUSTER_H*/
