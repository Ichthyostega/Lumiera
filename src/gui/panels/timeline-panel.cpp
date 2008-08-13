/*
  timeline-panel.cpp  -  Implementation of the timeline panel
 
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
 
* *****************************************************/

#include "../gtk-lumiera.hpp"
#include "timeline-panel.hpp"

using namespace Gtk;
using namespace sigc;
using namespace lumiera::gui::widgets;

namespace lumiera {
namespace gui {
namespace panels {
  
const int TimelinePanel::ZoomToolSteps = 2; // 2 seems comfortable

TimelinePanel::TimelinePanel() :
  Panel("timeline", _("Timeline"), "panel_timeline"),
  arrowTool(Gtk::StockID("tool_arrow")),
  iBeamTool(Gtk::StockID("tool_i_beam")),
  zoomIn(Stock::ZOOM_IN),
  zoomOut(Stock::ZOOM_OUT),
  updatingToolbar(false)
{
  // Setup the toolbar
  toolbar.append(arrowTool, mem_fun(this,
    &TimelinePanel::on_arrow_tool));
  toolbar.append(iBeamTool, mem_fun(this,
    &TimelinePanel::on_ibeam_tool));
  toolbar.append(seperator1);
  toolbar.append(zoomIn, mem_fun(this, &TimelinePanel::on_zoom_in));
  toolbar.append(zoomOut, mem_fun(this, &TimelinePanel::on_zoom_out));
  
  toolbar.set_icon_size(IconSize(ICON_SIZE_LARGE_TOOLBAR));
  toolbar.set_toolbar_style(TOOLBAR_ICONS);
  
  // Add the toolbar
  pack_start(toolbar, PACK_SHRINK);
  pack_start(timelineWidget, PACK_EXPAND_WIDGET);
  
  update_tool_buttons();
  update_zoom_buttons();
}

void
TimelinePanel::on_arrow_tool()
{
  if(updatingToolbar) return;
  timelineWidget.set_tool(timeline::Arrow);
  update_tool_buttons();
}

void
TimelinePanel::on_ibeam_tool()
{
  if(updatingToolbar) return;
  timelineWidget.set_tool(timeline::IBeam);
  update_tool_buttons();
}

void
TimelinePanel::on_zoom_in()
{

  timelineWidget.zoom_view(ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::on_zoom_out()
{
  timelineWidget.zoom_view(-ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::update_tool_buttons()
{    
  if(!updatingToolbar)
  {
    updatingToolbar = true;
    const timeline::ToolType tool = timelineWidget.get_tool();
    arrowTool.set_active(tool == timeline::Arrow);
    iBeamTool.set_active(tool == timeline::IBeam);
    updatingToolbar = false;
  }
}

void
TimelinePanel::update_zoom_buttons()
{
  zoomIn.set_sensitive(timelineWidget.get_time_scale() != 1);
  zoomOut.set_sensitive(timelineWidget.get_time_scale() !=
    TimelineWidget::MaxScale);
}

}   // namespace panels
}   // namespace gui
}   // namespace lumiera
