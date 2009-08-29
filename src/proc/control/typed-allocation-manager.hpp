/*
  TYPED-ALLOCATION-MANAGER.hpp  -  abstract backbone to build custom memory managers 
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 ** have to deal with families of types rather then with individual types, and typically
 ** there will be some common or combined handling for all family members.
 ** 
 ** The intention is for this TypedAllocationManager template to be used both as a base
 ** class providing the implementation skeleton for the actual custom allocation manager,
 ** and as an abstract interface, which can be forwarded to the \em implementation classes
 ** in case there is some cooperation required to get the allocation done (for example,
 ** there might be some type erasure involved, leaving the (otherwise opaque) implementation
 ** class as the only entity with a limited knowledge about the actual memory layout, and
 ** thus the only way of creating a clone properly would be to forward down into this
 ** implementation class). As we can't expect to create a one-size-fits-all implementation,
 ** we'll allow for customising the TypedAllocationManager through policy classes. 
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
 ** 
 ** @see CommandRegistry
 ** @see AllocationCluster (another custom allocation scheme, which could be united)
 **
 */



#ifndef CONTROL_TYPED_ALLOCATION_MANAGER_H
#define CONTROL_TYPED_ALLOCATION_MANAGER_H

//#include "pre.hpp"
#include "lib/error.hpp"
#include "lib/format.hpp"
#include "include/logging.h"


#include <tr1/memory>



namespace control { ////////////////////////////////////////////////////////////TODO: should go into namespace lib
  
  using std::tr1::shared_ptr;
  
  
  
  
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

      /** this is an ABC */
      virtual ~TypedAllocationManager() {}   ////////////////////////////////////TODO: seems superfluous? We don't have any extension points (and this is a good thing!)
      
      
      
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
      
#define _EXCEPTION_SAFE_INVOKE(_CTOR_)                    \
                                                           \
          Slot<XX> slot = allocateSlot<XX>();               \
          try                                                \
            {                                                 \
              return slot.build (new(slot.storage_) _CTOR_ );  \
            }                                                  \
          catch(...)                                           \
            {                                                  \
              releaseSlot<XX>(slot.storage_);                  \
              throw;                                           \
            }
      
      template< class XX>
      shared_ptr<XX>                                                                   //_____________________
      create ()                                                                       ///< invoke default ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX() )
        }
      
      
      template< class XX, typename P1>
      shared_ptr<XX>                                                                   //___________________
      create (P1& p1)                                                                 ///< invoke 1-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              >
      shared_ptr<XX>                                                                   //___________________
      create (P1& p1, P2& p2)                                                         ///< invoke 2-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              >
      shared_ptr<XX>                                                                   //___________________
      create (P1& p1, P2& p2, P3& p3)                                                 ///< invoke 3-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              , typename P4
              >
      shared_ptr<XX>                                                                   //___________________
      create (P1& p1, P2& p2, P3& p3, P4& p4)                                         ///< invoke 4-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3,p4) )
        }
      
      
      template< class XX
              , typename P1
              , typename P2
              , typename P3
              , typename P4
              , typename P5
              >
      shared_ptr<XX>                                                                   //___________________
      create (P1& p1, P2& p2, P3& p3, P4& p4, P5& p5)                                 ///< invoke 5-arg ctor
        {
          _EXCEPTION_SAFE_INVOKE ( XX (p1,p2,p3,p4,p5) )
        }
      
#undef _EXCEPTION_SAFE_INVOKE

    
    
      
    protected: /* ======= Managed Allocation Implementation ========== */
      
      template<class XX>
      Slot<XX>
      allocateSlot ()
        {
          TODO ("redirect to the corresponding pool allocator");
          void* space = new char[sizeof(XX)];
          return Slot<XX> (this, space);
        }
      
      template<class XX>
      void
      releaseSlot (void* entry)
        {
          TODO ("redirect to the corresponding pool allocator");
          typedef char Storage[sizeof(XX)];
          delete[] reinterpret_cast<Storage*> (entry);
        }
      
      
      template<class XX>
      void
      destroyElement (XX* entry)
        {
          if (!entry) return;
          try
            {
              entry->~XX();
            }
          catch(...)
            {
              WARN (command_dbg, "dtor of %s failed: %s", util::tyStr(entry).c_str()
                                                        , lumiera_error() );
            }
          releaseSlot<XX> (entry);
        }
      
    };
  
  
} // namespace control
#endif
