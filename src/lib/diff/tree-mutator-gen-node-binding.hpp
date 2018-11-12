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
 ** tree diff operations onto an »External Tree Description«. This term denotes a
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
 ** bindings, together with suitable closures (lambdas) for layer selection, matching, most
 ** of which is already defined for collections of GenNode elements in general
 ** 
 ** @note the header tree-mutator-collection-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp
 ** 
 ** @see _DefaultBinding<GenNode>
 ** @see tree-mutator-collection-binding.hpp
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
    
    /**
     * Helper to deal with the magic "object type" attribute.
     * Our _meta representation_ for "objects" as Record<GenNode>
     * currently does not support metadata as a dedicated scope (as it should).
     * Rather, the only relevant piece of metadata, an object type ID field, is
     * treated with hard wired code and passed as a _magic attribute_ with key "type".
     * Unfortunately this means for our task here that a plain flat standard binding
     * for the collection of attributes does not suffice -- we need to intercept and
     * grab assignments to this magic attribute to forward them to the dedicated
     * type field found on diff::Record.
     * 
     * Since we build two layers of bindings, with the attributes necessarily on top,
     * this special treatment can be layered as a decorator on top, just overriding
     * the two operations which get to handle assignment to attribute values.
     */
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
        injectNew (GenNode const& spec)  override
          {
            if (spec.isNamed() and spec.isTypeID())
              {
                targetObj_.setType(spec.data.get<string>());
                return true;
              }
            else
              return PAR::injectNew (spec);
          }
        
        virtual bool
        assignElm (GenNode const& spec)  override
          {
            if (spec.isNamed() and spec.isTypeID())
              {
                targetObj_.setType(spec.data.get<string>());
                return true;
              }
            else
              return PAR::assignElm (spec);
          }
      };
    
    template<class MUT>
    inline Builder<ObjectTypeHandler<MUT>>
    filterObjectTypeAttribute (Rec::Mutator& targetTree, Builder<MUT>&& chain)
    {
      return ObjectTypeHandler<MUT> {targetTree, move(chain)};
    }
    
    
    
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
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    inline auto
    Builder<PAR>::attach (Rec::Mutator& targetTree)
    {
      auto rawBinding = this->attach (collection (accessChildren(targetTree)))
                             .attach (collection (accessAttribs(targetTree))
                                     .isApplicableIf ([](GenNode const& spec) -> bool
                                        {     // »Selector« : treat key-value elements here
                                          return spec.isNamed();
                                        }));
      
      return filterObjectTypeAttribute(targetTree, move(rawBinding));
    }
    
  }//(END)Mutator-Builder decorator components...
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H*/
