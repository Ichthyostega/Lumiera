/*
  TREE-MUTATOR-NOOP-BINDING.hpp  -  diff::TreeMutator implementation building block

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator-noop-binding.hpp
 ** Special binding implementation for TreeMutator, allowing to accept
 ** and ignore any tree diff without tangible effect. TreeMutator is a
 ** customisable intermediary, which enables otherwise opaque implementation
 ** data structures to receive and respond to generic structural change messages
 ** ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by combining various building blocks. This header defines
 ** a special `/dev/null` building block, which behaves as if successfully consuming
 ** the given diff without actually doing anything. Obviously, such a "black hole layer"
 ** need to be below any other diff binding, and may be used to absorb any diff verbs
 ** not matched and consumed by a more specific binding. Contrast this to the TreeMutator
 ** default implementation, which likewise absorbs diff verbs, but in a way to trigger
 ** a lumiera::error::LUMIERA_ERROR_DIFF_CONFLICT. 
 ** 
 ** @note the header tree-mutator-attribute-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_NOOP_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_NOOP_BINDING_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/format-string.hpp"
#include "lib/idi/entry-id.hpp"

#include <utility>


namespace lib {
namespace diff{

  namespace { // Mutator-Builder decorator components...
    
    
    /**
     * Diff binding for black holes, happily accepting anything.
     * @note absorbs and silently ignores any diff verb.
     */
    template<class PAR>
    class BlackHoleMutation
      : public PAR
      {
        
        
      public:
        BlackHoleMutation (PAR&& chain)
          : PAR{std::forward<PAR>(chain)}
          { }
        
        
      };
    
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    inline auto
    Builder<PAR>::ignoreAllChanges()
    {
      return chainedBuilder<BlackHoleMutation<PAR>>();
    }
    
    
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_NOOP_BINDING_H*/
