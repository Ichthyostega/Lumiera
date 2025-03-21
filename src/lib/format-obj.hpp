/*
  FORMAT-OBJ.hpp  -  simple means to display an object

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file format-obj.hpp
 ** Simple functions to represent objects, for debugging and diagnostics.
 ** The helpers provided here are rather commonplace, but written in a way
 ** to incur only modest header inclusion load. It should be OK to use them
 ** even on interface headers in widespread use.
 ** - util::toString() performs a failsafe to-String conversion, thereby preferring a
 **         built-in conversion operator, falling back to a lexical conversion (boost)
 **         or just an unmangled and simplified type string as default.
 ** - util::typedString() combines this with an always visible type display
 ** - lib::meta::demangleCxx() uses the built-in compiler support to translate a mangled
 **         type-ID (as given by `typeid(TY).name()`) into a readable, fully qualified
 **         C++ type name. This is only supported for GNU compatible compilers.
 ** 
 ** @todo is it possible to stash away the `boost::lexical_cast` behind a custom facade,
 **         the way we did it for `boost::format`? This would reduce inclusion cost...
 ** 
 ** @see FormatHelper_test
 ** @see [frontend for boost::format, printf-style](\ref format-string.hpp)
 ** 
 */


#ifndef LIB_FORMAT_OBJ_H
#define LIB_FORMAT_OBJ_H

#include "lib/symbol.hpp"
#include "lib/meta/trait.hpp"

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
  
  std::string demangleCxx (lib::Literal rawName);
  std::string humanReadableTypeID (lib::Literal);
  std::string primaryTypeComponent (lib::Literal);
  std::string sanitisedFullTypeName(lib::Literal);
  
}}// namespace lib::meta






namespace util {
  
  std::string showDouble (double)        noexcept;
  std::string showFloat (float)          noexcept;
  std::string showSize (size_t)          noexcept;
  std::string showAdr (void const* addr) noexcept;
  
  /** preconfigured format for pretty-printing of addresses */
  std::ostream& showAdr (std::ostream&, void const* addr);
  
  /** renders the size_t in hex, optionally only trailing bytes */
  std::string showHash (size_t hash, uint showBytes=8)  noexcept;
  
  inline std::string
  showHashLSB (size_t hash)  noexcept
  {
    return showHash(hash,1);
  }
  
  std::string showDecimal (double) noexcept;
  std::string showDecimal (float)  noexcept;
  std::string showDecimal (f128)   noexcept;
  std::string showComplete (double)noexcept;
  std::string showComplete (float) noexcept;
  std::string showComplete (f128)  noexcept;
  
  
  
  namespace {
    /** toggle to prefer specialisation with direct lexical conversion */
    template<typename X>
    using enable_LexicalConversion = lib::meta::enable_if< lib::meta::use_LexicalConversion<X>>;
    
    template<typename SP>
    using show_SmartPointer = lib::meta::enable_if< lib::meta::is_smart_ptr<typename lib::meta::Strip<SP>::TypeReferred>>;
  }
  
  
  
  /* === generalise the failsafe string conversion === */
  
  /** @note base case is defined in meta/util.hpp */
  template<typename X>
  struct StringConv<X,     enable_LexicalConversion<X>>
    {
      static std::string
      invoke (X const& val) noexcept
        try        { return boost::lexical_cast<std::string> (val); }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  template<typename SP>
  struct StringConv<SP,    show_SmartPointer<SP>>
    {
      static std::string
      invoke (SP const& smP) noexcept
        try        { return showSmartPtr (smP, lib::meta::typeSymbol(smP)); }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  /** explicit specialisation to control precision of double values.
   * @note we set an explicit precision, since this is a diagnostic facility
   *       and we typically do not want to see all digits, but, for test code,
   *       we do want a predictable string representation of simple fractional
   *       values like `0.1` (which can not be represented as binary floats)
   */
  template<>
  struct StringConv<double>
    {
      static std::string
      invoke (double val) noexcept
      {
        return util::showDouble (val);
      }
    };
  template<>
  struct StringConv<float>
    {
      static std::string
      invoke (float val) noexcept
      {
        return util::showFloat (val);
      }
    };
  template<>
  struct StringConv<bool>
    {
      static std::string
      invoke (bool val) noexcept
      {
        return util::showBool (val);
      }
    };
  
  
  
  
  /**
   * get some string representation of any object, reliably.
   * A custom string conversion operator is invoked, if applicable,
   * while all lexically convertible types (numbers etc) are treated
   * by boost::lexical_cast. For double or float values, hard wired
   * rounding to a fixed number of digits will be performed, to yield
   * a predictable display of printed unit-test results.
   * @remark while the actual parameter is passed by const-ref,
   *         cv-qualifiactions and references are stripped from the type
   * @note Deliberately there is no magic detection/support for pointers.
   *       This function _must not be overloaded_ (to avoid ambiguities
   *       in more elaborate template instantiations).
   * @remark Since 10/2023 an attempt was made to solve this problem down
   *       in `StringConf<TY>`, yet this solution might be brittle. If you
   *       want pointers to be indicated (with address), consider using
   *       util::showPtr explicitly.
   */
  template<typename TY>
  inline std::string
  toString (TY const& val)  noexcept
  {
    using PlainVal = typename lib::meta::Strip<TY>::TypeReferred;
    return StringConv<PlainVal>::invoke (val);
  }
  
  
  /**
   * indicate type and possibly a (custom) conversion to string
   * @return human readable type name '|' string representation.
   *    or just the type, when no string representation available
   */
  template<typename TY>
  inline std::string
  typedString (TY const& val)  noexcept
    try {
      std::string repr = StringConv<TY>::invoke (val);
      return 0 == repr.rfind("«", 0)? repr
                                    : "«"+typeStr(val)+"»|"+repr;
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  
  
} // namespace util
#endif /*LIB_FORMAT_OBJ_H*/
