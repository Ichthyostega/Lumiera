/*
  FORMAT-COUT.hpp  -  use custom string conversions in stream output

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file format-cout.hpp
 ** Automatically use custom string conversion in C++ stream output.
 ** This diagnostics facility allows just to dump any object into `cout` or `cerr`.
 ** Pointers will be detected, checked for NULL and printed as address, followed
 ** by the representation of the pointee. When the displayed entity defines an
 ** `operator string()`, this custom string conversion will be used (suppressing
 ** any exceptions, of course). As fallback, a simplified type string is printed.
 **
 ** \par policy
 ** What shall be expected from a generic toString conversion?
 ** It should be _minimal_, it should be _transparent_ and it should
 ** always work and deliver a string, irrespective of the circumstances.
 ** By extension, this means that we do not want to differentiate much
 ** between values, references and pointers, which also means, we do
 ** not want to indicate pointers explicitly (just signal NULL, when
 ** encountered). The situation is slightly different for the `ostream`
 ** inserter; in a modern GUI application, there isn't much use for
 ** STDOUT and STDERR, beyond error messages and unit testing.
 ** Thus, we can strive at building a more convenient flavour
 ** here, which does indeed even show the address of pointers.
 ** 
 ** @see FormatCOUT_test
 ** @see FormatHelper_test
 ** @see [generic string conversion helper](\ref util::toString() )
 ** @see [frontend for boost::format, printf-style](\ref format-string.hpp)
 ** 
 */


#ifndef LIB_FORMAT_COUT_H
#define LIB_FORMAT_COUT_H

#include "lib/format-obj.hpp"

#include <string>
#include <iostream>

// make those generally visible
using std::cout;
using std::cerr;
using std::endl;




namespace std {
  
  namespace { // toggle for the following generic overloads of operator<<
    
    template<typename X>
    using enable_StringConversion = lib::meta::enable_if< lib::meta::use_StringConversion4Stream<X>>;
  }
  
  
  /** generic overload to use custom string conversions in output */
  template<typename _CharT, typename _Traits, typename X, typename =    enable_StringConversion<X>>
  inline basic_ostream<_CharT, _Traits>&
  operator<< (basic_ostream<_CharT, _Traits>& os, X const& obj)
  {
    return os << util::StringConv<X>::invoke (obj);
  }
  
  
  /** generic overload to pretty-print any pointer in output
   * @note possibly also invokes custom string conversion,
   *       in case the pointee defines one
   */
  template<typename _CharT, typename _Traits, typename X, typename =    enable_StringConversion<X>>
  inline basic_ostream<_CharT, _Traits>&
  operator<< (basic_ostream<_CharT, _Traits>& os, X const* ptr)
  {
    if (ptr)
      return util::showAddr(os, ptr) << " ↗" << util::StringConv<X>::invoke (*ptr);
    else
      return os << "⟂ «" << lib::meta::typeStr<X>() << "»";
  }
  
  
  
} // namespace std
#endif /*LIB_FORMAT_COUT_H*/
