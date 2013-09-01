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


#include <string>


namespace lib {
namespace meta {
  
  
  /* types for figuring out the overload resolution chosen by the compiler */
  
  typedef char Yes_t;
  struct No_t { char more_than_one[4]; };
  
  
  
  
  /** Compile-time Type equality:
   *  Simple Trait template to pick up types considered
   *  \em identical by the compiler.
   * @warning identical, not sub-type!
   */
  template<typename T1, typename T2>
  struct is_sameType
    {
      static const bool value = false;
    };
  
  template<typename T>
  struct is_sameType<T,T>
    {
      static const bool value = true;
    };
  
  
  /** detect possibility of a conversion to string.
   *  Naive implementation just trying the direct conversion.
   *  The embedded constant #value will be true in case this succeeds.
   *  Might fail in more tricky situations (references, const, volatile)
   * @see string-util.hpp more elaborate solution including lexical_cast
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
  
  
  
  /** semi-automatic detection if an instantiation is possible.
   *  Requires help by the template to be tested, which needs to define
   *  a typedef member \c is_defined. The embedded metafunction Test can be used
   *  as a predicate for filtering types which may yield a valid instantiation
   *  of the candidate template in question.
   *  \par
   *  A fully automated solution for this problem is impossible by theoretic reasons.
   *  Any non trivial use of such a \c is_defined trait would break the "One Definition Rule",
   *  as the state of any type can change from "partially defined" to "fully defined" over
   *  the course of any translation unit. Thus, even if there may be a \em solution out there,
   *  we can expect it to break at some point by improvements/fixes to the C++ Language.
   */
  template<template<class> class _CandidateTemplate_>
  struct Instantiation
    {
      
      template<class X>
      class Test
        {
          typedef _CandidateTemplate_<X> Instance;
          
          template<class U>
          static Yes_t check(typename U::is_defined *);
          template<class U>
          static No_t  check(...);
          
        public:
          static const bool value = (sizeof(Yes_t)==sizeof(check<Instance>(0)));
        };
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
  
  
  
}} // namespace lib::meta
#endif
