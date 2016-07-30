/*
  TREE-DIFF-APPLICATION.hpp  -  consume and apply a tree diff

  Copyright (C)         Lumiera.org
    2014,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-diff-application.hpp
 ** Concrete implementation to apply structural changes to hierarchical
 ** data structures. Together with the generic #DiffApplicator, this allows
 ** to receive linearised structural diff descriptions and apply them to
 ** a given target data structure, to effect the corresponding changes.
 ** 
 ** ## Design considerations
 ** While -- conceptually -- our tree diff handling can be seen as an extension
 ** and generalisation of list diffing, the decision was \em not to embody this
 ** extension into the implementation technically, for sake of clarity. More so,
 ** since the Record, which serves as foundation for our »External Tree Description«,
 ** was made to look and behave like a list-like entity, but built with two distinct
 ** scopes at implementation level: the attribute scope and the contents scope. This
 ** carries over to the fine points of the list diff language semantics, especially
 ** when it comes to fault tolerance and strictness vs fuzziness in diff application.
 ** The implementation is thus faced with having to deal with an internal focus and
 ** a switch from scope to scope, which adds a lot of complexity. So the list diff
 ** application strategy can be seen as blueprint and demonstration of principles.
 ** 
 ** Another point in question is whether to treat the diff application as
 ** manipulating a target data structure, or rather building a reshaped copy.
 ** The fact that GenNode and Record are designed as immutable values seems to favour
 ** the latter, yet the very reason to engage into building this diff framework was
 ** how to handle partial updates within a expectedly very large UI model, reflecting
 ** the actual session model in Proc-Layer. So we end up working on a Mutator,
 ** which clearly signals we're going to reshape and re-rig the target data.
 ** 
 ** \par related
 ** Closely related to this generic application of tree changes is the situation,
 ** where we want to apply structural changes to some non-generic and private data
 ** structure. In fact, it is possible to _use the same tree diff language_ for
 ** this specific case, with the help of an _adapter_. Thus, within our diff
 ** framework, we provide a _similar binding_ for the DiffApplicator, but
 ** then targeted towards such an [structure adapter](\ref TreeMutator)
 ** 
 ** ## State and nested scopes
 ** Within the level of a single #Record, our tree diff language works similar to
 ** the list diff (with the addition of the \c after(ID) verb, which is just a
 ** shortcut to accept parts of the contents unaltered). But after possibly rearranging
 ** the contents of an "object" (Record), the diff might open some of its child "objects"
 ** by entering a nested scope. This is done with the \c mut(ID)....emu(ID) bracketing
 ** construct. On the implementation side, this means we need to use a stack somehow.
 ** The decision was to manage this stack explicitly, as a std::stack (heap memory).
 ** Each entry on this stack is a "context frame" for list diff. Which makes the
 ** tree diff applicator a highly statefull component.
 ** 
 ** Even more so, since -- for \em performance reasons -- we try to alter the
 ** tree shaped data structure \em in-place. We want to avoid the copy of possibly
 ** deep sub-trees, when in the end we might be just rearranging their sequence order.
 ** This design decision comes at a price tag though
 ** - it subverts the immutable nature of \c Record<GenNode> and leads to
 **   high dependency on data layout and implementation details of the latter.
 **   This is at least prominently marked by working on a diff::Record::Mutator,
 **   so the client has first to "open up" the otherwise immutable tree
 ** - the actual list diff on each level works by first \em moving the entire
 **   Record contents away into a temporary buffer and then \em moving them
 **   back into new shape one by one. In case of a diff conflict  (i.e. a
 **   mismatch between the actual data structure and the assumptions made
 **   for the diff message on the sender / generator side), an exception
 **   is thrown, leaving the client with a possibly corrupted tree, where
 **   parts might even still be stashed away in the temporary buffer,
 **   and thus be lost.
 ** We consider this unfortunate, yet justified  by the very nature of applying a diff.
 ** When the user needs safety or transactional behaviour, a deep copy should be made
 ** before attaching the #DiffApplicator
 ** 
 ** @note as of 2/2016, there is the possibility this solution will become part
 **       of a more generic solution, currently being worked out in tree-diff-mutator-binding.hpp
 ** 
 ** @see DiffTreeApplication_test
 ** @see DiffListApplication_test
 ** @see GenNodeBasic_test
 ** @see tree-diff.hpp
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_APPLICATION_H
#define LIB_DIFF_TREE_DIFF_APPLICATION_H


#include "lib/diff/tree-diff.hpp"
#include "lib/diff/tree-diff-traits.hpp"
#include "lib/diff/tree-diff-mutator-binding.hpp"
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
   * Interpreter for the tree-diff-language to work on arbitrary
   * opaque target data structures. A concrete strategy to apply a structural diff
   * to otherwise undisclosed, recursive, tree-like target data. The only requirement
   * is for this target structure to expose a hook for building a customised
   * TreeMutator able to work on and transform the private target data.
   * 
   * In the extended configuration for tree-diff-application to given opaque target
   * data, the setup uses the [metaprogramming adapter traits](\ref TreeDiffTraits)
   * to pave a way for building the custom TreeMutator implementation internally wired
   * to the given opaque target. Moreover, based on the concrete target type, a suitable
   * ScopeManager implementation can be provided. Together, these two dynamically created
   * adapters allow the generic TreeDiffMutatorBinding to perform all of the actual
   * diff application and mutation task.
   * 
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @see DiffVirtualisedApplication_test usage example of this combined machinery
   * @see #TreeDiffInterpreter explanation of the verbs
   */
  template<class TAR>
  class DiffApplicationStrategy<TAR,  enable_if<TreeDiffTraits<TAR>>>
    : public TreeDiffMutatorBinding
    {
      using Scopes = StackScopeManager<TreeMutatorSizeTraits<TAR>::siz>;
      
      
      TAR& subject_;
      Scopes scopes_;
      
      
      void
      buildMutator (DiffMutable& targetBinding)
        {
          scopes_.clear();
          TreeMutator::Handle buffHandle = scopes_.openScope();
          targetBinding.buildMutator (buffHandle);
        }
      
    public:
      explicit
      DiffApplicationStrategy(TAR& subject)
        : TreeDiffMutatorBinding()
        , subject_(subject)
        , scopes_()
        { }
      
      void
      initDiffApplication()
        {
          auto target = mutatorBinding (subject_);
          buildMutator (target);
          TreeDiffMutatorBinding::scopeManger_ = &scopes_;
          TreeDiffMutatorBinding::treeMutator_ = &scopes_.currentScope();
          TreeDiffMutatorBinding::initDiffApplication();
        }
    };
  
  
  /**
   * Interpreter for the tree-diff-language to work on GenNode elements
   * A concrete strategy to apply a structural diff to a target data structure
   * made from #Record<GenNode> elements. This data structure is assumed to be
   * recursive, tree-like. But because Record elements are conceived as immutable
   * and value-like, the tree diff application actually works on a Rec::Mutator
   * wrapping the target record to be altered through consuming the diff.
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @see #TreeDiffInterpreter explanation of the verbs
   */
  template<>
  class DiffApplicationStrategy<Rec::Mutator>
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
              if (not target.empty())      // re-entrance:
                {                         //  discard garbage from previous usage.
                  Rec pristineSequence;  //   must start new sequence from scratch
                  target.swap (pristineSequence);
                }
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
      
      
      
      /* == Implementation of the list diff application primitives == */
      
      virtual void
      ins (GenNode const& n)  override
        {
          if (n.isNamed())
            if (n.isTypeID())
              out().setType (n.data.get<string>());
            else
              out().appendAttrib(n);                            //////////////TICKET #969  dto.
          else
            {
              out().appendChild(n);
              if (src().currIsAttrib())
                src().jumpToChildScope();
            }
        }
      
      virtual void
      del (GenNode const& n)  override
        {
          __expect_in_target(n, "remove");
          ++src();
        }
      
      virtual void
      pick (GenNode const& n)  override
        {
          __expect_in_target(n, "pick");
          move_into_new_sequence (srcPos());
          ++src();
        }
      
      virtual void
      skip (GenNode const& n)  override
        {
          __expect_further_elements (n);
          ++src();
        }      // assume the actual content has been moved away by a previous find()
      
      virtual void
      find (GenNode const& n)  override
        {
          __expect_further_elements (n);
          Iter found = find_in_current_scope(n);
          __expect_found (n, found);
          move_into_new_sequence (found);
        }      // consume and leave waste, expected to be cleaned-up by skip() later
      
      
      
      /* == Implementation of the tree diff application primitives == */
      
      /** cue to a position behind the named node,
       *  thereby picking (accepting) all traversed elements
       *  into the reshaped new data structure as-is */
      virtual void
      after (GenNode const& n)  override
        {
          if (n.matches(Ref::ATTRIBS))
            while (not endOfData() and srcPos()->isNamed())
              {
                move_into_new_sequence (srcPos());
                ++src();
              }
          else
          if (n.matches(Ref::END))
            while (not endOfData())
              {
                move_into_new_sequence (srcPos());
                ++src();
              }
          else
            while (not (endOfData() or srcPos()->matches(n)))
              {
                move_into_new_sequence (srcPos());
                ++src();
              }
          
          __expect_successful_location(n);
          
          if (not endOfData() and srcPos()->matches(n))
            {
              move_into_new_sequence (srcPos());
              ++src(); // get /after/ an explicitly given position
            }
        }
      
      /** assignment of changed value in one step */
      virtual void
      set (GenNode const& n)  override
        {
          GenNode const& elm = find_child (n.idi);
          unConst(elm).data = n.data;
        }
      
      /** open nested scope to apply diff to child object */
      virtual void
      mut (GenNode const& n)  override
        {
          GenNode const& child = find_child (n.idi);
          Rec const& childRecord = child.data.get<Rec>();
          
          TRACE (diff, "tree-diff: ENTER scope %s", cStr(childRecord));
          scopes_.emplace (mutateInPlace (unConst(childRecord)));
          scopes_.top().init();
        }
      
      /** finish and leave child object scope, return to parent */
      virtual void
      emu (GenNode const& n)  override
        {
          TRACE (diff, "tree-diff: LEAVE scope %s", cStr(alteredRec()));
          
          __expect_end_of_scope (n.idi);
          scopes_.pop();
          __expect_valid_parent_scope (n.idi);
        }
      
      
    public:
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
    };
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
