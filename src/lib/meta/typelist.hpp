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
 ** much easier to write and to follow for the reader. Effectively the set
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
  
  

  /** variadic sequence of types */
  template<typename...TYPES>
  struct Types;
  
  template<typename T, typename...TS>
  struct Types<T,TS...>
    {
      using List = Node<T, typename Types<TS...>::List>;
      using Seq  = Types;
    };
  
  template<>
  struct Types<>
    {
      using List = Nil;
      using Seq  = Types<>;
    };
  
  
  
  
  /**
   * Concept to require a generic type sequence `Types<TYPES...>`
   */
  template<class TSEQ>
  concept typeseq = requires (TSEQ seq)
    {
      // TSEQ can invoke a function with a Types<TYPES> subclass
      []<typename...TYPES>(Types<TYPES...>&){/*placeholder*/}(seq);
    };
  
  template<class TSEQ>
  static constexpr bool is_Typeseq_v = typeseq<TSEQ>;
  
}} // namespace lib::meta
#endif
