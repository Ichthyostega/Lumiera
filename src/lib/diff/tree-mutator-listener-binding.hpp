/*
  TREE-MUTATOR-LISTENER-BINDING.hpp  -  decorator for TreeMutator to attach change listeners

   Copyright (C)
     2019,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file tree-mutator-listener-binding.hpp
 ** Special supplement for TreeMutator, to attach listeners for notification
 ** on specific changes, especially structural ones. TreeMutator is a
 ** customisable intermediary, which enables otherwise opaque implementation
 ** data structures to receive and respond to generic structural change messages
 ** ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by combining various building blocks. This header defines
 ** a special decorator to be layered on top of such a TreeMutator binding; it will
 ** not interfere with the received diff, but detect relevant changes and invoke the
 ** functor after the triggering diff has been applied completely to the bound scope.
 ** 
 ** @note the header tree-mutator-listener-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp -- no need to
 **       include it explicitly
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_LISTENER_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_LISTENER_BINDING_H


#include "lib/error.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator.hpp"

namespace lib {
namespace diff{

  namespace { // Mutator-Builder decorator components...
    
    
    /**
     * Decorator for TreeMutator bindings, to fire a listener function
     * when the applied diff describes a relevant change. Changes can be
     * _structural,_ they can be _value mutations_ or _child mutations._
     * By convention, all changes pertaining the sequence of children are
     * classified as structural changes. Thus, effectively a structural
     * change incurs usage of the `INS`, `DEL`, `SKIP` or `FIND` verbs,
     * which in turn will be translated into the three API operations
     * intercepted here in the basic setup. When value assignments
     * count as "relevant", then we'll also have to intercept the
     * `assignElm` API operation. However, the relevance of
     * mutations to child elements is difficult to assess
     * on this level, since we can not see what a nested
     * scope actually does to the mutated child elements.
     * @tparam assign also trigger on assignments in addition to
     *       structural changes (which will always trigger).
     *       Defaults to `false`
     * @note TreeMutator is a disposable one-way object;
     *       the triggering mechanism directly relies on that.
     *       The listener is invoked, whenever a scope is complete,
     *       including processing of any nested scopes.
     */
    template<class PAR, typename LIS, bool assign =false>
    class Detector4StructuralChanges
      : public PAR
      {
        LIS changeListener_;
        bool triggered_ = false;
        
        void
        trigger(bool relevant =true)
          {
            if (relevant)
              triggered_ = true;
          }
        
      public:
        Detector4StructuralChanges (LIS functor, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , changeListener_{functor}
          { }
        
        // move construction allowed and expected to happen
        Detector4StructuralChanges (Detector4StructuralChanges&&) =default;
        
        /** once the diff for this level is completely applied,
         *  the TreeMutator will be discarded, and we can fire
         *  our change listener at that point. */
       ~Detector4StructuralChanges()
          {
            if (triggered_)
              changeListener_();
          }
        
        /* ==== TreeMutator API ==== */
        
        using Elm  = GenNode const&;
        
        bool injectNew (Elm elm) override { trigger();       return PAR::injectNew (elm); }
        bool findSrc   (Elm elm) override { trigger();       return PAR::findSrc   (elm); }
        void skipSrc   (Elm elm) override { trigger();              PAR::skipSrc   (elm); }
        bool assignElm (Elm elm) override { trigger(assign); return PAR::assignElm (elm); }
      };
    
    
    
    /** Entry point for DSL builder: attach a functor as listener to be notified after structural changes.
     */
    template<class PAR>
    template<typename LIS>
    inline auto
    Builder<PAR>::onSeqChange (LIS changeListener)
    {
      ASSERT_VALID_SIGNATURE (LIS, void(void))
      
      return chainedBuilder<Detector4StructuralChanges<PAR,LIS>> (changeListener);
    }
    
    /** Entry point for DSL builder: attach a functor as listener to be notified after either
     * a structural change, or a value assignment within the local scope of this TreeMutator.
     */
    template<class PAR>
    template<typename LIS>
    inline auto
    Builder<PAR>::onLocalChange (LIS changeListener)
    {
      ASSERT_VALID_SIGNATURE (LIS, void(void))
                                                            //  vvvv---note: including assignments
      return chainedBuilder<Detector4StructuralChanges<PAR,LIS, true>> (changeListener);
    }
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_LISTENER_BINDING_H*/
