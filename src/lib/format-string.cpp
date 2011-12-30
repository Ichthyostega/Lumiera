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

#include <boost/static_assert.hpp>
#include <boost/format.hpp>
#include <iostream>





namespace util {
  
  using boost::format;
  
  namespace { // implementation details...
    
    inline boost::format&
    accessImpl (char* buffer)
    {
      return reinterpret_cast<boost::format&> (*buffer);
    }
    
  }//(End) implementation details
  
  
  /** */
  _Fmt::_Fmt (string formatString)
  {
    BOOST_STATIC_ASSERT (sizeof(boost::format) <= FORMATTER_SIZE);
    
    new(formatter_) boost::format(formatString);
  }
  
  _Fmt::~_Fmt ()
  {
    accessImpl(formatter_).~format();
  }
  
  
  /** @internal access points for the frontend,
   * allowing to push a parameter value down into
   * the implementation for the actual formatting.
   * @note we need to generate instantiations of this template function
   *       explicitly for all basic types to be supported for direct handling,
   *       otherwise we'll get linker errors. Lumiera uses the ``inclusion model''
   *       for template instantiation, which means there is no compiler magic
   *       involved and a template function either needs to be \em defined in
   *       a header or explicitly instantiated in some translation unit.
   */
  template<typename VAL>
  void
  _Fmt::pushParameter (VAL const& val)
  {
    accessImpl(formatter_) % val;
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
  
  template<>
  void
  _Fmt::pushParameter (const char * const cString)
  {
    pushParameter (cString);
  }
  
  
  
  /* ===== explicitly supported =================== */
  
  template void _Fmt::pushParameter(string const&);
  template void _Fmt::pushParameter(char const&);
  template void _Fmt::pushParameter(uchar const&);
  template void _Fmt::pushParameter(int const&);
  template void _Fmt::pushParameter(uint const&);
  template void _Fmt::pushParameter(short const&);
  template void _Fmt::pushParameter(ushort const&);
  template void _Fmt::pushParameter(int64_t const&);
  template void _Fmt::pushParameter(uint64_t const&);
  template void _Fmt::pushParameter(float const&);
  template void _Fmt::pushParameter(double const&);
  template void _Fmt::pushParameter(void * const&);
  
  template void _Fmt::pushParameter(const string * const);
  template void _Fmt::pushParameter(const uchar * const);
  template void _Fmt::pushParameter(const int * const);
  template void _Fmt::pushParameter(const uint * const);
  template void _Fmt::pushParameter(const short * const);
  template void _Fmt::pushParameter(const ushort * const);
  template void _Fmt::pushParameter(const int64_t * const);
  template void _Fmt::pushParameter(const uint64_t * const);
  template void _Fmt::pushParameter(const float * const);
  template void _Fmt::pushParameter(const double * const);
  
  
  
  
  /** @remarks usually the _Fmt helper is used inline
   * at places where a string is expected. The '%' operator
   * used to fed parameters has a higher precedence than the
   * assignment or comparison operators, ensuring that all parameters
   * are evaluated and formatted prior to receiving the formatted result
   */
  _Fmt::operator string()  const
  {
    return accessImpl(formatter_).str();
  }
  
  
  /** send the formatted buffer directly to the output stream.
   * @note this is more efficient than creating a string and outputting that,
   *       because boost::format internally uses a stringstream to generate
   *       the formatted representation, relying on the C++ output framework
   */
  std::ostream&
  operator<< (std::ostream& os, _Fmt const& fmt)
  {
    return os << accessImpl(fmt.formatter_);
  }
  
  
  
} // namespace util
