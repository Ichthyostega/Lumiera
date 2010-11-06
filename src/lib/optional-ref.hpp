/*
  OPTIONAL-REF.hpp  -  optional and switchable reference
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef LIB_OPTIONAL_REF_H
#define LIB_OPTIONAL_REF_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/util.hpp"

//#include <boost/type_traits/remove_const.hpp>
//#include <boost/noncopyable.hpp>
//#include <tr1/functional>


namespace lib {
  
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  /** 
   * Reference wrapper implemented as constant function,
   * returning the (fixed) reference on invocation
   */
  template<typename T>
  class ReturnRef
    {
      T& ref_;
      
    public:
      ReturnRef(T& target) : ref_(target) { }
      T& operator() ()  const { return ref_;}
    };
  
  template<typename T>
  ReturnRef<T>
  refFunction (T& target)
  {
    return ReturnRef<T> (target);
  }
  
  
  
  /** 
   * Optional or switchable link to an existing object.
   * This reference wrapper normally behaves like a reference,
   * but has the ability to be \em disabled. This disabled state
   * is managed automatically by ctor and dtor, can be detected
   * through \c bool check and -- contrary to a \c NULL pointer
   * -- produces a real exception instead of crashing.
   * @note \em not taking ownership of the pointee
   */
  template<typename T>
  class OptionalRef
    {
      T* ref_;
      
    public:
      OptionalRef() : ref_(0) { }
      OptionalRef(T& target) : ref_(&target) { }
     ~OptionalRef() { ref_ = 0; }
    };
  
  
  
} // namespace lib
#endif
