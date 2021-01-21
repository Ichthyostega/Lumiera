/*
  TREE-MUTATOR-DIFFMUTABLE-BINDING.hpp  -  default configuration to attach a collection of DiffMutable objects

  Copyright (C)         Lumiera.org
    2021,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator-Diffmutable-binding.hpp
 ** Special supplement for TreeMutator and the STL collection binding,
 ** to provide a shortcut and default wiring for a collection holding
 ** [DiffMutable](\ref diff-mutable.hpp) objects -- either directly or
 ** by smart-ptr. TreeMutator is a customisable intermediary, which
 ** enables otherwise opaque implementation data structures to receive
 ** and respond to generic structural change messages ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by combining various building blocks. One of the most relevant
 ** binding cases is to attach to a collection of child objects, which are themselves
 ** _recursively diff mutable_. This header is based on the
 ** [generic STL collection binding](\ref tree-mutator-collection-binding.hpp)
 ** and provides the most common default implementation for a »Matcher« and
 ** for building a recursive TreeMutator for the child elements by means of
 ** delegating to their DiffMutable::buildMutator() function. An additional
 ** requirement for this standard setup to be used is that the objects in the
 ** collection must expose a `getID()` function to determine the object identity.
 ** 
 ** @note the header tree-mutator-collection-diffmutable-binding.hpp was split off 
 **       or sake of readability and is included automatically from bottom of
 **       tree-mutator.hpp -- no need to include it explicitly
 ** 
 ** @see tree-mutator-test.cpp
 ** @see _DefaultBinding
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_DIFFMUTABLE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_DIFFMUTABLE_BINDING_H


#include "lib/error.hpp"
#include "lib/meta/trait.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/diff/tree-mutator-collection-binding.hpp"

namespace lib {
namespace diff{
  
  class DiffMutable;
  
  namespace { // Mutator-Builder decorator components...
    
    using lib::meta::enable_if;
    using lib::meta::Yes_t;
    using lib::meta::No_t;
    
    META_DETECT_FUNCTION(typename X::iterator, begin,(void));
    
    template<typename T>
    class can_recursively_bind_DiffMutable
      {
//        typedef typename Strip<T>::Type Type;
         
        
        
      public:
        enum { value = false
//          is_iterable::value
//                    or is_const_iterable::value
//                    or is_noexcept_iterable::value
//                    or is_const_noexcept_iterable::value
             };
      };
    
    
    template<typename T>
    class Is_DiffMutable
      { };
    
    template<typename T>
    class Is_wrapped_DiffMutable
      { };
    
    template<typename T>
    class Can_access_ID
      { };
    
    
    template<class TAR, typename SEL =void>
    struct _AccessID
      {
        GenNode::ID const&
        getID (TAR const& target)
          {
            throw error::Logic ("Unable to access the target element's object ID. "
                                "Please define a »Matcher« explicitly by invoking the builder function \"matchElement\".");
          }
      };
    template<class TAR>
    struct _AccessID<TAR, enable_if<Can_access_ID<TAR>>>
      {
        GenNode::ID const&
        getID (TAR const& target)
          {
            return target.getID();
          }
      };
    
    
    template<class ELM, typename SEL =void>
    struct _AccessTarget
      {
        DiffMutable&
        access (ELM const& elm)
          {
            throw error::Logic ("Unable to determine if the target is DiffMutable, and how to access it. "
                                "Please define a »Mutator« explicitly by invoking the builder function \"buildChildMutator\".");
          }
      };
    template<class ELM>
    struct _AccessTarget<ELM, enable_if<Is_DiffMutable<ELM>>>
      {
        DiffMutable&
        access (ELM const& elm)
          {
            return elm;
          }
      };
    template<class ELM>
    struct _AccessTarget<ELM, enable_if<Is_wrapped_DiffMutable<ELM>>>
      {
        DiffMutable&
        access (ELM const& elm)
          {
            return *elm;
          }
      };

    
    
    /** */
    template<class ELM>
    struct _DefaultBinding<ELM, enable_if<can_recursively_bind_DiffMutable<ELM>>>
      : private _AccessTarget<ELM>
      , private _AccessID<ELM>
      {
        template<class COLL>
        static auto
        attachTo (COLL& coll)
          {
            return _EmptyBinding<ELM>::attachTo(coll)
                      .matchElement([](GenNode const& spec, ELM const& elm)
                         {
                           return spec.idi == getID (access (elm));
                         })
                      .buildChildMutator ([&](ELM& target, GenNode::ID const&, TreeMutator::Handle buff) -> bool
                         {
                           access(target).buildMutator (buff);
                           return true;
                         });
          }
      };
    
    

    
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_DIFFMUTABLE_BINDING_H*/
