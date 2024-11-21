/*
  FormatObj  -  simple means to display an object

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file format-obj.cpp
 ** Some implementation helpers for simple object display.
 ** This file provides some basic implementation functions, which
 ** can be called from a generic front-end (header). The reason we
 ** stash away some functions into an implementation unit is to keep
 ** the include overhead low, which helps to reduce both code size
 ** and compilation time. The functions here perform some formatting
 ** tasks commonly used from debugging and diagnostics code, both to
 ** investigate object contents and show types and addresses. They
 ** are referred from our [lightweight string converter](\ref
 ** util::StringConv), but also from the util::toString() function
 ** and more common [formatting utils](\ref format-util.hpp).
 ** 
 ** @see FormatHelper_test
 ** @see FormatString_test
 ** 
 */



#include "lib/error.hpp"
#include "lib/format-obj.hpp"
#include "lib/ios-savepoint.hpp"
//#include "lib/format-string.hpp"
#include "lib/unique-malloc-owner.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include <limits>
#include <iomanip>
#include <sstream>
#include <string>
#include <regex>

//using util::_Fmt;
using util::removePrefix;
using util::removeSuffix;
using util::isnil;
using std::string;

using std::regex;
using std::regex_replace;


namespace { // hard-wired configuration for debugging output....
  
  // precision for rendering of double values
  const auto DIAGNOSTICS_DOUBLE_PRECISION = 8;
  const auto DIAGNOSTICS_FLOAT_PRECISION  = 5;
  
  /** show only this amount of trailing bytes from an address */
  const size_t DIAGNOSTICS_ADDRESS_SUFFIX_LEN = 4;
  
  
  /** maximum decimal digits able to pass through a round trip without value change */ 
  template<typename F>
  constexpr size_t PRECISION_DECIMAL = std::numeric_limits<F>::digits10;
  
  /** decimal digits require tod represent each different floating-point value */ 
  template<typename F>
  constexpr size_t PRECISION_COMPLETE = std::numeric_limits<F>::max_digits10;
}



namespace lib  {
namespace meta {
  
  // pre-allocated failure indicators, which can be returned failsafe.
  
  extern const string BOTTOM_INDICATOR  = "⟂";
  extern const string FAILURE_INDICATOR = "↯";
  extern const string VOID_INDICATOR    = "void";
  extern const string FUNCTION_INDICATOR= "Function";
  
  extern const string BOOL_FALSE_STR = "false";
  extern const string BOOL_TRUE_STR = "true";
  
  
  
#ifdef __GNUG__
  /**
   * # Implementation notes
   * 
   * GCC / G++ subscribes to a cross-vendor ABI for C++, sometimes called the IA64 ABI
   * because it happens to be the native ABI for that platform. It is summarised at
   * \link http://www.codesourcery.com/cxx-abi/ mentor-embedded \endlink
   * along with the current specification. For users of GCC greater than or equal to 3.x,
   * entry points are exposed through the standard library in `<cxxabi.h>`
   * 
   * This implementation relies on a vendor neutral ABI for C++ compiled programs
   * 
   *     char* abi::__cxa_demangle(const char* mangled_name,
   *                               char* output_buffer, size_t* length,
   *                               int* status)
   * 
   * Parameters:
   *  - \c mangled_name
   *    NUL-terminated character string containing the name to be demangled.
   *  - \c output_buffer
   *    region of memory, allocated with \c malloc, of `*length` bytes,
   *    into which the demangled name is stored. If \c output_buffer is not long enough,
   *    it is expanded using \c realloc. output_buffer may instead be NULL; in that case,
   *    the demangled name is placed in a region of memory allocated with \c malloc.
   *  - \c length
   *    If length is non-NULL, the length of the buffer containing the demangled name is placed in `*length`.
   *  - \c status
   *    error flag: `*status` is set to one of the following values:
   *             
   *              0: The demangling operation succeeded.
   *             -1: A memory allocation failure occurred.
   *             -2: mangled_name is not a valid name under the C++ ABI mangling rules.
   *             -3: One of the arguments is invalid.
   * 
   * The function returns a pointer to the start of the NUL-terminated demangled name,
   * or NULL if the demangling fails. The caller is responsible for deallocating
   * this memory using \c free.
   */
  string
  demangleCxx (Literal rawName)
  {
    int error = -4;
    UniqueMallocOwner<char> demangled (abi::__cxa_demangle (rawName,
                                                            NULL,
                                                            NULL,
                                                            &error));
    return 0==error? demangled.get()
                   : string(rawName);
  }
  
  
#else
#warning "Lumiera was _not_ built with a GCC compatible compiler."
#warning "There are good chances this works without problems, but up to now, \
the Lumiera developers lacked the resources to investigate that option; \
apologies for that."
  
