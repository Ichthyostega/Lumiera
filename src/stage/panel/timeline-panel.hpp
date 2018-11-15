/*
  TIMELINE-PANEL.hpp  -  Dockable panel to hold the main timeline view            

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


/** @file timeline-panel.hpp
 ** A dockable container to hold a notebook of timeline displays.
 ** 
 ** ## Management of Timeline display
 ** Within the Lumiera GTK UI, there is an UI top-level circle of managing entities to handle global
 ** concerns and actions. The ctrl::InteractionDirector incorporates the role of _model root_ and thus
 ** manages a collection of timelines. The timeline panel exposes this collection and allows to view and
 ** interact with one of the given timelines. The build-up of the timeline display and thus the initiative
 ** to expose a timeline comes from the interaction director -- but as usual with graphical user interfaces,
 ** any user interaction after this point is handled autonomously by the gui::timeline::TimelineWidget and
 ** gui::timeline::TimelineController
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo as of 10/2018 we start to build a new timeline widget, connected to the UI-Bus
 */


#ifndef GUI_PANEL_TIMELINE_PANEL_H
#define GUI_PANEL_TIMELINE_PANEL_H

#include "gui/panel/panel.hpp"
#include "gui/timeline/timeline-widget.hpp"

#include <gtkmm/notebook.h>
#include <memory>
#include <vector>



namespace gui  {
namespace panel{
  
  
  /**
   * Dockable panel to hold timeline widget(s).
   * @todo WIP 10/2018 the actual re-implementation of Lumiera's Timeline display
   */
  class TimelinePanel
    : public Panel
    {
      using PageHandle = std::unique_ptr<timeline::TimelinePage>;
      using Timelines = std::vector<PageHandle>;
      
      Gtk::Notebook tabs_;
      Timelines pages_;
      
    public:
      /**
       * @param panel_manager The owner panel manager widget.
       * @param dock_item The GdlDockItem that will host this panel.
       */
      TimelinePanel (workspace::PanelManager&, Gdl::DockItem&);
      
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      /** take ownership of the widget and place it into a new tab */
      void addTimeline (PageHandle &&);
      
    private:
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_TIMELINE_PANEL_H*/
