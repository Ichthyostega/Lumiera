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
 ** @see proc::control::MementoTie
 ** 
 */


#ifndef LIB_REPLACABLE_ITEM_H
#define LIB_REPLACABLE_ITEM_H

#include "lib/error.hpp"
#include "lib/null-value.hpp"
#include "lib/meta/util.hpp"
#include "lib/util.hpp"

#include <utility>
#include <type_traits>



namespace lib {
namespace wrapper {
  
  using util::unConst;
  using util::isSameObject;
  using std::forward;
//  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  
  
  /**
   * 
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
      
      template<typename Z>
      void
      emplace (Z&& otherValue)
        try {
          new(&content_) X{forward<Z> (otherValue)};
        } 
        catch(...) {
          NullValue<X>::build (&content_);
        }
      
      void
      destroy()
        {
          access().~X();
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
  
  
  template<typename X>
  class ReplaceableItem<X,    meta::enable_if<is_assignable_value<X>>>
    {
      X val_;
      
    public:
      ReplaceableItem() : val_(NullValue<X>::get())         { }
      
      ReplaceableItem(X const& val) : val_(val)             { }
      ReplaceableItem(X &&     val) : val_(forward<X>(val)) { }
      
      ReplaceableItem& operator= (X const& val) { val_=val;             return *this; }
      ReplaceableItem& operator= (X &&     val) { val_=forward<X>(val); return *this; }
      
      operator X&()             { return val_; }
      operator X const&() const { return val_; }
      
      X&       get()       { return val_; }
      X const& get() const { return val_; }
    };
  
  
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
#endif /*LIB_REPLACABLE_ITEM_H*/
