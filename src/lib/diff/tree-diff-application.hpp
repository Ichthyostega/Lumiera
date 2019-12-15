/*
  TREE-DIFF-APPLICATION.hpp  -  consume and apply a tree diff

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


/** @file tree-diff-application.hpp
 ** Concrete implementation to apply structural changes to hierarchical
 ** data structures. Together with the generic #DiffApplicator, this allows
 ** to receive linearised structural diff descriptions and apply them to
 ** a given target data structure, to effect the corresponding changes.
 ** 
 ** ## Design considerations
 ** While -- conceptually -- our tree diff handling can be seen as an extension
 ** and generalisation of list diffing, the decision was \em not to embody this
 ** extension into the implementation technically, for sake of clarity. This would
 ** be implementation re-use, as opposed to building a new viable abstraction.
 ** No one outside the implementation realm would benefit from such an abstraction,
 ** so we prefer to understand the tree diff language as the abstraction, which
 ** needs to embodied into two distinct contexts of implementation. So the list diff
 ** application strategy can be seen as blueprint and demonstration of principles.
 ** 
 ** ### Use cases
 ** Initially, we'd have to distinguish two usage situations
 **  - apply a diff to a generic tree representation, based on Record<GenNode>
 **  - apply a diff to some tree shaped implementation data structure.
 ** _Conceptually_ we use the former as blueprint and base to define the semantics
 ** of our »tree-diff language«, while the latter is an extension and can be supported
 ** within the limits of precisely these tree-diff semantics. That is, we support diff
 ** application to all implementation data structures which are _conceptually congruent_
 ** to the generic tree representation. This extension happens in accordance to the
 ** goals of our "diff framework", since we want to allow collaboration between
 ** loosely coupled subsystems, without the need of a shared data structure.
 ** 
 ** ### Implementation
 ** On the implementation level though, relations are the other way round: the
 ** framework and technique to enable applying a diff onto private implementation data
 ** is used also to apply the diff onto the (likewise private) implementation of our
 ** generic tree representation. Because the common goal is loose coupling, we strive
 ** at imposing as few requirements or limitations onto the implementation as possible.
 ** 
 ** Rather we require the implementation to provide a _binding,_ which can then be used
 ** to _execute_ the changes as dictated by the incoming diff. But since this binding
 ** has to consider intricate details of the diff language's semantics and implementation,
 ** we provide a *Builder DSL*, so the client may assemble the concrete binding from
 ** preconfigured building blocks for the most common cases
 ** - binding "attributes" to object fields
 ** - binding "children" to a STL collection of values
 ** - binding especially to a collection of GenNode elements,
 **   which basically covers also the generic tree representation.
 ** 
 ** #### State and nested scopes
 ** For performance reasons, the diff is applied _in place_, directly mutating the
 ** target data structure. This makes the diff application _stateful_ -- and in case of
 ** a *diff conflict*, the target *will be corrupted*.
 ** 
 ** Our tree like data structures are conceived as a system of nested scopes. Within
 ** each scope, we have a list of elements, to which a list-diff is applied. When commencing
 ** diff application, a one time adapter and intermediary is constructed: the TreeMutator.
 ** This requires the help of the target data structure to set up the necessary bindings,
 ** since the diff applicator as such has no knowledge about the target data implementation.
 ** At this point, the existing (old) contents of the initial scope are moved away into an
 ** internal _source sequence buffer,_ from where they may be "picked" and moved back into
 ** place step by step through the diff. After possibly establishing a new order, inserting
 ** or omitting content within a given "object" (Record), the tree diff language allows in
 ** a second step to _open_ some of the child "objects" by entering nested scopes, to effect
 ** further changes within the selected child node. This is done within the `mut(ID)....emu(ID)`
 ** bracketing construct of the diff language. On the implementation side, this recursive
 ** descent and diff application is implemented with the help of a stack, where a new
 ** TreeMutator is constructed whenever we enter (push) a new nested scope.
 ** 
 ** #### Yet another indirection
 ** Unfortunately this leads to yet another indirection layer: Implementing a
 ** language in itself is necessarily a double dispatch (we have to abstract the
 ** verbs and we have to abstract the implementation side). And now we're decoupling
 ** the implementation side from a concrete data structure. Which means, that the user
 ** will have to provide a set of closures (which might even partially be generated
 ** functors) to translate the _implementation actions_ underlying the language into
 ** _concrete actions_ working on local data.
 ** 
 ** #### Generic and variable parts
 ** So there is a link between generic [»tree diff language«](\ref tree-diff.hpp)
 ** interpretation and the concrete yet undisclosed private data structure, and
 ** most of this implementation is entirely generic, since the specifics are
 ** abstracted away behind the TreeMutator interface. For this reason, most of
 ** this _delegating implementation_ can be emitted right here, within the
 ** library module. With the sole exception of the ctor, which needs to
 ** figure out a way how to "get" a suitable TreeMutator implementation
 ** for the given concrete target data.
 ** 
 ** ### the TreeMutator DSL
 ** In the end, for each target structure, a concrete TreeMutator needs to be built
 ** or provided within the realm of the actual data implementation, so the knowledge
 ** about the actual data layout remains confined there. While this requires some
 ** understanding regarding structure and semantics of the diff language, most data
 ** implementation will rely on some very common representation techniques, like using
 ** object fields as "attributes" and a STL collection to hold the "children". Based
 ** on this insight, we provide a DSL with standard adapters and building blocks,
 ** to ease the task of generating ("binding") the actual TreeMutator. The usage site
 ** needs to supply only some functors or lambda expressions to specify how to deal
 ** with the actual representation data values:
 ** - how to construct a new entity
 ** - decide when the binding actually becomes active
 ** - how to determine a diff verb "matches" the actual data
 ** - how to set a value or how to recurse into a sub scope
 ** 
 ** @see DiffTreeApplication_test
 ** @see DiffComplexApplication_test
 ** @see DiffListApplication_test
 ** @see GenNode_test
 ** @see tree-diff.hpp
 ** 
 */


