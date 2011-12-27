/*
  FormatString  -  string template formatting based on boost::format 

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

* *****************************************************/

/** @file format-string.cpp
 ** Implementation for printf-style formatting, based on boost::format.
 ** This file holds the generic implementation of our format frontend,
 ** which actually just invokes boost::format. The corresponding header
 ** format-string.hpp contains some template functions and classes,
 ** which select an appropriate wrapper to pass the calls down.
 ** Here, we define explicit specialisations for the frontend to invoke,
 ** which in turn just pass on the given argument value to the embedded
 ** boost::format object, which in turn integrates the formatted result
 ** into an embedded string stream.
 ** 
 ** To avoid exposing boost::format in the frontend header, we use an
 ** opaque buffer of appropriate size to piggyback the formatter object.
 ** @warning unfortunately this requires a hard coded buffer size constant
 **          in the front-end, which we define there manually, based on
 **          the current implementation layout found in the boost libraries.
 **          If Boost eventually changes the implementation, the assertion
 **          in our constructor will trigger.
 ** 
 ** @see FormatString_test
 ** 
 */



#include "lib/error.hpp"
#include "lib/format-string.hpp"

#include <boost/format.hpp>
#include <iostream>





namespace util {
  
  namespace { // implementation details...
    
    
  }//(End) implementation details
  
  
  /** */
  _Fmt::_Fmt (string formatString)
  {
    UNIMPLEMENTED ("create the embedded boost::format object");
  }
  
  
  /** */
  template<typename VAL>
  void
  _Fmt::pushParameter (VAL const& val)
  {
    UNIMPLEMENTED ("feed the parameter to the embedded formatter");
  }
  
  template<typename VAL>
  void
  _Fmt::pushParameter (const VAL * const pVal)
  {
    if (pVal)
      pushParameter(*pVal);
    else
      pushParameter(string("(null)"));
  }
  
  
  /* ===== explicitly supported =================== */
  
  template void _Fmt::pushParameter(string const&);
  template void _Fmt::pushParameter(int const&);
  template void _Fmt::pushParameter(uint const&);
  
  template void _Fmt::pushParameter(const string * const);
  template void _Fmt::pushParameter(const int * const);
  template void _Fmt::pushParameter(const uint * const);
  
  
  
  
  /** */
  _Fmt::operator string()  const
  {
    UNIMPLEMENTED ("forward to the embedded boost::format object");
  }
  
  
  std::ostream&
  operator<< (std::ostream& os, _Fmt const& fmt)
  {
    return os << string(fmt);
  }
  
  
  
} // namespace util
