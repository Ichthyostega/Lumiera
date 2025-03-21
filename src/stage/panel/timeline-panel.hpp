/*
  TIMELINE-PANEL.hpp  -  Dockable panel to hold the main timeline view            

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file timeline-panel.hpp
 ** A dockable container to hold a notebook of timeline displays.
 ** 
 ** # Management of Timeline display
 ** Within the Lumiera GTK UI, there is an UI top-level circle of managing entities to handle global
 ** concerns and actions. The ctrl::InteractionDirector incorporates the role of _model root_ and thus
 ** manages a collection of timelines. The timeline panel exposes this collection and allows to view and
 ** interact with one of the given timelines. The build-up of the timeline display and thus the initiative
 ** to expose a timeline comes from the interaction director -- but as usual with graphical user interfaces,
 ** any user interaction after this point is handled autonomously by the stage::timeline::TimelineWidget and
 ** stage::timeline::TimelineController
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo as of 10/2018 we start to build a new timeline widget, connected to the UI-Bus
 */


#ifndef STAGE_PANEL_TIMELINE_PANEL_H
#define STAGE_PANEL_TIMELINE_PANEL_H

#include "stage/panel/panel.hpp"
#include "stage/timeline/timeline-widget.hpp"

#include <gtkmm/notebook.h>
#include <memory>
#include <vector>



namespace stage  {
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
  
  
}}// namespace stage::panel
#endif /*STAGE_PANEL_TIMELINE_PANEL_H*/
