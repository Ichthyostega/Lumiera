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

//using util::cStr;
//using util::isnil;
using lib::Symbol;


namespace gui {
namespace interact {
  
  /** @internal working data for path resolution */
  struct UICoordResolver::ResolutionState
    {
      using ChildIter = LocationQuery::ChildIter;

      lib::IterStack<ChildIter> backlog;
      lib::IterQueue<Resolution> solutions;
    };
  
  namespace { //


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
    
  }
  
  
  
}}// namespace gui::interact
