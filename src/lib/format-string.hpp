/*
  FORMAT-STRING.hpp  -  string template formatting based on boost::format

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


/** @file format-string.hpp
 ** Front-end for printf-style string template interpolation.
 ** While the actual implementation just delegates to boost::format, this front-end
 ** hides the direct dependency, additionally invokes a custom toSting conversion
 ** whenever possible, provides a direct conversion to string and catches any exceptions.
 ** This front-end is used pervasively for diagnostics and logging, so keeping down the
 ** compilation and object size cost and reliably handling any error is more important
 ** than the (small) performance gain of directly invoking boost::format (which is
 ** known to be 10 times slower than printf anyway).
 ** 
 ** @remarks The implementation is invoked through a set of explicit specialisations.
 **          For custom types, we prefer to invoke operator string(), which is determined
 **          by a directly coded metaprogramming test. The compile time and object size
 **          overhead incurred by using this header was verified to be negligible. 
 ** 
 ** @see FormatString_test
 ** @see format-helper
 ** 
 */


#ifndef UTIL_FORMAT_STRING_H
#define UTIL_FORMAT_STRING_H

//#include "lib/meta/trait.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <string>
//#include <cstring>
#include <typeinfo>
//#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>



namespace std { // forward declaration to avoid including <iostream>
  
  template<typename C>
  class char_traits;
  
  template<typename C, class _TRAITS>
  class basic_ostream;
  
  typedef basic_ostream<char, char_traits<char> > ostream;
}


namespace util {
  
  using boost::enable_if;
//using util::isnil;
  using std::string;
  
  
  namespace { // helpers to pick a suitable specialisation....
    
    enum{ FORMATTER_SIZE = 100 };
    
  }//(End) guards/helpers
  
  
  
  
  /** 
   * @todo write type comment
   */
  class _Fmt
    : boost::noncopyable
    {
      char formatter_[FORMATTER_SIZE];
      
      template<typename VAL, class SEL =void>
      struct Param;
      
      template<typename VAL, class SEL>
      friend struct Param;
      
      
    public:
      _Fmt (string formatString);
      
      operator string()  const;
      
      template<typename VAL>
      _Fmt&
      operator% (VAL const&);
      
      
      friend std::ostream&
      operator<< (std::ostream& os, _Fmt const&);
      
      friend bool operator== (_Fmt const&,        _Fmt const&);
      friend bool operator== (_Fmt const&,      string const&);
      friend bool operator== (_Fmt const&, const char * const);
      friend bool operator== (string const&     , _Fmt const&);
      friend bool operator== (const char * const, _Fmt const&);
      
      template<typename X>
      friend bool operator != (_Fmt const& fmt, X const& x) { return !(fmt == x); }
      template<typename X>
      friend bool operator != (X const& x, _Fmt const& fmt) { return !(x == fmt); }
    };
  
  
  
  
  
  /* == forwarding into the implementation == */
  
  template<typename VAL>
  inline _Fmt&
  _Fmt::operator% (VAL const& val)
  {
    Param<VAL>::push (val);
    return *this;
  }
  
  
  template<typename VAL, class SEL>
  struct _Fmt::Param
    {
      static void
      push (VAL const&)
        {
          UNIMPLEMENTED ("get type string");
        }
    };
  
  
  
  inline bool
  operator== (_Fmt const& left, _Fmt const& right)
  {
    return string(left) == string(right);
  }
  
  inline bool
  operator== (_Fmt const& fmt, string const& str)
  {
    return string(fmt) == str;
  }
  
  inline bool
  operator== (_Fmt const& fmt, const char * const cString)
  {
    return string(fmt) == string(cString);
  }
  
  inline bool
  operator== (string const& str, _Fmt const& fmt)
  {
    return fmt == str;
  }
  
  inline bool
  operator== (const char * const cString, _Fmt const& fmt)
  {
    return fmt == cString;
  }
  
  
  
} // namespace util

#endif /*UTIL_FORMAT_STRING_H*/
