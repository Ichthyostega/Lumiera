/*
  ITER-TREE-EXPLORER.hpp  -  building blocks for iterator evaluation strategies

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file iter-tree-explorer.hpp
 ** Building tree expanding and backtracking evaluations within hierarchical scopes.
 ** Based on the <b>Lumiera Forward Iterator</b> concept and using the basic IterAdaptor templates,
 ** these components allow to implement typical evaluation strategies, like e.g. depth-first or
 ** breadth-first exploration of a hierarchical structure. Since the access to this structure is
 ** abstracted through the underlying iterator, what we effectively get is a functional datastructure.
 ** The implementation is based on the IterStateWrapper, which is one of the basic helper templates
 ** provided by iter-adapter.hpp.
 ** 
 ** @remark as of 2017, this template, as well as the initial IterExplorer (draft from 2012) can be
 **         seen as steps towards designing a framework of building blocks for tree expanding and
 **         backtracking algorithms. Due to the nature of Lumiera's design, we repeatedly encounter
 **         this kind of computation pattern, when it comes to matching flexible configuration against
 **         a likewise hierarchical and rules based model. To keep the code base maintainable,
 **         we deem it crucial to reduce the inherent complexity in such algorithms by clearly
 **         separate the _mechanics of evaluation_ from the actual logic of the target domain.
 ** 
 ** # Iterators as Monad
 ** The fundamental idea behind the implementation technique used here is the \em Monad pattern
 ** known from functional programming. A Monad is a (abstract) container created by using some specific functions.
 ** This is an rather abstract concept with a wide variety of applications (things like IO state, parsers, combinators,
 ** calculations with exception handling but also simple data structures like lists or trees). The key point with any
 ** monad is the ability to \em bind a function into the monad; this function will work on the \em internals of the
 ** monad and produce a modified new monad instance. In the simple case of a list, "binding" a function
 ** basically means to map the function onto the elements in the list.
 ** 
 ** ## Rationale
 ** The primary benefit of using the monad pattern is to separate the transforming operation completely from
 ** the mechanics of applying that operation and combining the results. More specifically, we rely on an iterator
 ** to represent an abstracted source of data and we expose the combined and transformed results again as such
 ** an abstracted data sequence. While the transformation to apply can be selected at runtime (as a functor),
 ** the monad pattern defines a sane way to represent partial evaluation state without requiring a container
 ** for intermediary results. This is especially helpful when
 ** - a flexible and unspecific source data structure needs to be processed
 ** - and this evaluation needs to be done asynchronously and in parallel (no locking, immutable data)
 ** - and a partial evaluation needs to be stored as continuation (not relying on the stack for partial results)
 ** 
 ** @todo WIP-WIP-WIP initial draft as of 11/2017
 ** 
 ** @see IterTreeExplorer_test
 ** @see iter-adapter.hpp
 ** @see itertools.hpp
 ** @see IterSource (completely opaque iterator)
 **
 */


#ifndef LIB_ITER_TREE_EXPLORER_H
#define LIB_ITER_TREE_EXPLORER_H


#include "lib/error.hpp"
#include "lib/meta/trait.hpp"
#include "lib/meta/duck-detector.hpp"
#include "lib/meta/function.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/iter-stack.hpp"
#include "lib/meta/trait.hpp" ////////////////TODO
#include "lib/null-value.hpp" ////////////////TODO
#include "lib/util.hpp"

//#include <boost/utility/enable_if.hpp>  //////////////TODO
#include <stack>                        ////////////////TODO
#include <utility>
#include <functional>


namespace lib {
  
  using std::move;
  using std::forward;
  using std::function;
  
  namespace iter_explorer {
    
    template<class CON>
    using iterator = typename meta::Strip<CON>::TypeReferred::iterator;
    template<class CON>
    using const_iterator = typename meta::Strip<CON>::TypeReferred::const_iterator;
    
    template<class CON>
    struct StlRange
      : RangeIter<iterator<CON>>
      {
        StlRange() =default;
        StlRange (CON& container)
          : RangeIter<iterator<CON>> {begin(container), end(container)}
          { }
       // standard copy operations acceptable
      };
    
    template<class CON>
    struct StlRange<const CON>
      : RangeIter<const_iterator<CON>>
      {
        StlRange() =default;
        StlRange (CON const& container)
          : RangeIter<const_iterator<CON>> {begin(container), end(container)}
          { }
       // standard copy operations acceptable
      };
    
  }//(End) namespace iter_explorer : predefined policies and configurations
  
  
  namespace { // TreeExplorer traits
    
    using meta::enable_if;
    using meta::Yes_t;
    using meta::No_t;
    using std::__and_;
    using std::__not_;
    using std::is_constructible;
    using meta::can_IterForEach;
    using meta::can_STL_ForEach;
    
    META_DETECT_EXTENSION_POINT(checkPoint);
    META_DETECT_EXTENSION_POINT(iterNext);
    META_DETECT_EXTENSION_POINT(yield);
    
