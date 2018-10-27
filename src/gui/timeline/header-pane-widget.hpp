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
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_HEADER_PANE_WIDGET_H
#define GUI_TIMELINE_HEADER_PANE_WIDGET_H

#include "gui/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class HeaderPaneWidget
    : public Gtk::Box
    {
    public:
      HeaderPaneWidget();
     ~HeaderPaneWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_HEADER_PANE_WIDGET_H*/
