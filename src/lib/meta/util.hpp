/*
  UTIL.hpp  -  metaprogramming helpers and utilities

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file meta/util.hpp
 ** Simple and lightweight helpers for metaprogramming and type detection.
 ** This header is a collection of very basic type detection and metaprogramming utilities.
 ** @warning indirectly, this header gets included into the majority of compilation units.
 **          Avoid anything here which increases compilation times or adds much debugging info.
 ** 
 ** @see MetaUtils_test
 ** @see trait.hpp
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_UTIL_H
#define LIB_META_UTIL_H


#include <typeinfo>
#include <string>

namespace std {
  
  // forward declarations for common smart ptrs
  template<typename T>
  class shared_ptr;
  
  template <typename T, typename D>
  class unique_ptr;
  
}



namespace lib {
  class Literal;
  class Symbol;
  
namespace meta {
  
  /* === conditional definition selector === */
  
  template <bool B, class T = void>
  struct enable_if_c {
    typedef T type;
  };

  template <class T>
  struct enable_if_c<false, T> {};
  
  
  /** SFINAE helper to control the visibility of specialisations and overloads.
   * \par explanation
   * This template needs to be interspersed somehow into a type expression, which
   * is driven by an external, primary type parameter. Thus, it is possible to use
   * it on an extraneous, possibly default template parameter, or when forming the
   * return type of a function. The purpose is to remove a given definition from
   * sight, unless a boolean condition `Cond::value` holds true. In the typical
   * usage, this condition is suppled by a _metafunction_, i.e. a template, which
   * detects some feature or other circumstantial condition with the types involved.
   * @remarks this is a widely used facility, available both from boost and from
   *          the standard library. For the most common case, we roll our own
   *          variant here, which is slightly stripped down and a tiny bit
   *          more concise than the boost variant. This way, we can avoid
   *          a lot of boost inclusions, which always bear some weight.
   * @see [std::enable_if](http://en.cppreference.com/w/cpp/types/enable_if)
   */
  template <class Cond, class T = void>
  using enable_if = typename enable_if_c<Cond::value, T>::type;
  
  template <class Cond, class T = void>
  using disable_if = typename enable_if_c<not Cond::value, T>::type;
  
  
  
  
  /* === building metafunctions === */
  
  /** helper types to detect the overload resolution chosen by the compiler */
  
  typedef char Yes_t;
  struct No_t { char more_than_one[4]; };
  
  
  
  
  /** detect possibility of a conversion to string.
   *  Naive implementation just trying the direct conversion.
   *  The embedded constant #value will be true in case this succeeds.
   *  Might fail in more tricky situations (references, const, volatile)
   * @see \ref format-obj.hpp more elaborate solution including lexical_cast
   */
  template<typename X>
  struct can_convertToString
    {
      static X & probe();
      
      static Yes_t check(std::string);
      static No_t  check(...);
      
    public:
      static const bool value = (sizeof(Yes_t)==sizeof(check(probe())));
    };
  
  /** toggle for explicit specialisations */
  template<typename X>
  using enable_CustomStringConversion = enable_if<can_convertToString<X>>;
  
  
  
  /** strip const from type: naive implementation */
  template<typename T>
  struct UnConst
    {
      typedef T Type;
    };
  
  template<typename T>
  struct UnConst<const T>
    {
      typedef T Type;
    };
  template<typename T>
  struct UnConst<const T *>
    {
      typedef T* Type;
    };
  template<typename T>
  struct UnConst<T * const>
    {
      typedef T* Type;
    };
  template<typename T>
  struct UnConst<const T * const>
    {
      typedef T* Type;
    };
  
  
  
  /** Trait template for detecting a typelist type.
   *  For example, this allows to write specialisations
   *  with the help of lib::meta::enable_if
   *  (or the equivalent std::enable_if)
   */
  template<typename TY>
  class is_Typelist
    {
      template<class X>
      static Yes_t check(typename X::List *);
      template<class>
      static No_t  check(...);
      
    public:
      static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
    };
  
  
  /** Trait template to detect presence of a simple function call operator
   * @note this metafunction fails to detect an overloaded or templated `operator()`
   */
  template<typename FUN>
  class has_FunctionOperator
    {
      template<typename FF,
               typename SEL = decltype(&FF::operator())>
      struct Probe
        { };
      
      template<class X>
      static Yes_t check(Probe<X> * );
      template<class>
      static No_t  check(...);
      
    public:
      static const bool value = (sizeof(Yes_t)==sizeof(check<FUN>(0)));
    };
  
  
  
  
  
  
  
  
  /* ==== generic string representation ==== */
  
  /** pretty-print an internal C++ type representation
   * @see \ref format-obj.cpp implementation
   */
  std::string humanReadableTypeID (lib::Literal);
  
  /** extract core name component from a raw type spec
   * @return simple identifier possibly "the" type
   * @warning implemented lexically, not necessarily correct!
   */
  std::string primaryTypeComponent (lib::Literal);
  
  /** build a sanitised ID from full type name */
  std::string sanitisedFullTypeName(lib::Literal);
  
  /** condense a string and retain only valid identifiers
   * @return string starting with letter, followed by
   *         letters, digits and underscore
   */
  std::string sanitisedSymbol(std::string const&);
  
  /** reverse the effect of C++ name mangling.
   * @return string in language-level form of a C++ type or object name,
   *         or a string with the original input if demangling fails.
   * @warning implementation relies on the cross vendor C++ ABI in use
   *         by GCC and compatible compilers, so portability is limited.
   *         The implementation is accessed through libStdC++
   *         Name representation in emitted object code and type IDs is
   *         essentially an implementation detail and subject to change.
   */
  std::string demangleCxx (lib::Literal rawName);
  
  
  extern const std::string FUNCTION_INDICATOR;
  extern const std::string FAILURE_INDICATOR;
  extern const std::string BOTTOM_INDICATOR;
  extern const std::string VOID_INDICATOR;
  
  extern const std::string BOOL_FALSE_STR;
  extern const std::string BOOL_TRUE_STR;
  
  
  /** failsafe human readable type display
   * @return string representing the C++ type.
   * @remarks the purpose of this function is diagnostics
   *    and unit-testing. When possible, RTTI is exposed, otherwise
   *    the implementation falls back on the static type as seen by
   *    the compiler on usage site. An attempt is made to de-mangle
   *    and further simplify the type string, leaving out some common
   *    (hard wired) namespace prefixes, and stripping typical adornments
   *    like `const`, `*` and `&`
   * @remarks almost all calls will enter through the `const&` variant of this
   *    function, since C++ considers this best match in template substitution.
   *    Thus, we deliberately force calls with pointer to enter here, since we
   *    do want the pointer itself (and not a pointer to the pointer). We then
   *    pass the "object" as so called "glvalue" to the `typeid()` function,
   *    so to get the evaluation of RTTI, when applicable.
   * @warning this function does string transformations behind the scene,
   *    and thus should not be used in performance critical context. Moreover,
   *    the returned type string is not necessarily exact and re-parsable.
   */
  template<typename TY>
  inline std::string
  typeStr (TY const* obj =nullptr)  noexcept
    try {
      auto mangledType = obj? typeid(*obj).name()
                            : typeid(TY).name();
      return humanReadableTypeID (mangledType);
    }
    catch(...)
    { return FAILURE_INDICATOR; }
  
  template<typename TY>
  inline                   disable_if<std::is_pointer<TY>,
  std::string              >
  typeStr (TY const& ref)  noexcept
  {
    return typeStr (&ref);
  }
  
  inline std::string
  typeStr (void const*)  noexcept
  {
    return VOID_INDICATOR;
  }
  
  
  
  
  /** simple expressive symbol to designate a type
   * @return single word identifier, derived from the
   *    full type name, not necessarily correct or unique
   */
  template<typename TY>
  inline std::string
  typeSymbol (TY const* obj =nullptr)
  {
    auto mangledType = obj? typeid(*obj).name()
                          : typeid(TY).name();
    return primaryTypeComponent (mangledType);
  }
  
  template<typename TY>
  inline                   disable_if<std::is_pointer<TY>,
  std::string              >
  typeSymbol (TY const& ref)
  {
    return typeSymbol (&ref);
  }
  
}}// namespace lib::meta




