/*
  NavigatorWidget  -  display of navigation controls in the timeline UI

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file navigator-widget.cpp
 ** Implementation detail of the navigation control elements presented
 ** at top of the timeline UI's header pane.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/style-scheme.hpp"
#include "stage/timeline/navigator-widget.hpp"

//#include "stage/ui-bus.hpp"
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
  
  
  
  
  
  
  NavigatorWidget::~NavigatorWidget() { }
  
  
  NavigatorWidget::NavigatorWidget ()
    : Gtk::Grid{}
    {
      get_style_context()->add_class (CLASS_timeline_navi);
    }
  
  
  
  
}}// namespace stage::timeline
