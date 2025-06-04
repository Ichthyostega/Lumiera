/*
  TYPELIST.hpp  -  typelist meta programming facilities

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

====================================================================
This code is heavily inspired by
 The Loki Library (loki-lib/trunk/include/loki/Sequence.h)
    Copyright (c) 2001 by Andrei Alexandrescu
    Copyright (c) 2005 by Peter Kümmel
    This Loki code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming
        and Design Patterns Applied".
        Copyright (c) 2001. Addison-Wesley. ISBN 0201704315

  Loki Copyright Notice:
  Permission to use, copy, modify, distribute and sell this software for any
  purpose is hereby granted without fee, provided that the above copyright
  notice appear in all copies and that both that copyright notice and this
  permission notice appear in supporting documentation.
  The author makes no representations about the suitability of this software
  for any purpose. It is provided "as is" without express or implied warranty.
*/


/** @file typelist.hpp
 ** A template metaprogramming technique for manipulating collections of types.
 ** - **Type Sequences** are used to transport a variadic sequence of types
 ** - **Type Lists** (»Loki style«) can be created from a type sequence and
 **   represent the same information by _nodes_ with _head_ and _tail_.
 ** The latter brings LISP style recursive manipulation techniques into the
 ** realm of type metaprogramming; this approach was pioneered with the
 ** **Loki Library** by Alexandrescu (2001) and makes complex processing
 ** much easier to write  and to understand following. Effectively the set
 ** of definitions used here is a tailored version of what could be found
 ** in the Loki library, and was in the following years integrated with
 ** processing of variadics, function manipulation and std::tuple.
 ** This framework is typically used to generate repetitive code,
 ** based on a collection of types, with type-specific adaptations.
 ** @remark If you tend to find the use of template metaprogramming detrimental
 **         (or functional programming and generally any kind of abstraction)
 **         please kindly ignore the technical details and just consider the
 **         benefit of simplification for the client code.
 ** 
 ** Interface for using this facility is the template `Types<TS...>` (the type sequence).
 ** To start typelist processing, other templates typically pick up the Types<...>::List type.
 ** This allows for LISP-style list processing, with a pattern match on either `Node<TY,TYPES>`
 ** or the type `Nil` to terminate recursion. In C++ template metaprogramming, "pattern match"
 ** is done by partial template specialisations (the compiler will pick up and thus
 ** match the template parameters). A type definition acts like a variable declaration
 ** in regular programming. Because such a "declaration" can't be changed after the fact,
 ** effectively this is a flavour of functional programming; the _execution environment_
 ** is the compiler, and evaluation is set off by some template instantiation.
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
 ** @see TypeList_test
 ** @see TypeListManip_test
 ** @see TypeSecManip_test
 ** @see function.hpp
 ** @see tuple-helper.hpp
 ** @see variadic-helper.hpp
 ** 
 */


#ifndef LIB_META_TYPELIST_H
#define LIB_META_TYPELIST_H




namespace lib {
namespace meta {
  
  /** »Empty« mark */
  struct Nil
    {
      using List = Nil;
    };
  
  /** Type list with head and tail;
   *  T ≡ Nil marks list end. */
  template<class H, class T>
  struct Node
    {
      using List = Node;
      using Head = H;
      using Tail = T;
    };
  
  using NilNode = Node<Nil,Nil>;
  
  
  
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 : this is the old non-variadic definition from lib Loki -- it will be obsoleted with the transition
    template
      < class T01=Nil
      , class T02=Nil
      , class T03=Nil
      , class T04=Nil
      , class T05=Nil
      , class T06=Nil
      , class T07=Nil
      , class T08=Nil
      , class T09=Nil
      , class T10=Nil
      , class T11=Nil
      , class T12=Nil
      , class T13=Nil
      , class T14=Nil
      , class T15=Nil
      , class T16=Nil
      , class T17=Nil
      , class T18=Nil
      , class T19=Nil
      , class T20=Nil
      >
    class TyOLD
      {
        typedef typename TyOLD<      T02, T03, T04
                              , T05, T06, T07, T08
                              , T09, T10, T11, T12
                              , T13, T14, T15, T16
                              , T17, T18, T19, T20>::List ListTail;
      public:
        using List = Node<T01, ListTail>;
        using Seq  = TyOLD;
      };
    
    template<>
    struct TyOLD<>
      {
        using List = Nil;
        using Seq  = TyOLD<>;
      };
    
    
   //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #987 temporary WORKAROUND -- transition to variadic type-sequences
  /**
   * temporary workaround:
   * alternative definition of "type sequence",
   * already using variadic template parameters.
   * @remarks the problem with our existing type sequence type
   *    is that it fills the end of each sequence with Nil,
   *    which was the only way to get a flexible type sequence
   *    prior to C++11. Unfortunately these trailing Nil
   *    entries do not play well with other variadic defs.
   * @deprecated when we switch our primary type sequence type
   *    to variadic parameters, this type will be obsoleted.             ////////////////////////////////////TICKET #987 : make lib::meta::Types<TYPES...> variadic
   * @todo 6/25 the transition is now mostly settled
   *       and will be completed by just _renaming_ this
   *       definition back into `Types<...>`
   */
  template<typename...TYPES>
  struct TySeq;
  
  template<typename T, typename...TS>
  struct TySeq<T,TS...>
    {
      using List = Node<T, typename TySeq<TS...>::List>;
      using Seq  = TySeq;
    };
  
  template<>
  struct TySeq<>
    {
      using List = Nil;
      using Seq  = TySeq<>;
    };
  
}} // namespace lib::meta
#endif
