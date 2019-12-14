/*
  TREE-MUTATOR-LISTENER-BINDING.hpp  -  decorator for TreeMutator to attach change listeners

  Copyright (C)         Lumiera.org
    2019,               Hermann Vosseler <Ichthyostega@web.de>

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
 ** bound functor after the triggering diff has been applied completely.
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
     * when the applied diff describes a structural change. By convention,
     * all changes pertaining the sequence of children count as structural.
     * Thus, effectively a structural change incurs usage of the `INS`, `DEL`
     * `SKIP` or `FIND` verbs, which in turn are translated into the three
     * API operation intercepted here.
     * @note TreeMutator is a disposable one-way object;
     *       the triggering mechanism directly relies on that.
     */
    template<class PAR, typename LIS>
    class Detector4StructuralChanges
      : public PAR
      {
        LIS changeListener_;
        bool triggered_ = false;
        
      public:
        Detector4StructuralChanges (LIS functor, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , changeListener_{functor}
          { }
        
        /** once the diff is for this level is completely applied,
         *  the TreeMutator will be discarded, and we can fire our
         *  change listener at that point. */
       ~Detector4StructuralChanges()
          {
            if (triggered_)
              changeListener_();
          }
        
        /* ==== TreeMutator API ==== */
        
        using Elm  = GenNode const&;
        
        bool injectNew (Elm elm) override { triggered_ = true; return PAR::injectNew (elm); }
        bool findSrc (Elm elm)   override { triggered_ = true; return PAR::findSrc (elm); }
        void skipSrc (Elm elm)   override { triggered_ = true;        PAR::skipSrc (elm); }
      };
    
    
    
    /** Entry point for DSL builder: attach a functor as listener to be notified after structural changes.
     */
    template<class PAR>
    template<typename LIS>
    inline auto
    Builder<PAR>::onStructuralChange (LIS changeListener)
    {
      ASSERT_VALID_SIGNATURE (LIS, void(void))
      
      return chainedBuilder<Detector4StructuralChanges<PAR,LIS>> (changeListener);
    }
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_LISTENER_BINDING_H*/
