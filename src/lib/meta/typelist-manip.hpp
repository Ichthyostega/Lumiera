/*
  TYPELIST-MANIP.hpp  -  Utils for working with lists-of-types

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file typelist-manip.hpp
 ** Metaprogramming: Helpers for manipulating lists-of-types.
 ** Sometimes, we use metaprogramming to generate a variation of concrete
 ** implementations by combining some basic building blocks. Typically, there
 ** is a number of similar, but not suitably related types involved. We want to
 ** process those types using a common scheme, without being forced to squeeze
 ** all those types into a artificial inheritance relationship. Instead, generating
 ** some kind of common factory or adapter, while mixing in pieces of code tailored
 ** specifically to the individual types, allows still to build a common processing
 ** in such situations.
 ** 
 ** The facilities in this header provide the basics of simple functional list
 ** processing (mostly with tail recursion). Usually, there is one template parameter
 ** TYPES, which accepts a _type-list_. The result of the processing step is then
 ** accessible as nested type definition named `List`. Here, all of the „processing“
 ** to calculate this result is performed by the compiler, as a side-effect set
 ** off by the need to compute a requested concrete result type. At run time,
 ** in the generated code, typically the resulting classes are empty, maybe
 ** just exposing a function outfitted for a specific purpose.
 ** 
 ** @remark historically, these functions were developed for the first draft of
 **         the render engine, which (for other reasons) did not come to fruition.
 **         Over time, some of these functions were used on and off for various
 **         purposes; and since (as of 2025) Loki-style type lists are considered
 **         an integral part of the Lumiera framework, useful meta-functions are
 **         added and documented here, even when not currently in use.
 ** 
 ** @see generator.hpp
 ** @see typelist-manip-test.cpp
 ** @see TimeControl_test usage example
 ** @see typelist.hpp
 ** 
 */


#ifndef LIB_META_TYPELIST_MANIP_H
#define LIB_META_TYPELIST_MANIP_H



#include "lib/meta/typelist.hpp"

#include <sys/types.h>

namespace lib {
namespace meta {
    
    
    /** pick the n-th element from a typelist */
    template<class TYPES, size_t i>
    struct Pick
      {
        using Type = Nil;
      };
    template<class TY, class TYPES>
    struct Pick<Node<TY,TYPES>, 0>
      {
        using Type = TY;
      };
    template<class TY, class TYPES, size_t i>
    struct Pick<Node<TY,TYPES>, i>
      {
        using Type = Pick<TYPES, i-1>::Type;
      };
    
    
    
    
    /** apply a transformation (template) to each type in the list */
    template<class TY, template<class> class _TRANS_>
    struct Apply                           { using List = TY; };
    
    template< class TY, class TYPES
            , template<class> class _TRANS_
            >
    struct Apply<Node<TY,TYPES>, _TRANS_ > { using List = Node< typename _TRANS_<TY>::Type
                                                              , typename Apply<TYPES,_TRANS_>::List
                                                              >;};
    
    
    /** conditional node: skip an element based on evaluating a predicate */
    template<bool, class T, class TAIL>
    struct CondNode                        { using Next = TAIL; };
    
    template<class T, class TAIL>
    struct CondNode<true, T, TAIL>         { using Next = Node<T,TAIL>; };
    
    /** filter away those types which don't fulfil a predicate metafunction */
    template< class TYPES
            , template<class> class _P_    ///< a template providing a boolean member \c ::value
            >
    struct Filter;
    
    template<template<class> class _P_>
    struct Filter<Nil,_P_>                 { using List = Nil; };
    
    template< class TY, class TYPES
            , template<class> class _P_
            >
    struct Filter<Node<TY,TYPES>,_P_>      { using List = CondNode< _P_<TY>::value
                                                                  , TY
                                                                  , typename Filter<TYPES,_P_>::List
                                                                  >::Next
                                                                  ; };
    
    
    /** append (concatenate) lists-of-types */
    template<class TY1, class TY2>
    struct Append                          { using List = Node<TY1, typename Append<TY2,Nil>::List>; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Append<Node<TY,TYPES>, TAIL>    { using List = Node<TY,  typename Append<TYPES,  TAIL>::List>; };
    
    template<class TY, class TYPES>
    struct Append<Nil, Node<TY,TYPES>>     { using List = Node<TY,TYPES>; };
    
    template<class TY, class TYPES>
    struct Append<Node<TY,TYPES>, Nil>     { using List = Node<TY,TYPES>; };
    
    template<class TY1>
    struct Append<TY1,Nil>                 { using List = Node<TY1,Nil>; };
    
    template<class TY2>
    struct Append<Nil,TY2>                 { using List = Node<TY2,Nil>; };
    
    template<>
    struct Append<Nil,Nil>                 { using List = Nil; };
    
    
    
    
    /** access the last list element */
    template<class TYPES>
    struct PickLast;
    
    template<>
    struct PickLast<Nil>                   { using Type = Nil;
                                             using List = Nil; };
    template<class TY>
    struct PickLast<Node<TY,Nil>>          { using Type = TY;
                                             using List = Nil; };
    
    template<class TY, class TYPES>
    struct PickLast<Node<TY,TYPES>>        { using Type = PickLast<TYPES>::Type;
                                             using List = Append< TY
                                                                , typename PickLast<TYPES>::List
                                                                >::List
                                                                ; };
    
    
    
    /**
     * splice some typelist like an overlay
     * into a base typelist, starting at given index.
     * @return either the combined (spliced) List, or
     *         the Front/Back part before or after the Overlay
     * @remark can be used to _exchange_ some elements in a list,
     *         without shifting the position of the other ones.
     * @note using a Nil-type as OVERLAY allows to extract
     *         an arbitrary Front/Back part of the list
     */
    template<class BASE, class OVERLAY, uint i=0>
    struct Splice;
    