#ifndef LIB_DIFF_TREE_DIFF_APPLICATION_H
#define LIB_DIFF_TREE_DIFF_APPLICATION_H


#include "lib/diff/tree-diff.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/diff/diff-mutable.hpp"
#include "lib/diff/tree-diff-traits.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-string.hpp"
#include "lib/util.hpp"

#include <utility>
#include <stack>

namespace lib {
namespace diff{
  
  /**
   * Management interface to deal with storage for
   * TreeMutators dedicated to nested scopes
   */
  class ScopeManager
    : util::NonCopyable
    {
    public:
      virtual ~ScopeManager();  ///< this is an interface
      
      virtual TreeMutator::Handle openScope()   =0;      ///< @remark picks up `treeMutatorSize(...)`
      virtual TreeMutator&        closeScope()  =0;
      virtual void                clear()       =0;
      
      virtual size_t depth()  const             =0;
    };
  
  
  
  /**
   * Typical standard implementation of the ScopeManager.
   * Using Heap memory for the nested scopes, we create a stack
   * of opaque InPlaceBuffers for each scope, which allows the
   * PlantingHandle mechanism to let the target object corresponding
   * to this scope build its own TreeMutator implementation into
   * this buffer space for this scope.
   */
  template<size_t buffSiz>
  class StackScopeManager
    : public ScopeManager
    {
      using MutatorBuffer = InPlaceBuffer<TreeMutator, buffSiz>;
      using MutatorStack = std::stack<MutatorBuffer>;
      
      /** Allocate Heap Storage for nested TreeMutator(s) */
      MutatorStack scopes_;
      
      
    public:
      StackScopeManager()
        : scopes_()
        { }
      
      TreeMutator&
      currentScope()  const
        {
          if (0 == depth())
            throw error::Logic("Attempt to access the current scope "
                               "without establishing a root scope beforehand."
                              , error::LUMIERA_ERROR_LIFECYCLE);
          return *scopes_.top();
        }
      
      
      /* ==== ScopeManager interface ==== */
      
      virtual TreeMutator::Handle
      openScope()
        {
          scopes_.emplace();
          TreeMutator::Handle placementHandle (scopes_.top());
          
          static_assert (buffSiz >= sizeof(typename MutatorStack::value_type)
                        ,"insufficient working buffer for TreeMutator");
          return placementHandle;
        }
      
      virtual TreeMutator&
      closeScope()
        {
          scopes_.pop();
          REQUIRE (0 < depth(), "attempt to return beyond root scope");
          return *scopes_.top();
        }
      
      virtual void
      clear()
        {
          while (0 < scopes_.size())
            scopes_.pop();
          
          ENSURE (scopes_.empty());
        }

      
      virtual size_t
      depth()  const
        {
          return scopes_.size();
        }
    };
  
  
  
  
  
  /* ======= Implementation of Tree Diff Application via TreeMutator ======= */
  
