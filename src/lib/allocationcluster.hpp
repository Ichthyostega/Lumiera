/*
  ALLOCATIONCLUSTER.hpp  -  allocating and owning a pile of objects 
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/

/** @file allocationcluster.hpp
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
 ** defined in allocationcluster.cpp . In a similar vein, the 
 ** AllocationCluster::MemoryManger is just forward declared.
 **
 ** @see allocationclustertest.cpp
 ** @see builder::ToolFactory
 ** @see frameid.hpp
 */


#ifndef LIB_ALLOCATIONCLUSTER_H
#define LIB_ALLOCATIONCLUSTER_H

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "common/multithread.hpp"
#include "common/error.hpp"
#include "common/util.hpp"



namespace lib {
  using std::string;
  using boost::scoped_ptr;


  /**
   * A pile of objects sharing common allocation and lifecycle.
   * AllocationCluster owns a number of object families of various types.
   * Each of those contains a initially undetermined (but rather large)
   * number of individual objects, which can be expected to be allocated
   * within a short timespan and which are to be released cleanly on
   * destruction of the AllocationCluster. There is a service creating
   * individual objects with arbitrary ctor parameters and it is possible
   * to control the oder in which the object families are to be discarded.
   * @warning make sure the objects dtors aren't called and object references
   *          aren't used after shutting down a given AllocationCluster.
   */
  class AllocationCluster
    : boost::noncopyable
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

      
    protected:
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
      
      
      typedef scoped_ptr<MemoryManager> PMemManager;
      typedef std::vector<PMemManager> ManagerTable;
      ManagerTable typeHandlers_;
      
      
      /** implementation of the actual memory allocation
       *  is pushed down to the MemoryManager impl. */
      void* initiateAlloc (PMemManager&);
      
      /** enrol the allocation after successful ctor call */ 
      void finishAlloc (PMemManager&, void*);
      
      /** create a new MemoryManager implementation */
      MemoryManager* setupMemoryManager (TypeInfo);
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
  struct AllocationCluster::TypeInfo
        {
          size_t allocSize;
          void (*killIt)(void*);
          
          template<class TY>
          TypeInfo(TY*)
            : allocSize(sizeof(TY)),
              killIt(&TypeSlot<TY>::kill)
            { }
        };
  
  

  template<class TY>
  struct AllocationCluster::TypeSlot
    {
      static size_t id_; ///< table pos of the memory manager in charge for type TY
      
      static PMemManager&
      get(ManagerTable& handlers)
        {
          ENSURE (id_ < handlers.size() || 1 <= handlers.size()); // 0th Element used as "undefined" marker
          
          return handlers[id_<handlers.size()? id_ : 0 ];
        }
      
      static void
      setup(ManagerTable& handlers)
        {
          lumiera::Thread::Lock<AllocationCluster> guard   SIDEEFFECT;
          if (!id_)
            id_= ++maxTypeIDs;
          if (id_ >= handlers.size())
            handlers.resize(id_);
          if (!handlers[id_])
            handlers[id_].reset (setupMemoryManager (TypeInfo((TY*)0)));
        }
      
      static void
      kill (void* obj)
        {
          ASSERT (INSTANCEOF (TY,obj));
          TY* p = static_cast<TY*>(obj);
          ASSERT (p);
          p->~TY();
        }
      
    };
  
  
  /** storage for static bookkeeping of type allocation slots */
  template<class TY>
  size_t AllocationCluster::TypeSlot<TY>::id_;
    
  size_t AllocationCluster::maxTypeIDs;
  
  

  
  template<class TY>
  void*
  AllocationCluster::allocation()
  {
    scoped_ptr<MemoryManager> typeHandler (TypeSlot<TY>::get (typeHandlers_));
    if (!typeHandler)
      TypeSlot<TY>::setup (typeHandlers_);
    return initiateAlloc (typeHandler);
  }
    
  template<class TY>
  TY&
  AllocationCluster::commit (TY* obj)
  {
    scoped_ptr<MemoryManager> typeHandler (TypeSlot<TY>::get (typeHandlers_));
    REQUIRE (typeHandler);
    REQUIRE (obj);
    finishAlloc (typeHandler, obj);
    return *obj;
  }
  
  
  
  
} // namespace lib
#endif
