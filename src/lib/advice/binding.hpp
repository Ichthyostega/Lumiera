/*
  BINDING.hpp  -  pattern defining a specific attachment to the Advice system
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file binding.hpp
 ** A pattern to define and identify a specific attachment to the Advice system.
 ** This pattern is comprised of a predicate list and intended to be matched or unified
 ** against a similar pattern associated with the attachment of a possible collaboration partner.
 ** Semantically, this list of atoms forms an conjunction of predicates to be resolved against 
 ** similar predicates of the partner. Informally, when two entities attach to the Advice system,
 ** each specifying a binding, they can be paired up if, when combining, the expressions in their
 ** bindings all evaluate to true.
 ** 
 ** Typically, a binding includes a \em type-guard predicate \c adviceType(xx) where \c xx is an 
 ** identifier denoting a type used within an instantiation of the Advice collaboration, i.e. a type
 ** used as advice value in a instantiation of the PointOfAdvice<AD> template. Besides the type guard,
 ** a binding may narrow down the topic of the advice by providing further predicates. This allows for
 ** Advice collaborations targeted at a more specific topic. The goal and intention behind this Advice
 ** system is to allow collaboration of entities without requiring them to be tightly coupled. Indeed,
 ** the only dependency besides the common type used as advice is to know any specific topic used
 ** within the binding. Thus, and advisor entity could put up a piece of advice, i.e. a value of
 ** the advice type, and another client entity (the advised entity) could pick up this value
 ** without the need to now anything about the advisor.
 ** 
 ** Any binding can be normalised into a hash value, which plays a crucial role within the
 ** implementation of the advice system.
 ** 
 ** TODO WIP-WIP
 ** 
 ** @note as of 4/2010 this is an experimental setup and implemented just enough to work out
 **       the interfaces. Ichthyo expects this collaboration service to play a central role
 **       at various places within proc-layer.
 ** 
 ** @see configrules.hpp
 ** @see typed-lookup.cpp corresponding implementation
 ** @see typed-id-test.cpp
 ** 
 */


#ifndef LIB_ADVICE_BINDING_H
#define LIB_ADVICE_BINDING_H


#include "lib/error.hpp"
//#include "proc/asset.hpp"
//#include "proc/asset/struct-scheme.hpp"
//#include "lib/hash-indexed.hpp"
//#include "lib/util.hpp"
#include "lib/symbol.hpp"

//#include <boost/operators.hpp>
//#include <tr1/memory>
//#include <iostream>
//#include <string>

namespace lib    {
namespace advice {
  
  /**
   * Conjunction of predicates to be matched
   * against a collaboration partner for establishing
   * an Advice connection.
   * TODO type comment
   */
  class Binding
    {
      public:
        /** create the empty binding, equivalent to \c true */
        Binding();
      
        /** create the binding as defined by the given textual definition.
         *  @note implicit type conversion deliberately intended */
        Binding (Literal spec);
        
        /*-- Binding is default copyable --*/
        
        /** extend the definition of this binding
         *  by adding a predicate according to the
         *  given textual definition */
        void addPredicate (Literal spec);
    };
  
  
  ////TODO define the hash function here, to be picked up by ADL
  
  
  
  
  
}} // namespace lib::advice
#endif
