/*
  GENERATOR.hpp  -  metaprogramming utilities for generating classes and interfaces

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


/** @file generator.hpp
 ** Helpers for working with lib::meta::Types (i.e. lists-of-types). 
 ** The main purpose is to build interfaces and polymorphic implementations
 ** (using virtual functions) based on templated Types or Collections of types,
 ** which is not possible without Template Metaprogramming.
 ** 
 ** The facilities in this header work by instantiating another template,
 ** which is passed in as (template template) parameter, for each of a
 ** given sequence of types. What varies is the way how this "for each"
 ** instantiation is mixed or inherited into the resulting product. 
 ** 
 ** @see generator-test.cpp
 ** @see lumiera::query::ConfigRules usage example
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_GENERATOR_H
#define LIB_META_GENERATOR_H

#include "lib/meta/typelist.hpp"



namespace lib {
namespace meta{
    
    /** 
     * Apply a template to a collection of types. 
     * The resulting class ends up inheriting from an instantiation
     * of the template for each of the types in the list. The inheritance
     * graph is built in a "mixin" (multiple inheritance) style. 
     */
    template
      < class TYPES                  // List of Types 
      , template<class> class _X_   //  your-template-goes-here
      , class BASE = NullType      //   Base class at end of chain
      >
    class InstantiateForEach;
    
    
    template<template<class> class _X_, class BASE>
    class InstantiateForEach<NullType, _X_, BASE>
      : public BASE
      { 
      public:
        typedef BASE     Unit;
        typedef NullType Next;
      };
    
      
    template
      < class TY, typename TYPES
      , template<class> class _X_
      , class BASE
      >
    class InstantiateForEach<Node<TY, TYPES>, _X_, BASE> 
      : public _X_<TY>,
        public InstantiateForEach<TYPES, _X_, BASE>
      { 
      public:
        typedef _X_<TY> Unit;
        typedef InstantiateForEach<TYPES,_X_> Next;
      };
    
    
    /** Helper to just inherit from the given type(s) */
    template<typename T>
    struct InheritFrom : T
      { };
    
    
    
    
    /** 
     * Build a single inheritance chain of template instantiations.
     * Needs the help of the user provided Template, which now has
     * to take a second parameter and use this as Base class. 
     * The resulting class ends up (single) inheriting from an 
     * instantiation of the template for each of the types, while
     * overriding/implementing the provided base class.
     */
    template
      < class TYPES                      // List of Types 
      , template<class,class> class _X_ //  your-template-goes-here
      , class BASE = NullType          //   Base class at end of chain
      >
    class InstantiateChained;
    
    
    template<template<class,class> class _X_, class BASE>
    class InstantiateChained<NullType, _X_, BASE>
      : public BASE
      { 
      public:
        typedef BASE     Unit;
        typedef NullType Next;
      };
    
      
    template
      < class TY, typename TYPES
      , template<class,class> class _X_
      , class BASE
      >
    class InstantiateChained<Node<TY, TYPES>, _X_, BASE> 
      : public _X_< TY
                  , InstantiateChained<TYPES, _X_, BASE>
                  >
      { 
      public:
        typedef InstantiateChained<TYPES,_X_,BASE> Next;
        typedef _X_<TY,Next> Unit;
      };
    
    
    
    /** 
     * A Variation of InstantiateChained providing an incremented
     * Index value template parameter. This index can e.g. be used
     * to store pointers in a dispatcher table in the Base class.
     * Similar to InstantiateChained, this template builds a linear
     * chain of inheritance. The user-provided template, which is
     * to be instantiated for all types in the Typelist, now has to
     * accept an additional third parameter (uint i).
     */
    template
      < class TYPES                           // List of Types
      , template<class,class,uint> class _X_ //  your-template-goes-here
      , class BASE = NullType               //   Base class at end of chain
      , uint i = 0                         //    incremented on each instantiation
      >
    class InstantiateWithIndex;
    
    
    template< template<class,class,uint> class _X_
            , class BASE
            , uint i
            >
    class InstantiateWithIndex<NullType, _X_, BASE, i>
      : public BASE
      { 
      public:
        typedef BASE     Unit;
        typedef NullType Next;
        enum{ COUNT = i };
      };
    
    
    template
      < class TY, typename TYPES
      , template<class,class,uint> class _X_
      , class BASE
      , uint i
      >
    class InstantiateWithIndex<Node<TY, TYPES>, _X_, BASE, i> 
      : public _X_< TY
                  , InstantiateWithIndex<TYPES, _X_, BASE, i+1 >
                  , i
                  >
      { 
      public:
        typedef InstantiateWithIndex<TYPES,_X_,BASE,i+1> Next;
        typedef _X_<TY,Next,i> Unit;
        enum{ COUNT = Next::COUNT };
      };
    
    
}} // namespace lib::meta
#endif
