/*
  TREE-MUTATOR-GEN-NODE-BINDING.hpp  -  diff::TreeMutator implementation building block

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


/** @file tree-mutator-gen-node-binding.hpp
 ** Special binding implementation for TreeMutator, allowing to map
 ** tree diff operations onto an »External Tree Description«. Such is is a
 ** DOM like representation of tree like structures, comprised of GenNode elements.
 ** TreeMutator is a customisable intermediary, which enables otherwise opaque
 ** implementation data structures to receive and respond to generic structural
 ** change messages ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by implementing binding templates, in the way of building
 ** blocks, layered on top of each other. This header defines a special setup, based
 ** on two layered bindings for STL collections. The reason is that our »External
 ** Tree Description« of object-like structures is comprised of recursively nested
 ** Record<GenNode> to represent "objects", and this representation is actually implemented
 ** internally based on two collections -- one to hold the _attributes_ and one to hold the
 ** _children._ So this special setup relies on implementation inside knowledge to apply
 ** structural changes to such a representation. There is an implicit convention that
 ** "objects" are to be spelled out by first giving the metadata, then enumerating the
 ** attributes (key-value properties) and finally the child elements located within the
 ** scope of this "object" node. This implicit convention is in accordance with the
 ** structure of our _diff language_ -- thus it is sufficient just to layer two collection
 ** bindings, together with suitable closures (lambdas) for layer selection, matching, etc.
 ** 
 ** @note the header tree-mutator-collection-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H


#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator-collection-binding.hpp"
#include "lib/diff/tree-mutator.hpp"

#include <tuple>


namespace lib {
namespace diff{

  namespace { // Mutator-Builder decorator components...
    
    
    using Storage = RecordSetup<GenNode>::Storage;
    
    
    inline Storage&
    accessAttribs (Rec::Mutator& targetTree)
    {
      return std::get<0> (targetTree.exposeToDiff());
    }
    
    inline Storage&
    accessChildren (Rec::Mutator& targetTree)
    {
      return std::get<1> (targetTree.exposeToDiff());
    }
    
    template<class PAR>
    class ObjectTypeHandler
      : public PAR
      {
        Rec::Mutator& targetObj_;
        
      public:
        ObjectTypeHandler(Rec::Mutator& targetObj, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , targetObj_(targetObj)
          { }
        
        virtual bool
        injectNew (GenNode const& n)  override
          {
            if (n.isNamed() and n.isTypeID())
              {
                targetObj_.setType(n.data.get<string>());
                return true;
              }
            else
              return PAR::injectNew (n);
          }
        
        virtual bool
        assignElm (GenNode const& n)  override
          {
            if (n.isNamed() and n.isTypeID())
              {
                targetObj_.setType(n.data.get<string>());
                return true;
              }
            else
              return PAR::assignElm (n);
          }
      };
    
    template<class MUT>
    inline Builder<ObjectTypeHandler<MUT>>
    filterObjectType (Rec::Mutator& targetTree, Builder<MUT>&& baseBuilder)
    {
      return ObjectTypeHandler<MUT> {targetTree, move(baseBuilder)};
    }
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    inline auto
    Builder<PAR>::attach (Rec::Mutator& targetTree)
    {
      return filterObjectType(targetTree,
             this->attach (collection (accessChildren(targetTree)))
                  .attach (collection (accessAttribs(targetTree))
                              .isApplicableIf ([](GenNode const& spec) -> bool
                                   {
                                     return spec.isNamed();  // »Selector« : treat key-value elements here
                                   })));
    }
    
    inline void
    buildNestedMutator(Rec& nestedScope, TreeMutator::Handle buff)
    {
       buff.create (
         TreeMutator::build()
           .attach (mutateInPlace (nestedScope)));
    }
    
    
  }//(END)Mutator-Builder decorator components...
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H*/
