/*
  TimelinePanel  -  Dockable panel to hold the main timeline view

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


/** @file timeline-panel.cpp
 ** Implementation of the dockable panel to hold the main timeline display.
 ** 
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo as of 10/2018 we start to build a new timeline widget, connected to the UI-Bus
 ** @see timeline-widget.hpp
 ** 
 */


#include "gui/gtk-base.hpp"
#include "gui/panel/timeline-panel.hpp"
#include "gui/timeline/timeline-widget.hpp"
#include "gui/timeline/timeline-widget-empty.hpp"

using std::make_unique;


namespace gui {
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
    bool
    isEmptyTimeline (auto& pages)
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
  
  
}}   // namespace gui::panel
