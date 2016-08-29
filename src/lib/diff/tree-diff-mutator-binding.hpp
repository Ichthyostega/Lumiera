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
 ** private data structures with hierarchical nature. This is a variation
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
 ** use will have to provide a set of closures (which might even partially be generated
 ** functors) to translate the _implementation actions_ underlying the language into
 ** _concrete actions_ working on local data.
 ** 
 ** ### Generic and variable parts
 ** So this is a link between generic [»tree diff language«](\ref tree-diff.hpp)
 ** interpretation and the concrete yet undisclosed private data structure, and
 ** most of this implementation is entirely generic, since the specifics are
 ** abstracted away behind the TreeMutator interface. For this reason, most of
 ** this explicit template specialisation code, especially. the virtual functions,
 ** can be emitted right here, within the library module. This helps to reduce
 ** "template bloat" and simplifies the dynamic linking. Thus, this header
 ** only contains the definition and the ctor code, which indeed needs to
 ** be adapted to each usage situation, while the main body of the
 ** functionality has been moved to the corresponding implementation
 ** file, where this template is explicitly instantiated, to force
 ** code generation into the library module.
 ** 
 ** @todo this is WIP as of 2/2016 -- in the end it might be merged back or even
 **       replace the tree-diff-application.hpp
 ** 
 ** @see DiffComplexApplication_test
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
#include "lib/diff/diff-mutable.hpp"
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
    : boost::noncopyable
    {
    public:
      virtual ~ScopeManager();  ///< this is an interface
      
      virtual TreeMutator::Handle openScope()   =0;
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
   * Interpreter for the tree-diff-language to work on arbitrary, undisclosed
   * local data structures. The key point to note is that this local data is
   * not required to implement any specific interface. The only requirement is
   * the ability somehow to support the basic operations of applying a structural
   * diff. This is ensured with the help of a _customisable adapter_ the TreeMutator.
   * @throws  lumiera::error::State when diff application fails structurally.
   * @throws  _unspecified errors_ when delegated operations fail.
   * @see TreeDiffInterpreter explanation of the verbs
   * @see DiffComplexApplication_test demonstration of usage
   */
  class TreeDiffMutatorBinding
    : public TreeDiffInterpreter
    {
    protected:
      TreeMutator*  treeMutator_;
      ScopeManager* scopeManger_;
      
    private:
      
      /* == Forwarding: error handling == */
      
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
  
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_DIFF_MUTATOR_BINDING_H*/
