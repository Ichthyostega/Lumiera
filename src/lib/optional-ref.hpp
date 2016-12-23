/*
  OPTIONAL-REF.hpp  -  optional and switchable reference

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LIB_OPTIONAL_REF_H
#define LIB_OPTIONAL_REF_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"


namespace lib {
  
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  
  
  
  /** 
   * Optional or switchable link to an existing object.
   * This reference wrapper is accessed like a functor,
   * but has the ability to be \em disabled. This disabled state
   * is managed automatically by ctor and dtor, can be detected
   * through \c bool check and -- contrary to a \c NULL pointer
   * -- produces a real exception instead of crashing.
   * 
   * @note \em not taking ownership of the pointee
   * 
   * @see OptionalRef_test
   * @see lib::AutoRegistered usage example
   * @see SessionInterfaceModules::~SessionInterfaceModules()
   * 
   */
  template<typename T>
  class OptionalRef
    : public lib::BoolCheckable<OptionalRef<T>>
    {
      T* ref_;
      
      
    public:
      OptionalRef()
        : ref_(0)
        { }
      
     ~OptionalRef()
       {
         clear();
       }
      
      // using default copy operations...
      
      explicit
      OptionalRef(T& target)     ///< ...\em not allowing implicit conversion from T&
        : ref_(&target)
        { }
      
      T&
      operator() ()  const
        {
          if (!isValid())
            throw lumiera::error::Logic ("access to this object is (not/yet) enabled"
                                        , LUMIERA_ERROR_BOTTOM_VALUE);
          return *ref_;
        }
      
      
      /* === mutations ===*/
      
      void
      link_to (T& target)
        {
          ref_ = &target;
        }
      
      void
      clear()
        {
          ref_ = 0;
        }
      
      
      /* === comparison and diagnostics === */
      
      bool
      isValid()  const
        {
          return bool(ref_);
        }
      
      bool
      points_to (T const& target)  const
        {
          return isValid()
              && ref_ == &target;
        }
      
      friend bool
      operator== (OptionalRef const& r1, OptionalRef const& r2)
      {
        return r1.ref_ == r2.ref_;
      }
      friend bool
      operator!= (OptionalRef const& r1, OptionalRef const& r2)
      {
        return r1.ref_ != r2.ref_;
      }
      
      // mixed comparisons
      friend bool
      operator== (OptionalRef const& ref, T const& otherTarget)  ///< @note might throw
      {
        return ref() == otherTarget;
      }
      
      friend bool operator== (T const& otherTarget, OptionalRef const& ref) { return ref == otherTarget; }
      friend bool operator!= (T const& otherTarget, OptionalRef const& ref) { return !(ref == otherTarget); }
      friend bool operator!= (OptionalRef const& ref, T const& otherTarget) { return !(ref == otherTarget); }
    };
  
  
  template<typename T>
  OptionalRef<T>
  optionalRefTo (T& target)
  {
    return OptionalRef<T> (target);
  }
  
  
  
} // namespace lib
#endif
