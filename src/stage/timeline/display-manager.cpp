/*
  DisplayManager  -  abstraction to translate model structure and extension into screen layout

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

* *****************************************************/


/** @file display-manager.cpp
 ** Implementation details regarding timeline layout management and widget coordination.
 ** 
 ** @todo WIP-WIP-WIP as of 11/2018
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/display-manager.hpp"

//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  DisplayManager::~DisplayManager() { }   // emit VTables here....
  
  
//DisplayManager::DisplayManager()
//  : paneSplitPosition_{topLevelContainer.property_position()}
//  , bodyCanvas_{}
//  , headerPane_{bodyCanvas_.get_vadjustment()}   // wire the header pane (Gtk::Viewport) to follow the body vertical scroll movement
//  {
//    topLevelContainer.add1 (headerPane_);
//    topLevelContainer.add2 (bodyCanvas_);
//  }
  
  
  /**
   * This function is
   */
  
  
  
  
}}// namespace stage::timeline
