/*
  TEST-HELPER.hpp  -  collection of functions supporting unit testing

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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
#include "lib/meta/trait.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/test/transiently.hpp"
#include "lib/format-obj.hpp"
#include "lib/random.hpp"

#include <boost/lexical_cast.hpp>
#include <typeinfo>
#include <cstdlib>
#include <limits>
#include <string>
#include <cmath>



namespace lib {
namespace test{
  
  using lib::Literal;
  using std::string;
  using lib::rani;
  using lib::meta::demangleCxx;
  
  
  constexpr auto ROUGH_PRECISION  = pow (10, -3);
  constexpr auto EPSILON_ULP      = 5;
  
  
  template<typename F, typename N>
  constexpr inline                                   meta::enable_if< std::is_floating_point<F>,
  bool                                                              >
  roughEQ (F val, N target, F limit =ROUGH_PRECISION)
  {
    REQUIRE (0 < limit);
    return abs (val - target) < limit * abs(target);
  }                                  //////////////////////////////////////////////////////////////////////////TICKET #1360 looks like this problem was solved several times
  
  
  template<typename F>
  constexpr inline                                   meta::enable_if< std::is_floating_point<F>,
  F                                                                 >
  ulp (F val)
  {
    val = fabs (val);
    const int exp = val < std::numeric_limits<F>::min()
                  ? std::numeric_limits<F>::min_exponent - 1  // fixed exponent for subnormals
                  : std::ilogb (val);
    auto scaledUlp = std::ldexp (std::numeric_limits<F>::epsilon(), exp);
    ENSURE (F(0) < scaledUlp);
    return scaledUlp;
  }
  
  template<typename F, typename N>
  constexpr inline                                   meta::enable_if< std::is_floating_point<F>,
  bool                                                              >
  epsEQ (F val, N target, uint ulps =EPSILON_ULP)
  {
    return abs (val - target) < ulps * ulp<F> (target);
  }
  
  
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
  
  template<typename...TS>
  string
  showTypes()
  {
    return "<| " + ((showType<TS>()+", ") + ... + "|>");
  }
  
  
  /** helper for investigating a variadic argument pack
   * @remark you can pass arguments directly to this function,
   *  but be sure to use `std::forward<ARGS>(args)...` when forwarding
   *  universal references received from a variadic argument pack \a ARG.
   * @note due to reference collapsing it is not possible to distinguish receiving
   *  a value from receiving a rvalue-reference.
   * @see test-helper-variadic-test.cpp
   */
  template<typename... EMPTY>
  inline string
  showVariadicTypes ()
  {
    return " :.";
  }
  
  template<typename XX, typename... XS>
  inline string
  showVariadicTypes (XX&& x, XS&&... xs)
  {
    return " :---#"
         + util::toString (1 + sizeof...(xs))
         + " -- Type: "  + showType<XX&&>()
         + "  \tAdr"     + util::showAdr (x)
         + "\n"
         + showVariadicTypes (std::forward<XS>(xs)...);
  }
  
  
  
  
  
  
  /** create a random but not insane Time value between 1s ... 10min + 500ms */
  inline lib::time::Time
  randTime ()
  {
    return lib::time::Time (500 * rani(2), 1 + rani(600));
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
      
      friend ExpectString
      operator+ (std::string&& l, ExpectString&& r)
      {
        return ExpectString{(l+r).c_str()};
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
 * Macro to verify that a statement indeed raises an exception.
 * If no exception is thrown, the #NOTREACHED macro will trigger
 * an assertion failure. In case of exception, the #lumiera_error
 * state is checked, cleared and verified.
 */
#define VERIFY_ERROR(ERROR_ID, ERRONEOUS_STATEMENT)               \
          try                                                      \
            {                                                       \
              ERRONEOUS_STATEMENT ;                                  \
              NOTREACHED("expected »%s« failure in: %s",              \
                          #ERROR_ID, #ERRONEOUS_STATEMENT);            \
            }                                                           \
          catch (lumiera::Error& ex)                                     \
            {                                                             \
              CHECK (ex.getID()                                            \
                     == lib::test::ExpectString{LUMIERA_ERROR_##ERROR_ID} );\
              lumiera_error();                                               \
            }                                                                 \
          catch (...)                                                          \
            {                                                                   \
              CHECK (lumiera_error_peek()                                        \
                     == lib::test::ExpectString{LUMIERA_ERROR_##ERROR_ID} );      \
              lumiera_error();                                                     \
            }

/**
 * Macro to verify that a statement indeed raises a std::exception,
 * which additionally contains some FAILURE_MSG in its description.
 */
#define VERIFY_FAIL(FAILURE_MSG, ERRONEOUS_STATEMENT)   \
          try                                            \
            {                                             \
              ERRONEOUS_STATEMENT ;                        \
              NOTREACHED("expected »%s«-failure in: %s"     \
                        , FAILURE_MSG, #ERRONEOUS_STATEMENT);\
            }                                                 \
          catch (std::exception& sex)                          \
            {                                                   \
              CHECK (util::contains (sex.what(), FAILURE_MSG)    \
                    ,"expected failure with »%s« -- but got: %s"  \
                    ,FAILURE_MSG, sex.what());                     \
              lumiera_error();                                      \
            }                                                        \
          catch (...)                                                 \
            {                                                          \
              NOTREACHED("expected »%s«-failure, "                      \
                         "yet something scary happened instead...",      \
                          FAILURE_MSG);                                   \
            }


/**
 * Macro to mark the current test function in STDOUT.
 * This can be helpful to digest a long test output dump
 */
#define MARK_TEST_FUN \
          cout << "|" << endl << "|  »"<<__FUNCTION__<<"«" <<endl;

#endif /*LIB_TEST_TEST_HELPER_H*/
