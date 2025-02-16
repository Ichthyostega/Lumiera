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
 ** @warning the metaprogramming part of Lumiera to deal with type sequences is in a
 **          state of transition, since C++11 now offers direct language support for
 **          processing of flexible template parameter sequences ("parameter packs").
 **          It is planned to regroup and simplify our homemade type sequence framework
 **          to rely on variadic templates and integrate better with std::tuple.
 **          It is clear that we will _retain some parts_ of our own framework,
 **          since programming with _Loki-style typelists_ is way more obvious
 **          and straight forward than handling of template parameter packs,
 **          since the latter can only be rebound through pattern matching.
 ** @todo transition lib::meta::Types to variadic parameters  /////////////////////////////////TICKET #987
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
  
  
  /**
   * Find the index of the first incidence of a type in a type-sequence.
   * @note static assertion if the type is not in the type sequence
   * @see https://stackoverflow.com/a/60868425/444796
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
  
  
  /** 
   * Helper: prepend a type to an existing type sequence,
   * thus shifting all elements within the sequence 
   * to the right, eventually dropping the last element
   *  @todo support variadic type-seq        ////////////////////////////////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic, then replace this by a single variadic template
   */
  template<class T, class TYPES>
  struct Prepend;
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : the following specialisation will be obsoleted by the removal of old-style type-sequences
  template< typename T01
          , typename T02
          , typename T03
          , typename T04
          , typename T05
          , typename T06
          , typename T07
          , typename T08
          , typename T09
          , typename T10
          , typename T11
          , typename T12
          , typename T13
          , typename T14
          , typename T15
          , typename T16
          , typename T17
          , typename T18
          , typename T19
          , typename T20
          , typename IGN
          >
  struct Prepend<T01, Types<     T02,T03,T04,T05
                           , T06,T07,T08,T09,T10
                           , T11,T12,T13,T14,T15
                           , T16,T17,T18,T19,T20
                           , IGN
                           > >
  {
    typedef Types< T01,T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19,T20 > Seq;
    
    typedef typename Seq::List          List;
  };
  
  
  
  /** 
   * Additional specialisation of the basic type sequence type,
   * allowing to re-create a (flat) type sequence from a typelist. 
   */
  template<class H, class T>
  struct Types< Node<H,T> >
    {
      typedef Node<H,T> List;
      
      typedef typename Prepend< H
                              , typename Types<T>::Seq
                              >::Seq  Seq;
    };
  
  
  
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND -- to be obsoleted
  /**
   * temporary workaround: additional specialisation for the template
   * `Prepend` to work also with the (alternative) variadic TySeq.
   * @see typeseq-util.hpp
   */
  template<typename T, typename...TYPES>
  struct Prepend<T, TySeq<TYPES...>>
  {
    using Seq  = TySeq<T, TYPES...>;
    using List = typename Types<T, TYPES...>::List;
  };
  
  template<class H, class T>
  struct TySeq< Node<H,T> >
    {
      using List = Node<H,T>;
      using Seq  = typename Prepend< H
                                   , typename TySeq<T>::Seq
                                   >::Seq;
    };
  template<>
  struct TySeq<NullType>
    {
      using List = NullType;
      using Seq  = TySeq<>;
    };
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND -- to be obsoleted
  
  
  /**
   * temporary workaround: strip trailing NullType entries from a
   * type sequence, to make it compatible with new-style variadic
   * template definitions.
   * @note the result type is a TySec, to keep it apart from our
   *    legacy (non-variadic) lib::meta::Types
   * @deprecated necessary for the transition to variadic sequences      ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
   */
  template<typename SEQ>
  struct StripNullType;
  
  template<typename T, typename...TYPES>
  struct StripNullType<Types<T,TYPES...>>
    {
      using TailSeq = typename StripNullType<Types<TYPES...>>::Seq;
      
      using Seq = typename Prepend<T, TailSeq>::Seq;
    };
  
  template<typename...TYPES>
  struct StripNullType<Types<NullType, TYPES...>>
    {
      using Seq = TySeq<>;  // NOTE: this causes the result to be a TySeq
    };
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : the following specialisation is a catch-all and becomes obsolete
  template<typename...TYPES>
  struct StripNullType<TySeq<TYPES...>>
    {
      using Seq = TySeq<TYPES...>;
    };
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND(End) -- to be obsoleted
  
  
  
  /** Helper: separate parts of a type sequence
   *  @todo support variadic type-seq        ////////////////////////////////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic, then replace this by a single variadic template
   */
  template<class TYPES>
  struct Split;
  
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : this specialisation handles the variadic case and will be the only definition in future
  template<typename T1, typename...TS>
  struct Split<TySeq<T1,TS...> >
  {
    using List = typename TySeq<T1,TS...>::List;
    
    using Head  = T1;
    using First = TySeq<T1>;
    using Tail  = TySeq<TS...>;
    
    // for finding the end we need the help of typelist-util.hpp
    
    using PrefixList = typename SplitLast<List>::List;
    using TailList   = typename Tail::List;
    
    using Prefix     = typename TySeq<PrefixList>::Seq;
    using End        = typename SplitLast<List>::Type;
    using Last       = TySeq<End>;
  };
  
  template<>
  struct Split<TySeq<>>
  {
    using List = NullType;
    
    using Head  = NullType;
    using First = TySeq<>;
    using Tail  = TySeq<>;
    
    // for finding the end we need the help of typelist-util.hpp
    
    using PrefixList = NullType;
    using TailList   = NullType;
    
    using Prefix     = TySeq<>;
    using Last       = TySeq<>;
    using End        = NullType;
  };
                          ///////////////////////////////////////////////////////////////////////////////////TICKET #987 : the following specialisation will be obsoleted by the removal of old-style type-sequences
  template< typename T01
          , typename T02
          , typename T03
          , typename T04
          , typename T05
          , typename T06
          , typename T07
          , typename T08
          , typename T09
          , typename T10
          , typename T11
          , typename T12
          , typename T13
          , typename T14
          , typename T15
          , typename T16
          , typename T17
          , typename T18
          , typename T19
          , typename T20
          >
  struct Split<Types< T01,T02,T03,T04,T05
                    , T06,T07,T08,T09,T10
                    , T11,T12,T13,T14,T15
                    , T16,T17,T18,T19,T20
                    > >
  {
    typedef typename 
            Types< T01,T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19,T20
                 >::List                 List;
    
    typedef        T01                   Head;
    typedef Types< T01                 > First;
    typedef Types<     T02,T03,T04,T05
                 , T06,T07,T08,T09,T10
                 , T11,T12,T13,T14,T15
                 , T16,T17,T18,T19,T20 > Tail;

    // for finding the end we need the help of typelist-util.hpp
    
    typedef typename SplitLast<List>::List  PrefixList;
    typedef typename Tail::List             TailList;
    
    typedef typename Types<PrefixList>::Seq Prefix;
    typedef typename SplitLast<List>::Type  End;
    typedef Types<End>                      Last;
  };
  
  
  
  
  /**
   * Helper: generate a type sequence left shifted 
   * by i steps, filling in NullType at the end
   */  
  template<class TYPES, uint i=1>
  class Shifted
    {
      typedef typename Split<TYPES>::Tail Tail;
    public:
      typedef typename Shifted<Tail,i-1>::Type Type;
      typedef typename Split<Type>::Head       Head;
    };
  
  template<class TYPES>
  struct Shifted<TYPES,0>
    { 
      typedef TYPES                      Type;
      typedef typename Split<Type>::Head Head;
    };
  
  
  
  /** 
   * specialisation: pick n-th element from a type sequence
   * @see typelist-manip.hpp
   */
  template<typename...TYPES, size_t i>
  struct Pick<Types<TYPES...>, i>
    {
      using Type = typename lib::meta::Shifted<Types<TYPES...>, i>::Head;
    };
  template<typename...TYPES, size_t i>
  struct Pick<TySeq<TYPES...>, i>
    {
      using Type = typename Shifted<TySeq<TYPES...>, i>::Head;
    };
  
  
  
  
  
}} // namespace lib::meta
#endif
