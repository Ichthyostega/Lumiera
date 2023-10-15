/*
  TYPED-COUNTER.hpp  -  maintain a set of type based contexts 

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file typed-counter.hpp
 ** Creating series of type-based contexts.
 ** The idea is to get a "slot" for any given type, so we can build
 ** tables or families of implementations based on these types. Each of
 ** those "slots" can be addressed by a distinct (compile time) type, but
 ** at the same time holds a numeric ID (runtime assigned on demand). This
 ** setup allows to bridge between metaprogramming and (runtime) dispatcher tables.
 ** 
 ** Each such series of type-id-slots is associated to a distinct usage context.
 ** Those usage contexts are discerned by the template parameter \c CX. Each of
 ** these usage contexts uses a separate numbering scheme on his own, i.e. every
 ** new type encountered at runtime gets the next higher ID number (slot).
 ** @warning the actual ID numbers depend on the sequence of first encountering
 **          a given type. If this sequence isn't reproducible between runs, then
 **          also the generated type-IDs aren't reproducible. Thus its advisable
 **          \em not to rely on any specific numeric value here, but always just
 **          access the service through the type slots.
 ** @note Locking is based on a class lock per usage context, but a lock needs
 **       only be acquired to allocate a new ID number (double checked locking).
 **       Thus lock contention is considered not to be a problem, yet we need
 **       actually to verify this by real measurements (as of 2011)
 ** @todo 2010 ... this is the first, preliminary version of a facility,
 **       which is expected to get quite important for custom allocation management.
 ** @remark in 2023 changed partially to use atomics; measurements indicate however
 **       that the impact of locking technology is negligible. In concurrent use,
 **       the wait times are dominating. In single threaded use, both using Atomics
 **       or a nonrecursive mutex yield amortised invocation times around 60ns.
 ** @see typed-counter-test.cpp
 ** @see TypedAllocationManager
 ** @see AllocationCluster (custom allocation scheme using a similar idea inline)
 **
 */



#ifndef LIB_TYPED_COUNTER_H
#define LIB_TYPED_COUNTER_H

#include "lib/error.hpp"
#include "lib/sync-classlock.hpp"

#include <atomic>
#include <string>
#include <deque>


namespace util {
  std::string showSize (size_t)   noexcept;
}

namespace lib {
  
  typedef size_t IxID;              //////////////////////TICKET #863
  
  using std::deque;
  using std::string;
  
  
  /**
   * Provide type-IDs for a specific context.
   * This facility allows to access a numeric ID for each
   * given distinct type. Type-IDs may be used e.g. for
   * dispatcher tables or for custom allocators.
   * The type-IDs generated here are not completely global though.
   * Rather, they are tied to a specific type context, e.g. a class
   * implementing a custom allocator. These typed contexts are
   * considered to be orthogonal and independent of each other.
   * @remark 2023: allocation of global ID counters are protected by
   *         double-checked locking with mutex, which is deemed adequate.
   */
  template<class CX>
  class TypedContext
    {
      static IxID lastGeneratedTypeID;
      
    public:
      static IxID
      newTypeID (IxID& typeID)
        {
          ClassLock<TypedContext> synchronised;
          if (!typeID)
            typeID = ++lastGeneratedTypeID;
          return typeID;
        }
      
      /** type-ID */
      template<typename TY>
      class ID
        {
          static IxID typeID;
          
        public:
          static IxID
          get()
            {
              if (typeID)
                return typeID;
              else
                return newTypeID(typeID);
            }
        };
    };
  
  /** storage for the type-ID generation mechanism */
  template<class CX>
  IxID TypedContext<CX>::lastGeneratedTypeID (0);
  
  /** table holding all the generated type-IDs */
  template<class CX>
  template<typename TY>
  IxID TypedContext<CX>::ID<TY>::typeID (0);
  
  
  
  
  /**
   * Utility providing a set of counters, each tied to a specific type.
   * The actual allocation of id numbers is delegated to TypedContext.
   * Such a counter is used to build [symbolic instance IDs](\ref lib::meta::generateSymbolicID)
   * with a per-type running counter. Such IDs are used for lib::idi::EntryID and for lib::diff::GenNode.
   * @warning the index space for typeIDs is application global; the more distinct types are used, the more
   *          slots will be present in _each instance of TypedCounter._ As of 2023 we are using < 30 distinct
   *          types for these use cases, and thus the wasted memory is not much of a concern.
   */
  class TypedCounter
    : public Sync<>
    {
      mutable deque<std::atomic_int64_t> counters_;
      
      template<typename TY>
      IxID
      slot()  const
        {
          IxID typeID = TypedContext<TypedCounter>::ID<TY>::get();
          if (size() < typeID)
            {       // protect against concurrent slot allocations
              Lock sync{this};
              if (size() < typeID)
                counters_.resize (typeID);
            }
          
          ENSURE (counters_.size() >= typeID);
          return (typeID - 1);
        }
      
      
    public:
      template<class X>
      int64_t
      get()  const
        {
          return counters_[slot<X>()].load(std::memory_order_relaxed);
        }
      
      template<class X>
      int64_t
      inc()
        {                                 // yields the value seen previously
          return 1 + counters_[slot<X>()].fetch_add(+1, std::memory_order_relaxed);
        }
      
      template<class X>
      int64_t
      dec()
        {
          return -1 + counters_[slot<X>()].fetch_add(-1, std::memory_order_relaxed);
        }
      
      
      /* == diagnostics == */
      
      size_t size() const { return counters_.size(); }
      bool empty()  const { return counters_.empty();}
    };
  
  
  
  /**
   * Utility to produce member IDs
   * for objects belonging to a "Family",
   * as defined by a distinguishing type.
   * Within each family, each new instance of
   * FamilyMember holds a new distinct id number.
   * @remark this builds a structure similar to TypedContext,
   *         however the second level is not assigned _per type_
   *         but rather _per instance_ of FamilyMember<TY>
   */
  template<typename TY>
  class FamilyMember
    {
      const size_t id_;
      
      /** member counter shared per template instance */
      static std::atomic_size_t memberCounter;
      
      /** threadsafe allocation of member ID */
      static size_t
      allocateNextMember()
        {                       // Note : returning previous value before increment
          return memberCounter.fetch_add(+1, std::memory_order_relaxed);
        }
      
    public:
      FamilyMember()
        : id_{allocateNextMember()}
        { }
      
      operator size_t()  const
        {
          return id_;
        }
      
      operator string()  const
        {
          return util::showSize (this->id_);
        }
      
      friend string
      operator+ (string const& prefix, FamilyMember id)
      {
        return prefix+string(id);
      }
      
      friend string
      operator+ (const char* prefix, FamilyMember id)
      {
        return string(prefix)+id;
      }
    };
  
  /** allocate storage for the counter per type family */
  template<typename TY>
  std::atomic_size_t FamilyMember<TY>::memberCounter{0};
  
  
  
} // namespace lib
#endif
