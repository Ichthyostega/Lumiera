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
 ** @note this file is organised in a way which doesn't bind the
 ** client code to the memory manager implementation. Parts of the
 ** interface depending on the usage situation are implemented using
 ** templates, and thus need to be in the header. This way they can
 ** exploit the type information available in call context. This
 ** information is passed to generic implementation functions
 ** defined in allocation-cluster.cpp . In a similar vein, the 
 ** AllocationCluster::MemoryManger is just forward declared.
 **
 ** @see allocation-cluster-test.cpp
 ** @see builder::ToolFactory
 ** @see frameid.hpp
 */


#ifndef LIB_ALLOCATION_CLUSTER_H
#define LIB_ALLOCATION_CLUSTER_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/sync-classlock.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/scoped-holder-transfer.hpp"

#include <vector>



namespace lib {
  
  
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
    : util::NonCopyable
    {
      
    public:
      AllocationCluster ();
      ~AllocationCluster ()  throw();
      
      
      template<class TY>
      TY&
      create ()
        {
          TY* obj = new(allocation<TY>()) TY();
          return commit(obj);
        }
      
      template<class TY, typename P0>
      TY&
      create (P0& p0)
        {
          TY* obj = new(allocation<TY>()) TY (p0);
          return commit(obj);
        }
      
      template<class TY, typename P0, typename P1>
      TY&
      create (P0& p0, P1& p1)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1);
          return commit(obj);
        }
      
      template<class TY, typename P0, typename P1, typename P2>
      TY&
      create (P0& p0, P1& p1, P2& p2)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1,p2);
          return commit(obj);
        }
      
      template<class TY, typename P0, typename P1, typename P2, typename P3>
      TY&
      create (P0& p0, P1& p1, P2& p2, P3& p3)
        {
          TY* obj = new(allocation<TY>()) TY (p0,p1,p2,p3);
          return commit(obj);
        }
      
      
      /* === diagnostics === */
      
      size_t size()  const;
      
      template<class TY>
      size_t count() const;
      
      
    private:
      /** initiate an allocation for the given type */
      template<class TY>
      void*
      allocation();
      
      /** finish the allocation after the ctor is successful */
      template<class TY>
      TY&
      commit (TY* obj);
      
      
      /**
       * The type-specific configuration information
       * any low-level memory manager needs to know
       */
      struct TypeInfo;
      
      /**
       * low-level memory manager responsible for
       * the allocations of one specific type.
       */
      class MemoryManager;
      
      /**
       * organising the association Type -> table entry
       */
      template<class TY>
      struct TypeSlot;
      
      static size_t maxTypeIDs;
      
      
      typedef ScopedPtrHolder<MemoryManager> HMemManager;
      typedef Allocator_TransferNoncopyable<HMemManager> Allo;
      typedef std::vector<HMemManager,Allo> ManagerTable;
      
      ManagerTable typeHandlers_;  ///< table of active MemoryManager instances
      
      
      
      HMemManager&
      handler (size_t slot)
        {
          ASSERT (0<slot && slot<=typeHandlers_.size());
          return typeHandlers_[slot-1];
        }
      
      HMemManager const&
      handler (size_t slot)  const
        {
          ASSERT (0<slot && slot<=typeHandlers_.size());
          return typeHandlers_[slot-1];
        }
      
      /** implementation of the actual memory allocation
       *  is pushed down to the MemoryManager impl. */
      void* initiateAlloc (size_t& slot);
      void* initiateAlloc (TypeInfo type, size_t& slot);
      
      /** enrol the allocation after successful ctor call */
      void finishAlloc (size_t& slot, void*);
      
      /** @internal helper for diagnostics,
       *            delegating to actual memory manager */
      size_t countActiveInstances (size_t& slot)  const;
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
  struct AllocationCluster::TypeInfo
    {
      size_t allocSize;
      void (*killIt)(void*);  ///< deleter function
      
      template<class TY>
      TypeInfo(TY*)
        : allocSize(sizeof(TY)),
          killIt(&TypeSlot<TY>::kill)
        { }
      
      TypeInfo()  ///< denotes "unknown" type
        : allocSize(0),
          killIt(0)
        { }  
    };
  
  
  
  template<class TY>
  struct AllocationCluster::TypeSlot
    {
      static size_t id_; ///< table pos+1 of the memory manager in charge for type TY
      
      static size_t &
      get()
        {
          return id_; 
        }
      
      static TypeInfo
      setup()
        {
          ClassLock<AllocationCluster> guard;
          if (!id_)
            id_= ++maxTypeIDs;
          
          return TypeInfo ((TY*) 0 );
        }
      
      static void
      kill (void* obj)
        {
          TY* p = static_cast<TY*>(obj);
          ASSERT (p);
          ASSERT (INSTANCEOF (TY,p));
          p->~TY();
        }
      
    };
  
  
  /** storage for static bookkeeping of type allocation slots */
  template<class TY>
  size_t AllocationCluster::TypeSlot<TY>::id_;
  
  
  
  template<class TY>
  inline void*
  AllocationCluster::allocation()
  {
    void *mem = initiateAlloc (TypeSlot<TY>::get());
    if (!mem)
      mem = initiateAlloc (TypeSlot<TY>::setup(),TypeSlot<TY>::get());
    ENSURE (mem);
    return mem;
  }
  
  template<class TY>
  inline TY&
  AllocationCluster::commit (TY* obj)
  {
    REQUIRE (obj);
    finishAlloc (TypeSlot<TY>::get(), obj);
    return *obj;
  }
  
  
  /** helper for diagnostics
   * @return number of currently allocated
   *         object instances of the given type
   */
  template<class TY>
  size_t
  AllocationCluster::count()  const
  {
    return countActiveInstances (TypeSlot<TY>::get());
  }
  
  
  
  
  
} // namespace lib
#endif
