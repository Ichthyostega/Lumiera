/*
  DisplayManager  -  abstraction to translate model structure and extension into screen layout

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file display-manager.cpp
 ** Implementation details regarding timeline layout management and widget coordination.
 ** 
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** @todo 3/2020 do we actually need this global translation unit?
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