  using util::unConst;
  using util::cStr;
  using util::_Fmt;
  using std::move;
  using std::swap;
  
  
  /**
   * Implementation of the tree-diff-language to work on arbitrary tree-like data.
   * This is the core part of the implementation, which maps the _diff verbs_
   * onto the corresponding _primitive operations_ of the TreeMutator interface.
   * The concrete implementation of TreeMutator then is responsible to translate
   * those operations into the correct manipulation of target data.
   * @note implementation of these functions is emitted in tree-diff.cpp and thus
   *       within the library module. For an actual diff-applicator, we also need
   *       to bind to a concrete TreeMutator, for which we need to instantiate
   *       the template DiffApplicationStrategy with the concrete target type
   *       as parameter (see below). This concrete instantiation happens
   *       inline from within the usage context, while inheriting the
   *       actual implementation logic from this baseclass here 
   * 
   * @throws  lumiera::error::State when diff application fails structurally.
   * @throws  _unspecified errors_ when delegated operations fail.
   * @see TreeDiffInterpreter explanation of the verbs
   * @see DiffComplexApplication_test demonstration of usage
   * @see [implementation of the binding functions](\ref tree-diff.cpp)
   */
  class TreeDiffMutatorBinding
    : public TreeDiffInterpreter
    {
    protected:
      TreeMutator*  treeMutator_;
      ScopeManager* scopeManger_;
      
    private:
      
      /* == error handling helpers == */
      
      void __failMismatch (Literal oper, GenNode const& spec);
      void __expect_further_elements (GenNode const& elm);
      void __fail_not_found (GenNode const& elm);
      void __expect_end_of_scope (GenNode::ID const& idi);
      void __expect_valid_parent_scope (GenNode::ID const& idi);
      
      
      
      
      /* == Implementation of the list diff application primitives == */
      
      virtual void ins  (GenNode const& n) override;
      virtual void del  (GenNode const& n) override;
      virtual void pick (GenNode const& n) override;
      virtual void skip (GenNode const& n) override;
      virtual void find (GenNode const& n) override;
      
      
      /* == Implementation of the tree diff application primitives == */
      
      virtual void after(GenNode const& n) override;
      virtual void set  (GenNode const& n) override;
      virtual void mut  (GenNode const& n) override;
      virtual void emu  (GenNode const& n) override;
      
      
    public:
      TreeDiffMutatorBinding()
        : treeMutator_(nullptr)
        , scopeManger_(nullptr)
        { }
    };
  
  
  
  
  
  
  /**
   * Interpreter for the tree-diff-language to work on arbitrary
   * opaque target data structures. A concrete strategy to apply a structural diff
   * to otherwise undisclosed, recursive, tree-like target data. The only requirement
   * is for this target structure to expose a hook for building a customised
   * TreeMutator able to work on and transform the private target data.
   * 
   * This generic setup for diff application covers especially the case where the
   * target data is a "GenNode tree", and the root is accessible as Rec::Mutator
   * (We use the Mutator as entry point, since GenNode trees are by default immutable).
   * 
   * In the extended configuration for tree-diff-application to given opaque target
   * data, the setup uses the [metaprogramming adapter traits](\ref TreeDiffTraits)
   * to pave a way for building the custom TreeMutator implementation, wired internally
   * to the given opaque target. Moreover, based on the concrete target type, a suitable
   * ScopeManager implementation can be provided. Together, these two dynamically created
   * adapters allow the generic TreeDiffMutatorBinding to perform all of the actual
   * diff application and mutation task.
   * 
   * @throws  lumiera::error::State when diff application fails due to the
   *          target sequence being different than assumed by the given diff.
   * @see DiffComplexApplication_test usage example of this combined machinery
   * @see #TreeDiffInterpreter explanation of the verbs
   */
  template<class TAR>
  class DiffApplicationStrategy<TAR,  enable_if<TreeDiffTraits<TAR>>>
    : public TreeDiffMutatorBinding
    {
      using Scopes = StackScopeManager<TreeMutatorSizeTraits<TAR>::siz>;
      
      
      TAR& subject_;
      Scopes scopes_;
      
      
      TreeMutator*
      buildMutator (DiffMutable& targetBinding)
        {
          scopes_.clear();
          TreeMutator::Handle buffHandle = scopes_.openScope();
          targetBinding.buildMutator (buffHandle);
          return buffHandle.get();
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
          using Target = typename TreeDiffTraits<TAR>::Ret;
          
          Target target = mutatorBinding (subject_);
          buildMutator(target)->init();
          TreeDiffMutatorBinding::scopeManger_ = &scopes_;
          TreeDiffMutatorBinding::treeMutator_ = &scopes_.currentScope();
          REQUIRE (this->treeMutator_);
        }
      
      void
      completeDiffApplication()
        {
          if (not treeMutator_->completeScope())
            throw error::State(_Fmt("Unsettled content remains after diff application. "
                                    "Top level == %s") % subject_
                              , LERR_(DIFF_STRUCTURE));
          // discard storage
          treeMutator_ = nullptr;
          scopes_.clear();
        }
    };
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_APPLICATION_H*/
