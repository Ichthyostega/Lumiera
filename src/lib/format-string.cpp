/*
  FormatString  -  string template formatting based on boost::format

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file format-string.cpp
 ** Implementation for printf-style formatting, based on boost::format.
 ** This file holds the generic implementation of our format frontend,
 ** which actually just invokes boost::format. The corresponding header
 ** format-string.hpp provides some template functions and classes,
 ** either invoking a custom string conversion, or passing primitive
 ** values down unaltered.
 ** 
 ** Here, we define explicit specialisations for the frontend to invoke,
 ** which in turn just pass on the given argument value to the embedded
 ** boost::format object, which in turn dumps the formatted result
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
#include "lib/format-util.hpp"
#include "lib/format-string.hpp"

#include <boost/static_assert.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <cstddef>
#include <new>





namespace util {
  
  using boost::format;
  
  
  namespace { // implementation details...
    
    inline boost::format&
    accessImpl (std::byte* buffer)
    {
      return * std::launder (reinterpret_cast<boost::format*> (buffer));
    }
    
    
    inline void
    destroyImpl (std::byte* buffer)
    {
      accessImpl(buffer).~format();
    }
    
    
    /** in case the formatting of a (primitive) value fails,
     *  we try to supply an error indicator instead */
    void
    pushFailsafeReplacement (std::byte* formatter, const char* errorMsg =NULL)
    try {
        string placeholder("<Error");
        if (errorMsg){
            placeholder += ": ";
            placeholder += errorMsg;
        }
        placeholder += ">";
        
        accessImpl(formatter) % placeholder;
      }
    ERROR_LOG_AND_IGNORE (progress, "Supplying placeholder for problematic format parameter")
    
    
    inline void
    suppressInsufficientArgumentErrors (std::byte* formatter)
    {
      using namespace boost::io;
      accessImpl(formatter).exceptions (all_error_bits ^ too_few_args_bit);
    }
    
    
  }//(End) implementation details
  
  
  
  
  
  /** Build a formatter object based on the given format string.
   *  The actual implementation is delegated to an boost::format object,
   *  which is placement-constructed into an opaque buffer embedded into
   *  this object. Defining the necessary size for this buffer relies
   *  on implementation details of boost::format (and might break)
   * @see lib::meta::SizeTrait::BOOST_FORMAT
   */
  _Fmt::_Fmt (string formatString)
  try {
      static_assert (sizeof(boost::format) <= FORMATTER_SIZE,
                     "opaque working buffer insufficient "
                     "to hold a boost::format instance. "
                     "Maybe boost implementation change. "
                     "Please verify lib/meta/size-trait.hpp");
      
      new(formatter_) boost::format(formatString);
      suppressInsufficientArgumentErrors (formatter_);
    }
  catch (boost::io::bad_format_string& syntaxError)
    {
      throw lumiera::error::Fatal (syntaxError
                                  , _Fmt("Format string '%s' is broken") % formatString
                                  , LUMIERA_ERROR_FORMAT_SYNTAX);
    }
  
  
  _Fmt::~_Fmt ()
  {
    destroyImpl (formatter_);
  }
  
  
  /** @internal access point for the frontend,
   * allowing to push a single parameter value down
   * into the implementation for the actual formatting.
   * Only selected primitive types are handled directly this way,
   * while all custom conversions, the handling of pointers and the
   * fallback (showing a type string) is done in the frontend.
   * @note we need to generate instantiations of this template function
   *       explicitly for all basic types to be supported for direct handling,
   *       otherwise we'll get linker errors. Lumiera uses the ``inclusion model''
   *       for template instantiation, which means there is no compiler magic
   *       involved and a template function either needs to be \em defined in
   *       a header or explicitly instantiated in some translation unit.
   */
  template<typename VAL>
  void
  _Fmt::format (const VAL val, Implementation& formatter)
  try {
      accessImpl(formatter) % val;
    }
  
  catch (boost::io::too_many_args& argErr)
    {
      WARN (progress, "Format: excess argument '%s' of type «%s» ignored."
                    , cStr(toString(val))
                    , cStr(typeStr(val)));
    }
  catch (std::exception& failure)
    {
      _clear_errorflag();
      WARN (progress, "Format: Parameter '%s' causes problems: %s"
                    , cStr(toString(val))
                    , failure.what());
      pushFailsafeReplacement (formatter, failure.what());
    }
  catch (...)
    {
      _clear_errorflag();
      WARN (progress, "Format: Unexpected problems accepting format parameter '%s'", cStr(toString(val)));
      pushFailsafeReplacement (formatter);
    }
  
  
  
  
  /* ===== explicitly supported =================== */
  
  template void _Fmt::format (const char,    Implementation&);
  template void _Fmt::format (const uchar,   Implementation&);
  template void _Fmt::format (const int16_t, Implementation&);
  template void _Fmt::format (const uint16_t,Implementation&);
  template void _Fmt::format (const int32_t, Implementation&);
  template void _Fmt::format (const uint32_t,Implementation&);
  template void _Fmt::format (const int64_t, Implementation&);
  template void _Fmt::format (const uint64_t,Implementation&);
  template void _Fmt::format (const float,   Implementation&);
  template void _Fmt::format (const double,  Implementation&);
  template void _Fmt::format (const string,  Implementation&);
  template void _Fmt::format (const void *,  Implementation&);
  template void _Fmt::format (const char *,  Implementation&);
#ifndef __x86_64__
  template void _Fmt::format (const long,    Implementation&);
  template void _Fmt::format (const ulong,   Implementation&);
#endif
  
  
  
  
  /** @remarks usually the _Fmt helper is used inline
   * at places where a string is expected. The '%' operator
   * used to fed parameters has a higher precedence than the
   * assignment or comparison operators, ensuring that all parameters
   * are evaluated and formatted prior to receiving the formatted result
   * @note EX_FREE
   */
  _Fmt::operator string()  const
  try {
      return accessImpl(formatter_).str();
    }
  
  catch (std::exception& failure)
    {
      _clear_errorflag();
      WARN (progress, "Format: Failure to receive formatted result: %s", failure.what());
      return "<formatting failure>";
    }
  catch (...)
    {
      _clear_errorflag();
      WARN (progress, "Format: Unexpected problems while formatting output.");
      return "<unexpected problems>";
    }
  
  
  
  /** send the formatted buffer directly to the output stream.
   * @note this is more efficient than creating a string and outputting that,
   *       because boost::format internally uses a string-stream to generate
   *       the formatted representation, relying on the C++ output framework
   */
  std::ostream&
  operator<< (std::ostream& os, _Fmt const& fmt)
  try {
      return os << accessImpl(fmt.formatter_);
    }
  
  catch(std::exception& failure)
    {
      _clear_errorflag();
      WARN (progress, "Format: Failure when outputting formatted result: %s", failure.what());
      return os << "<formatting failure>";
    }
  catch(...)
    {
      _clear_errorflag();
      WARN (progress, "Format: Unexpected problems while producing formatted output.");
      return os << "<unexpected problems>";
    }
  
  
  LUMIERA_ERROR_DEFINE (FORMAT_SYNTAX, "Syntax error in format string for boost::format");
  
  
  
} // namespace util
