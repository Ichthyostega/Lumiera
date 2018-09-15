/*
  ITER-CHAIN-SEARCH.hpp  -  chained search with backtracking based on (bidirectional) iterator

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


/** @file iter-chain-search.hpp
 ** Evaluation mechanism to apply a sequence of conditions onto a linear search.
 ** This search algorithm is implemented on top of a tree expanding (monadic) filter pipeline,
 ** to allow for backtracking. The intention is not just to combine the individual conditions, but
 ** rather to apply them one by one. After finding a match for the first condition, we'll search
 ** for the next condition _starting at the position of the previous match_. In the most general
 ** case, this immediate progression down the search chain might be too greedy; it could be that
 ** we don't find a match for the next condition, but if we backtrack and first search further
 ** on the previous condition, continuing with the search from that further position might
 ** then lead to a match. Basically we have to try all combinations of all possible local
 ** matches, to find a solution to satisfy the whole chain of conditions.
 ** 
 ** ## Design
 ** 
 ** The IterChainSearch component is built as a processing pipeline, based on the
 ** [Tree-Explorer framework](\ref iter-tree-explorer.hpp). This yields several benefits, but
 ** imposes some drawbacks. Without much effort, we get an extremely flexible and configurable
 ** solution, with acceptable / moderate performance. The result automatically adapts to a huge
 ** selection of data sources; it is possible (and is even intended) to attach it on top of an
 ** existing on-demand data processing pipeline. The "source" can itself be a "state core" and
 ** we may install suitable filter predicates to possibly collaborate with the internals of
 ** such a state core. Yet we may also confine ourselves to pure functions and value processing.
 ** 
 ** The obvious downside of such an approach is its complexity in terms of code to understand.
 ** The collaboration between several layers in a filter pipeline can be intricate, leading to
 ** chains of layered similar functions calling each other. Most of these abstractions and
 ** decorations will be resolved by the compiler and optimiser, typically leading to long
 ** compilation times and rather large size of the generated code (which in debugging mode
 ** with full type information can even become excessively large).
 ** 
 ** Some caveats
 ** - the resulting pipeline is copyable, and it is typically moved out from a builder
 **   function into the target location. So beware of _lambda captures by reference._
 **   When you capture _anything_ within the pipeline itself, like some `this` pointer,
 **   you'll end up with a dangling reference for sure. However, it is just fine to
 **   capture extended facilities within the scope _enclosing_ the pipeline, as long
 **   as this scope remains intact during the whole usage time of the pipeline.
 ** - the resulting entity, an instantiation of the IterChainSearch template, is
 **   not a generic TreeExplorer (builder). Be careful when using any builder
 **   functions still accessible. The only functions meant to be used in
 **   builder style are the IterChainSearch::search() variations.
 ** - if you copy, the embedded state is copied alongside, but not
 **   any further external state referred by it.
 ** 
 ** @see IterChainSearch_test
 ** @see IterCursor_test
 ** @see iter-tree-explorer.hpp
 ** @see [usage example](event-log.hpp)
 ** 
 */


#ifndef SRC_LIB_ITER_CHAIN_SEARCH_H
#define SRC_LIB_ITER_CHAIN_SEARCH_H


#include "lib/error.hpp"
#include "lib/iter-tree-explorer.hpp"
#include "lib/meta/util.hpp"

#include <utility>
#include <vector>
#include <string>


namespace lib {
namespace iter {
  
  using std::move;
  using std::forward;
  using std::string;
  
  using lib::meta::disable_if;
  
  
  namespace { // type construction helpers...
    
    template<class SRC>
    auto
    buildSearchFilter (SRC&& dataSource)
    {
      return treeExplore (forward<SRC> (dataSource))
                .mutableFilter();
    }
    
    template<class SRC>
    auto
    buildExplorer (SRC&& dataSource)
    {
      return buildSearchFilter (forward<SRC> (dataSource))
                .expand ([](auto it){ return it; });   // child iterator starts as copy of current level iterator
    }
    
    /**
     * @internal helper to rebind on inferred types.
     * @remark we use the TreeExplorer framework to assemble the processing pipeline
     *         from suitable building blocks configured with some lambdas. However, we
     *         also want to _inherit_ from this filter pipeline, so to expose the typical
     *         iterator operations without much ado. Thus we use some (static) helper function
     *         templates, instantiate them with the actual source data iterator and pick up
     *         the inferred type.
     */
    template<class SRC>
    struct _IterChainSetup
      {
        using Filter = decltype( buildSearchFilter(std::declval<SRC>()).asIterator() );
        using Pipeline = decltype( buildExplorer (std::declval<SRC>()) );
        
        /** each step in the chain is a functor to reconfigure the underlying filter */
        using StepFunctor = std::function<void(Filter&)>;
      };
    
  }//(End)type construction helpers
  
  
  
  
  
