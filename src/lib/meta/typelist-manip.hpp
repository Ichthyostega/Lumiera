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
 ** all those types into a artificial inheritance relationship. Now, generating
 ** some kind of common factory or adapter, while mixing in pieces of code tailored
 ** specifically to the individual types, allows still to build a common processing
 ** in such situations.
 ** 
 ** The facilities in this header provide the basics of simple functional list
 ** processing (mostly with tail recursion). Usually, there is one template parameter
 ** TYPES, which accepts a \em Type-list. The result of the processing step is then
 ** accessible as an embedded typedef named \c List . Here, all of the 'processing'
 ** to calculate this result is performed by the compiler, as a side-effect of
 ** figuring out the resulting concrete type. At run time, in the generated
 ** code, typically the resulting classes are empty, maybe just
 ** exposing a specifically built-up function. 
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
        typedef NullType Type;  
      };
    template<class TY, class TYPES>
    struct Pick<Node<TY,TYPES>, 0>
      {
        typedef TY Type;  
      };
    template<class TY, class TYPES, size_t i>
    struct Pick<Node<TY,TYPES>, i>
      {
        typedef typename Pick<TYPES, i-1>::Type Type;
      };
    
    
    
    
    /** apply a transformation (template) to each type in the list */
    template<class TY, template<class> class _TRANS_>
    struct Apply                           { typedef TY List; };
    
    template< class TY, class TYPES
            , template<class> class _TRANS_
            >
    struct Apply<Node<TY,TYPES>, _TRANS_ > { typedef Node< typename _TRANS_<TY>::Type
                                                         , typename Apply<TYPES,_TRANS_>::List
                                                         > List; };
    
    
    /** conditional node: skip an element based on evaluating a predicate */
    template<bool, class T, class TAIL>
    struct CondNode                        { typedef TAIL  Next; };
    
    template<class T, class TAIL>
    struct CondNode<true, T, TAIL>         { typedef Node<T,TAIL>  Next; };
    
    /** filter away those types which don't fulfil a predicate metafunction */
    template< class TYPES 
            , template<class> class _P_    ///< a template providing a boolean member \c ::value
            >
    struct Filter;
    
    template<template<class> class _P_>
    struct Filter<NullType,_P_>            { typedef NullType  List; };
    
    template< class TY, class TYPES
            , template<class> class _P_
            >
    struct Filter<Node<TY,TYPES>,_P_>      { typedef typename CondNode< _P_<TY>::value
                                                                      , TY
                                                                      , typename Filter<TYPES,_P_>::List
                                                                      >::Next      
                                                                      List; };
    
    
    /** append lists-of-types */
    template<class TY1, class TY2>
    struct Append                          { typedef Node<TY1, typename Append<TY2,NullType>::List>  List; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Append<Node<TY,TYPES>, TAIL>    { typedef Node<TY,  typename Append<TYPES,  TAIL>::List>  List; };
    
    template<class TY, class TYPES>
    struct Append<NullType, Node<TY,TYPES>>    { typedef Node<TY,TYPES>   List; };
    
    template<class TY, class TYPES>
    struct Append<Node<TY,TYPES>, NullType>    { typedef Node<TY,TYPES>   List; };
    
    template<class TY1>
    struct Append<TY1,NullType>            { typedef Node<TY1,NullType>   List; };
    
    template<class TY2>
    struct Append<NullType,TY2>            { typedef Node<TY2,NullType>   List; };
    
    template<>
    struct Append<NullType,NullType>       { typedef NullType             List; };
    
    
    
    
    /** access the last list element */
    template<class TYPES>
    struct SplitLast;
    
    template<>
    struct SplitLast<NullType>             { typedef NullType Type;
                                             typedef NullType List; };
    template<class TY>
    struct SplitLast<Node<TY,NullType>>    { typedef TY       Type;
                                             typedef NullType List; };
    
    template<class TY, class TYPES>
    struct SplitLast<Node<TY,TYPES>>       { typedef typename SplitLast<TYPES>::Type Type;
                                             typedef typename Append< TY,
                                                                      typename SplitLast<TYPES>::List
                                                                    >::List 
                                                                    List; };
    
    
    
    /** 
     * splice a typelist like an overlay
     * into an base typelist, starting at given index.
     * @return either the combined (spliced) List, or
     *         the Front/Back part before or after the Overlay
     * @note using a NullType as OVERLAY allows to extract
     *         an arbitrary Front/Back part of the list
     */
    template<class BASE, class OVERLAY, uint i=0>
    struct Splice;
    
    template<class B, class BS,
             class OVERLAY, uint i>
    struct Splice<Node<B,BS>, OVERLAY, i>  { typedef Node<B, typename Splice<BS, OVERLAY, i-1>::List>  List;
                                             typedef Node<B, typename Splice<BS, OVERLAY, i-1>::Front> Front;
                                             typedef         typename Splice<BS, OVERLAY, i-1>::Back   Back; };
    
    template<class B, class BS,
             class O, class OS >
    struct Splice<Node<B,BS>,Node<O,OS>,0> { typedef Node<O, typename Splice<BS,OS, 0>::List>          List;
                                             typedef NullType                                          Front;
                                             typedef         typename Splice<BS,OS, 0>::Back           Back; };
    
    template<class B, class BS>
    struct Splice<Node<B,BS>, NullType, 0> { typedef Node<B, BS> List;
                                             typedef NullType    Front;
                                             typedef Node<B, BS> Back; };
    
    template<class XX, uint i>
    struct Splice<NullType, XX, i>         { typedef NullType    List;
                                             typedef NullType    Front;
                                             typedef NullType    Back; };
    
    
    
    
    /**
     * Allows to access various parts of a given typelist:
     * Start and End, Prefix and Tail...
     */
    template<class TYPES>
    struct Dissect;
    
    template<class T, class TYPES>
    struct Dissect<Node<T,TYPES>>
      {
        typedef Node<T,TYPES>                  List;  ///< the complete list
        typedef T                              Head;  ///< first element
        typedef Node<T,NullType>               First; ///< a list containing the first element
        typedef TYPES                          Tail;  ///< remainder of the list starting with the second elm.
        typedef typename SplitLast<List>::List Prefix;///< all of the list, up to but excluding the last element
        typedef typename SplitLast<List>::Type End;   ///< the last element
        typedef Node<End,NullType>             Last;  ///< a list containing the last element
      };
    
    template<>
    struct Dissect<NullType>
      {
        typedef NullType                       List;
        typedef NullType                       Head;
        typedef NullType                       First;
        typedef NullType                       Tail;
        typedef NullType                       Prefix;
        typedef NullType                       End;
        typedef NullType                       Last;
      };
    
    
    
    
    /** 
     * prefix each of the elements,
     * yielding a list-of lists-of-types
     */
    template<class T, class TY>
    struct PrefixAll                       { typedef Node< typename Append<T,TY>::List, NullType>  List; };
    
    template<class T>
    struct PrefixAll<T, NullType>          { typedef NullType  List; };
    
    template<class T>
    struct PrefixAll<T, NodeNull>          { typedef Node< typename Append<T,NodeNull>::List, NullType>  List; };
    
    template< class T
            , class TY, class TYPES
            >
    struct PrefixAll<T, Node<TY,TYPES>>    { typedef Node< typename Append<T,TY>::List
                                                         , typename PrefixAll<T,TYPES>::List
                                                         >     List; };
    
    
    
    
    /**
     * build a list-of lists, where each element of the first arg list
     * gets in turn prepended to all elements of the second arg list.
     * Can be used to build all possible combinations from two
     * sources, i.e. the Cartesian product.
     */
    template<class TY1,class TY2>
    struct Distribute                      { typedef typename PrefixAll<TY1,TY2>::List  List; };
    
    template<class TY>
    struct Distribute<NullType,TY>         { typedef NullType List; };
    
    template< class TY, class TYPES
            , class TAIL
            >
    struct Distribute<Node<TY,TYPES>,TAIL> { typedef typename Append< typename PrefixAll<TY,TAIL>::List
                                                                    , typename Distribute<TYPES,TAIL>::List
                                                                    >::List                    
                                                                    List; };
    
    
    
    /** 
     * build all possible combinations, based on a enumeration of the basic cases.
     * For each of the types in the argument list, an "enumeration generator" template is invoked,
     * yielding a list of the possible base cases. These base cases are then combined with all the
     * combinations of the rest, yielding all ordered combinations of all cases. Here, "ordered"
     * means that the base cases of the n-th element will appear in the n-th position of the
     * resulting lists,
     * 
     * For the typical example, the "base cases" are {flag(on), flag(off)}, so we get a
     * list-of-lists, featuring all possibilities to combine these distinct toggles. 
     */
    template< class X
            , template<class> class _ENUM_>
    struct Combine                         { typedef typename Distribute< typename _ENUM_<X>::List
                                                                        , Node<NullType,NullType>
                                                                        >::List  List; };
    template< template<class> class _ENUM_>
    struct Combine<NullType, _ENUM_ >      { typedef NodeNull                    List; };
    
    template< class TY, class TYPES
            , template<class> class _ENUM_>
    struct Combine<Node<TY,TYPES>,_ENUM_>  { typedef typename Distribute< typename _ENUM_<TY>::List
                                                                        , typename Combine<TYPES,_ENUM_>::List
                                                                        >::List  List; };
    
    /** enumeration generator for the Combine metafunction,
     *  yielding an "on" and "off" case
     */
    template<class F>
    struct FlagOnOff
      { 
        typedef Node<F, Node<NullType,NullType>>  List;
      };
    
    
    /** generate all possible on-off combinations of the given flags */
    template<class FLAGS>
    struct CombineFlags
      { 
        typedef typename Combine<FLAGS, FlagOnOff>::List  List;
      };
    
    
    
    
}} // namespace lib::meta
#endif
