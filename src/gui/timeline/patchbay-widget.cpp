/*
  PatchbayWidget  -  display of the patchbay to control track placement parameters

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file patchbay-widget.cpp
 ** Implementation details of the _patchbay_, allowing to tweak
 ** parameters of a fork's placement.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/timeline/patchbay-widget.hpp"

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


namespace gui {
namespace timeline {
  
  
  
  
  
  PatchbayWidget::~PatchbayWidget() { }
  
  
  PatchbayWidget::PatchbayWidget ()
    : Gtk::Grid{}
    { }
  
  
  /**
   * The Lumiera Timeline model does not rely on a list of tracks, as most conventional
   * video editing software does -- rather, each sequence holds a _fork of nested scopes._
   * This recursively nested structure is reflected in the patchbay area corresponding to
   * each track in the _header pane_ of the timeline display, located to the left. The
   * patchbay for each track is a grid with four quadrants, and the 4th quadrant is the
   * _content area,_ which is recursively extended to hold nested PatchbayWidget elements,
   * corresponding to the child tracks of this track. To _fund_ this recursively extensible
   * structure, we need to set up the first four quadrants
   */
  void
  PatchbayWidget::installFork (TrackHeadWidget& rootTrackHead)
  {
    UNIMPLEMENTED ("how actually to represent the track in the patchbay");
  }
  
  
  
}}// namespace gui::timeline
