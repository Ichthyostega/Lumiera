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


#include "stage/gtk-base.hpp"
#include "stage/timeline/patchbay-widget.hpp"
#include "stage/timeline/track-head-widget.hpp"
//#include "stage/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
using Gtk::Adjustment;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  
  PatchbayWidget::~PatchbayWidget() { }
  
  
  /**
   * Set up the patchbay area of the timeline UI.
   * The patchbay is a container to hold the actual placement control widgets
   * arranged alongside with each track, according to the nested track structure.
   * The header pane and thus especially the patchbay needs to follow as a slave
   * to the vertical scrolling adjustments of the Timeline display; if the users
   * scrolls up or down over the more or less expanded tree of tracks, the header
   * has to follow this scrolled display in sync. Thus the implementation of the
   * patchbay is based on a Gtk::Viewport, which is attached to the vertical
   * Gtk::Adjustment, as exposed by the Gtk::ScrolledWindow holding the
   * timeline body canvas on the right side of the Timeline UI.
   * @note the Patchbay is not scrollable in horizontal direction,
   *       thus we create a disabled Adjustment for this parameter.
   */
  PatchbayWidget::PatchbayWidget (PAdjustment const& vScroll)
    : Gtk::ScrolledWindow{Gtk::Adjustment::create (0,0,0,0,0,0), vScroll}
    {
      set_shadow_type (Gtk::SHADOW_NONE);
      set_policy (Gtk::POLICY_NEVER, Gtk::POLICY_EXTERNAL); // horizontal extension is fixed, using the track body vertical scrollbar
      property_expand() = true;                            //  dynamically grab additional space
    }
  
  
  void
  PatchbayWidget::installFork (TrackHeadWidget& rootTrackHead)
  {
    this->add (rootTrackHead);
  }
  
  void
  PatchbayWidget::disable()
  {
    this->remove();
  }
  
  
  
}}// namespace stage::timeline
