/*
  TimelinePanel  -  Dockable panel to hold the main timeline view

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline-panel.cpp
 ** Implementation of the dockable panel to hold the main timeline display.
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo as of 10/2018 we start to build a new timeline widget, connected to the UI-Bus
 ** @see timeline-widget.hpp
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/panel/timeline-panel.hpp"
#include "stage/timeline/timeline-widget.hpp"
#include "stage/timeline/timeline-widget-empty.hpp"

using std::make_unique;


namespace stage {
namespace panel {
  
  using timeline::TimelinePage;
  using timeline::TimelineWidget;
  using timeline::TimelineWidgetEmpty;
  
  
  TimelinePanel::TimelinePanel (workspace::PanelManager& panelManager,
                                Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
    , tabs_{}
    , pages_{}
    {
      addTimeline (PageHandle{new TimelineWidgetEmpty{}});
      
      // show everything....
      this->add(tabs_);
      this->show_all();
    }
  
  const char*
  TimelinePanel::getTitle()
  {
    return _("Timeline");
  }
  
  const gchar*
  TimelinePanel::getStockID()
  {
    return "panel_timeline";
  }
  
  
  namespace {
    template<class P>
    inline bool
    isEmptyTimeline (P const& pages)
    {
      return 1 == pages.size()
         and dynamic_cast<TimelineWidgetEmpty*> (pages[0].get());
    }
  }
  
  void
  TimelinePanel::addTimeline (PageHandle&& pTimelineWidget)
  {
    if (isEmptyTimeline (pages_))
      {
        tabs_.remove_page();
        pages_.clear();
      }
    pages_.push_back (move (pTimelineWidget));
    TimelinePage& addedPage = *pages_.back();
    tabs_.append_page (addedPage, addedPage.getLabel());
  }
  
  
}}   // namespace stage::panel
