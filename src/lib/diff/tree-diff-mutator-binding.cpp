/*
  TreeDiffMutatorBinding  -  implementation of diff application to opaque data

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

* *****************************************************/


/** @file tree-diff-mutator-binding.cpp
 ** Implementation of diff application to unspecific private data structures.
 ** This binding is the link between a generic interpreter for our
 ** »tree diff language« and a concrete TreeMutator implementation,
 ** as provided by the target data structure. We do not require much
 ** additional knowledge regarding the opaque target structure, beyond
 ** the ability to construct such a customised TreeMutator. For this reason,
 ** the implementation is mostly generic and thus can be emitted here within
 ** the library module -- with the exception of the ctor, which indeed picks
 ** up some specifics of the concrete usage situation and thus needs to be
 ** generated in usage context.
 ** 
 ** @see tree-diff.cpp
 ** @see tree-diff-mutator-binding.cpp
 ** @see DiffVirtualisedApplication_test
 ** 
 */


#include "lib/error.hpp"
#include "lib/diff/tree-diff-mutator-binding.hpp"



namespace lib {
namespace diff{
  
  /* ======= Implementation of Tree Diff Application via TreeMutator ======= */
  
  using util::unConst;
  using util::cStr;
  using util::_Fmt;
  using std::move;
  using std::swap;
  
  
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
  template<>
  class DiffApplicationStrategy<DiffMutable>
    : public TreeDiffInterpreter
    {
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
      bool endOfData() { return srcPos() == src().end(); }   /////TODO split into an actual scope end check and an non-null check
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
      
      /* == Forwarding: error handling == */
      
      void
      __expect_in_target  (GenNode const& elm, Literal oper)
        {
          
        }
      
      void
      __expect_further_elements (GenNode const& elm)
        {
          
        }
      
      void
      __fail_not_found (GenNode const& elm)
        {
          
        }
      
      void
      __expect_end_of_scope (GenNode::ID const& idi)
        {
          
        }
      
      void
      __expect_valid_parent_scope (GenNode::ID const& idi)
        {
          
        }
      
      
      /* == Forwarding: mutation primitives == */
      
      void
      skipSrc()
        {
          UNIMPLEMENTED("skip next src element and advance abstract source position");
        }
      
      void
      injectNew (GenNode const& n)
        {
          UNIMPLEMENTED("inject a new element at current abstract position");
        }
      
      bool
      matchSrc (GenNode const& n)
        {
          UNIMPLEMENTED("ensure the next source element matches with given spec");
        }
      
      bool
      acceptSrc (GenNode const& n)
        {
          UNIMPLEMENTED("accept existing element, when matching the given spec");
        }
      
      bool
      findSrc (GenNode const& n)
        {
          UNIMPLEMENTED("locate designated element and accept it at current position");
        }
      
      bool
      accept_until (GenNode const& refMark)
        {
          UNIMPLEMENTED("repeatedly accept until encountering the mark");
        }
      
      void
      assignElm (GenNode const& n)
        {
          UNIMPLEMENTED("locate already accepted element and assign given new payload");
        }
      
      void
      open_subScope (GenNode const& n)
        {
          UNIMPLEMENTED("locate already accepted element and open recursive sub-scope for mutation");
        }
      
      void
      close_subScope()
        {
          UNIMPLEMENTED("finish and leave sub scope and return to invoking parent scope");
        }
      
      
      
      /* == Implementation of the list diff application primitives == */
      
      virtual void
      ins (GenNode const& n)  override
        {
          injectNew (n);
        }
      
      virtual void
      del (GenNode const& n)  override
        {
          __expect_in_target(n, "remove");
          skipSrc();
        }
      
      virtual void
      pick (GenNode const& n)  override
        {
          __expect_in_target(n, "pick");
          acceptSrc (n);
        }
      
      virtual void
      skip (GenNode const& n)  override
        {
          __expect_further_elements (n);
          skipSrc();
        }      // assume the actual content has been moved away by a previous find()
      
      virtual void
      find (GenNode const& n)  override
        {
          __expect_further_elements (n);
               // consume and leave waste, expected to be cleaned-up by skip() later
          if (not findSrc(n));
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
      
      /** assignment of changed value in one step */
      virtual void
      set (GenNode const& n)  override
        {
          assignElm (n);
        }
      
      /** open nested scope to apply diff to child object */
      virtual void
      mut (GenNode const& n)  override
        {
          open_subScope (n);
          
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          Rec const& childRecord = child.data.get<Rec>();
          TRACE (diff, "tree-diff: ENTER scope %s", cStr(childRecord));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        }
      
      /** finish and leave child object scope, return to parent */
      virtual void
      emu (GenNode const& n)  override
        {
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          TRACE (diff, "tree-diff: LEAVE scope %s", cStr(describeScope()));
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          
          __expect_end_of_scope (n.idi);
          close_subScope();
          __expect_valid_parent_scope (n.idi);
        }
      
      
    public:
      explicit
      DiffApplicationStrategy(DiffMutable& targetBinding)
        {
          TODO("attach to the given Target");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          scopes_.emplace(mutableTargetRecord);
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        }
      
      void
      initDiffApplication()
        {
          TODO("(re)initialise the diff application machinery");
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
          REQUIRE (1 == scopes_.size());
          scopes_.top().init();
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        }
    };
  
  
  
}} // namespace lib::diff