    template<class B, class BS,
             class OVERLAY,
             uint i>
    struct Splice<Node<B,BS>, OVERLAY, i>  { using List  = Node<B, typename Splice<BS, OVERLAY, i-1>::List>;
                                             using Front = Node<B, typename Splice<BS, OVERLAY, i-1>::Front>;
                                             using Back  =         typename Splice<BS, OVERLAY, i-1>::Back; };
    
    template<class B, class BS,
             class O, class OS >
    struct Splice<Node<B,BS>,Node<O,OS>,0> { using List  = Node<O, typename Splice<BS,OS, 0>::List>;
                                             using Front = Nil;
                                             using Back  =         typename Splice<BS,OS, 0>::Back; };
    
    template<class B, class BS>
    struct Splice<Node<B,BS>, Nil, 0>      { using List  = Node<B, BS>;
                                             using Front = Nil;
                                             using Back  = Node<B, BS>; };
    
    template<class XX, uint i>
    struct Splice<Nil, XX, i>              { using List  = Nil;
                                             using Front = Nil;
                                             using Back  = Nil; };
    
    /** extract prefix of given length */
    template<class LI, uint l>
    using Prefix = Splice<LI, Nil, l>::Front;
    
    /** extract suffix starting at given pos */
    template<class LI, uint p>
    using Suffix = Splice<LI, Nil, p>::Back;
    
    
    
    
    /**
     * Allows to access various parts of a given typelist:
     * Start and End, Prefix and Tail...
     */
    template<class TYPES>
    struct Dissect;
    
    template<class T, class TYPES>
    struct Dissect<Node<T,TYPES>>
      {
        using   List = Node<T,TYPES>;                     ///< the complete list
        using   Head = T;                                 ///< first element
        using  First = Node<T,Nil>;                       ///< a list containing the first element
        using   Tail = TYPES;                             ///< remainder of the list starting with the second elm.
        using Prefix = PickLast<List>::List;              ///< all of the list, up to but excluding the last element
        using    End = PickLast<List>::Type;              ///< the last element
        using   Last = Node<End,Nil>;                     ///< a list containing the last element
      };
    
    template<>
    struct Dissect<Nil>
      {
        using   List = Nil;
        using   Head = Nil;
        using  First = Nil;
        using   Tail = Nil;
        using Prefix = Nil;
        using    End = Nil;
        using   Last = Nil;
      };
    
    
    
    
    /**
     * prefix each of the elements,
     * yielding a list-of lists-of-types
     */
    template<class T, class TY>
    struct PrefixAll                       { using List = Node< typename Append<T,TY>::List, Nil>; };
    
    template<class T>
    struct PrefixAll<T, Nil>               { using List = Nil; };
    
    template<class T>
    struct PrefixAll<T, NilNode>           { using List = Node< typename Append<T, NilNode>::List, Nil>; };
    
    template< class T
            , class TY, class TYPES
            >
    struct PrefixAll<T, Node<TY,TYPES>>    { using List = Node< typename Append<T,TY>::List
                                                              , typename PrefixAll<T,TYPES>::List
                                                              >;};
    
    
    
    
    /**
     * build a list-of lists, where each element of the first arg list
     * gets in turn prepended to all elements of the second arg list.
     * Can be used to build all possible combinations from two
     * sources, i.e. the Cartesian product.
     */
    template<class TY1,class TY2>
    struct Distribute                      { using List = PrefixAll<TY1,TY2>::List; };
    
    template<class TY>
    struct Distribute<Nil,TY>              { using List = Nil; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Distribute<Node<TY,TYPES>,TAIL> { using List = Append< typename PrefixAll<TY,TAIL>::List
                                                                , typename Distribute<TYPES,TAIL>::List
                                                                >::List
                                                                ; };
    
    
    
    /**
     * Build all possible combinations, based on a enumeration of the basic cases.
     * For each of the types in the argument list, an "enumeration generator" template is invoked,
     * to produce a list of the possible base cases. These base cases are then combined with all the
     * combinations of the rest, yielding all ordered combinations of all cases. Here, "ordered"
     * means that the base cases of the n-th element will appear in the n-th position of the
     * resulting lists,
     * @tparam X a type or type list to drive the tabulation process
     * @tparam _ENUM_ a case generator meta function, which takes one type from the input(list)
     *                and generates a type-list of cases, which are then feed to `Distribute`
     * @remark For the typical example, the "base cases" are {flag(on), flag(off)}, so we get a
     *         list-of-lists, featuring all possibilities to combine these distinct toggles.
     */
    template< class X
            , template<class> class _ENUM_>
    struct Combine                         { using List = Distribute< typename _ENUM_<X>::List
                                                                    , NilNode
                                                                    >::List; };
    template< template<class> class _ENUM_>
    struct Combine<Nil, _ENUM_ >           { using List = NilNode; };
    
    template< class TY, class TYPES
            , template<class> class _ENUM_>
    struct Combine<Node<TY,TYPES>,_ENUM_>  { using List = Distribute< typename _ENUM_<TY>::List
                                                                    , typename Combine<TYPES,_ENUM_>::List
                                                                    >::List; };
    
    /** enumeration generator for the Combine metafunction,
     *  yielding an "on" and "off" case; the latter is
     *  represented by a list with a Nil-Entry
     */
    template<class F>
    struct FlagOnOff
      {
        using List = Node<F, NilNode>;
      };
    
    
    /** generate all possible on-off combinations of the given flags.
     * @remark for the «off» case, the flag entry is not present */
    template<class FLAGS>
    struct CombineFlags
      {
        using List = Combine<FLAGS, FlagOnOff>::List;
      };
    
    
    
    
}} // namespace lib::meta
#endif
