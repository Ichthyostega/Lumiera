/*
  GEN-NODE-LOCATION-QUERY.hpp  -  pose UI-coordinate location queries against a GenNode structure

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


/** @file gen-node-location-query.hpp
 ** Implementation of the gui::interact::LocationQuery interface to work on a GenNode tree.
 ** The interface allows to pose queries against a concrete structure to verify and reshape some
 ** [UI Coordinate specification](\ref UICoord); basically it offers methods to navigate within a
 ** tree-like structure. While in the actual implementation, such a query interface would be backed
 ** by navigating real UI structures, the implementation given here instead uses a generic tree structure
 ** given as `Record<GenNode>`.
 ** 
 ** @todo WIP 10/2017 started in the effort of shaping the LoactionQuery interface, and used
 **       to support writing unit tests, to verify the UiCoordResolver. It remains to be seen
 **       if this implementation can be used beyond this limited purpose
 ** 
 ** @see UICoordResolver_test
 ** @see ui-coord-resolver.hpp
 ** @see navigator.hpp
 */


#ifndef GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H
#define GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H

#include "lib/error.hpp"
//#include "lib/symbol.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <utility>
//#include <memory>


namespace gui {
namespace interact {
  
  namespace error = lumiera::error;
  
//  using std::unique_ptr;
//  using std::string;
//  using lib::Literal;
//  using lib::Symbol;
  using lib::diff::Rec;
//  using util::unConst;
//  using util::isnil;
//  using util::min;
  

  
  
  /**
   * Query and mutate UICoord specifications in relation to actual UI topology.
   * 
   * @todo initial draft as of 10/2017
   */
  class GenNodeLocationQuery
    : public LocationQuery
    {
      
    public:
      GenNodeLocationQuery(Rec const& backingStructure)
      { }
      
      
      /* === LocationQuery interface === */
      
      /** */
      
    private:
    };
  
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_GEN_NODE_LOCATION_QUERY_H*/
