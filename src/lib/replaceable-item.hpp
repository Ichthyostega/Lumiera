/*
  REPLACEABLE-ITEM.hpp  -  adapter to take snapshot from non-assignable values

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file replaceable-item.hpp
 ** Adapter wrapper to treat non-assignable values as if they were assignable.
 ** Prerequisite is for the type to be copy constructible (maybe even just move constructible).
 ** The typical usage is when we want to take a _snapshot_ from some value, but do not actually
 ** need the ability to assign something to a specific object instance. In such a situation,
 ** we may just destroy a previous snapshot and then place a new copy initialised value into
 ** the same storage space. For all really assignable types we fall back to a trivial
 ** implementation.
 ** 
 ** \par motivation
 ** The typical usage is in library or framework code, where we do not know what types to expect.
 ** For example, the Lumiera command framework automatically captures an _UNDO memento_ based
 ** on the return type of a state capturing function. It would be highly surprising for the
 ** user if it wasn't possible to capture e.g. time values or durations as _old state_,
 ** because these entities can not be re-assigned with new values, only created.
 ** But obviously a command handling framework needs the ability to capture
 ** state consecutively several times, and this adapter was created
 ** to bridge this conceptual discrepancy
 ** 
 ** \par extensions
 **  - the type can be solely move constructible, in which case the payload needs to be
 **    moved explicitly when embedding into this adapter   ///////////////////////////////////////////////TICKET 1059 : does not work yet on GCC-4.9
 **  - when the payload is equality comparable, the container is as well, by delegation
 **  - container instances can be default created, which emplaces an _empty value_.
 **    The actual value to embed is retrieved from the lib::NullValue template,
 **    which is a static per-type singleton.
 **  - thus types which are not even default constructible can still be used,
 **    by providing an explicit specialisation of lib::NullValue for this type.
 ** 
 ** @see ReplaceableIterm_test
 ** @see steam::control::MementoTie
 ** 
 */


#ifndef LIB_REPLACEABLE_ITEM_H
#define LIB_REPLACEABLE_ITEM_H

#include "lib/error.hpp"
#include "lib/null-value.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <type_traits>
#include <utility>



namespace lib {
namespace wrapper {
  
  using util::isSameObject;
  using util::unConst;
  using std::forward;
  
  
  
  
  /**
   * Adapter container to take snapshots from non-assignable values.
   * Implemented by placing the subject into an inline buffer.
   * @note uses lib::NullValue to retrieve an _empty payload_.
   */
  template<typename X, typename COND =void>
  class ReplaceableItem
    {
      char content_[sizeof(X)];
      
    public:
      ReplaceableItem()
        {
          emplace (NullValue<X>::get());
        }
     ~ReplaceableItem()
        {
          destroy();
        }
      
      template<typename Z>
      ReplaceableItem (Z&& otherValue)
        {
          emplace (forward<Z> (otherValue));
        }
      
      template<typename Z>
      ReplaceableItem&
      operator= (Z&& otherValue)
        {
          if (not isSameObject(*this, otherValue))
            reAssign (forward<Z> (otherValue));
          return *this;
        }
      
      ReplaceableItem&
      clear()  noexcept
        {
          reAssign (NullValue<X>::get());
          return *this;
        }
      
      operator X&()             { return access();                }
      operator X const&() const { return unConst(this)->access(); }
      
      X&       get()            { return access();                }
      X const& get()      const { return unConst(this)->access(); }
      
      
    private:
      X&
      access()
        {
          return reinterpret_cast<X&>(content_);
        }
      
      void
      destroy()
        {
          access().~X();
        }
      
      template<typename Z>
      void
      emplace (Z&& otherValue)
        try {
          new(&content_) X{forward<Z> (otherValue)};
        } 
        catch(...) {
          new(&content_) X{NullValue<X>::get()};
        }
      
      template<typename Z>
      void
      reAssign (Z&& otherValue)
        {
          destroy();
          emplace (forward<Z> (otherValue));
        }
    };
  
  
  
  template<typename X>
  struct is_assignable_value
    : std::__and_<std::is_copy_assignable<X>, std::__not_<std::is_reference<X>>>
    { };
  
  /**
   * simple delegating implementation
   * to use for regular cases
   */
  template<typename X>
  class ReplaceableItem<X,    meta::enable_if<is_assignable_value<X>>>
    {
      X val_;
      
    public:
      ReplaceableItem() : val_(NullValue<X>::get())         { }
      
      ReplaceableItem(X const& val) : val_(val)             { }
      ReplaceableItem(X &&     val) : val_(forward<X>(val)) { }
      
      ReplaceableItem& operator= (X const& val) { val_=val;                 return *this; }
      ReplaceableItem& operator= (X &&     val) { val_=forward<X>(val);     return *this; }
      ReplaceableItem& clear()  noexcept        { val_=NullValue<X>::get(); return *this; }
      
      operator X&()             { return val_; }
      operator X const&() const { return val_; }
      
      X&       get()       { return val_; }
      X const& get() const { return val_; }
    };
  
  
  /** disallow embedding references */
  template<typename X>
  class ReplaceableItem<X,    meta::enable_if<std::is_reference<X>>>
    {
      static_assert( not sizeof(X), "ReplaceableItem for references is pointless");
    };
  
  
  /* ===== Equality comparison delegated to contained element ===== */
  
  template<typename X>
  inline bool
  operator== (ReplaceableItem<X> const& li, ReplaceableItem<X> const& ri)
  {
    return li.get() == ri.get();
  }
  template<typename X>
  inline bool
  operator!= (ReplaceableItem<X> const& li, ReplaceableItem<X> const& ri)
  {
    return li.get() != ri.get();
  }
  
  template<typename X, typename Z>
  inline bool
  operator== (ReplaceableItem<X> const& item, Z const& something)
  {
    return item.get() == something;
  }
  template<typename X, typename Z>
  inline bool
  operator!= (ReplaceableItem<X> const& item, Z const& something)
  {
    return item.get() != something;
  }
  
  template<typename Z, typename X>
  inline bool
  operator== (Z const& something, ReplaceableItem<X> const& item)
  {
    return item.get() == something;
  }
  template<typename Z, typename X>
  inline bool
  operator!= (Z const& something, ReplaceableItem<X> const& item)
  {
    return item.get() != something;
  }
  
  
  
}} // namespace lib::wrap
#endif /*LIB_REPLACEABLE_ITEM_H*/
