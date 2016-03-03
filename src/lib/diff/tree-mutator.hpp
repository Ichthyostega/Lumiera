/*
  TREE-MUTATOR.hpp  -  flexible binding to map generic tree changing operations

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator.hpp
 ** Customisable intermediary to abstract generic tree mutation operations.
 ** This is the foundation for generic treatment of tree altering operations,
 ** and especially the handling of changes (diff) to hierarchical data structures.
 ** The goal is to represent a standard set of conceptual operations working on
 ** arbitrary data structures, without the need for these data structures to
 ** comply to any interface or base type. Rather, we allow each instance to
 ** define binding closures, which allows to tap into arbitrary internal data
 ** representation, without any need of disclosure. The only assumption is
 ** that the data to be treated is \em hierarchical and \em object-like,
 ** i.e. it has (named) attributes and it may have a collection of children.
 ** If necessary, typing constraints can be integrated through symbolic
 ** representation of types as chained identifiers. (path dependent types).
 ** 
 ** The interface implemented by the TreeMutator is shaped such as to support
 ** the primitives of Lumiera's tree \link diff-language.hpp diff handling language. \endlink
 ** By default, each of these primitives is implemented as a \c NOP -- but each operation
 ** can be replaced by a binding closure, which allows to invoke arbitrary code in the
 ** context of the given object's implementation internals.
 ** 
 ** ## Builder/Adapter concept
 ** TreeMutator is both an interface and a set of building blocks.
 ** On concrete usage, the (private, non disclosed) target data structure is assumed
 ** to _build a subclass of TreeMutator._ To this end, the TreeMutator is complemented
 ** by a builder API. Each call on this builder -- typically providing some closure --
 ** will add yet another decorating layer on top of the basic TreeMutator (recall all
 ** the "mutation primitives" are implemented NOP within the base class). So the actual
 ** TreeMutator will be structured like an onion, where each layer cares for the sole
 ** concrete aspect it was tied for by the supplied closure. For example, there might
 ** be a decorator to handle setting of a "foobar" attribute. Thus, when the diff
 ** dictates to mutate "foobar", the corresponding closure will be invoked.
 ** 
 ** \par test dummy target
 ** There is a special adapter binding to support writing unit tests. The corresponding
 ** API is only declared (forward) by default. The TestMutationTarget is a helper class,
 ** which can be attached through this binding and allows a unit test fixture to record
 ** and verify all the mutation operations encountered.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_H
#define LIB_DIFF_TREE_MUTATOR_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

#include <functional>
#include <string>
//#include <vector>
//#include <map>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  using lib::Literal;
  using std::function;
  using std::string;
  
  
  class TestMutationTarget; // for unit testing
  
  
  namespace {
    template<class PAR>
    struct Builder;
    
    using ID        = Literal;
    using Attribute = DataCap;
  }
  
  
  /**
   * Customisable intermediary to abstract
   * mutating operations on arbitrary, hierarchical object-like data.
   * The TreeMutator exposes two distinct interfaces
   * - the \em operation API -- similar to what a container exposes --
   *   is the entirety of abstract operations that can be done to the
   *   subsumed, tree like target structure
   * - the \em binding API allows to link some or all of these generic
   *   activities to concrete manipulations known within target scope.
   */
  class TreeMutator
    {
      
    public:
      
      /* ==== operation API ==== */
      
      virtual void
      injectNew (GenNode const& n)
        {
          UNIMPLEMENTED("establish new child node at current position");
        }
      
      virtual void
      deleteChild (ID id)
        {
          UNIMPLEMENTED("destroy child node at current position");
        }
      
      virtual void
      findChild (ID id)
        {
          UNIMPLEMENTED("look ahead, find and retrieve denoted child to be relocated at current position");
        }
      
      virtual TreeMutator&
      mutateChild (ID id)
        {
          UNIMPLEMENTED("expose a recursive TreeMutator to transform the denoted child");
        }
      
      virtual void setAttribute (ID, Attribute&) { /* do nothing by default */ }
      
      /**
       * start building a custom adapted tree mutator,
       * where the operations are tied by closures or
       * wrappers into the current implementation context.
       */
      static Builder<TreeMutator> build();
    };
  
  namespace { // Mutator-Builder decorator components...
    
    /**
     * Type rebinding helper to pick up the actual argument type.
     * Works both for functors and for lambda expressions
     * @remarks Solution proposed 10/2011 by \link http://stackoverflow.com/users/224671/kennytm user "kennytm" \endlink
     *          in this \link http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda/7943765#7943765
     *          answer on stackoverflow \endlink
     */
    template<typename FUN>
    struct _ClosureType
      : _ClosureType<decltype(&FUN::operator())>
      { };

    template<class C, class RET, class ARG>
    struct _ClosureType<RET (C::*)(ARG)  const>
      {
        typedef ARG ArgType;
        typedef RET ReturnType;
      };
    
    
    template<class PAR, class CLO>
    struct ChangeOperation
      : PAR
      {
        ID attribID_;
        CLO change_;
        
        virtual void
        setAttribute (ID id, Attribute& newValue)
          {
            using ValueType = typename _ClosureType<CLO>::ArgType;
            
            if (id == attribID_)
              change_(newValue.get<ValueType>());
            
            else // delegate to other closures (Decorator-style)
              PAR::setAttribute(id, newValue);
          }
        
        ChangeOperation(ID id, CLO clo, PAR const& chain)
          : PAR(chain)
          , attribID_(id)
          , change_(clo)
          { }
      };
    
    
    template<class PAR>
    struct TestWireTap;
    
    
    template<class PAR>
    struct Builder
      : PAR
      {
        Builder(PAR par)
          : PAR(par)
          { }
        
        template<class CLO>
        using Change = ChangeOperation<PAR,CLO>;
        
        using WireTap = TestWireTap<PAR>;
        
        
        /* ==== binding API ==== */
        
        template<typename CLO>
        Builder<Change<CLO>>
        change (Literal attributeID, CLO closure)
          {
            return Change<CLO> (attributeID, closure, *this);
          }
        
        Builder<WireTap>
        attachDummy (TestMutationTarget& dummy);
        
      };
    
  }//(END) Mutator-Builder decorator components...
  
  
  Builder<TreeMutator>
  TreeMutator::build ()
  {
    return TreeMutator();
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_H*/
