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


/** @file util.hpp
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

namespace std { // forward declaration for std::string...
  
  template<typename C>
  struct char_traits;
  
  template<typename T>
  class allocator;

  template<typename c, class TRAIT, class _ALLO>
  class basic_string;
  
  using string = basic_string<char, char_traits<char>, allocator<char>>;
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
  template<typename T>
  struct can_convertToString
    {
      static T & probe();
      
      static Yes_t check(std::string);
      static No_t  check(...);
      
    public:
      static const bool value = (sizeof(Yes_t)==sizeof(check(probe())));
    };
  
  
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
   *  For example, this allows to write specialisations with the help of
   *  boost::enable_if
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
  
  
  
  
  
  
  /* ==== generic string representation ==== */
  
  /** pretty-print an internal C++ type representation
   * @see \ref format-obj.cpp implementation
   */
  std::string humanReadableTypeID (lib::Literal);
  
  std::string demangleCxx (lib::Literal rawName);
  
  
  
  /** failsafe human readable type display
   * @return string representing the C++ type.
   * @remarks the purpose of this function is diagnostics
   *    and unit-testing. When possible, RTTI is exposed, otherwise
   *    the implementation falls back on the static type as seen by
   *    the compiler on usage site. An attempt is made to de-mangle
   *    and further simplify the type string, leaving out some common
   *    (hard wired) namespace prefixes, and stripping typical adornments
   *    like `const`, `*` and `&`
   * @warning this function does string transformations behind the scenes,
   *    and thus should not be used in performance critical context. Moreover,
   *    the returned type string is not necessarily exact and re-parsable.
   */
  template<typename TY>
  inline std::string
  typeStr (const TY* obj=nullptr)
  {
    auto mangledType = obj? typeid(obj).name()
                          : typeid(TY).name();
    return humanReadableTypeID (mangledType);
  }
  
  template<typename TY>
  inline std::string
  typeStr (TY const& ref)
  {
    return typeStr (&ref);
  }
  
  
  
}} // namespace lib::meta
#endif
