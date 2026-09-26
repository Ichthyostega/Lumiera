/*
  TYPE-REGISTRY.hpp  -  generic registry for type or policy descriptors

   Copyright (C)
     2026             Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file type-registry.hpp
 ** Generic registration table and access mechanism to type descriptors keyed by ID.
 ** The implementation of parameters is based on layered virtual interfaces and implementations,
 ** packaged into a fixed storage buffer that is transported with value semantics. This overall shape
 ** implies a strict storage size regime and thus forces to _fold away_ the more flexible parts of
 ** the behaviour variations into a second layer, retaining only an ID in the core implementation.
 ** 
 ** The type registry for parameters provides the generic mechanism underlying this setup scheme:
 ** - it is parametrised by an implementation type (of the descriptor) with a fixed size
 ** - the storage is organised by a fixed-sized index table and heap allocated storage extents.
 ** - entries are never discarded, and thus an atomic lock suffices _for read access_
 ** @remark while this scheme is defined in a generic way, it aims at one specific
 **   usage pattern and should not be stretched beyond that.
 ** 
 ** @todo WIP-WIP-WIP can be considered an initial draft and placeholder, as of 2026
 */


#ifndef LIB_PAR_TYPE_REGISTRY_H
#define LIB_PAR_TYPE_REGISTRY_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "lib/meta/trait.hpp"
//#include "lib/util.hpp"

//#include <concepts>
//#include <limits>


namespace lib {
namespace par {
  
  /**
   * Management of descriptor entries keyed by numeric ID.
   * Based on a fixed index table and extents added dynamically.
   * Slots can be claimed, creating an entry with stable address.
   * Creation and access are thread safe, Entries are never discarded.
   */
  template<class ELM>
  class TypeRegistry
    : util::NonCopyable
    {
    public:
      class ID;
      
      template<typename...INIT>
      ID makeSlot (INIT&&...);
      
      ELM& operator[] (ID)  const;
    };
  
  
    /** type tagged numeric registry entry ID */
  template<class ELM>
  class TypeRegistry<ELM>::ID
    {
      uint idx_;
      
    public:
      ID (uint ix =0) : idx_{ix} { }
      operator uint() const { return idx_; }
      // default copy operations
    };
  
  
  
  template<class ELM>
  template<typename...INIT>
  inline TypeRegistry<ELM>::ID
  TypeRegistry<ELM>::makeSlot (INIT&&... initArg)
  {
    UNIMPLEMENTED ("allocate new slot, possibly grow extents, under mutex lock");
  }
  
  template<class ELM>
  inline ELM&
  TypeRegistry<ELM>::operator[] (ID slot)  const
  {
    UNIMPLEMENTED ("access slot by ID, protected by atomic acquire");
  }
  
  
  
}} // namespace lib::par
#endif /*LIB_PAR_TYPE_REGISTRY_H*/
