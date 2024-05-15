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
#include "lib/sync-classlock.hpp"
#include "lib/scoped-holder.hpp"
#include "lib/scoped-holder-transfer.hpp"

#include <utility>
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
      
    public:
      AllocationCluster ();
     ~AllocationCluster ()  noexcept;
      
      
      template<class TY, typename...ARGS>
      TY&
      create (ARGS&& ...args)
        {
          return * new(allotMemory (sizeof(TY))) TY (std::forward<ARGS> (args)...);
        }
      
      template<typename X>
      Allocator<X>
      getAllocator()
        {
          return Allocator<X>{this};
        }
      
      
      /* === diagnostics === */
      
      size_t
      numExtents()  const
        {
          UNIMPLEMENTED ("Allocation management");
        }
      
      size_t
      numBytes()  const
        {
          UNIMPLEMENTED ("Allocation management");
        }
      
      
    private:
      /**
       * portion out the requested amount of memory,
       * possibly claiming a new pool block.
       */
      void*
      allotMemory (size_t bytes)
        {
          UNIMPLEMENTED ("actual memory management");
        }
      
      template<typename X>
      X*
      allot (size_t cnt =1)
        {
          return static_cast<X*> (allotMemory (cnt * sizeof(X)));
        }
    };
  
  
  
  
  
  //-----implementation-details------------------------
  
  
} // namespace lib
#endif /*LIB_ALLOCATION_CLUSTER_H*/
