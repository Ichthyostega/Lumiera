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
 ** Effectively this is a tailored and simplified version of what can be found in the Loki library.
 ** We use it in other generic library-style code to generate repetitive code. If you tend to find
 ** template metaprogramming (or functional programming in general) offending, please ignore the 
 ** technical details and just consider the benefit of such an simplification for the client code.
 **
 ** Interface for using this facility is the template Types(.....) for up to 20 Type parameters.
 ** To start typelist processing, other templates typically pick up the Types<...>::List type.
 ** This allows for LISP-style list processing, with a pattern match on either Node<TY,TYPES>
 ** or NullType to terminate recursion. In C++ template metaprogramming, "pattern match"
 ** is done by partial template specialisations (the compiler will pick up and thus
 ** match the template parameters). A typedef acts like a declaration in normal
 ** programming. Because such a "declaration" can't be changed after the fact,
 ** effectively this is a flavour of functional programming. Just the
 ** "execution environment" is the compiler, during compilation.
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
 ** @see lib::visitor::Applicable usage example
 ** @see control::CommandSignature more elaborate usage example (dissecting a functor signature)
 ** @see TypeList_test
 ** @see TypeListManip_test
 ** 
 */


#ifndef LIB_META_TYPELIST_H
#define LIB_META_TYPELIST_H




namespace lib {
namespace meta {
    
    struct NullType 
      { 
        typedef NullType List;
      };
    
    template<class H, class T> 
    struct Node
      {
        typedef Node List;
        typedef H Head;
        typedef T Tail;
      };
    
    typedef Node<NullType,NullType> NodeNull;
    
    
    
    template
      < class T01=NullType
      , class T02=NullType
      , class T03=NullType
      , class T04=NullType
      , class T05=NullType
      , class T06=NullType
      , class T07=NullType
      , class T08=NullType
      , class T09=NullType
      , class T10=NullType
      , class T11=NullType
      , class T12=NullType
      , class T13=NullType
      , class T14=NullType
      , class T15=NullType
      , class T16=NullType
      , class T17=NullType
      , class T18=NullType
      , class T19=NullType
      , class T20=NullType
      >
    class Types
      {
        typedef typename Types<      T02, T03, T04
                              , T05, T06, T07, T08
                              , T09, T10, T11, T12
                              , T13, T14, T15, T16
                              , T17, T18, T19, T20>::List ListTail;
      public:
        using List = Node<T01, ListTail>;
        using Seq  = Types;
      };
    
    template<> 
    struct Types<>
      {
        using List = NullType;
        using Seq  = Types<>;
      };
    
    
    
}} // namespace lib::meta
#endif
