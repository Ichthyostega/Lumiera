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
 ** @todo as of 10/2016 this is WIP-WIP-WIP : canvas widgets experiment
 ** @todo build a new timeline widget, connected to the UI-Bus
 */


#ifndef GUI_PANEL_TIMELINE_PANEL_H
#define GUI_PANEL_TIMELINE_PANEL_H

#include "gui/panel/panel.hpp"

//#include <memory>



namespace gui  {
namespace model{
    class Sequence;
  }
  namespace panel {
  
//using std::shared_ptr;
  
  
  
  /**
   * Dockable panel to hold timeline widget(s).
   */
  class TimelinePanel
    : public Panel
    {
    public:
      /**
       * @param panel_manager The owner panel manager widget.
       * @param dock_item The GdlDockItem that will host this panel.
       */
      TimelinePanel(workspace::PanelManager&, Gdl::DockItem&);
      
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      
    protected:
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_TIMELINE_PANEL_H*/
