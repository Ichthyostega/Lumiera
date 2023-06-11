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
#include "lib/nocopy.hpp"

#include <utility>
#include <list>



namespace lib {
  
  
  /**
   * Placeholder implementation for a custom allocator
   * @todo shall be replaced by an AllocationCluster eventually
   * @note conforming to a prospective C++20 Concept `Allo<JobTicket>`
   * @remark using `std::list` container, since re-entrant allocation calls are possible,
   *         meaning that `emplace_front` will be called recursively from another ctor
   *         call invoked from `emplace_front`. Vector or Deque would be corrupted
   *         by such re-entrant calls, since both would alias the same entry....
   */
  template<typename T>
  class AllocatorHandle
    : public std::list<T>
    {
    public:
      template<typename...ARGS>
      T&
      operator() (ARGS&& ...args)
        {
          return this->emplace_front (std::forward<ARGS> (args)...);
        }
    };
  
  
  
} // namespace lib
#endif /*LIB_ALLOCATOR_HANDLE_H*/
