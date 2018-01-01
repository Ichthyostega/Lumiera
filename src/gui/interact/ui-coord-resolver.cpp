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
  
  
  
  
  namespace { //
    
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
            UNIMPLEMENTED ("truncate to currDepth_");
            PathArray::normalise();
            return *this;
          }
        
        void
        setAt (size_t depth, Literal newSpec)
          {
            UNIMPLEMENTED ("forcibly set content, ensure storage");
            currDepth_ = depth;
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
   * coverage, to produce a single solution.
   */
  bool
  UICoordResolver::pathResolution()
  {
    size_t maxDepth = 0;
    PathManipulator coverage;
    auto searchAlgo = query_.getChildren (this->uic_, 0)
                            .expandOnIteration()
                            .filter([&](auto& iter)
                                       {
                                          size_t depth = iter.depth();
                                          Literal elm = uic_[depth];
                                          coverage.setAt (depth,elm);
                                          iter.expandChildren();
                                          UNIMPLEMENTED ("bollocks");
                                          return true;
                                       })
                            .filter([&](auto& iter)
                                       {
                                          if (iter.depth() <= maxDepth)
                                            return false;
                                          maxDepth = iter.depth();
                                          return true;
                                       })
                            .transform([&](auto&)
                                       {
                                          return coverage.retrieveResult();
                                       });
    if (isnil (searchAlgo))
      return false;
    
    while (searchAlgo)
      if (not res_.covfefe)
        res_.covfefe.reset (new UICoord{*searchAlgo});
      else
        *res_.covfefe = *searchAlgo;
    
    return true;
  }
  
  
  
}}// namespace gui::interact
