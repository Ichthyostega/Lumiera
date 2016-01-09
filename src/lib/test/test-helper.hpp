/*
  TEST-HELPER.hpp  -  collection of functions supporting unit testing

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

*/


/** @file test-helper.hpp
 ** A collection of frequently used helper functions to support unit testing.
 ** Some are test data generators, some are diagnostics helpers to produce readable
 ** output. Some of these support meta programming to figure out the \em actual
 ** reference kind (value, lvalue, rvalue) of a template parameter instantiation.
 ** For GNU compatible compilers, we expose also the interface to the internal
 ** ABI for [demangling type names](\ref demangleCxx).
 ** 
 ** @note this header is included into a large number of tests.
 ** @see TestHelper_test
 ** @see TestHelperDemangling_test
 ** 
 */


#ifndef LIB_TEST_TEST_HELPER_H
#define LIB_TEST_TEST_HELPER_H


#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-obj.hpp"

#include <boost/lexical_cast.hpp>
#include <typeinfo>
#include <cstdlib>
#include <string>



namespace lib {
namespace test{
  
  using lib::Literal;
  using std::string;
  using std::rand;
  using lib::meta::demangleCxx;
  
  
  
  
  /** for printing sizeof().
   *  prints the given size and name literally, without any further magic */
  string
  showSizeof (size_t siz, string name);
  
  /** for printing sizeof(), possibly figuring out the type name automatically
   * @param name when given, this name will be used for display,
   *    instead of auto detecting the type
   */
  template<typename T>
  inline string
  showSizeof (T const* obj =0, const char* name =0)
  {
    return showSizeof (obj?  sizeof(*obj) : sizeof(T),
                       name? name : util::typeStr(obj));
  }
  
  template<typename T>
  inline                                      meta::disable_if<std::is_pointer<T>,
  string                                      >                                 // note:: force invocations with pointer to the first overload
  showSizeof (T const& obj, const char* name=0)
  {
    return showSizeof (&obj, name);
  }
  
  template<typename T>
  inline string
  showSizeof (const char* name)
  {
    return showSizeof<T> (nullptr, name);
  }
  
  
  
  /** helper to discern the kind of reference of the argument type */
  template<typename R>
  string
  showRefKind()
  {
    return std::is_lvalue_reference<R>::value?       "REF"
               : std::is_rvalue_reference<R>::value? "MOV"
                                                   : "VAL";
  }
  
  /** helper for investigating a variadic argument pack
   * @warning always spell out the template arguments explicitly
   *          when invoking this diagnostics, e.g. \c showVariadicTypes<ARGS...>(args...)
   *          otherwise the template argument matching for functions might mess up the
   *          kind of reference you'll see in the diagnostics.
   * @see test-helper-variadic-test.cpp
   */
  template<typename... EMPTY>
  inline string
  showVariadicTypes ()
  {
    return " :.";
  }
  
  template<typename X, typename... XS>
  inline string
  showVariadicTypes (X const& x, XS const&... xs)
  {
    return " :---#"
         + boost::lexical_cast<string>(1 + sizeof...(xs))
         + "  -- Type: " + util::typeStr(x)
         + "  "          + showRefKind<X>()
         + "  Address* " + boost::lexical_cast<string>(&x)
         + "\n"
         + showVariadicTypes<XS...> (xs...);
  }
  
  
  
  
  
  /** create a random but not insane Time value */
  inline lib::time::Time
  randTime ()
  {
    return lib::time::Time (500 * (rand() % 2), (rand() % 600) + 1);
  }
  
  /** create garbage string of given length
   *  @return string containing arbitrary lower case letters and numbers
   */
  string randStr (size_t len);
  
  
}} // namespace lib::test




/* === test helper macros === */

/**
 * Macro to verify a statement indeed raises an exception.
 * If no exception is thrown, the #NOTREACHED macro will trigger
 * an assertion failure. In case of an exception, the #lumiera_error
 * state is checked, cleared and verified.
 */
#define VERIFY_ERROR(ERROR_ID, ERRONEOUS_STATEMENT)          \
          try                                                 \
            {                                                  \
              ERRONEOUS_STATEMENT ;                             \
              NOTREACHED("expected '%s' failure in: %s",         \
                          #ERROR_ID, #ERRONEOUS_STATEMENT);       \
            }                                                      \
          catch (...)                                               \
            {                                                        \
              CHECK (lumiera_error_expect (LUMIERA_ERROR_##ERROR_ID));\
            }

#endif /*LIB_TEST_TEST_HELPER_H*/
