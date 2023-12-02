/*
  TYPELIST-UTIL.hpp  -  simple helpers for working with lists-of-types

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
 ** embedded constant definitions (usually the enum constant named \c value)
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

namespace lib {
namespace meta {
    
    
    /**
     * Metafunction counting the number of Types in the collection
     * @return an embedded constant \c value holding the result
     */
    template<class TYPES>
    struct count;
    template<>
    struct count<NullType>
      {
        enum{ value = 0 };
      };
    template<class TY, class TYPES>
    struct count<Node<TY,TYPES>>
      {
        enum{ value = 1 + count<TYPES>::value };
      };
    
    
    /**
     * Metafunction " max( sizeof(T) ) for T in TYPES "
     */
    template<class TYPES>
    struct maxSize;
    template<>
    struct maxSize<NullType>
      {
        static constexpr int value = 0;
      };
    template<class TY, class TYPES>
    struct maxSize<Node<TY,TYPES>>
      {
        static constexpr size_t thisval = sizeof(TY);
        static constexpr size_t nextval = maxSize<TYPES>::value;
        static constexpr size_t value   = nextval > thisval?  nextval:thisval;
      };
    
    
    /**
     * Metafunction " max( alignof(T) ) for T in TYPES "
     */
    template<class TYPES>
    struct maxAlign;
    template<>
    struct maxAlign<NullType>
      {
        static constexpr int value = 0;
      };
    template<class TY, class TYPES>
    struct maxAlign<Node<TY,TYPES>>
      {
        static constexpr size_t thisval = alignof(TY);
        static constexpr size_t nextval = maxAlign<TYPES>::value;
        static constexpr size_t value   = nextval > thisval?  nextval:thisval;
      };
    
    
    /**
     * Metafunction to check if a specific type is contained
     * in a given typelist. Only exact match is detected.
     */
    template<typename TY, typename TYPES>
    struct IsInList
      {
        enum{ value = false };
      };
    
    template<typename TY, typename TYPES>
    struct IsInList<TY, Node<TY,TYPES>>
      {
        enum{ value = true };
      };
    
    template<typename TY, typename XX, typename TYPES>
    struct IsInList<TY, Node<XX,TYPES>>
      {
        enum{ value = IsInList<TY,TYPES>::value };
      };
    
    /** convenience shortcut: query function */
    template<typename TY, typename TYPES>
    constexpr bool
    isInList()
    {
      return IsInList<TY,TYPES>::value;
    }
    
    
    /**
     * Build a list of const types from a given typelist.
     */
    template<typename TYPES>
    struct ConstAll;
    
    template<>
    struct ConstAll<NullType>
      {
        typedef NullType List;
      };
    
    template<typename TY, typename TYPES>
    struct ConstAll<Node<TY,TYPES>>
      {
        typedef Node<const TY, typename ConstAll<TYPES>::List> List;
      };
    
    
    
}} // namespace lib::meta
#endif
