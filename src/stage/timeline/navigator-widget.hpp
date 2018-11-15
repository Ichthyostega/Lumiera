/*
  NAVIGATOR-WIDGET.hpp  -  display of navigation controls in the timeline UI

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


/** @file navigator-widget.hpp
 ** Widget for navigation controls within the timeline UI.
 ** Since the timeline is effectively some kind of table grid, with a track header pane
 ** on the right and the timeline body display to the left, and since the latter has a
 ** [time ruler](\ref timeline::RulerWidget) running along it's top, there is a basically
 ** unused screen area in the first cell within the typical timeline layout of any media
 ** handling application. Lumiera uses this area to render a navigation control, since
 ** the player control (which also would be a natural choice of usage for this area) is
 ** already located in the timeline pane's general toolbar, or alternatively in the
 ** viewer pane. Since this area is always prominently visible, it is very attractive
 ** for global navigation controls
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_NAVIGATOR_WIDGET_H
#define GUI_TIMELINE_NAVIGATOR_WIDGET_H

#include "gui/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class NavigatorWidget
    : public Gtk::Grid
    {
    public:
      NavigatorWidget ();
     ~NavigatorWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_NAVIGATOR_WIDGET_H*/
