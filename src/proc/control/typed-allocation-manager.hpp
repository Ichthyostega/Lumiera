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
 ** @see CommandRegistry
 ** @see AllocationCluster (another custom allocation scheme, which could be united)
 **
 */



#ifndef CONTROL_TYPED_ALLOCATION_MANAGER_H
#define CONTROL_TYPED_ALLOCATION_MANAGER_H

//#include "pre.hpp"
#include "lib/error.hpp"
//#include "lib/singleton.hpp"
//#include "lib/sync.hpp"
#include "lib/format.hpp"
#include "include/logging.h"
//#include "lib/bool-checkable.hpp"


//#include <tr1/memory>
//#include <iostream>
//#include <string>



namespace control {
  
//  using std::tr1::shared_ptr;
//  using std::ostream;
//  using std::string;
  
  
  
  
  /**
   * TODO type comment
   */
  class TypedAllocationManager
    {
      typedef TypedAllocationManager _TheManager;
      
    public:

      /** this is an ABC */
      virtual ~TypedAllocationManager() {}
      
      
      
      
      template<class XOX>
      struct Slot
        {
          typedef void DelFun(XOX*);
          
          void*   const storage;
          DelFun* const killer;
          
        protected:
          Slot(void* sto, DelFun* kil)
            : storage(sto)
            , killer(kil)
            { }
          
          friend class _TheManager;
        };
      
      
      /** static entry point to be used by
       *  handles and smart pointers to trigger
       *  preconfigured destruction */
      template<class XOX>
      static void
      kill (XOX* entry)
        {
                                  ///////////////////////////////////////////////TODO: clean behaviour while in App shutdown (Ticket #196)
          instance().destroyElement(entry);
        }
      ///////////////////TODO solve the problem how to re-access the instance from within the deletor
      
      
      
    protected:
      template<class XX>
      Slot<XX>
      allocateSlot ()
        {
          UNIMPLEMENTED ("redirect to the corresponding pool allocator");
          void* space = 0;
          return Slot<XX> (space, &kill<XX>);
        }
      
      template<class XX>
      void
      releaseSlot (void* entry)
        {
          UNIMPLEMENTED ("redirect to the corresponding pool allocator");
        }
      
      
      template<class XX>
      void
      destroyElement (XX* entry)
        {
          if (!entry) return;
          try
            {
              entry->~IMP();
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