    template<class SRC>
    struct is_StateCore
      : __and_< HasExtensionPoint_checkPoint<SRC const&>
              , HasExtensionPoint_iterNext<SRC &>
              , HasExtensionPoint_yield<SRC const&>
              >
      { };
    
    template<class SRC>
    struct shall_wrap_STL_Iter
      : __and_<can_STL_ForEach<SRC>
              ,__not_<can_IterForEach<SRC>>
              >
      { };
    
    
    template<class SRC, typename SEL=void>
    struct _TreeExplorerTraits
      {
        static_assert (!sizeof(SRC), "Can not build TreeExplorer: Unable to figure out how to iterate the given SRC type.");
      };
    
    template<class SRC>
    struct _TreeExplorerTraits<SRC,   enable_if<is_StateCore<SRC>>>
      {
        using SrcVal  = typename std::remove_reference<decltype(yield (std::declval<SRC>()))>::type;
        using SrcIter = IterStateWrapper<SrcVal, SRC>;
      };
    
    template<class SRC>
    struct _TreeExplorerTraits<SRC,   enable_if<can_IterForEach<SRC>>>
      {
        using SrcIter = typename std::remove_reference<SRC>::type;
      };
    
    template<class SRC>
    struct _TreeExplorerTraits<SRC,   enable_if<shall_wrap_STL_Iter<SRC>>>
      {
        static_assert (not std::is_rvalue_reference<SRC>::value,
                       "container needs to exist elsewhere during the lifetime of the iteration");
        using SrcIter = iter_explorer::StlRange<SRC>;
      };
    
  }//(End) TreeExplorer traits
  
  
  
  namespace iter_source {
    
    template<class EXP, class SIG, class SEL =void>
    class Expander
      : public EXP
      {
        function<SIG> expandChildren_;
        
      public:
        Expander() =default;
        // inherited default copy operations
        
        template<typename FUN>
        Expander (EXP&& parentExplorer, FUN&& expandFunctor)
          : EXP{move (parentExplorer)}
          , expandChildren_{forward<FUN> (expandFunctor)}
          { }
        
        
        /** core operation: expand current head element */
        Expander&
        expand()
          {
            UNIMPLEMENTED ("expand-children core operation");
          }
        
        /** diagnostics: current level of nested child expansion */
        size_t
        depth()  const
          {
            UNIMPLEMENTED ("implementation of the expand mechanics");
          }
      };
  }
  
  
  
  
  /**
   * Adapter to build a demand-driven tree expanding and exploring computation
   * based on a custom opaque _state core_. TreeExploer adheres to the _Monad_
   * pattern known from functional programming, insofar the _expansion step_ is
   * tied into the basic template by means of a function provided at usage site.
   * 
   * @todo WIP -- preliminary draft as of 11/2017
   */
  template<class SRC
          >
  class TreeExplorer
    : public SRC
    {
      
      
    public:
      typedef typename SRC::value_type value_type;
      typedef typename SRC::reference reference;
      typedef typename SRC::pointer  pointer;
      
      
      /** by default create an empty iterator */
      TreeExplorer() { }
      
      // default copy acceptable (unless prohibited by nested state core)
      
      
      /** wrap an iterator-like state representation
       *  to build it into a monad. The resulting entity
       *  is both an iterator yielding the elements generated
       *  by the core, and it provides the (monad) bind operator.
       */
      explicit
      TreeExplorer (SRC iterStateCore)
        : SRC{std::move (iterStateCore)}
        { }
      
      
      /* ==== Builder functions ==== */
      
      template<class FUN>
      auto
      expand (FUN&& expandFunctor)
        {
          using FunSig = typename meta::_Fun<FUN>::Sig;
          using This   = typename meta::Strip<decltype(*this)>::TypeReferred;
          
          return iter_source::Expander<This, FunSig> {move(*this), forward<FUN>(expandFunctor)};
        }
    private:
    };
  
  
  
  
  
  
  
  
  
  
  namespace iter_explorer {
    
    /////TODO RLY?
    
//  using util::unConst;
//  using lib::meta::enable_if;
//  using lib::meta::disable_if;
//  using std::function;
//  using meta::_Fun;
  }  
  
  
  
  /* ==== convenient builder free functions ==== */

  template<class IT>
  inline auto
  treeExplore (IT&& srcSeq)
  {
    using SrcIter = typename _TreeExplorerTraits<IT>::SrcIter;
    
    return TreeExplorer<SrcIter> {std::forward<IT>(srcSeq)};
  }
  
  
/*
  template<class IT>
  inline iter_explorer::DepthFirst<IT>
  depthFirst (IT const& srcSeq)
  {
    return iter_explorer::DepthFirst<IT> (srcSeq);
  }
  
  
  template<class IT>
  inline iter_explorer::BreadthFirst<IT>
  breadthFirst (IT const& srcSeq)
  {
    return iter_explorer::BreadthFirst<IT> (srcSeq);
  }
  
*/
  
  
} // namespace lib
#endif /* LIB_ITER_TREE_EXPLORER_H */
