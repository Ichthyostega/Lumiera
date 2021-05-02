/*
  TreeDiff  -  implementation of diff application to opaque data

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


/** @file tree-diff.cpp
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
 ** @see tree-diff.hpp
 ** @see tree-diff-application.hpp
 ** @see DiffComplexApplication_test
 ** 
 */


#include "lib/error.hpp"
#include "lib/diff/tree-diff-application.hpp"



namespace lib {
namespace diff{
  
  TreeMutator::~TreeMutator() { }  ///< emit VTables here...
  
  ScopeManager::~ScopeManager() { };

  
  
  
  
  /* ======= Implementation of Tree Diff Application via TreeMutator ======= */
  
  using util::unConst;
  using util::cStr;
  using util::_Fmt;
  using std::move;
  using std::swap;
  
  
  /** @internal possibly recursive invocation to build a TreeMutator binding
   * to an "object" / scope /child node. This function is invoked when creating
   * a DiffApplicator<Rec::Mutator>, and it is then invoked recursively, when
   * the top level TreeMutator enters a nested scope (child node).
   */
  template<>
  void
  Record<GenNode>::Mutator::buildMutator (BufferHandle buff)
  {
     buff.emplace (
       TreeMutator::build()
                   .attach (*this));
  }
  
  
  
  
  
  /* == Forwarding: error handling == */
  
  
  void
  TreeDiffMutatorBinding::__failMismatch (Literal oper, GenNode const& spec)
  {
    throw error::State(_Fmt("Unable to %s element %s. Current shape of target "
                            "data does not match expectations") % oper % spec
                      , LUMIERA_ERROR_DIFF_CONFLICT);
  }
  
  void
  TreeDiffMutatorBinding::__expect_further_elements (GenNode const& elm)
  {
    if (not treeMutator_->hasSrc())
      throw error::State(_Fmt("Premature end of target sequence, still expecting element %s; "
                              "unable to apply diff further.") % elm
                        , LUMIERA_ERROR_DIFF_CONFLICT);
  }
  
  void
  TreeDiffMutatorBinding::__fail_not_found (GenNode const& elm)
  {
    throw error::State(_Fmt("Premature end of sequence; unable to locate "
                            "element %s in the remainder of the target.") % elm
                      , LUMIERA_ERROR_DIFF_CONFLICT);
  }
  
  void
  TreeDiffMutatorBinding::__expect_end_of_scope (GenNode::ID const& idi)
  {
    if (not treeMutator_->completeScope())
      throw error::State(_Fmt("Diff application floundered in nested scope %s; "
                              "unexpected extra elements found when diff "
                              "should have settled everything.") % idi.getSym()
                        , LUMIERA_ERROR_DIFF_STRUCTURE);
  }
  
  void
  TreeDiffMutatorBinding::__expect_valid_parent_scope (GenNode::ID const& idi)
  {
    if (0 == scopeManger_->depth())
      throw error::Fatal(_Fmt("Diff application floundered after leaving scope %s; "
                              "unbalanced nested scopes, diff attempts to pop root.") % idi.getSym()
                        , LUMIERA_ERROR_DIFF_STRUCTURE);
  }
  
  
  
  
  
  /* == Implementation of the list diff application primitives == */
  
  void
  TreeDiffMutatorBinding::ins (GenNode const& n)
  {
    bool success = treeMutator_->injectNew(n);
    if (not success)
      __failMismatch ("insert", n);
  }
  
  void
  TreeDiffMutatorBinding::del (GenNode const& n)
  {
    __expect_further_elements(n);
    if (not treeMutator_->matchSrc(n))
      __failMismatch("remove", n);
    
    treeMutator_->skipSrc(n);
  }
  
  void
  TreeDiffMutatorBinding::pick (GenNode const& n)
  {
    bool success = treeMutator_->acceptSrc (n);
    if (not success)
      __failMismatch ("pick", n);
  }
  
  void
  TreeDiffMutatorBinding::skip (GenNode const& n)
  {
    __expect_further_elements (n);
    treeMutator_->skipSrc(n);
  }      // assume the actual content has been moved away by a previous find()
  
  void
  TreeDiffMutatorBinding::find (GenNode const& n)
  {
    __expect_further_elements (n);
         // consume and leave waste, expected to be cleaned-up by skip() later
    if (not treeMutator_->findSrc(n))
      __fail_not_found (n);
  }
  
  
  
  
  /* == Implementation of the tree diff application primitives == */
  
  /** cue to a position behind the named node,
   *  thereby picking (accepting) all traversed elements
   *  into the reshaped new data structure as-is */
  void
  TreeDiffMutatorBinding::after (GenNode const& n)
  {
    if (not treeMutator_->accept_until(n))
      __fail_not_found (n);
  }
  
  /** assignment of changed value in one step */
  void
  TreeDiffMutatorBinding::set (GenNode const& n)
  {
    if (not treeMutator_->assignElm(n))
      __failMismatch("assign", n);
  }
  
  /** open nested scope to apply diff to child object */
  void
  TreeDiffMutatorBinding::mut (GenNode const& n)
  {
    TreeMutator::Handle buffHandle = scopeManger_->openScope();      // hint: treeMutatorSize(...)
    if (not treeMutator_->mutateChild(n, buffHandle))
      __failMismatch("enter nested scope", n);
    
    TRACE (diff, "tree-diff: ENTER scope %s", cStr(n.idi));
    treeMutator_ = buffHandle.get();
  }
  
  /** finish and leave child object scope, return to parent */
  void
  TreeDiffMutatorBinding::emu (GenNode const& n)
  {
    TRACE (diff, "tree-diff: LEAVE scope %s", cStr(n.idi));
    
    __expect_end_of_scope (n.idi);
    treeMutator_ = &scopeManger_->closeScope();
    __expect_valid_parent_scope (n.idi);
  }
  
  
  
}} // namespace lib::diff
