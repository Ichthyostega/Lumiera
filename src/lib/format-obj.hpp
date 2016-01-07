/*
  FORMAT-OBJ.hpp  -  simple means to display an object

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


/** @file format-obj.hpp
 ** Simple functions to represent objects, for debugging and diagnostics.
 ** The helpers provided here are rather commonplace, but written in a way
 ** as to incur a rather modest header inclusion load. It should be OK to
 ** use these even on widely used interface headers.
 ** - util::toString() performs a failsafe to-String conversion, thereby preferring a
 **         built-in conversion operator, falling back to a lexical conversion (boost)
 **         or just a unmangled and simplified type string as default.
 ** - lib::meta::demangleCxx() uses the built-in compiler support to translate a mangled
 **         type-ID (as given by `typeid(TY).name()`) into a readable, fully qualified
 **         C++ type name. This is only supported for GNU compatible compilers.
 ** 
 ** @see FormatHelper_test
 ** @see [frontend for boost::format, printf-style](format-string.hpp)
 ** 
 */


#ifndef LIB_FORMAT_OBJ_H
#define LIB_FORMAT_OBJ_H

#include "lib/meta/trait.hpp"
//#include "lib/util.hpp"

#include <string>
#include <boost/lexical_cast.hpp>


namespace std { // forward declaration to avoid including <iostream>
  
  template<typename C>
  struct char_traits;
  
  template<typename C, class _TRAITS>
  class basic_ostream;
  
  using ostream = basic_ostream<char, char_traits<char>>;
}


namespace lib  {
  class Literal;
  
namespace meta {
  
  /** reverse the effect of C++ name mangling.
   * @return string in language-level form of a C++ type or object name,
   *         or a string with the original input if demangling fails.
   * @warning implementation relies on the cross vendor C++ ABI in use
   *         by GCC and compatible compilers, so portability is limited.
   *         The implementation is accessed through libStdC++
   *         Name representation in emitted object code and type IDs is
   *         essentially an implementation detail and subject to change.
   */
  std::string demangleCxx (lib::Literal rawName);
  
  
  std::string humanReadableTypeID (lib::Literal);
  
}}// namespace lib::meta


namespace util {
  
  std::string showDouble (double) noexcept;
  std::string showFloat (float)    noexcept;
  std::string showAddr (void const* addr) noexcept;
  
  /** preconfigured format for pretty-printing of addresses */
  std::ostream& showAddr (std::ostream&, void const* addr);
  
  
  
} // namespace util
#endif /*LIB_FORMAT_OBJ_H*/
