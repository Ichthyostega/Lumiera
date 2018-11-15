/*
  MAYBE.hpp  -  dealing with optional values

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file maybe.hpp
 ** Support for representation of optional values.
 ** This implements a concept ("option monad") known from functional programming,
 ** allowing to express the fact of some value possibly be unavailable. Using this
 ** approach allows to avoid the dangerous technique of (ab)using NULL pointers to
 ** represent missing values. 
 ** 
 ** While a NULL pointer carries this special meaning just by convention, marking a
 ** parameter or return value as optional states this fact first class, and enforces
 ** the necessary "is available" check through the type system. Surprisingly, this
 ** leads not only to more secure, but also much more compact code, as we're now
 ** able to substitute a fallback just by a "or else use this" clause.
 ** Basically, there are different ways to access the actual value
 ** - access through implicit conversion raises an exception for missing values
 ** - evaluation as boolean allows to check, if the value is available
 ** - an alternative or fallback value may be attached.
 ** 
 ** @todo WIP and rather brainstorming as of 2/10
 ** @deprecated as of 2016 : the upcoming C++17 will provide an optional type!!
 ** 
 ** @see vault::ThreadJob usage example
 */



#ifndef LIB_MAYBE_H
#define LIB_MAYBE_H

#include "lib/error.hpp"
//#include "lib/wrapper.hpp"
#include "lib/util.hpp"

#include <string>



namespace lib {
  
  using util::isnil;
  using std::string;
  namespace error = lumiera::error;
  
  
  namespace maybe {
    
  }
  
  
  /**
   * A value, which might be unavailable
   * @throw error::State on any attempt to access a missing value
   *        without prior checking the availability
   */
  template<typename VAL>
  class Maybe
    {
      VAL value_;
      
    public:
      /** mark an invalid/failed result */
      Maybe ()
        { }
      
      /** standard case: valid result */
      Maybe (VAL const& value)
       : value_(value)
       { }
      
      
      
      bool
      isValid()  const
        {
          UNIMPLEMENTED ("check if optional value is available");
        }
      
      void
      maybeThrow(Literal explanation =0)  const
        {
          if (!isValid())
            throw error::State (explanation.empty()? "optional value not available" : string(explanation),
                                error::LUMIERA_ERROR_BOTTOM_VALUE);
        }
      
      
      VAL
      get()  const
        {
          maybeThrow();
          return value_;
        }
    };
  
  
  
  
  
} // namespace lib
#endif
