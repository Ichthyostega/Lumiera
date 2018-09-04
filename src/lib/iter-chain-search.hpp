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
//#include "lib/iter-adapter.hpp"

//#include <type_traits>
//#include <utility>
#include <utility>
#include <string>


namespace lib {
  
  using std::move;
  using std::string;
  
  namespace iter {
    
    /**
     * @internal implementation for....
     */
    template<class IT>
    class Cur
      {
        
        
      public:
      };
    
    
  } // namespace iter
  
  
  
  
  /**
   * Iterator based linear search mechanism, with the ability to perform consecutive search with backtracking.
   * The IterChainSearch can be configured with a sequence of search goals (filter conditions), and will
   * apply these in succession on the underlying iterator. It will search for the first hit of the first
   * condition, and then continue to search _from there_ matching on the second condition, and so on.
   * After the first combination of matches is exhausted, the search will backtrack and try to evaluate
   * the next combination, leading to a tree of search solutions.
   */
//  template<class IT>
  class IterChainSearch
    {
//      using _Core = iter::CursorGear<IT>;
//      using _Parent = IterStateWrapper<typename _Core::value_type, _Core>;
      
    public:
//    IterChainSearch()                                   =default;
//    IterChainSearch (IterChainSearch&&)                 =default;
//    IterChainSearch (IterChainSearch const&)            =default;
//    IterChainSearch& operator= (IterChainSearch&&)      =default;
//    IterChainSearch& operator= (IterChainSearch const&) =default;
      
      
//    template<class CON>
//    explicit
//    IterChainSearch (CON& container)
////    : _Parent(_Core(container.begin(), container.end()))
//      { }
//    
//    IterChainSearch (IT&& begin, IT&& end)
////    : _Parent(_Core(std::forward<IT>(begin), std::forward<IT>(end)))
//      { }
      
      
      operator bool()  const
        {
          UNIMPLEMENTED ("solution test");
        }
      
      bool
      empty()  const
        {
          return not(*this);
        }
      
      
      /** */
      IterChainSearch&&
      search (string target)
        {
          UNIMPLEMENTED ("configure additional chained search condition");
          return move(*this);
        }
      
      IterChainSearch&&
      clearFilter()
        {
          UNIMPLEMENTED ("drop all search condition frames");
          return move(*this);
        }
      
      
      /////////////////////////TODO dummy placeholder code. Replace by real iterator access
      string&
      operator*()  const
        {
          static string boo{"bääääh"};
          return boo;
        }
      
      IterChainSearch&
      operator++()
        {
          return *this;
        }
      /////////////////////////TODO dummy placeholder code. Replace by real iterator access
    };
  
  
  
  
  /* ==== convenient builder free function ==== */
  
  /** setup a chain search configuration by suitably wrapping the given container.
   * @return a TreeEplorer, which is an Iterator to yield all the source elements,
   *         but may also be used to build an processing pipeline.
   * @warning if you capture the result of this call by an auto variable,
   *         be sure to understand that invoking any further builder operation on
   *         TreeExplorer will invalidate that variable (by moving it into the
   *         augmented iterator returned from such builder call).
   */
  template<class CON>
  inline auto
  chainSearch (CON&& srcData)
  {
//  using SrcIter = typename _DecoratorTraits<IT>::SrcIter;
//  using Base = iter_explorer::BaseAdapter<SrcIter>;
    
    UNIMPLEMENTED ("figure out the Iterator type and build a suitable IterChainSearch instance");
    return IterChainSearch();
//  return TreeExplorer<Base> (std::forward<IT> (srcSeq));
  }
  
  
  
} // namespace lib
#endif /*SRC_LIB_ITER_CHAIN_SEARCH_H*/
