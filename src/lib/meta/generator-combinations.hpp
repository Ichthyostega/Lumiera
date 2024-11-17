/*
  GENERATOR-COMBINATIONS.hpp  -  generate combinations and variations

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file generator-combinations.hpp
 ** Metaprogramming facilities to generate combination cases.
 ** Similar to the plain typelist-based generators, a custom supplied
 ** template will be instantiated with combinations of the parameter types
 ** and then mixed into the resulting type
 ** 
 ** @see generator-combinations-test.cpp
 ** @see generator.hpp
 ** 
 */


#ifndef LIB_META_GENERATOR_COMBINATIONS_H
#define LIB_META_GENERATOR_COMBINATIONS_H

#include "lib/meta/typelist.hpp"
#include "lib/meta/typelist-manip.hpp"
#include "lib/meta/generator.hpp"



namespace lib {
namespace meta{
  
  
  template<class TYPES_1, class TYPES_2>
  struct CartesianProduct
    : Distribute< typename TYPES_1::List
                , typename TYPES_2::List
                >
    { };
  
  
  
  template< template<class,class,class> class _X_>
  struct PickParametersFromSublist
    {
      template<class SUBLIST, class BASE>
      struct SingleCaseInstantiation
        : _X_< typename Pick<SUBLIST,0>::Type
             , typename Pick<SUBLIST,1>::Type
             , BASE
             >
        { };
    };
  
  
  /**
   * Build a Case matrix.
   * The given parameter template _X_
   * will be instantiated for each possible combination
   * of the elements from both parameter type-lists.
   * All these instantiations will be chained up
   * into a linear inheritance chain rooted
   * at the BASE type.
   * @note the custom-supplied template _X_ needs to take a 3rd parameter,
   *       and inherit from this parameter, in order to form that chain.
   *       Typically you'll define some (static) functions within that
   *       template, which then forward the call to the given BASE
   *       (and of course, that BASE then needs to define this
   *       function as well). 
   */
  template
    < class TYPES_1, class TYPES_2           ///< the two type collections to pick combinations from
    , template<class,class,class> class _X_  ///< template with two arg types and a base type
    , class BASE = NullType
    >
  struct InstantiateChainedCombinations
    : InstantiateChained< typename CartesianProduct<TYPES_1,TYPES_2>::List
                        , PickParametersFromSublist<_X_>::template SingleCaseInstantiation
                        , BASE
                        >
    { };
  
  
  
}} // namespace lib::meta
#endif
