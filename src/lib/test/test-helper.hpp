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
  
  
  
  /**
   * Helper to show types involved in metaprogramming.
   * Place an instantiation of this template into some scope
   * to provoke a compilation failure, which reveals what type X was bound to.
   * Look for "in instantiation of TypeDebuger<..." in the error output.
   */
  template<typename X>
  struct TypeDebugger
    {
      static_assert (not sizeof(X), "### Type Debugging ###");
    };
  
  template<typename X>
  void
  typeDebugger(X&& x)
  {
    static_assert (not sizeof(X), "### Type Debugging ###");
  }
  
  
  
  namespace { // helper for printing type diagnostics
    
    template<typename X>
    struct TypeDiagnostics
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = "";
      };
    template<typename X>
    struct TypeDiagnostics<const X>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = "";
      };
    template<typename X>
    struct TypeDiagnostics<X&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = "&";
      };
    template<typename X>
    struct TypeDiagnostics<X&&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " &&";
      };
    template<typename X>
    struct TypeDiagnostics<X const&>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " const&";
      };
    template<typename X>
    struct TypeDiagnostics<X const&&>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " &&";
      };
    template<typename X>
    struct TypeDiagnostics<X *>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " *";
      };
    template<typename X>
    struct TypeDiagnostics<const X *>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " *";
      };
    template<typename X>
    struct TypeDiagnostics<const X * const>
      {
        using Type = X;
        static constexpr auto prefix  = "const ";
        static constexpr auto postfix = " * const";
      };
    template<typename X>
    struct TypeDiagnostics<X * const>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " * const";
      };
    template<typename X>
    struct TypeDiagnostics<X * const *>
      {
        using Type = X;
        static constexpr auto prefix  = "";
        static constexpr auto postfix = " * const *";
      };
  }
  
  /** diagnostic type output, including const and similar adornments
   * @warning operates after-the-fact and relies on mangled type names
   *          plus several heuristics. Output might thus not be entirely correct,
   *          especially when several levels of const, pointer and references are involved.
   *          If in doubt, place the TypeDebugger<T> to reveal the type as the compiler sees it.
   * @remarks the function lib::meta::typeStr removes adornments and does not work on all kinds
   *          of reference. This helper attempts to work around those limitations.
   */
  template<typename X>
  inline string
  showType()
  {
    using Case = TypeDiagnostics<X>;
    using Type = typename Case::Type;
    
    return Case::prefix
         + meta::humanReadableTypeID (typeid(Type).name())
         + Case::postfix;
  }
  
  
  
  
  /** create a random but not insane Time value between 1s ... 10min + 500ms */
  inline lib::time::Time
  randTime ()
  {
    return lib::time::Time (500 * (rand() % 2), (rand() % 600) + 1);
  }
  
  /** create garbage string of given length
   *  @return string containing arbitrary lower case letters and numbers
   */
  string randStr (size_t len);
  
  
  /**
   * Helper to produce better diagnostic messages when comparing
   * to an expected result string. This type can be used to mark a
   * `std::string` in order to invoke a special rigged equality test.
   * The counterpart for equality conversion can be any arbitrary type,
   * on which some kind of _string conversion_ can be performed
   * @see format-obj.hpp
   */
  class ExpectString
    : public std::string
    {
      using std::string::string;
      
      template<typename X>
      friend bool
      operator== (X const& x, ExpectString const& expected)
      {
        std::string actual{util::StringConv<X>::invoke (x)};
        return expected.verify (actual);
      }
      
      template<typename X>
      friend bool
      operator== (ExpectString const& expected, X const& x)
      {
        std::string actual{util::StringConv<X>::invoke (x)};
        return expected.verify (actual);
      }
      
      bool verify (std::string const& actual)  const;
    };
  
}} // namespace lib::test



/**
 * user defined literal for expected result strings.
 * On equality comparison to any other string convertible object,
 * the difference to this expected string is printed to STDERR
 * 
 * @example
 * \code
 * CHECK (result23 == "[-100..100]"_expect);
 * \endcode
 */
inline lib::test::ExpectString
operator""_expect (const char* lit, size_t siz)
{
  return lib::test::ExpectString{lit, siz};
}



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


/**
 * Macro to mark the current test function in STDOUT.
 * This can be helpful to digest a long test output dump
 */
#define MARK_TEST_FUN \
          cout << "|" << endl << "|  »"<<__FUNCTION__<<"«" <<endl;

#endif /*LIB_TEST_TEST_HELPER_H*/