  /** Fallback type-ID:
   * @return unaltered internal type-ID
   */
  string
  demangleCxx (Literal rawName)
  {
    return string (rawName);
  }
#endif
  
  
  
  
  
  /** \par implementation notes
   * - we do not strip type adornments like `const`, `&` or `*`,
   *   however, the typical usage from within util::typeStr()
   *   is arranged in a way to absorb these adornments by the way
   *   the template signatures are defined
   * - we _do_ simplify the type display and strip some obnoxious
   *   namespace prefixes with the help of `std::regex_replace`
   * - we perform those simplifying rewrites _in place_ thus _overwriting_
   *   the result string. This exploits the fact that the replacements are
   *   _always shorter_ than what is being replaced (**beware**).
   * - standard regular expressions can be [assumed][stdRegEx-threadsafe]
   *   to be threadsafe. Thus, we're able to build an embedded shared
   *   static variable _on demand_ and use the performance optimisation
   *   offered by the standard library
   * - performance wise we'll assume the transformation happens within
   *   the cache, so it doesn't make much of a difference if we scan
   *   the same comparatively short string multiple times
   * 
   * [stdRegEx-threadsafe]: http://stackoverflow.com/questions/15752910/is-stdregex-thread-safe
   */
  string
  humanReadableTypeID (Literal rawType)
  {
    string typeName = demangleCxx (rawType);
    
    #define TYP_EXP "[\\w<>\\(\\):,\\s]+"
    
    static regex commonPrefixes {"std::"
                                "|(\\w+::)+\\(anonymous namespace\\)::"
                                "|lib::meta::"
                                "|lib::time::"
                                "|lib::test::"
                                "|lib::diff::"
                                "|lib::"
                                "|util::"
                                "|steam::(asset::|mobject::(session::)?|play::)?"
                                "|stage::model"
                                "|stage::ctrl"
                                "|lumiera::"
                                , regex::ECMAScript | regex::optimize};
    
    static regex lolong    {"long long"
                                , regex::ECMAScript | regex::optimize};
    static regex unSigned  {"unsigned (\\w+)"
                                , regex::ECMAScript | regex::optimize};
    
    static regex stdString {"(__cxx11::)?basic_string<char, char_traits<char>, allocator<char>\\s*>(\\s+\\B)?"
                                , regex::ECMAScript | regex::optimize};
    
    static regex stdAllocator {"(\\w+<(" TYP_EXP ")), allocator<\\2>\\s*"
                                , regex::ECMAScript | regex::optimize};
    
    static regex mapAllocator {"(map<(" TYP_EXP "), (" TYP_EXP ")),.+allocator<pair<\\2 const, \\3>\\s*>\\s*"
                                , regex::ECMAScript | regex::optimize};
    
    static regex uniquePtr {"unique_ptr<(\\w+), default_delete<\\1>\\s*"
                                , regex::ECMAScript | regex::optimize};
    
    static regex lumieraP  {"P<(\\w+), shared_ptr<\\1>\\s*"
                                , regex::ECMAScript | regex::optimize};
    
    
    auto pos = typeName.begin();
    auto end = typeName.end();
    
    end = regex_replace(pos, pos, end, commonPrefixes, "");
    end = regex_replace(pos, pos, end, lolong, "llong");
    end = regex_replace(pos, pos, end, unSigned, "u$1");
    end = regex_replace(pos, pos, end, stdString, "string");
    end = regex_replace(pos, pos, end, stdAllocator, "$1");
    end = regex_replace(pos, pos, end, mapAllocator, "$1");
    end = regex_replace(pos, pos, end, uniquePtr, "unique_ptr<$1");
    end = regex_replace(pos, pos, end, lumieraP, "P<$1");

    typeName.resize(end - typeName.begin());
    return typeName;
  }
  
  
  
