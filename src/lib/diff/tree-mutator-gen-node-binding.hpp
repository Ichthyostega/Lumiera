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
 ** on the two layered bindings for STL collections. The reason is that our »External
 ** Tree Description«  of object-like structures is comprised of recursively nested
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
 ** @note the header tree-mutator-collection-binding.hpp with specific builder templates
 **       is included way down, after the class definitions. This is done so for sake
 **       of readability.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H
#ifndef LIB_DIFF_TREE_MUTATOR_H
  #error "this header shall not be used standalone (see tree-mutator.hpp)"
#endif



//== anonymous namespace...
    
    
    
    using lib::diff::GenNode;
    
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
    
    /**
     * Attach to GenNode tree: Special setup to build a concrete `TreeMutator`.
     * This decorator is already outfitted with the necessary closures to work on
     * a diff::Record<GenNode> -- which is typically used as "meta representation"
     * of object-like structures. Thus this binding allows to apply a diff message
     * onto such a given »External Tree Description«, mutating it into new shape.
     */
    template<class PAR>
    inline auto
    twoLayeredGenNodeTreeMutator (Rec::Mutator& targetTree, PAR&& builderBase)
    {
      return builderBase
               .attach (collection (accessChildren(targetTree)))
               .attach (collection (accessAttribs(targetTree)));
    }
    
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_GEN_NODE_BINDING_H*/
