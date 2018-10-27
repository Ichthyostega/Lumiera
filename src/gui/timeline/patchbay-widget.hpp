/*
  PATCHBAY-WIDGET.hpp  -  display of the patchbay to control track placement parameters

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

*/


/** @file patchbay-widget.hpp
 ** Widget to control the placement parameters of a fork within the timeline header pane UI.
 ** [The fork](\ref session::Fork), a recursively nested system of scopes, is rendered within
 ** the timeline display as a system of nested tracks. Each of these tracks possibly holds some
 ** child tracks plus some actual media clips, which all inherit parameters of placement from
 ** this fork ("track"). These parameters address various aspects of how content is attached
 ** ("placed") into the model at large, examples being
 ** - how to route the output
 ** - how to "place" this content into an _output space_, like e.g.
 **   + sound panning
 **   + video overlay parameters (additive, opaque, transparent)
 **   + video or audio _level_ (=fader)
 ** - how to locate this content in time (e.g. relative to some marker)
 ** For each track, we show a patchbay in the timeline header pane, which serves to control
 ** such aspects relevant for all content contained within the scope of this track. 
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_PATCHBAY_WIDGET_H
#define GUI_TIMELINE_PATCHBAY_WIDGET_H

#include "gui/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * Header pane control area corresponding to a Track with nested child Tracks.
   * This structure is used recursively to build up the Fork of nested Tracks.
   * - first row: Placement + Property pane
   * - second row: content or nested tracks.
   * @todo WIP-WIP as of 12/2016
   */
  class PatchbayWidget
    : public Gtk::Grid
    {
    public:
      PatchbayWidget ();
     ~PatchbayWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_PATCHBAY_WIDGET_H*/