  /** \par implementation notes
   * We want to get at the name of the _most relevant_ type entity.
   * This in itself is a heuristic. But we can work on the assumption,
   * that we get a sequence of nested namespaces and type names, and
   * we'll be interested in the last, the innermost of these types.
   * In the most general case, each type could be templated, and
   * thus will be followed by parameter specs enclosed in angle
   * braces. Behind this spec, only type adornments will follow.
   * Thus we'll inspect the string _from the back side_, skipping
   * over all type parameter contents, until we reach brace
   * level zero again. From this point, we have to search
   * backwards to the first namespace separator `::`
   * @warning we acknowledge this function can fail in various ways,
   *    some of which will be indicated by returning the string "void".
   *    But it may well happen that the returned string contains
   *    whitespace, superfluous punctuation or even the whole
   *    demangled type specification as is.
   */
  string
  primaryTypeComponent (Literal rawType)
  {
    string typeStr = demangleCxx (rawType);
    
    removeSuffix (typeStr, " const");
    removeSuffix (typeStr, " const *");
    removeSuffix (typeStr, "*");
    removeSuffix (typeStr, "&");
    
    if (isnil (typeStr)) return VOID_INDICATOR;
    if (')' == typeStr.back()) return FUNCTION_INDICATOR;
    
    auto end = typeStr.end();
    auto beg = typeStr.begin();
    int level=0;
    while (--end != beg)
      {
        if ('>' == *end)
          ++level;
        else if ('<' == *end and level>0)
          --level;
        else
          if (level==0)
          {
            ++end;
            break;
          }
      }
    if (end == beg) return VOID_INDICATOR;
    
    auto pos = typeStr.rfind("::", end-beg);
    typeStr = (pos==string::npos? typeStr.substr(0, end-beg)
                                : typeStr.substr(pos+2, (end-beg)-pos-2));
    return typeStr;
  }
  
  
  string
  sanitisedFullTypeName (lib::Literal rawName)
  {
    return util::sanitise (humanReadableTypeID (rawName));
  }
  
  
  string
  sanitisedSymbol (string const& text)
  {
    static regex identifierChars {"[A-Za-z]\\w*", regex::ECMAScript | regex::optimize};
    
    return regex_replace (text, identifierChars, "$&", std::regex_constants::format_no_copy);
  }                                                      // don't copy what does not match
  
  
  
}}// namespace lib::meta





/* === formatting and pretty printing support utils === */

namespace util {
  
  using std::hex;
  using std::setw;
  using std::right;
  using std::setfill;
  using std::uppercase;
  using std::noshowbase;
  using std::ostringstream;
  using std::ostream;

  template<typename F>
  string
  showFloatingPoint (F val, size_t precision)  noexcept
    try {
      ostringstream buffer;
      buffer.precision (precision);
      buffer << val;
      return buffer.str();
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  /**
   * @return fixed point string representation, never empty
   * @note we set an explicit precision, since this is a diagnostic facility
   * @remarks typically do not want to see all digits, but, for test code,
   *       we do want a predictable string representation of simple fractional
   *       values like `0.1` (which can not be represented as binary floats)
   */
  string showDouble (double val)   noexcept { return showFloatingPoint (val, DIAGNOSTICS_DOUBLE_PRECISION); }
  string showFloat  (float val)    noexcept { return showFloatingPoint (val, DIAGNOSTICS_FLOAT_PRECISION); }
  
  string showDecimal (double val)  noexcept { return showFloatingPoint (val, PRECISION_DECIMAL<double>); }
  string showDecimal (float val)   noexcept { return showFloatingPoint (val, PRECISION_DECIMAL<float>); }
  string showDecimal (f128 val)    noexcept { return showFloatingPoint (val, PRECISION_DECIMAL<f128>); }
  
  string showComplete (double val) noexcept { return showFloatingPoint (val, PRECISION_COMPLETE<double>); }
  string showComplete (float val)  noexcept { return showFloatingPoint (val, PRECISION_COMPLETE<float>); }
  string showComplete (f128 val)   noexcept { return showFloatingPoint (val, PRECISION_COMPLETE<f128>); }
  
  
  string
  showSize (size_t val)   noexcept
    try {
      ostringstream buffer;
      buffer << val;
      return buffer.str();
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  
  /** @note show only the trailing X bytes of any address */
  ostream&
  showAdr (ostream& stream, void const* addr)
  {
    IosSavepoint save{stream};
    size_t suffix_modulus = size_t(1) << DIAGNOSTICS_ADDRESS_SUFFIX_LEN * 8;
    return stream << "╲"
                  << hex
                  << noshowbase
                  << setw (DIAGNOSTICS_ADDRESS_SUFFIX_LEN * 2)  // need 2 hex digits per byte
                  << setfill('_')
                  << right
                  << size_t(addr) % suffix_modulus;
  }
  
  
  string
  showAdr (void const* addr)  noexcept
    try {
      ostringstream buffer;
      showAdr (buffer, addr);
      return buffer.str();
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  
  string
  showHash (size_t hash, uint showBytes)  noexcept
    try {
      showBytes = util::limited (1u, showBytes, 8u);
      size_t suffix_modulus = size_t(1) << showBytes * 8;
      ostringstream buffer;
      buffer << hex
             << uppercase
             << noshowbase
             << setw (showBytes * 2)  // need 2 hex digits per byte
             << setfill('0')
             << right
             << (showBytes==8? hash : hash % suffix_modulus);
      return buffer.str();
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  
  
} // namespace util
