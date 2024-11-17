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
   * Helper: prepend a type to an existing type sequence,
   * thus shifting all elements within the sequence 
   * to the right, eventually dropping the last element
   */
  template<class T, class TYPES>
  struct Prepend;
  
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
  
  
  
  
  
  
  /** Helper: separate parts of a type sequence */
  template<class TYPES>
  struct Split;
  
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
  
  
  
  
  
}} // namespace lib::meta
#endif
