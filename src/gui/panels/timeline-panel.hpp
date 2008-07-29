/*
  timeline-panel.hpp  -  Definition of the timeline panel            
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/
/** @file timeline-panel.hpp
 ** This file contains the definition of the timeline panel
 */

#ifndef TIMELINE_PANEL_HPP
#define TIMELINE_PANEL_HPP

#include "panel.hpp"
#include "../widgets/timeline-widget.hpp"

using namespace lumiera::gui::widgets;

namespace lumiera {
namespace gui {
namespace panels {

class TimelinePanel : public Panel
{
public:
  TimelinePanel();

private:
  //----- Event Handlers -----//
  void on_arrow_tool();
  void on_ibeam_tool();
  
  void on_zoom_in();
  void on_zoom_out();
  
private:
  void update_tool_buttons();
  void update_zoom_buttons();

private:

  //----- Data -----//

  // Widgets
  Gtk::Toolbar toolbar;
  TimelineWidget timelineWidget;
  
  // Toolbar Widgets
  Gtk::ToggleToolButton arrowTool;
  Gtk::ToggleToolButton iBeamTool;
  
  Gtk::ToolButton zoomIn;
  Gtk::ToolButton zoomOut;
  
  //----- Constants -----//
  static const int ZoomToolSteps;
};

}   // namespace panels
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_PANEL_H
