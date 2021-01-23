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


/** @file tree-mutator-diffmutable-binding.hpp
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
 ** _recursively diff mutable_. This header here is based on the
 ** [generic STL collection binding](\ref tree-mutator-collection-binding.hpp)
 ** and provides the most common default implementation for a »Matcher« and
 ** for building a recursive TreeMutator for the child elements by means of
 ** delegating to their DiffMutable::buildMutator() function. An additional
 ** requirement for this standard setup to be used is that the objects in the
 ** collection must expose a `getID()` function to determine the object identity.
 ** 
 ** @note the header tree-mutator-diffmutable-binding.hpp was split off
 **       for sake of readability and is included automatically from the
 **       bottom of diff-mutable.hpp -- no need to include it explicitly
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
#include "lib/diff/diff-mutable.hpp"

namespace lib {
namespace diff{
  
  namespace { // Mutator-Builder decorator components...
    
    using lib::meta::enable_if;
    using lib::meta::Yes_t;
    using lib::meta::No_t;
    using std::__and_;
    using std::__or_;
    
    /**
     * Metaprogramming helper to detect if the given target class allows us
     * to build a default »Matcher« automatically. (The »Matcher« is used to
     * determine the applicability of a given diff verb to this target object)
     * @note we directly probe the desired functionality: can we equality compare
     *       a given GenNode::ID (from the diff) with this type's object ID?
     * @see duck-detector.hpp for explanation of this technique
     */
    template<typename TY>
    class Can_retrieve_and_compare_ID
      {
        template<typename X,
                 typename SEL = decltype(std::declval<GenNode::ID>() == std::declval<X>().getID())>
        struct Probe
          { };
        
        template<class X>
        static Yes_t check(Probe<X> * );
        template<class>
        static No_t  check(...);
        
      public:
        static const bool value = (sizeof(Yes_t)==sizeof(check<TY>(0)));
      };
    
    template<typename T>
    struct Can_access_ID
      : Can_retrieve_and_compare_ID<typename meta::Strip<T>::Type>
      { };
    
    template<typename T>
    struct Is_DiffMutable          ///< Metafunction: does the target implement the DiffMutable interface?
      : meta::is_Subclass<T,DiffMutable>
      { };
    
    template<typename T>
    struct Is_wrapped_DiffMutable  ///< Metafunction: is this a DiffMutable wrapped into a smart-ptr?
      : __and_< meta::Unwrap<T>
              , Is_DiffMutable<typename meta::Unwrap<T>::Type>>
      { };
    
    template<typename T>
    struct can_recursively_bind_DiffMutable
      : __or_< Is_DiffMutable<T>
             , Is_wrapped_DiffMutable<T>>
      { } ;
    
    
    /**
     * Metaprogramming helper to retrieve the object identity, whenever
     * the target object for the diff defines a `getID()` function.
     */
    template<class TAR, typename SEL =void>
    struct _AccessID
      {
        static idi::BareEntryID const&
        getID (TAR const&)
          {
            throw error::Logic ("TreeMutator::build() .attach(collection...) : Unable to access the target element's object ID. "
                                "Please define a »Matcher« explicitly by invoking the builder function \"matchElement\".");
          }
      };
    template<class ELM>
    struct _AccessID<ELM, enable_if<Can_access_ID<ELM>>>
      {
        static idi::BareEntryID const&
        getID (ELM const& target)
          {
            return meta::unwrap(target).getID();
          }
      };

    
    
    /**
     * Entry Point: Specialisation of the collection binding to work on a collection of DiffMutable objects,
     * either embedded directly, or attached via smart-ptr. Since the DiffMutable interface directly
     * exposes a function to build a TreeMutator, a generic implementation for recursive child mutation
     * can be supplied automatically. Moreover, if the target objects also offer a `getID()` function
     * to reveal their object identity, the »Matcher« (to check applicability of some diff verb) can
     * likewise be generated automatically.
     * @note different than in the base case, recursive child mutation is thus enabled automatically.
     */
    template<class ELM>
    struct _DefaultBinding<ELM, enable_if<can_recursively_bind_DiffMutable<ELM>>>
      {
        template<class COLL>
        static auto
        attachTo (COLL& coll)
          {
            return _EmptyBinding<ELM>::attachTo(coll)
                      .matchElement([](GenNode const& spec, ELM const& elm)
                         {
                           return spec.idi == _AccessID<ELM>::getID(elm);
                         })
                      .buildChildMutator ([&](ELM& target, GenNode::ID const&, TreeMutator::Handle buff) -> bool
                         {
                           DiffMutable& child = meta::unwrap(target);
                           child.buildMutator (buff);
                           return true;
                         });
          }
      };
    
    
    
    
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_DIFFMUTABLE_BINDING_H*/
