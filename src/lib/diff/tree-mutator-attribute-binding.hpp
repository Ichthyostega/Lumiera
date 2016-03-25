/*
  TREE-MUTATOR-ATTRIBUTE-BINDING.hpp  -  diff::TreeMutator implementation building block

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator-attribute-binding.hpp
 ** Special binding implementation for TreeMutator, allowing to map
 ** tree diff operations onto native object attributes. TreeMutator is a
 ** customisable intermediary, which enables otherwise opaque implementation
 ** data structures to receive and respond to generic structural change messages
 ** ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by implementing binding templates, in the way of building
 ** blocks, attached and customised through lambdas. It is possible to layer
 ** several bindings on top of a single TreeMutator -- and this header defines
 ** a building block for one such layer, especially for binding to object fields
 ** through getter / setter lambdas.
 **  
 ** @note the header tree-mutator-attribute-binding.hpp with specific builder templates
 **       is included way down, after the class definitions. This is done so for sake
 **       of readability.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H
#ifndef LIB_DIFF_TREE_MUTATOR_H
  #error "this header shall not be used standalone (see tree-mutator.hpp)"
#endif


  
//== anonymous namespace...
    
    
    template<class PAR, class CLO>
    struct ChangeOperation
      : PAR
      {
        ID attribID_;
        CLO change_;
        
        virtual void
        setAttribute (ID id, Attribute& newValue)
          {
            using Args = typename _ClosureType<CLO>::Args;
            using ValueType = typename lib::meta::Pick<Args, 0>::Type;
            
            if (id == attribID_)
              change_(newValue.get<ValueType>());
            
            else // delegate to other closures (Decorator-style)
              PAR::setAttribute(id, newValue);
          }
        
        ChangeOperation(ID id, CLO clo, PAR const& chain)
          : PAR(chain)
          , attribID_(id)
          , change_(clo)
          { }
      };
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H*/
