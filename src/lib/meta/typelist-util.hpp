/*
  TYPELIST-UTIL.hpp  -  simple helpers for working with lists-of-types

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file typelist-util.hpp
 ** Metaprogramming: simple helpers for working with lists-of-types.
 ** This header provides some very basic "meta functions" for extracting
 ** pieces of information from a list-of-types. In Lumiera, we use template
 ** metaprogramming and especially such lists-of-types, whenever we build
 ** some common implementation backbone, without being able to subsume all
 ** participating types (classes) into a single inheritance hierarchy.
 ** 
 ** The "meta functions" defined here are templates; to access the "result" of
 ** such a meta function, we instantiate the template and then access one of the
 ** embedded constant definitions (usually the compile-time constant named `value`)
 ** 
 ** @see generator.hpp
 ** @see TypelistUtil_test
 ** @see lib::SimpleAllocator usage example (for isInList)
 ** @see TypelistManip_test
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_TYPELIST_UTIL_H
#define LIB_META_TYPELIST_UTIL_H



#include "lib/meta/typelist.hpp"
#include "lib/meta/util.hpp"

#include <algorithm>

namespace lib {
namespace meta {
    
    
    /**
     * Metafunction counting the number of Types in the collection
     * @return an std::integral_constant type, which can be used
     *         as constexpr value.
     * @note typeseq-util.hpp defines a specialisation for type-seq
     */
    template<class... TYPES>
    struct count;
    template<>
    struct count<Nil>
      : SizConst<0>
      { };
    template<class TY, class TYPES>
    struct count<Node<TY,TYPES>>
      : SizConst<1 + count<TYPES>()>
      { };
    
    
    /**
     * Metafunction " max( sizeof(T) ) for T in TYPES "
     */
    template<class TYPES>
    struct maxSize;
    template<>
    struct maxSize<Nil>
      : SizConst<0>
      { };
    template<class TY, class TYPES>
    struct maxSize<Node<TY,TYPES>>
      : SizConst<std::max (sizeof(TY), maxSize<TYPES>::value)>
      { };
    
    
    /**
     * Metafunction " max( alignof(T) ) for T in TYPES "
     */
    template<class TYPES>
    struct maxAlign;
    template<>
    struct maxAlign<Nil>
      : SizConst<0>
      { };
    template<class TY, class TYPES>
    struct maxAlign<Node<TY,TYPES>>
      : SizConst<std::max (alignof(TY), maxAlign<TYPES>::value)>
      { };
    
    
    /**
     * Metafunction to check if a specific type is contained
     * in a given typelist. Only exact match is detected.
     */
    template<typename TY, typename TYPES>
    struct isInList
      : std::false_type
      { };
    
    template<typename TY, typename TYPES>
    struct isInList<TY, Node<TY,TYPES>>
      : std::true_type
      { };
    
    template<typename TY, typename XX, typename TYPES>
    struct isInList<TY, Node<XX,TYPES>>
      : std::bool_constant<isInList<TY,TYPES>::value>
      { };
    
    
    
    /**
     * Build a list of const types from a given typelist.
     */
    template<typename TYPES>
    struct ConstAll;
    
    template<>
    struct ConstAll<Nil>
      {
        using List = Nil;
      };
    
    template<typename TY, typename TYPES>
    struct ConstAll<Node<TY,TYPES>>
      {
        using List = Node<std::add_const_t<TY>, typename ConstAll<TYPES>::List>;
      };
    
    
    
}} // namespace lib::meta
#endif
