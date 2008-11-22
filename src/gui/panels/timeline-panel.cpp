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

#include <boost/foreach.hpp>

#include "../gtk-lumiera.hpp"
#include "timeline-panel.hpp"

#include "../model/project.hpp"

extern "C" {
#include "../../lib/time.h"
}

using namespace Gtk;
using namespace sigc;
using namespace gui::widgets;
using namespace gui::model;

namespace gui {
namespace panels {
  
const int TimelinePanel::ZoomToolSteps = 2; // 2 seems comfortable

TimelinePanel::TimelinePanel(model::Project *const owner_project) :
  Panel(owner_project, "timeline", _("Timeline"), "panel_timeline"),
  previousButton(Stock::MEDIA_PREVIOUS),
  rewindButton(Stock::MEDIA_REWIND),
  playPauseButton(Stock::MEDIA_PLAY),
  stopButton(Stock::MEDIA_STOP),
  forwardButton(Stock::MEDIA_FORWARD),
  nextButton(Stock::MEDIA_NEXT),
  arrowTool(Gtk::StockID("tool_arrow")),
  iBeamTool(Gtk::StockID("tool_i_beam")),
  zoomIn(Stock::ZOOM_IN),
  zoomOut(Stock::ZOOM_OUT),
  timeIndicator(),
  updatingToolbar(false),
  currentTool(timeline::IBeam)
{
  // Setup the widget
  //timelineWidget.mouse_hover_signal().connect(
  //  mem_fun(this, &TimelinePanel::on_mouse_hover));
  //timelineWidget.playback_period_drag_released_signal().connect(
  //  mem_fun(this, &TimelinePanel::on_playback_period_drag_released));
  notebook.signal_switch_page().connect(
    mem_fun(this, &TimelinePanel::on_page_switched));

  // Setup the toolbar
  timeIndicatorButton.set_label_widget(timeIndicator);
  
  toolbar.append(timeIndicatorButton);
  
  toolbar.append(previousButton);
  toolbar.append(rewindButton);
  toolbar.append(playPauseButton,
    mem_fun(this, &TimelinePanel::on_play_pause));
  toolbar.append(stopButton,
    mem_fun(this, &TimelinePanel::on_stop));
  toolbar.append(forwardButton);
  toolbar.append(nextButton);
  
  toolbar.append(seperator1);
  
  toolbar.append(arrowTool,
    mem_fun(this, &TimelinePanel::on_arrow_tool));
  toolbar.append(iBeamTool,
    mem_fun(this, &TimelinePanel::on_ibeam_tool));
    
  toolbar.append(seperator2);
  
  toolbar.append(zoomIn, mem_fun(this, &TimelinePanel::on_zoom_in));
  toolbar.append(zoomOut, mem_fun(this, &TimelinePanel::on_zoom_out));
  
  toolbar.set_toolbar_style(TOOLBAR_ICONS);
  
  // Add the toolbar
  pack_start(toolbar, PACK_SHRINK);
  pack_start(notebook, PACK_EXPAND_WIDGET);
  
  // Set the initial UI state
  update_notebook();
  update_tool_buttons();
  update_zoom_buttons();
  show_time(0);
}

TimelinePanel::~TimelinePanel()
{
  // Free allocated widgets
  BOOST_FOREACH( TimelineWidget* widget, notebook_pages )
    delete widget;
}

void
TimelinePanel::on_play_pause()
{ 
  // TEST CODE! 
  if(!is_playing())
    {
      play();
    }
  else
    {
      frameEvent.disconnect();
    }
  
  update_playback_buttons();
}

void
TimelinePanel::on_stop()
{
  // TEST CODE! 
  /*timelineWidget.set_playback_point(GAVL_TIME_UNDEFINED);
  frameEvent.disconnect();
  show_time(timelineWidget.get_playback_period_start());
  
  update_playback_buttons();*/
}

void
TimelinePanel::on_arrow_tool()
{
  set_tool(timeline::Arrow);
}

void
TimelinePanel::on_ibeam_tool()
{  
  set_tool(timeline::IBeam);
}

void
TimelinePanel::on_zoom_in()
{
  TimelineWidget *const  widget = get_current_page();
  ASSERT(widget != NULL);
  
  widget->get_view_window().zoom_view(ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::on_zoom_out()
{
  TimelineWidget *const  widget = get_current_page();
  ASSERT(widget != NULL);
  
  widget->get_view_window().zoom_view(-ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::on_page_switched(GtkNotebookPage*, guint)
{
  // The page has changed. Update the UI for this new page
  
  // Set the tool in the new page to be the same as the tool in the last
  // page
  set_tool(currentTool); 

  update_zoom_buttons();
}

void
TimelinePanel::on_mouse_hover(gavl_time_t time)
{

}

void
TimelinePanel::on_playback_period_drag_released()
{
  //----- TEST CODE - this needs to set the playback point via the
  // real backend
  
  TimelineWidget *const  widget = get_current_page();
  ASSERT(widget != NULL);
  
  widget->set_playback_point(widget->get_playback_period_start());
  //----- END TEST CODE
  
  play();
}

void
TimelinePanel::update_notebook()
{
  BOOST_FOREACH( Sequence* const sequence, project->get_sequences() )
    {
      TimelineWidget * const widget = new TimelineWidget();
      notebook_pages.push_back(widget);
      notebook.append_page(*widget, sequence->get_name());
      notebook.set_tab_reorderable(*widget);
    }
}

void
TimelinePanel::update_playback_buttons()
{
  playPauseButton.set_stock_id(is_playing() ?
    Stock::MEDIA_PAUSE : Stock::MEDIA_PLAY);  
}

void
TimelinePanel::update_tool_buttons()
{    
  if(!updatingToolbar)
  {
    updatingToolbar = true;
    arrowTool.set_active(currentTool == timeline::Arrow);
    iBeamTool.set_active(currentTool == timeline::IBeam);
    updatingToolbar = false;
  }
}

void
TimelinePanel::update_zoom_buttons()
{
  TimelineWidget *const widget = get_current_page();
  ASSERT(widget != NULL);
  
  zoomIn.set_sensitive(widget->get_view_window().get_time_scale()
    != 1);
  zoomOut.set_sensitive(widget->get_view_window().get_time_scale()
    != TimelineWidget::MaxScale);
}

void
TimelinePanel::play()
{
  /*if(timelineWidget.get_playback_point() == GAVL_TIME_UNDEFINED)
    timelineWidget.set_playback_point(
      timelineWidget.get_playback_period_start());
  frameEvent = Glib::signal_timeout().connect(
    sigc::mem_fun(this, &TimelinePanel::on_frame),
    1000 / 25);*/
}

bool
TimelinePanel::is_playing() const
{
  // TEST CODE! - this should be hooked up to the real playback control
  return frameEvent.connected();
}

void
TimelinePanel::set_tool(timeline::ToolType tool)
{
  if(updatingToolbar) return;
  
  TimelineWidget *const  widget = get_current_page();
  ASSERT(widget != NULL);
   
  currentTool = tool;
  widget->set_tool(tool);
  update_tool_buttons();
}

void
TimelinePanel::show_time(gavl_time_t time)
{
  timeIndicator.set_text(lumiera_tmpbuf_print_time(time));
}

TimelineWidget*
TimelinePanel::get_current_page()
{
  Widget* const widget = (*notebook.get_current()).get_child();
  REQUIRE(widget != NULL);
  return (TimelineWidget*)widget;
}

bool
TimelinePanel::on_frame()
{
  // TEST CODE!  
  /*const gavl_time_t point = timelineWidget.get_playback_point()
    + GAVL_TIME_SCALE / 25;
  if(point < timelineWidget.get_playback_period_end())
    {
      show_time(point);
      timelineWidget.set_playback_point(point);
      
      
    }
  else
    on_stop();*/
    
  return true;
}

}   // namespace panels
}   // namespace gui
