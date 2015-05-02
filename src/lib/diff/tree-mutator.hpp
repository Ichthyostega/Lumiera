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
 ** @see generic-tree-mutator-test.cpp
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
  
  namespace {
    template<class PAR>
    class Builder;
    
    typedef Literal ID;
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
      insertChild (ID id)
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
  
  namespace {
    
    template<class PAR, typename VAL>
    struct ChangeOperation
      : PAR
      {
        using Closure = function<void(VAL)>;
        
        ID attribID_;
        Closure change_;
        
        virtual void
        setAttribute (ID id, Attribute& newValue)
          {
            if (id == attribID_)
              change_(newValue.get<VAL>());
            
            else // delegate to other closures (Decorator-style)
              PAR::setAttribute(id, newValue);
          }
        
        ChangeOperation(ID id, Closure clo, PAR const& chain)
          : PAR(chain)
          , attribID_(id)
          , change_(clo)
          { }
      };
    
    template<class PAR>
    struct Builder
      : PAR
      {
        Builder(PAR par)
          : PAR(par)
          { }
        
        template<typename VAL>
        using Change = ChangeOperation<PAR,VAL>;
        template<typename VAL>
        using Closure = typename Change<VAL>::Closure;
        
        
        /* ==== binding API ==== */
        
        template<typename VAL>
        Builder<Change<VAL>>
        change (Literal attributeID, Closure<VAL> closure)
          {
            return Change<VAL> (attributeID, closure, *this);
          }
      };

  }
  
  Builder<TreeMutator>
  TreeMutator::build ()
  {
    return TreeMutator();
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_H*/