namespace util {
  
  using lib::meta::typeStr;
  using lib::meta::FAILURE_INDICATOR;
  using lib::meta::BOTTOM_INDICATOR;
  
  
  /** failsafe invocation of custom string conversion.
   * @return string to represent the object, by default a [type display](\ref typeStr)
   * @remarks this is a lightweight solution to at least _get any human readable string
   *    representation for pretty much every language object._ This minimal solution
   *    is defined here, to allow for built-in diagnostics for custom types without
   *    the danger of creating much header inclusion and code size bloat. A more
   *    elaborate, [extended solution](lib::toString), including _lexical conversions
   *    for numbers,_ is defined in format-obj.hpp
   * @note any exceptions during string conversion are caught and silently ignored;
   *    the returned string indicates "↯" in this case.
   */
  template<typename X, typename COND =void>
  struct StringConv
    {
      static std::string
      invoke (X const& x)  noexcept
        try        { return "«"+typeStr(x)+"»"; }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  template<typename X>
  struct StringConv<X,     lib::meta::enable_CustomStringConversion<X>>
    {
      static std::string
      invoke (X const& val) noexcept
        try        { return std::string(val); }
        catch(...) { return FAILURE_INDICATOR; }
    };
  
  // NOTE: this is meant to be extensible;
  //       more specialisations are e.g. in format-obj.hpp
  
  
  
  
  /** pretty-print a double in fixed-point format */
  std::string showDouble (double) noexcept;
  std::string showFloat (float)   noexcept;
  
  /** pretty-print an address as hex-suffix */
  std::string showAddr (void const* addr) noexcept;
  
  template<typename X>
  inline std::string
  showAddr (X& elm)  noexcept
  {
    return showAddr(&elm);
  }
  
  
  /** diagnostics helper for explicitly indicating pointers */
  template<typename X>
  inline std::string
  showPtr (X* ptr =nullptr)
  {
    return ptr? showAddr(ptr) + " ↗" + StringConv<X>::invoke(*ptr)
              : BOTTOM_INDICATOR + " «" + typeStr(ptr) + "»";
  }
  
  template<typename SP>
  inline std::string
  showSmartPtr (SP const& smPtr, std::string label = "smP")
  {
    using TargetType = typename SP::element_type;
    
    return smPtr? label+"("+showAddr(smPtr.get()) + ") ↗" + StringConv<TargetType>::invoke(*smPtr)
                : BOTTOM_INDICATOR + " «" + typeStr(smPtr) + "»";
  }
  
  /** human readable display of boolean values
   * @return "`true`" or "`false`"
   */
  inline std::string
  showBool (bool yes)  noexcept
  {
    return yes? lib::meta::BOOL_TRUE_STR
              : lib::meta::BOOL_FALSE_STR;
  }
  
  
  
}// namespace util
#endif /*LIB_META_UTIL_H*/
