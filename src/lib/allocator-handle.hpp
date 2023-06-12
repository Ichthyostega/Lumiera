/*
  ALLOCATOR-HANDLE.hpp  -  front-end handle for custom allocation schemes

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file allocator-handle.hpp
 ** A front-end/concept to allow access to custom memory management.
 ** Minimalistic definition scheme for a functor-like object, which can be
 ** passed to client code, offering a callback to generate new objects into
 ** some custom allocation scheme not further disclosed.
 ** 
 ** Lumiera employs various flavours of custom memory management, to handle
 ** allocation demands from performance critical parts of the application.
 ** Irrespective of the actual specifics of the allocation, typically there
 ** is some _instance_ of an allocator maintained within a carefully crafted
 ** context — leading to the necessity to dependency-inject a suitable front-end
 ** into various connected parts of the application, to allow for coherent use
 ** of allocation while avoiding tight coupling of implementation internals.
 ** 
 ** Reduced to the bare minimum, the _ability to allocate_ can be represented
 ** as a functor, which accepts arbitrary (suitable) arguments and returns a
 ** reference to a newly allocated instance of some specific type; such an
 ** _allocation front-end_ may then be passed as additional (template)
 ** parameter to associated classes or functions, allowing to generate new
 ** objects at stable memory location, which can then be wired internally.
 ** 
 ** @todo 6/2023 this specification describes a *Concept*, not an actual
 **       interface type. After the migration to C++20, it will thus be
 **       possible to mark some arbitrary custom allocator / front-end
 **       with such a concept, thereby documenting proper API usage.
 ** 
 ** @see allocation-cluster.hpp
 ** @see steam::fixture::Segment
 ** @see steam::engine::JobTicket
 */


#ifndef LIB_ALLOCATOR_HANDLE_H
#define LIB_ALLOCATOR_HANDLE_H

#include "lib/error.hpp"

#include <utility>
#include <list>



namespace lib {
  
  
  /**
   * Placeholder implementation for a custom allocator
   * @todo shall be replaced by an AllocationCluster eventually
   * @note conforming to a prospective C++20 Concept `Allo<TYPE>`
   * @remark using `std::list` container, since re-entrant allocation calls are possible,
   *         meaning that further allocations will be requested recursively from a ctor.
   *         Moreover, for the same reason we separate the allocation from the ctor call,
   *         so we can capture the address of the new allocation prior to any possible
   *         re-entrant call, and handle clean-up of allocation without requiring any
   *         additional state flags.....
   */
  template<typename TY>
  class AllocatorHandle
    {
      struct Allocation
        {
          char buf_[sizeof(TY)];
          
          template<typename...ARGS>
          TY&
          create (ARGS&& ...args)
            {
              return *new(&buf_) TY {std::forward<ARGS> (args)...};
            }
          
          TY&
          access()
            {
              return reinterpret_cast<TY&> (buf_);
            }
          void
          discard() /// @warning strong assumption made here: Payload was created 
            {
              access().~TY();
            }
        };
      
      std::list<Allocation> storage_;
      
    public:
      template<typename...ARGS>
      TY&
      operator() (ARGS&& ...args)
        {                  // EX_STRONG
          auto pos = storage_.emplace (storage_.end());  ////////////////////////////////////////////////////TICKET #230 : real implementation should care for concurrency here
          try {
              return pos->create (std::forward<ARGS> (args)...);
            }
          catch(...)
            {
              storage_.erase (pos); // EX_FREE
              
              const char* errID = lumiera_error();
              ERROR (memory, "Allocation failed with unknown exception. "
                             "Lumiera errorID=%s", errID?errID:"??");
              throw;
            }
        }
      
      /** @note need to do explicit clean-up, since a ctor-call might have been failed,
       *        and we have no simple way to record this fact internally in Allocation,
       *        short of wasting additional memory for a flag to mark this situation */
     ~AllocatorHandle()
        try {
            for (auto& alloc : storage_)
              alloc.discard();
          }
        ERROR_LOG_AND_IGNORE (memory, "clean-up of custom AllocatorHandle")
    };
  
  
  
} // namespace lib
#endif /*LIB_ALLOCATOR_HANDLE_H*/