  /*********************************************************************************************************//**
   * Iterator based linear search mechanism, with the ability to perform consecutive search with backtracking.
   * The IterChainSearch can be configured with a sequence of search goals (filter conditions), and will apply
   * these in succession on the underlying iterator. It will search _by linear search_ for the first hit of the
   * first condition, and then continue to search _from there_ matching on the second condition, and so on.
   * After the first combination of matches is exhausted, the search will backtrack and try to evaluate
   * the next combination, leading to a tree of on-demand search solutions.
   */
  template<class SRC>
  class IterChainSearch
    : public _IterChainSetup<SRC>::Pipeline
    {
      using _Trait = _IterChainSetup<SRC>;
      using _Base  = typename _Trait::Pipeline;
      
      using Value  = typename _Base::value_type;
      using Filter = typename _Trait::Filter;
      using Step   = typename _Trait::StepFunctor;
      
      /** Storage for a sequence of filter configuration functors */
      std::vector<Step> stepChain_;
      
      
      bool
      needsExpansion()  const
        {
          return _Base::depth() < stepChain_.size();
        }

    public:
      /** Build a chain-search mechanism based on the given source data sequence.
       * @remark iterators will be copied or moved as appropriate, while from a STL compliant
       *         container just a pair of (`begin()`, `end()`) iterators is retrieved; the latter
       *         is also the reason why a rvalue reference to STL container is rejected, since the
       *         container needs to reside elsewhere; only the iterator is wrapped here.
       */
      template<class SEQ>
      explicit
      IterChainSearch (SEQ&& srcData)
        : _Base{buildExplorer (forward<SEQ> (srcData))}
        {      // mark initial pristine state
          _Base::disableFilter();
        }
      
      // inherited default ctor and standard copy operations
      using _Base::_Base;
      
      
      /* === adapted iteration control API  === */
      void
      iterNext()
        {
          _Base::__throw_if_empty();
          if (not needsExpansion())
            _Base::iterNext();
          while (needsExpansion()                                   // Backtracking loop: attempt to establish all conditions
                 and _Base::checkPoint())                           // possibly trying further combinations until success:
            {
              uint depth =_Base::depth();
              _Base::expandChildren();                              // create copy of current filter embedded into child level
              stepChain_[depth] (_Base::accessCurrentChildIter());  // invoke step functor to reconfigure this filter...
              _Base::dropExhaustedChildren();                       // which thereby might become empty
            }
        }
      
      IterChainSearch&
      operator++()
        {
          iterNext();
          return *this;
        }
      
      
      
      /** configure additional chained search condition.
       * @param a manipulation functor `void(Filter&)`, which works on the current filter
       *       to possibly change its configuration.
       * @note the given functor, lambda or function reference will be wrapped and adapted
       *       to conform to the required function signature. When using a generic lambda,
       *       the argument type `Filter&` is used for instantiation
       * @remarks the additional chained search condition given here will be applied _after_
       *       matching all other conditions already in the filter chain. Each such condition
       *       is used to _filter_ the underlying source iterator, i.e. pull it until finding
       *       and element to match the condition. Basically these conditions are _not_ used in
       *       conjunction, but rather one after another. But since each such step in the chain
       *       is defined by a functor, which gets the previous filter configuration as argument,
       *       it is _possible_ to build a step which _extends_ or sharpens the preceding condition.
       */
      template<typename FUN>
      IterChainSearch&&
      addStep (FUN&& configureSearchStep)
        {
          if (not this->empty())
            {
              Step nextStep{forward<FUN> (configureSearchStep)};
              
              if (_Base::isDisabled())
                nextStep (*this);                               // apply first step immediately
              else                                             //  (implicitly enables the base filter)
                {
                  stepChain_.emplace_back (move (nextStep)); //    append all further steps into the chain...
                  this->iterNext();                         //     then establish invariant:
                }                                          //      expand to leaf and forward to first match
            }
          return move(*this);
        }
      
      /** attach additional search with the given filter predicate.
       *  After successfully searching for all the conditions currently in the filter chain,
       *  the embedded iterator will finally be pulled until matching the given target value.
       * @remarks adds a new layer on the stack of search conditions with a _copy_ of the
       *  previously used iterator, and installs the given filter predicate therein.
       */
      template<typename FUN>
                                          disable_if<is_convertible<FUN, Value>,
      IterChainSearch&&                             >
      search (FUN&& filterPredicate)
        {
          addStep ([predicate{forward<FUN> (filterPredicate)}]
                   (Filter& filter)
                     {        // manipulte current filter configuration
                       filter.setNewFilter (predicate);
                     });
          return move(*this);
        }
      
      /** attach additional direct search for a given value.
       *  After successfully searching for all the conditions currently in the filter chain,
       *  the embedded iterator will finally be pulled until matching the given target value.
       */
      IterChainSearch&&
      search (Value target)
        {
          search ([target](Value const& currVal) { return currVal == target; });
          return move(*this);
        }
      
      /** drop all search condition frames.
       * @remark the filter chain becomes empty,
       *         passing through the reset of the
       *         source sequence unaltered
       */
      IterChainSearch&&
      clearFilter()
        {
          stepChain_.clear();
          _Base::rootCurrent();
          _Base::disableFilter();
          return move(*this);
        }
    };
  
  
  
  
  /* ==== convenient builder free function ==== */
  
  /** setup a chain search configuration by suitably wrapping the given container.
   * @return an IterChainSearch instantiation, which is an Iterator to yield all the
   *         source elements, but can be outfitted with a sequence of filter conditions,
   *         to be applied to the underlying source one by one.
   * @param srcData either a »Lumiera Forward Iterator«, a _reference_ to a STL
   *         container, or a [»State Core«](\ref lib::IterStateWrapper) object.
   * @warning if you capture the result of this call by an auto variable,
   *         be sure to understand that invoking any further builder operation on
   *         TreeExplorer will invalidate that variable (by moving it into the
   *         augmented iterator returned from such builder call).
   */
  template<class SRC>
  inline auto
  chainSearch (SRC&& srcData)
  {
    return IterChainSearch<SRC>{forward<SRC> (srcData)};
  }
  
  
  
}} // namespace lib::iter
#endif /*SRC_LIB_ITER_CHAIN_SEARCH_H*/
