/*
  HEADER-PANE-WIDGET.hpp  -  display pane for track headers within the timeline

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


/** @file header-pane-widget.hpp
 ** Widget to show an always visible track header area within the timeline UI.
 ** [The fork](\ref Fork), a recursively nested system of scopes, is rendered within
 ** the timeline display as a system of nested tracks. Each of these tracks possibly holds some
 ** child tracks plus some actual media clips, which all inherit parameters of placement from
 ** this fork ("track"). These parameters address various aspects of how content is attached
 ** ("placed") into the model at large. So, for each track, we create a
 ** ["patchbay" area](\ref PatchbayWidget) to manage those placement parameters.
 ** 
 ** The HeaderPaneWidget aggregates those patchbay elements into a nested, collapsable tree
 ** structure in accordance with the nesting of scopes. For the actual layout, it has to collaborate
 ** with the timeline::LayoutManager to work out the space available for each individual track head
 ** and to keep these parts aligned with the tracks in the [timeline body](\ref BodyCanvasWidget).
 ** 
 ** The header area is shown at the left side of the timeline display, always visible.
 ** At the top of the header area, corresponding to the time ruler within the body at the right,
 ** we place a compact navigation control, including also the timecode display. The space below
 ** is kept in sync with the respective track entries of the timeline, and is itself built as
 ** a nested structure of PatchbayWidget entries.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef STAGE_TIMELINE_HEADER_PANE_WIDGET_H
#define STAGE_TIMELINE_HEADER_PANE_WIDGET_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/navigator-widget.hpp"
#include "stage/timeline/patchbay-widget.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using PAdjustment = Glib::RefPtr<Gtk::Adjustment>;
  
  
  class TrackHeadWidget;
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class HeaderPaneWidget
    : public Gtk::Box
    {
      NavigatorWidget navigator_;
      PatchbayWidget patchbay_;
      
    public:
     ~HeaderPaneWidget() { }
      
      HeaderPaneWidget (PAdjustment const& vScroll)
        : Gtk::Box{Gtk::ORIENTATION_VERTICAL}
        , navigator_{}
        , patchbay_{vScroll}
        {
          this->pack_start (navigator_, Gtk::PACK_SHRINK);
          this->pack_start (patchbay_, Gtk::PACK_EXPAND_WIDGET);
        }
      
      /**
       * Initially install the root node of the track fork,
       * which later can be extended recursively by adding nested
       * sub-forks ("Sub-Tracks").
       */
      void
      installForkRoot (TrackHeadWidget& rootTrackHead)
        {
          patchbay_.installFork (rootTrackHead);
        }
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_HEADER_PANE_WIDGET_H*/
