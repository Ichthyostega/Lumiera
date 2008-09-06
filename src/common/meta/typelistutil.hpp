/*
  TYPELISTUTIL.hpp  -  Utils for working with lists-of-types
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/


#ifndef LUMIERA_META_TYPELISTUTIL_H
#define LUMIERA_META_TYPELISTUTIL_H


  
#include "common/meta/typelist.hpp"

namespace lumiera {
  namespace typelist {
    
    
    /**
     * Metafunction counting the number of Types in the collection
     */
    template<class TYPES>
    struct count;
    template<>
    struct count<NullType>
      {
        enum{ value = 0 };
      };
    template<class TY, class TYPES>
    struct count<Node<TY,TYPES> >
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
        enum{ value = 0 };
      };
    template<class TY, class TYPES>
    struct maxSize<Node<TY,TYPES> >
      {
        enum{ thisval = sizeof(TY)
            , nextval = maxSize<TYPES>::value
            , value   = nextval > thisval?  nextval:thisval
            };
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
    
    
    template<bool, class T, class TAIL>
    struct CondNode                        { typedef TAIL  Next; };
    
    template<class T, class TAIL>
    struct CondNode<true, T, TAIL>         { typedef Node<T,TAIL>  Next; };
    
    /** filter away those types which don't fulfill a predicate metafunction */
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
    struct Append<NullType, Node<TY,TYPES> >   { typedef Node<TY,TYPES>   List; };
    
    template<class TY, class TYPES>
    struct Append<Node<TY,TYPES>, NullType>    { typedef Node<TY,TYPES>   List; };
    
    template<class TY1>
    struct Append<TY1,NullType>            { typedef Node<TY1,NullType>   List; };
    
    template<class TY2>
    struct Append<NullType,TY2>            { typedef Node<TY2,NullType>   List; };
    
    template<>
    struct Append<NullType,NullType>       { typedef NullType             List; };
    
    
    
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
    struct PrefixAll<T, Node<TY,TYPES> >   { typedef Node< typename Append<T,TY>::List
                                                         , typename PrefixAll<T,TYPES>::List
                                                         >     List; };
    
    
    
    
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
    
    
    
    /** use a permutation generator
     *  for creating a list of all possible combinations
     */
    template< class X
            , template<class> class _PERMU_>
    struct Combine                         { typedef typename Distribute< typename _PERMU_<X>::List
                                                                        , Node<NullType,NullType>
                                                                        >::List  List; };
    template< template<class> class _PERMU_>
    struct Combine<NullType, _PERMU_ >     { typedef Node<NullType,NullType>     List; };
    
    template< class TY, class TYPES
            , template<class> class _PERMU_>
    struct Combine<Node<TY,TYPES>,_PERMU_> { typedef typename Distribute< typename _PERMU_<TY>::List
                                                                        , typename Combine<TYPES,_PERMU_>::List
                                                                        >::List  List; };
    
    /** permutation generator for the Combine metafunction,
     *  yielding an "on" and "off" case
     */
    template<class F>
    struct FlagOnOff
      { 
        typedef Node<F, Node<NullType,NullType> >  List;
      };
    
    
    /** generate all possible on-off combinations of the given flags */
    template<class FLAGS>
    struct CombineFlags
      { 
        typedef typename Combine<FLAGS, FlagOnOff>::List  List; 
      };
    
    
    
    
  } // namespace typelist
  
} // namespace lumiera
#endif
