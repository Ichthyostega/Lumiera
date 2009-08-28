/*
  FORMAT.hpp  -  helpers for formatting and diagnostics
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file format.hpp
 ** Collection of small helpers and convenience shortcuts for diagnostics & formatting.
 ** 
 ** @todo we could add a facade to boost::format here, see Ticket #166
 ** 
 */


#ifndef UTIL_FORMAT_H
#define UTIL_FORMAT_H

//#include "lib/util.hpp"
#include "lib/meta/trait.hpp"
#include "include/symbol.hpp"

#include <string>
#include <cstring>
#include <typeinfo>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>



namespace util {
  
  using lumiera::typelist::can_ToString;
  using lumiera::typelist::can_lexical2string;
  using lumiera::Symbol;
  using boost::enable_if;
  using boost::disable_if;
  using std::string;
  
  
  namespace { // we need to guard the string conversion
             //  to avoid a compiler error in case the type isn't convertible....
    
    template<typename X>
    inline string
    invoke_2string (     typename enable_if< can_ToString<X>,
                    X >::type const& val)
    {
      return string(val);
    }
    
    template<typename X>
    inline string
    invoke_2string (     typename disable_if< can_ToString<X>,
                    X >::type const&)
    {
      return "";
    }
    
    
    
    template<typename X>
    inline string
    invoke_indirect2string (     typename enable_if< can_lexical2string<X>,
                            X >::type const& val)
    {
      try        { return boost::lexical_cast<string> (val); }
      catch(...) { return ""; }
    }
    
    template<typename X>
    inline string
    invoke_indirect2string (     typename disable_if< can_lexical2string<X>,
                            X >::type const&)
    {
      return "";
    }
  }
  
  
  
  /** try to get an object converted to string.
   *  An custom/standard conversion to string is used,
   *  if applicable; otherwise, some standard types can be
   *  converted by a lexical_cast (based on operator<< ).
   *  Otherwise, either the fallback string is used, or just
   *  a string denoting the (mangled) type.
   */
  template<typename TY>
  inline string
  str ( TY const& val
      , Symbol prefix=""      ///< prefix to prepend in case conversion is possible
      , Symbol fallback =0   /// < replacement text to show if string conversion fails
      )
  {
    if (can_ToString<TY>::value)
      return string(prefix) + invoke_2string<TY>(val);
    
    else
      {
        if (can_lexical2string<TY>::value)
          {
            string res (invoke_indirect2string<TY> (val));
            if ("" != res)
              return string(prefix) + res;
          }
      
        return fallback? fallback
                       : tyStr(val);
      }
  }
  
  
  /** @return a string denoting the type. */
  template<typename TY>
  inline string
  tyStr (const TY* =0)
  {
    return string("«")+typeid(TY).name()+"»";
  }
  
  template<typename TY>
  inline string
  tyStr (TY const& ref)
  { return tyStr(&ref); }
  
  
} // namespace util

#endif /*UTIL_FORMAT_H*/
