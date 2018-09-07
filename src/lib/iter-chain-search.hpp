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


/** @file iter-cursor.hpp
 ** An iterator with the ability to switch direction.
 ** This wrapper relies on the ability of typical STL container iterators
 ** to work in both directions, similar to std::reverse_iterator.
 ** Yet it is a single, self-contained element and in compliance to the
 ** ["Lumiera Forward Iterator"](iter-adapter.hpp) concept. But it has
 ** the additional ability to [switch the working direction](\ref IterCursor<IT>::switchDir).
 ** 
 ** @see IterCursor_test
 ** @see iter-adapter.hpp
 ** @see [usage example](event-log.hpp)
 ** 
 */


#ifndef SRC_LIB_ITER_CHAIN_SEARCH_H
#define SRC_LIB_ITER_CHAIN_SEARCH_H


#include "lib/error.hpp"
#include "lib/iter-tree-explorer.hpp"

//#include <type_traits>
//#include <utility>
#include <utility>
#include <vector>
#include <string>


namespace lib {
namespace iter {
  
  using std::move;
  using std::forward;
  using std::string;
  
  
  namespace { // type construction helpers...
    
    template<class SRC>
    auto
    buildSearchFilter (SRC&& dataSource)
    {
      return treeExplore (forward<SRC> (dataSource))
                .mutableFilter();
    }
    
    template<class SRC, class FUN>
    auto
    buildExplorer (SRC&& dataSource, FUN&& expandFunctor)
    {
      return buildSearchFilter (forward<SRC> (dataSource))
                .expand (forward<FUN> (expandFunctor))
                .expandAll();
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
        using Filter = decltype( buildSearchFilter (std::declval<SRC>()) );
        
        using StepFunctor = typename iter_explorer::_BoundFunctor<Filter(Filter const&), Filter>::Functor;
        
        using Pipeline = decltype( buildExplorer (std::declval<SRC>(), std::declval<StepFunctor>()) );
      };
    
  }//(End)type construction helpers
  
  
  
  
  
  /**
   * Iterator based linear search mechanism, with the ability to perform consecutive search with backtracking.
   * The IterChainSearch can be configured with a sequence of search goals (filter conditions), and will apply
   * these in succession on the underlying iterator. It will search _by linear search_ for the first hit of the
   * first condition, and then continue to search _from there_ matching on the second condition, and so on.
   * After the first combination of matches is exhausted, the search will backtrack and try to evaluate
   * the next combination, leading to a tree of search solutions.
   */
  template<class SRC>
  class IterChainSearch
    : public _IterChainSetup<SRC>::Pipeline
    {
      using _Trait = _IterChainSetup<SRC>;
      using _Base  = typename _Trait::Pipeline;
      
      using Filter = typename _Trait::Filter;
      using Step   = typename _Trait::StepFunctor;
      
      std::vector<Step> stepChain_;
      
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
        : _Base{move (buildExplorer (forward<SEQ> (srcData)
                                    ,Step{[this](Filter const& curr){ return configureFilterChain(curr); }}))}
        { }
      
      // inherited default ctor and standard copy operations
      using _Base::_Base;
      
      
      
      /** */
      IterChainSearch&&
      search (string target)
        {
          TODO ("configure additional chained search condition");
          return move(*this);
        }
      
      IterChainSearch&&
      clearFilter()
        {
          UNIMPLEMENTED ("drop all search condition frames");
          return move(*this);
        }
      
    private:
      Filter
      configureFilterChain (Filter const& currentFilterState)
        {
          uint depth = this->depth();
          return Filter{}; /////TODO empty filter means recursion end
        }
    };
  
  
  
  
  /* ==== convenient builder free function ==== */
  
  /** setup a chain search configuration by suitably wrapping the given container.
   * @return a TreeEplorer, which is an Iterator to yield all the source elements,
   *         but may also be used to build an processing pipeline.
   * @warning if you capture the result of this call by an auto variable,
   *         be sure to understand that invoking any further builder operation on
   *         TreeExplorer will invalidate that variable (by moving it into the
   *         augmented iterator returned from such builder call).
   * @param srcData either a »Lumiera Forward Iterator«, a _reference_ to a STL
   *         container, or a [»State Core«](\ref lib::IterStateWrapper) object.
   */
  template<class SRC>
  inline auto
  chainSearch (SRC&& srcData)
  {
    return IterChainSearch<SRC>{forward<SRC> (srcData)};
  }
  
  
  
}} // namespace lib::iter
#endif /*SRC_LIB_ITER_CHAIN_SEARCH_H*/
