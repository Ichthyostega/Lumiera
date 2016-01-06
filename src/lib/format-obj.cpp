/*
  FormatObj  -  simple means to display an object

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

* *****************************************************/

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
 ** lib::meta::CustomStringConv), but also from the util::toString()
 ** function and more common [formatting utils](format-util.hpp).
 ** 
 ** @see FormatHelper_test
 ** @see FormatString_test
 ** 
 */



#include "lib/error.hpp"
#include "lib/format-obj.hpp"
//#include "lib/format-string.hpp"
#include "lib/unique-malloc-owner.hpp"
#include "lib/symbol.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include <iomanip>
#include <sstream>
#include <string>

//using util::_Fmt;
using std::string;


namespace { // hard-wired configuration for debugging output....
  
  // precision for rendering of double values
  const auto DIAGNOSTICS_DOUBLE_PRECISION = 8;
  const auto DIAGNOSTICS_FLOAT_PRECISION  = 5;
  
  /** show only this amount of trailing bytes from an address */
  const size_t DIAGNOSTICS_ADDRESS_SUFFIX_LEN = 4;
}



namespace lib  {
namespace meta {
  
#ifdef __GNUG__
  /**
   * \par Implementation notes
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
  
  
  
  string
  humanReadableTypeID (Literal rawType)
  {
    string typeName = demangleCxx (rawType);
    return typeName;
  }
  
}}// namespace lib::meta




/* === formatting and pretty printing support uitls === */

namespace util {
  
  
  namespace { // implementation details...
    
  }//(End) implementation details
  
  using std::hex;
  using std::setw;
  using std::right;
  using std::setfill;
  using std::noshowbase;
  using std::ostringstream;
  using std::ostream;
  
  
  /**
   * @return fixed point string representation, never empty
   * @note we set an explicit precision, since this is a diagnostic facility
   * @remarks typically do not want to see all digits, but, for test code,
   *       we do want a predictable string representation of simple fractional
   *       values like `0.1` (which can not be represented as binary floats)
   */
  string
  showDouble (double val)  noexcept
    try {
      ostringstream buffer;
      buffer.precision (DIAGNOSTICS_DOUBLE_PRECISION);
      buffer << val;
      return buffer.str();
    }
    catch(...)
    { return "↯"; }
  
  
  string
  showFloat (float val)  noexcept
    try {
      ostringstream buffer;
      buffer.precision (DIAGNOSTICS_FLOAT_PRECISION);
      buffer << val;
      return buffer.str();
    }
    catch(...)
    { return "↯"; }
  
  
  /** @note show only the trailing X bytes of any address */
  ostream&
  showAddr (ostream& stream, void* addr)
  {
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
  showAddr (void *addr)  noexcept
    try {
      ostringstream buffer;
      showAddr (buffer, addr);
      return buffer.str();
    }
    catch(...)
    { return "↯"; }
  
  
  
} // namespace util
