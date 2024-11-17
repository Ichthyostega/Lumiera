/*
  TYPED-ALLOCATION-MANAGER.hpp  -  abstract backbone to build custom memory managers 

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file typed-allocation-manager.hpp
 ** Abstract foundation for building custom allocation managers.
 ** Currently (as of 8/09) this is a draft, factored out of the command-registry.
 ** The expectation is that we'll face several similar situations, and thus it
 ** would be good to build up a common set of operations and behaviour.
 ** @todo WIP WIP.
 **
 ** \par Concept Summary
 ** The idea is rather to tie the memory manager to a very specific usage situation,
 ** then to provide a general-purpose allocator to be used by any instance of a given
 ** type. Typically, the goal is to handle memory management for an index or registry,
 ** holding implementation objects to be shielded from the client code. Moreover, we'll
 ** have to deal with families of types rather then with individual types; usually
 ** there will be some common or combined handling for all family members.
 ** 
 ** The intention is for this TypedAllocationManager template to be used both as a base
 ** class providing the implementation skeleton for the actual custom allocation manager,
 ** and as an abstract interface, which can be forwarded to the \em implementation classes
 ** in case there is some cooperation required to get the allocation done (for example,
 ** there might be some type erasure involved, leaving the (otherwise opaque) implementation
 ** class as the only entity with a limited knowledge about the actual memory layout, and
 ** thus the only way of creating a clone properly would be to forward down into this
 ** implementation class).
 ** 
 ** Thus, TypedAllocationManager provides the classical operations of an allocator
 ** - allocate
 ** - construct
 ** - deallocate
 ** But each of these operations is to be invoked in a \em typed context. Besides,
 ** there is a facility allowing to create ref-counting handles and smart-pointers,
 ** which are internally tied to this memory manager through a deleter function.
 ** 
 ** @todo using a quick-n-dirty heap allocation implementation for now (8/09),
 **       but should write a custom allocator based on cehteh's mpool!
 ** @warning this quick-n-dirty heap allocation might produce misaligned storage!!
 ** 
 ** @see CommandRegistry
 ** @see AllocationCluster (another custom allocation scheme, which could be united)
 **
 */



#ifndef LIB_TYPED_ALLOCATION_MANAGER_H
#define LIB_TYPED_ALLOCATION_MANAGER_H

#include "lib/error.hpp"
#include "lib/meta/util.hpp"
#include "lib/typed-counter.hpp"
#include "include/logging.h"


#include <utility>
#include <memory>


namespace lib {
  
  using std::shared_ptr;
  
  
  
  
  /**
   * Foundation for a custom allocation manager,
   * tracking the created objects by smart-ptrs.
   * The public interface provides forwarding functions
   * to invoke the ctor of the objects to be created, thereby
   * placing them into the storage maintained by a low-level
   * allocator or pooled storage manager. The created smart-ptr
   * owns the new object and is wired internally to #releaseSlot.
   * Subclasses may also directly allocate and de-allocate 
   * such a (typed) storage slot.
   * 
   * @todo currently (as of 8/09) the low-level pooled allocator
   *       isn't implemented; instead we do just heap allocations.
   *       see Ticket 231
   */
  class TypedAllocationManager
    {
      typedef TypedAllocationManager _TheManager;
      
    public:
      template<class XX>
      size_t numSlots()  const;
      
      
      /* ======= managing the created objects ============= */
      
      /** opaque link to the manager, to be used by handles and
       *  smart-ptrs to trigger preconfigured destruction.  */
      template<class XOX>
      class Killer
        {
          _TheManager* manager_;
          
        public:
          void
          operator() (XOX* victim)
            {
              REQUIRE (manager_);
                                  ///////////////////////////////////////////////TODO: clean behaviour while in App shutdown (Ticket #196)
              manager_->destroyElement (victim);
            }
          
        protected:
          Killer(_TheManager* m)
            : manager_(m)
            { }
        };
      
      /** a token representing a newly opened slot
       *  capable for holding an object of type XOX .
       *  The receiver is responsible for
       *  - either calling releaseSlot
       *  - or building a smart-ptr / handle wired to
       *    the \link #getDeleter deleter function \endlink
       */
      template<class XOX>
      struct Slot
        : private Killer<XOX>
        {
          /** pointer to the allocated storage
           *  with \c sizeof(XOX) bytes */
          void* const storage_;
          
          /** build a refcounting smart-ptr,
           *  complete with back-link to the manager
           *  for de-allocation */
          shared_ptr<XOX>
          build (XOX* toTrack)
            {
              return shared_ptr<XOX> (toTrack, getDeleter());
            }
          
          Killer<XOX> const&
          getDeleter()
            {
              return *this;
            }
          
        protected:
          Slot(_TheManager* don, void* mem)
            : Killer<XOX>(don)
            , storage_(mem)
            { }
          
          friend class TypedAllocationManager;
        };
      
      
      
      
      /* ==== build objects with managed allocation ==== */
      
      template< class XX, typename...ARGS>
      shared_ptr<XX>
      create (ARGS&& ...args)
        {
          Slot<XX> slot = allocateSlot<XX>();
          try {
              return slot.build (new(slot.storage_) XX (std::forward<ARGS> (args)...) );
            }
          catch(...)
            {
              releaseSlot<XX>(slot.storage_);
              throw;
            }
        }
      
      
      
      
      
      
    protected: /* ======= Managed Allocation Implementation ========== */
      
      template<class XX>
      Slot<XX>
      allocateSlot ()
        {
          ////////////////////////////////////////////////TICKET #231 :redirect to the corresponding pool allocator
          TRACE (memory, "allocate «%s»", util::typeStr<XX>().c_str());
          void* space = new char[sizeof(XX)];
          allocCnt_.inc<XX>();
          return Slot<XX> (this, space);
        }
      
      template<class XX>
      void
      releaseSlot (void* entry)
        {
          ////////////////////////////////////////////////TICKET #231 :redirect to the corresponding pool allocator
          TRACE (memory, "release «%s»", util::typeStr<XX>().c_str());
          typedef char Storage[sizeof(XX)]; //////////////TICKET #1204 : WARNING this might produce misaligned storage when the array does not start on a "void* boundary"
          delete[] reinterpret_cast<Storage*> (entry);
          allocCnt_.dec<XX>();
        }
      
      
      template<class XX>
      void
      destroyElement (XX* entry)
        {
          if (!entry) return;
          ////////////////////////////////////////////////TODO: when actually implementing a custom allocation, please assert here that the entry is indeed managed by us
          try
            {
              entry->~XX();
            }
          catch(...)
            {
              lumiera_err errorID = lumiera_error();
              WARN (command_dbg, "dtor of «%s» failed: %s", util::typeStr(entry).c_str()
                                                          , errorID );
            }
          releaseSlot<XX> (entry);
        }
      
      template<class>
      friend class Killer;  ///< especially all Killers are entitled to desroyElement()
      
      
    private:
      lib::TypedCounter allocCnt_;
    };
  
  
  
  template<class XX>
  size_t
  TypedAllocationManager::numSlots()  const
  {
    return allocCnt_.get<XX>();
  }

} // namespace lib
#endif
