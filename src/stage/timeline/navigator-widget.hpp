/*
  NAVIGATOR-WIDGET.hpp  -  display of navigation controls in the timeline UI

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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


#ifndef STAGE_TIMELINE_NAVIGATOR_WIDGET_H
#define STAGE_TIMELINE_NAVIGATOR_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
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
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_NAVIGATOR_WIDGET_H*/
