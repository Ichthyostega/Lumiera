/*
  TREE-DIFF-MUTATOR-BINDING.hpp  -  consume a tree diff, but target arbitrary private data

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


/** @file tree-diff-mutator-binding.hpp
 ** Concrete implementation to apply structural changes to unspecific
 ** private datea structures with hierarchical nature. This is a variation
 ** of the generic [tree diff applicator](\ref tree-diff-application.hpp),
 ** using the same implementation concept, while relying on an abstract
 ** adapter type, the \ref TreeMutator. Similar to the generic case, when
 ** combined with the generic #DiffApplicator, this allows to receive
 ** linearised structural diff descriptions and apply them to a given
 ** target data structure, which in this case is even a decoupled
 ** private data structure.
 ** 
 ** ## Design considerations
 ** So this use case is implemented on the same conceptual framework used for
 ** the generic tree diff application, which in turn is -- conceptually -- an
 ** extension of applying a list diff. But, again, we follow the route _not_ to
 ** explicate those conceptual relations in the form of inheritance. This would
 ** be implementation re-use, as opposed to building a new viable abstraction.
 ** No one outside the implementation realm would benefit from such an abstraction,
 ** so we prefer to understand the tree diff language as the abstraction, which
 ** needs to embodied into two distinct contexts of implementation.
 ** 
 ** ### Yet another indirection
 ** Unfortunately this leads to yet another indirection layer: Implementing a
 ** language in itself is necessarily a double dispatch (we have to abstract the
 ** verbs and we have to abstract the implementation side). And now we're decoupling
 ** the implementation side from a concrete data structure. Which means, that the
 ** use will have to provide a set of closures (which might even partially generated
 ** functors) to translate the _implementation actions_ underlying the language into
 ** _concrete actions_ on local data.
 ** 
 ** @todo this is WIP as of 2/2016 -- in the end it might be merged back or even
 **       replace the tree-diff-application.hpp
 ** 
 ** @see DiffVirtualisedApplication_test
 ** @see DiffTreeApplication_test
 ** @see DiffListApplication_test
 ** @see GenNodeBasic_test
 ** @see tree-diff.hpp
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_MUTATOR_BINDING_H
#define LIB_DIFF_TREE_DIFF_MUTATOR_BINDING_H


#include "lib/diff/tree-diff.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>

namespace lib {
namespace diff{
  
  using util::unConst;
  using util::cStr;
  using util::_Fmt;
  using std::move;
  using std::swap;
  
  
  /**
   * Interpreter for the tree-diff-language to work on arbitrary, undiclosed
   * local data structures. The key point to note is that this local data is
   * not required to implement any specific interface. The only requirement is
   * the ability somehow to support the basic operations of applying a structural
   * diff. This is ensured with the help of a _customisable adapter_ the TreeMutator.
   * @throws  lumiera::error::State when diff application fails structurally.
   * @throws  _unspecified errors_ when delegated operations fail.
   * @see TreeDiffInterpreter explanation of the verbs
   * @see DiffVirtualisedApplication_test demonstration of usage
   */
  template<>
  class DiffApplicationStrategy<TreeMutator>
    : public TreeDiffInterpreter
    {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
      using Mutator = Rec::Mutator;
      using Content = Rec::ContentMutator;
      using Iter    = Content::Iter;
      
      struct ScopeFrame
        {
          Mutator& target;
          Content content;
          
          ScopeFrame(Mutator& toModify)
            : target(toModify)
            , content()
            { }
          
          void init()
            {
              target.swapContent (content);
              content.resetPos();
            }
        };
      
      /** Storage: a stack of workspaces
       * used to handle nested child objects */
      std::stack<ScopeFrame> scopes_;
      
      
      Mutator& out() { return scopes_.top().target; }
      Content& src() { return scopes_.top().content; }
      Iter& srcPos() { return scopes_.top().content.pos; }
      bool endOfData() { return srcPos() == src().end(); }
      Rec& alteredRec() { return out(); }
      
      
      void
      __expect_in_target (GenNode const& elm, Literal oper)
        {
          if (endOfData())
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "no (further) elements in target sequence") % oper % elm
                              , LUMIERA_ERROR_DIFF_CONFLICT);
          
          if (elm.matches(Ref::CHILD) and not srcPos()->isNamed())
            return; // allow for anonymous pick or delete of children
          
          if (not srcPos()->matches(elm))
            throw error::State(_Fmt("Unable to %s element %s from target as demanded; "
                                    "found element %s on current target position instead")
                                    % oper % elm % *srcPos()
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_further_elements (GenNode const& elm)
        {
          if (endOfData())
            throw error::State(_Fmt("Premature end of target sequence, still expecting element %s; "
                                    "unable to apply diff further.") % elm
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_found (GenNode const& elm, Iter const& targetPos)
        {
          if (targetPos == src().end())
            throw error::State(_Fmt("Premature end of sequence; unable to locate "
                                    "element %s in the remainder of the target.") % elm
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_successful_location (GenNode const& elm)
        {
          if (endOfData()
              and not (    elm.matches(Ref::END)                                      // after(_END_)     -> its OK we hit the end
                       or (elm.matches(Ref::ATTRIBS) and src().children.empty())))    // after(_ATTRIBS_) -> if there are no children, it's OK to hit the end
            throw error::State(_Fmt("Unable locate position 'after(%s)'") % elm.idi
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_valid_parent_scope (GenNode::ID const& idi)
        {
          if (scopes_.empty())
            throw error::State(_Fmt("Unbalanced child scope bracketing tokens in diff; "
                                    "When leaving scope %s, we fell out of root scope.") % idi.getSym()
                              , LUMIERA_ERROR_DIFF_CONFLICT);
          
          if (alteredRec().empty())
            throw error::State(_Fmt("Corrupted state. When leaving scope %s, "
                                    "we found an empty parent scope.") % idi.getSym()
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      __expect_end_of_scope (GenNode::ID const& idi)
        {
          if (not endOfData())
            throw error::State(_Fmt("Incomplete diff: when about to leave scope %s, "
                                    "not all previously existing elements have been confirmed by the diff. "
                                    "At least one spurious element %s was left over") % idi.getSym() % *srcPos()
                              , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      
      Iter
      find_in_current_scope (GenNode const& elm)
        {
          Iter end_of_scope = src().currIsAttrib()? src().attribs.end()
                                                  : src().children.end();
          return std::find_if (srcPos()
                              ,end_of_scope
                              ,[&](auto& entry)
                                   {
                                     return entry.matches(elm);
                                   });
        }
      
      GenNode const&
      find_child (GenNode::ID const& idi)
        {
          if (alteredRec().empty())
            throw error::State(_Fmt("Attempt to mutate element %s, but current target data scope is empty. "
                                    "Sender and receiver out of sync?") % idi.getSym()
                              , LUMIERA_ERROR_DIFF_CONFLICT);
          
          // Short-cut-mutation: look at the last element.
          // this should be the one just added. BUT NOTE: this fails
          // when adding an attribute after entering the child scope.
          // Since attributes are typically values and not mutated,
          // this inaccuracy was deemed acceptable
          auto& current = out().accessLast();
          if (Ref::THIS.matches(idi) or current.matches(idi))
            return current;
          
          for (auto & child : alteredRec())
            if (child.idi == idi)
              return child;
          
          throw error::State(_Fmt("Attempt to mutate non existing child record; unable to locate child %s "
                                  "after applying the diff. Current scope: %s") % idi.getSym() % alteredRec()
                            , LUMIERA_ERROR_DIFF_CONFLICT);
        }
      
      void
      move_into_new_sequence (Iter pos)
        {
          if (src().currIsAttrib())
            out().appendAttrib (move(*pos));                    //////////////TICKET #969  was it a good idea to allow adding attributes "after the fact"?
          else
            out().appendChild (move(*pos));
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
      void
      locate_and_assign (GenNode const& n)
        {
          
        }
      
      void
      locate_and_open_for_mutation (GenNode const& n)
        {
          
        }
      
      
      
      /* == Implementation of the list diff application primitives == */
      
      virtual void
      ins (GenNode const& n)  override
        {
          inject (n);
        }
      
      virtual void
      del (GenNode const& n)  override
        {
          __expect_in_target(n, "remove");
          next_src();
        }
      
      virtual void
      pick (GenNode const& n)  override
        {
          __expect_in_target(n, "pick");
          accept_src();
          next_src();
        }
      
      virtual void
      skip (GenNode const& n)  override
        {
          __expect_further_elements (n);
          next_src();
        }      // assume the actual content has been moved away by a previous find()
      
      virtual void
      find (GenNode const& n)  override
        {
          __expect_further_elements (n);
               // consume and leave waste, expected to be cleaned-up by skip() later
          if (not find_and_accept(n));
            __fail_not_found (n);
        }
      
      
      /* == Implementation of the tree diff application primitives == */
      
      /** cue to a position behind the named node,
       *  thereby picking (accepting) all traversed elements
       *  into the reshaped new data structure as-is */
      virtual void
      after (GenNode const& n)  override
        {
          if (not accept_until(n))
            __fail_not_found (n);
        }
      
      /** assignement of changed value in one step */
      virtual void
      set (GenNode const& n)  override
        {
          locate_and_assign (n);
        }
      
      /** open nested scope to apply diff to child object */
      virtual void
      mut (GenNode const& n)  override
        {
          locate_and_open_for_mutation (n);
          
          Rec const& childRecord = child.data.get<Rec>();
          TRACE (diff, "tree-diff: ENTER scope %s", cStr(childRecord));
        }
      
      /** finish and leave child object scope, return to parent */
      virtual void
      emu (GenNode const& n)  override
        {
          TRACE (diff, "tree-diff: LEAVE scope %s", cStr(describeScope()));
          
          __expect_end_of_scope (n.idi);
          close_subScope();
          __expect_valid_parent_scope (n.idi);
        }
      
      
    public:
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
      explicit
      DiffApplicationStrategy(Rec::Mutator& mutableTargetRecord)
        : scopes_()
        {
          scopes_.emplace(mutableTargetRecord);
        }
      
      void
      initDiffApplication()
        {
          REQUIRE (1 == scopes_.size());
          scopes_.top().init();
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_MUTATOR_BINDING_H*/
