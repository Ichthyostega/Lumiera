/*
  UI-COORD.hpp  -  generic topological location addressing scheme within the UI

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


/** @file ui-coord.hpp
 ** A topological addressing scheme to designate structural locations within the UI.
 ** Contrary to screen pixel coordinates, we aim at a topological description of the UI structure.
 ** This foundation allows us
 ** - to refer to some "place" or "space" within the interface
 ** - to remember and return to such a location
 ** - to move a work focus structurally within the UI
 ** - to describe and configure the pattern view access and arrangement
 ** 
 ** As starting point, we'll pick the notion of an access path within a hierarchical structure
 ** - the top-level window
 ** - the perspective used within that window
 ** - the panel within this window
 ** - a view group within the panel
 ** - plus a locally defined access path further down to the actual UI element
 ** 
 ** @todo WIP 9/2017 first draft ////////////////////////////////////////////////////////////////////////////TICKET #1106 generic UI coordinate system
 ** 
 ** @see UICoord_test
 ** @see id-scheme.hpp
 ** @see view-spec-dsl.hpp
 ** @see view-locator.hpp
 */


#ifndef GUI_INTERACT_UI_COORD_H
#define GUI_INTERACT_UI_COORD_H

#include "lib/error.hpp"
#include "lib/symbol.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  
  
  
  /**
   * Describe a location within the UI through structural/topological coordinates.
   * A UICoord specification is a tuple, elaborating a path through the hierarchy
   * of UI elements down to the specific element referred.
   * 
   * @todo initial draft as of 9/2017
   */
  class UICoord
    {
      
    public:
      
    private:
      
    };
  
  
}}// namespace gui::interact
#endif /*GUI_INTERACT_UI_COORD_H*/
