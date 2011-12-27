/*
  FORMAT-UTIL.hpp  -  helpers for formatting and diagnostics

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


/** @file format.hpp
 ** Collection of small helpers and convenience shortcuts for diagnostics & formatting.
 ** - util::str() performs a failsafe to-String conversion, thereby preferring a
 **         built-in conversion operator, falling back to just a mangled type string.
 ** - util::tyStr() generates a string corresponding to the type of the given object.
 **         Currently just implemented through the mangled RTTI type string  
 ** 
 ** @see FormatHelper_test
 ** @see format-string.hpp frontend for boost::format, printf-style
 ** 
 */


#ifndef LIB_FORMAT_UTIL_H
#define LIB_FORMAT_UTIL_H

#include "lib/meta/trait.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>
#include <cstring>
#include <typeinfo>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>



namespace util {
  
  using boost::enable_if;
  using lib::meta::can_ToString;
  using lib::meta::can_lexical2string;
  using lib::Symbol;
  using util::isnil;
  using std::string;
  
  
  namespace { // we need to guard the string conversion
             //  to avoid a compiler error in case the type isn't convertible....
    
    
    template<typename X>
    struct use_StringConversion : can_ToString<X> { };
    
    template<typename X>
    struct use_LexicalConversion
      {
        enum { value = can_lexical2string<X>::value
                  &&  !can_ToString<X>::value
             };
      };
    
    
    /** helper: reliably get some string representation for type X */
    template<typename X, typename COND =void>
    struct _InvokeFailsafe
      {
        static string toString (X const&) { return ""; }
      };
    
    template<typename X>
    struct _InvokeFailsafe<X,     typename enable_if< use_StringConversion<X> >::type>
      {
        static string
        toString (X const& val)
          try        { return string(val); }
          catch(...) { return ""; }
      };
    
    template<typename X>
    struct _InvokeFailsafe<X,     typename enable_if< use_LexicalConversion<X> >::type>
      {
        static string
        toString (X const& val)
          try        { return boost::lexical_cast<string> (val); }
          catch(...) { return ""; }
      };
  }//(End) guards/helpers
  
  
  
  
  /** try to get an object converted to string.
   *  A custom/standard conversion to string is used,
   *  if applicable; otherwise, some standard types can be
   *  converted by a lexical_cast (based on operator<< ).
   *  Otherwise, either the fallback string is used, or just
   *  a string based on the (mangled) type.
   */
  template<typename TY>
  inline string
  str ( TY const& val
      , Symbol prefix=""      ///< prefix to prepend in case conversion is possible
      , Symbol fallback =0   /// < replacement text to show if string conversion fails
      )
  {
    string res = _InvokeFailsafe<TY>::toString(val);
    if (!isnil (res))
      return string(prefix) + res;
    else
      return fallback? string(fallback)
                     : tyStr(val);
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
