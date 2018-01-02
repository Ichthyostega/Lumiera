/*
  UICoordResolver  -  resolve UI coordinate spec against actual window topology

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

* *****************************************************/


/** @file ui-coord-resolver.cpp
 ** Implementation details of resolving a UICoord path against the actual UI topology.
 ** 
 ** @todo WIP 10/2017 initial draft       ////////////////////////////////////////////////////////////TICKET #1107
 */


#include "gui/interact/ui-coord-resolver.hpp"
//#include "gui/ctrl/global-ctx.hpp"
//#include "lib/symbol.hpp"
//#include "lib/util.hpp"

#include <boost/noncopyable.hpp>

//using util::cStr;
//using util::isnil;
using lib::Symbol;
using lib::treeExplore;


namespace gui {
namespace interact {
  
  // emit typeinfo and VTables here....
  TreeStructureNavigator::~TreeStructureNavigator() { }
  
  LocationQuery::~LocationQuery() { }
  
  
  
  
  namespace { // Helpers for patch matching algorithm
    
    /**
     * Special UI-Coordinate builder
     * to define the solution path step by step
     * while we recurse down into the reference tree
     * @note when backtracking, an existing solution will
     *       be partially rewritten starting from lower depth
     */
    class PathManipulator
      : public UICoord
      {
        size_t currDepth_;
        
      public:
        PathManipulator ()
          : UICoord{}
          , currDepth_{0}
          { }
        // inherited copy operations
        
        UICoord const
        retrieveResult()
          {
            PathArray::truncateTo(currDepth_);
            PathArray::normalise();
            return *this;
          }
        
        void
        setAt (size_t depth, Literal newSpec)
          {
            Literal* storage = PathArray::expandPosition (depth);
            PathArray::setContent (storage, newSpec);
            currDepth_ = depth+1;
          }
      };


  }//(End) implementation details
  
  
  
  /**
   * Since UICoord path specifications may contain gaps and wildcards, we may attempt
   * to fill in these missing parts by matching against the topological structure of an actual UI.
   * In the general case, finding a solution requires a depth-first exponential brute-force search
   * over the whole structure tree, since we have to try every possible branch until we can disprove
   * the possibility of a match. Implemented as depth-first search with backtracking, this scanning
   * pass produces a list of possible matches, from which we pick the first one with maximum
   * coverage, to yield a single solution.
   * @remark the search and matching is based on an iterator pipeline builder, with the special ability
   *         to expand and recurse into the children of the current element on demand: when `expandChildren()`
   *         was invoked, the next iteration will continue with the first child element; there is a stack of
   *         such "child expansions" -- meaning that the search will backtrack and explore further possibilities
   *         later on. Each position where the pattern matches the actual tree is marked as possible solution.
   *         As a sideeffect, a new coordinate spec to reflect the actual coverage is built and re-written,
   *         while the algorithm proceeds. Thus, at any point marked as solution, the current (partial)
   *         solution can be retrieved and copied from this PathManipulator buffer.
   *         An additional filter layer discriminates the first maximal solutions seen thus far.
   */
  bool
  UICoordResolver::pathResolution()
  {
    size_t maxDepth = 0;
    PathManipulator coverage;
    size_t coordDepth = this->uic_.size();
    auto searchAlgo = query_.getChildren (uic_, 0)
                            .expandOnIteration()
                            .filter ([&](auto& iter)
                                        {
                                           size_t depth = iter.depth();     // we are at that depth in target tree
                                           if (depth >= coordDepth)         // search pattern exhausted
                                             return false;
                                           Literal elm = uic_[depth];       // pick search pattern component at that depth
                                           if (elm != *iter and             // if no direct match
                                               elm != Symbol::EMPTY)        // and not a wildcard in the pattern
                                             return false;                  // it's no solution; backtracking to next alternative
                                           
                                           coverage.setAt (depth, *iter);   // record match rsp. interpolate wildcard into output
                                           iter.expandChildren();           // next iteration will match one level down into the tree    
                                           return elm != Symbol::EMPTY;     // wildcard match itself does not count as solution
                                        })                                  // ...yet we'll continue matching with children
                            .filter ([&](auto& iter)
                                        {
                                           if (iter.depth()+1 <= maxDepth)  // filter for maximum solution length
                                             return false;
                                           maxDepth = 1 + iter.depth();
                                           return true;
                                        })
                            .transform ([&](auto&)
                                        {
                                           return coverage.retrieveResult();
                                        });
    // perform the matching
    if (isnil (searchAlgo))
      return false;     // no solution found
    
    while (searchAlgo)  // pull first maximal solution
      if (not res_.covfefe)
        res_.covfefe.reset (new UICoord{*searchAlgo});
      else
        *res_.covfefe = *searchAlgo;
    
    return true;
  }
  
  
  
}}// namespace gui::interact
