/*
  TYPESEQ-UTIL.hpp  -  basic metaprogramming utilities for type sequences

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file typeseq-util.hpp
 ** Some basic facilities for manipulating type sequences.
 ** While typelist.hpp provides the foundation of metaprogramming with typelists,
 ** manipulating the type sequences themselves (i.e. Types<T1,T2,...> instances)
 ** requires some additional helper templates supplemented here.
 ** - prepending to a type sequence
 ** - dissecting a type sequence
 ** - shifting a type sequence
 ** - re-generating a type sequence from a typelist.
 ** 
 ** @see typeseq-manip-test.cpp
 ** @see typelist.hpp
 ** @see typelist-util.hpp
 ** @see tuple-helper.hpp
 ** @see function.hpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_TYPESEQ_UTIL_H
#define LIB_META_TYPESEQ_UTIL_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/util.hpp"



namespace lib {
namespace meta {
  
  template<class T, class TYPES>
  struct Prepend;
  
  
  
  /* ========= Specialisations ========= */
  
  /**
   * Additional specialisation of the basic type sequence type,
   * allowing to re-create a (flat) type sequence from a typelist.
   * @remark recursively prepend-to-tail of the type sequence.
   */
  template<class H, class T>
  struct Types< Node<H,T> >
    {
      using List = Node<H,T>;
      using Seq  = Prepend< H
                          , typename Types<T>::Seq
                          >::Seq;
    };
  template<>
  struct Types<Nil>
    {
      using List = Nil;
      using Seq  = Types<>;
    };
  template<>
  struct Types<NilNode>
    : Types<Nil>
    { };
  
  
  
  
  
  
  
  /* ========= Information functions ========= */
  
  /**
   * Find the index of the first incidence of a type in a type-sequence.
   * @note static assertion if the type is not in the type sequence
   * @see https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type/60868425#60868425
   */
  template<class X>
  constexpr size_t
  indexOfType()
  {
    static_assert (not sizeof(X), "Type not found in type-sequence");
    return 0;
  }
  
  template<class X, class T, class... TYPES>
  constexpr size_t
  indexOfType()
  {
    if constexpr (std::is_same_v<X,T>)
      return 0;
    else
      return 1 + indexOfType<X,TYPES...>();
  }
  
  /** series of definitions to level across variadics,
   *  type-sequences and type-lists
   * @see typelist-util.hpp
   */
  template<class... TYPES>
  struct count
    : SizConst<sizeof...(TYPES)>
  { };
  template<class... TYPES>
  struct count<Types<TYPES...>>
    : SizConst<sizeof...(TYPES)>
  { };
  
  
  
  
  
  
  
  /* ========= Type Sequence Manipulation ========= */
  
  /**
   * Helper: prepend a type to an existing type sequence,
   * thus shifting all elements within the sequence
   * to the right, eventually dropping the last element
   */
  template<class T, class TYPES>
  struct Prepend;
  
  template<typename T, typename...TYPES>
  struct Prepend<T, Types<TYPES...>>
  {
    using Seq  = Types<T, TYPES...>;
    using List = Types<T, TYPES...>::List;
  };
  
  
  
  
  /** Helper: separate parts of a type sequence
   */
  template<class TYPES>
  struct Split;
  
  template<typename T1, typename...TS>
  struct Split<Types<T1,TS...> >
  {
    using List = Types<T1,TS...>::List;
    
    using Head  = T1;
    using First = Types<T1>;
    using Tail  = Types<TS...>;
    
    // for finding the end we need the help of typelist-util.hpp
    
    using PrefixList = PickLast<List>::List;
    using TailList   = Tail::List;
    
    using Prefix     = Types<PrefixList>::Seq;
    using End        = PickLast<List>::Type;
    using Last       = Types<End>;
  };
  
  template<>
  struct Split<Types<>>
  {
    using List  = Nil;
    
    using Head  = Nil;
    using First = Types<>;
    using Tail  = Types<>;
    
    // for finding the end we need the help of typelist-util.hpp
    
    using PrefixList = Nil;
    using TailList   = Nil;
    
    using Prefix     = Types<>;
    using Last       = Types<>;
    using End        = Nil;
  };
  
  
  
  
  /**
   * Helper: generate a type sequence left shifted
   * by i steps, filling in Nil at the end
   */
  template<class TYPES, uint i=1>
  class Shifted
    {
      using Tail = Split<TYPES>::Tail;
    public:
      using Type = Shifted<Tail,i-1>::Type;
      using Head = Split<Type>::Head;
    };
  
  template<class TYPES>
  struct Shifted<TYPES,0>
    {
      using Type = TYPES;
      using Head = Split<Type>::Head;       ///< @warning may be Nil in case of an empty list
    };
  
  
  
  /**
   * specialisation: pick n-th element from a type sequence
   * @see typelist-manip.hpp
   */
  template<typename...TYPES, size_t i>
  struct Pick<Types<TYPES...>, i>
    {
      using Type = Shifted<Types<TYPES...>, i>::Head;
    };
  
  
  
  /**
   * Generate a type-sequence filled with
   * \a N times the same type \a T
   */
  template<typename T, size_t N>
  struct Repeat
    {
      using Rem = Repeat<T, N-1>::Seq;
      using Seq = Prepend<T,Rem>::Seq;
    };
  
  template<typename T>
  struct Repeat<T,0>
    {
      using Seq = Types<>;
    };
  
  
  
}} // namespace lib::meta
#endif
