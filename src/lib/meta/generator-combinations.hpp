/*
  GENERATOR-COMBINATIONS.hpp  -  generate combinations and variations

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/** @file generator-combinations.hpp
 ** Metaprogramming facilities to generate combination cases.
 ** Similar to the plain typelist-based generators, a custom supplied
 ** template will be instantiated with combinations of the parameter types
 ** and then  mixed into the resulting type
 ** 
 ** @see generator-combinations-test.cpp
 ** @see generator.hpp
 ** 
 */


#ifndef LUMIERA_META_GENERATOR_COMBINATIONS_H
#define LUMIERA_META_GENERATOR_COMBINATIONS_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-util.hpp"
#include "lib/meta/generator.hpp"



namespace lumiera {
namespace typelist{
  
  
  template<class TYPES_1, class TYPES_2>
  class CartesianProduct
    : Distribute< typename TYPES_1::List
                , typename TYPES_2::List
                >
    { };
  
  
  template<class SUBLIST>
  struct PickFirst;
  
  template<class TY, class TAIL>
  struct PickFirst<Node<TY,TAIL> >
    {
      typedef TY Type;  
    };
  
  
  template<class SUBLIST>
  struct PickSecond;
  
  template<class TY, class TAIL>
  struct PickSecond<Node<TY,TAIL> >
    {
      typedef typename PickFirst<TAIL>::Type Type;  
    };
  
  
  
  template< template<class,class,class> class _X_>
  struct PickParametersFromSublist
    {
      template<class SUBLIST, class BASE>
      struct CaseInstantiation
        : _X_< PickFirst<SUBLIST>::Type
             , PickSecond<SUBLIST>::Type
             , BASE
             >
        { };
    };
  
  
  template
    < class TYPES_1, class TYPES_2           ///< the two type collections to pick combinations from
    , template<class,class,class> class _X_  ///< template with two arg types and a base type
    , class BASE = NullType
    >
  class InstantiateChainedCombinations
    : InstantiateChained< CartesianProduct<TYPES_1,TYPES_2>::List
                        , PickParametersFromSublist<_X_>::template CaseInstantiation
                        , BASE
                        >
    { };
  
  
  
}} // namespace lumiera::typelist
#endif
