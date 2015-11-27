/*
  Test-Helper  -  collection of functions supporting unit testing

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

* *****************************************************/


#include "lib/test/test-helper.hpp"
#include "lib/test/testdummy.hpp"
#include "lib/format-string.hpp"
#include "lib/unique-malloc-owner.hpp"

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include <string>

using std::string;

namespace lib {
namespace test{
  
  
  string
  showSizeof (size_t siz, const char* name)
  {
    static util::_Fmt fmt ("sizeof( %s ) %|30t|= %3d");
    return string (fmt % (name? name:"?") % siz);
  }
  
  
#ifdef __GNUG__
  /**
   * \par Implementation notes
   * GCC / G++ subscribes to a cross-vendor ABI for C++, sometimes called the IA64 ABI
   * because it happens to be the native ABI for that platform. It is summarised at
   * \link http://www.codesourcery.com/cxx-abi/ mentor-embedded \endlink
   * along with the current specification. For users of GCC greater than or equal to 3.x,
   * entry points are exposed through the standard library in \c <cxxabi.h>
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
  string
  demangleCxx (Literal rawName)
  {
    return string (rawName);
  }
#endif
  
  
  
  /** @todo probably this can be done in a more clever way. Anyone...?
   */
  string
  randStr (size_t len)
  {
    static const string alpha ("aaaabbccddeeeeffgghiiiijjkkllmmnnooooppqqrrssttuuuuvvwwxxyyyyzz0123456789");
    static const size_t MAXAL (alpha.size());
    
    string garbage(len,'\0');
    size_t p = len;
    while (p)
      garbage[--p] = alpha[rand() % MAXAL];
    return garbage;
  }
  
  
  
  /** storage for test-dummy flags */
    
  long Dummy::_local_checksum = 0;
  bool Dummy::_throw_in_ctor = false;
  
  
}} // namespace lib::test
