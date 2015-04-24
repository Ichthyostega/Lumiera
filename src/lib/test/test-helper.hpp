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


#ifndef LIB_TEST_TEST_HELPER_H
#define LIB_TEST_TEST_HELPER_H


#include "lib/symbol.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/lexical_cast.hpp>
#include <typeinfo>
#include <cstdlib>
#include <string>



namespace lib {
namespace test{
  
  using lib::Literal;
  using std::string;
  using std::rand;
  
  
  
  /** get a sensible display for a type or object 
   *  @param obj object of the type in question
   *  @param name optional name to be used literally
   *  @return either the literal name without any further magic,
   *          or the result of compile-time or run time 
   *          type identification as implemented by the compiler.
   */
  template<typename T>
  inline Literal
  showType (T const& obj, Literal name=0)
  {
    return name? name : Literal(typeid(obj).name());
  }
  
  /** get a sensible display for a type 
   *  @param name optional name to be used literally
   *  @return either the literal name without any further magic,
   *          or the result of compile-time or run time 
   *          type identification as implemented by the compiler.
   */
  template<typename T>
  inline Literal
  showType (Literal name=0)
  {
    return name? name : Literal(typeid(T).name());
  }
  
  
  /** reverse the effect of C++ name mangling.
   * @return string in language-level form of a C++ type or object name,
   *         or a string with the original input if demangling fails.
   * @warning implementation relies on the cross vendor C++ ABI in use
   *         by GCC and compatible compilers, so portability is limited.
   *         The implementation is accessed through libStdC++
   *         Name representation in emitted object code and type IDs is
   *         essentially an implementation detail and subject to change.
   */
  string
  demangleCxx (Literal rawName);
  
  
  /** short yet distinct name identifying the given type.
   * @return demangled type-id without any scopes. */
  template<typename TY>
  string
  tyAbbr()
  {
    string typeStr = demangleCxx (showType<TY>());
    size_t pos = typeStr.rfind("::");
    if (pos != string::npos)
      typeStr = typeStr.substr(pos+2);
    return typeStr;
  }
  
  template<typename TY>
  string
  tyAbbr(TY&&)
  {
    return tyAbbr<TY>();
  }
  
  
  /** for printing sizeof().
   *  prints the given size and name literally, without any further magic */
  string
  showSizeof (size_t siz, const char* name);
  
  /** for printing sizeof(), trying to figure out the type name automatically */
  template<typename T>
  inline string
  showSizeof(const char* name=0)
  {
    return showSizeof (sizeof (T), showType<T> (name));
  }
  
  template<typename T>
  inline string
  showSizeof(T const& obj, const char* name=0)
  {
    return showSizeof (sizeof (obj), showType (obj,name));
  }
  
  template<typename T>
  inline string
  showSizeof(T *obj, const char* name=0)
  {
    return obj? showSizeof (*obj, name)
              : showSizeof<T> (name);
  }
  
  
  
  /** helper to discern the kind of reference of the argument type */
  template<typename R>
  string
  showRefKind()
  {
    return std::is_lvalue_reference<R>::value? "REF"
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
         + "  -- Type: " + showType<X>()
         + "  "          + showRefKind<X>()
         + "  Address* " + boost::lexical_cast<string>(&x)
         + "\n"
         + showVariadicTypes<XS...> (xs...);
  }
  
  
  
  
  
  /** create a random but not insane Time value */
  inline lib::time::Time
  randTime ()
  {
    return lib::time::Time (500 * (rand() % 2), (rand() % 600));
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

#endif
