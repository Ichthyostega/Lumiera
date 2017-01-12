/*
  NULL-VALUE.hpp  -  maintain per-type NIL values in static memory 

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


/** @file null-value.hpp
 ** Singleton-style holder for \em NIL or \em default values.
 ** This template allows to maintain a "Null Object" value, with type-based access.
 ** It is implemented as a cluster of Meyer's Singletons, thus the creation happens
 ** on demand, and the destruction happens "sometime" during application shutdown.
 ** Contrary to the generic lumiera Singleton holder, this implementation is
 ** lightweight and without any further prerequisites and validity checks.
 ** @warning we can't make any assumptions regarding the exact time when the dtor
 **          is called, and it is even impossible to detect if this happened already.
 **          Any access after that point will use a defunct object, thus the user
 **          needs to assure this facility is *never used during application shutdown*
 ** 
 ** # purpose of NIL objects
 ** Employing the NIL object pattern instead of NULL pointers typically leads to
 ** greatly simplified and more robust code. Usually the only problem is these NIL
 ** marker objects need to exist somewhere. In case no factory is used for object
 ** creation, this NullValue holder might satisfy this need.
 ** 
 ** NIL objects are assumed to be default constructible for now. Initialisation
 ** may introduce a race, which is considered acceptable here, as these objects
 ** are assumed to be simple, constant and value-like.
 ** 
 ** @note deliberately this template is an extension point for explicit specialisation
 ** @todo initialisation could be extended to use a free function to fabricate
 **       the NIL value object, so to support the special case of an NIL object
 **       not being default constructible
 ** 
 ** @see null-value-test.cpp
 ** @see lib::advice::Request usage example
 **
 */



#ifndef LIB_NULL_VALUE_H
#define LIB_NULL_VALUE_H



namespace lib {
  
  /** 
   * Singleton holder for NIL or default value objects.
   * Implemented as a cluster of Meyer's singletons, maintaining
   * a single value per type. As an extension point for specialisation,
   * a function to emplace a "default" object is also provided.
   */
  template<class TY>
  struct NullValue
    {
      static TY const&
      get()
        {
          static TY nilValue;
          return nilValue;
        }
      
      static TY&
      build (void* storage)
        {
          return *new(storage) TY{};
        }
    };
  
  
  
} // namespace lib
#endif
