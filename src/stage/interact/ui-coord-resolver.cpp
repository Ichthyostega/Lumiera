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
 ** Algorithm to match a UI-Coord spec, possibly with wildcards, against the current actual tree of UI widgets.
 ** 
 */


#include "stage/interact/ui-coord-resolver.hpp"
#include "lib/util.hpp"

using util::isnil;
using lib::Symbol;
using lib::treeExplore;


namespace stage {
namespace interact {
  
  // emit typeinfo and VTables here....
  TreeStructureNavigator::~TreeStructureNavigator() { }
  
  LocationQuery::~LocationQuery() { }
  
  /** storage for the global LocationQuery service access point */
  lib::Depend<LocationQuery> LocationQuery::service;
  
  
  
  
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
        
        UICoord const&
        retrieveResult()
          {
            PathArray::truncateTo(currDepth_);
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
    
    
    size_t
    find_wildcardFree_suffix (UICoord const& uic)
    {
      size_t pos = uic.size();
      for ( ; 0 < pos; --pos)
        {
          Literal const& elm = uic[pos-1];
          if (elm == Symbol::ANY or elm == Symbol::EMPTY)
            break;
        }
      return pos;
    }


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
    // Helper to detect a wildcard match
    auto wildMatch = [&](Literal patt, Literal curr, size_t depth)
                        {
                          return patt == Symbol::ANY
                              or patt == Symbol::EMPTY
                              or patt == UIC_ELIDED // "existentially quantified"
                              or (isAnchored() and curr == res_.anchor and depth == UIC_WINDOW);
                        };    // transitive argument: assuming res_.anchor was computed for
                             //  the same coordinate pattern used here for patch resolution
    // algorithm state
    size_t maxDepth = 0;
    PathManipulator coverage;
    const size_t coordDepth = this->uic_.size();
    const size_t minSolutionDepth = find_wildcardFree_suffix (uic_);
    
    auto searchAlgo = query_.getChildren (uic_, 0)
                            .expandOnIteration()
                            .filter ([&](auto& iter)
                                        {
                                           size_t depth = iter.depth();     // we are at that depth in target tree
                                           if (depth >= coordDepth)         // search pattern exhausted without match...
                                             return false;
                                           Literal patt = uic_[depth];      // pick search pattern component at that depth
                                           Literal curr = *iter;            // iterator points at current tree position (ID)
                                           if (patt == curr or              // if either direct match
                                               wildMatch(patt,curr,depth))  // or wildcard match
                                             {
                                               coverage.setAt (depth,curr); // record match rsp. interpolate wildcard into output
                                               iter.expandChildren();       // next iteration will match one level down into the tree
                                             }
                                           return patt == curr              // direct match counts as (partial) solution
                                               or patt == UIC_ELIDED;       // existentially quantified elements also accepted
                                        })
                            .filter ([&](auto& iter)
                                        {
                                           if (iter.depth() < minSolutionDepth)
                                             return false;                  // filter solutions which did not bind all wildcards
                                           if (iter.depth()+1 <= maxDepth)  // filter for maximum solution length
                                             return false;
                                           maxDepth = 1 + iter.depth();
                                           return true;
                                        })
                            .transform ([&](auto&) -> UICoord const&
                                        {
                                           return coverage.retrieveResult();
                                        });
     // is (partial) coverage possible?
    //  search computes definitive answer!
    res_.isResolved = true;
    
    // perform the matching
    if (isnil (searchAlgo))
      return false;     // no solution found
    
    while (searchAlgo)  // pull first maximal solution
      {
        if (not res_.covfefe)
          res_.covfefe.reset (new UICoord {*searchAlgo});
        else
          *res_.covfefe = *searchAlgo;
        ++searchAlgo;
      }
    
    ENSURE (res_.covfefe and res_.covfefe->size() >= 1);
    res_.anchor = res_.covfefe->getWindow();
    
    // but depth reflects only that part coverable without wildcards
    if (res_.depth == 0)
      res_.depth = query_.determineCoverage(uic_);
    if (res_.depth == 0 and res_.anchor)
      res_.depth = 1;
    
    // signal success only when total coverage is possible
    return res_.covfefe->size() == uic_.size();
  }
  
  
  
}}// namespace stage::interact
