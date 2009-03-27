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

#include "../workspace/workspace-window.hpp"
#include "../model/project.hpp"
#include "../controller/controller.hpp"

extern "C" {
#include "../../lib/time.h"
}

using namespace Gtk;
using namespace sigc;
using namespace std;
using namespace boost;
using namespace util;
using namespace gui::widgets;
using namespace gui::model;

namespace gui {
namespace panels {
  
const int TimelinePanel::ZoomToolSteps = 2; // 2 seems comfortable

TimelinePanel::TimelinePanel(workspace::WorkspaceWindow
    &workspace_window) :
  Panel(workspace_window, "timeline", _("Timeline"), "panel_timeline"),
  timeIndicator(),
  timeIndicatorButton(),
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
  updatingToolbar(false),
  currentTool(timeline::IBeam)
{
  // Hook up notifications
  workspace.get_project().get_sequences().signal_changed().connect(
    mem_fun(this, &TimelinePanel::on_sequence_list_changed));
  
  // Setup the sequence chooser
  sequenceChooserModel = Gtk::ListStore::create(sequenceChooserColumns);
  ENSURE(sequenceChooserModel);
  
  sequenceChooser.set_model(sequenceChooserModel);
  sequenceChooser.pack_start(sequenceChooserColumns.nameColumn);
  sequenceChooser.show_all();
  
  sequenceChooserChangedConnection = sequenceChooser.signal_changed().
    connect( sigc::mem_fun(*this, &TimelinePanel::on_sequence_chosen) );
  
  panelBar.pack_start(sequenceChooser, PACK_SHRINK);

  // Setup the toolbar
  timeIndicatorButton.add(timeIndicator);
  timeIndicatorButton.set_relief(Gtk::RELIEF_NONE);
  timeIndicatorButton.set_focus_on_click(false);
  
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
   
  toolbar.show_all();
  panelBar.pack_start(toolbar, PACK_SHRINK);
   
  // Setup the timeline widget
  shared_ptr<Sequence> sequence
    = *workspace.get_project().get_sequences().begin();  
  timelineWidget.reset(new TimelineWidget(load_state(sequence)));
  pack_start(*timelineWidget, PACK_EXPAND_WIDGET);
  
  // Set the initial UI state
  update_sequence_chooser();
  update_tool_buttons();
  update_zoom_buttons();
  show_time(0);
}

TimelinePanel::~TimelinePanel()
{

}

void
TimelinePanel::on_play_pause()
{
  if(!is_playing())
    {
      play();
    }
  else
    {
      pause();
    }
  
  update_playback_buttons();
}

void
TimelinePanel::on_stop()
{
  workspace.get_controller().get_playback_controller().stop();  
  update_playback_buttons();
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
  REQUIRE(timelineWidget);
  timelineWidget->zoom_view(ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::on_zoom_out()
{
  REQUIRE(timelineWidget);
  timelineWidget->zoom_view(-ZoomToolSteps);
  update_zoom_buttons();
}

void
TimelinePanel::on_mouse_hover(gavl_time_t time)
{
  (void)time;
}

void
TimelinePanel::on_playback_period_drag_released()
{
  //----- TEST CODE - this needs to set the playback point via the
  // real backend
  
  REQUIRE(timelineWidget);
    
  timelineWidget->get_state()->set_playback_point(
    timelineWidget->get_state()->get_playback_period_start());
  //----- END TEST CODE
  
  play();
}

void
TimelinePanel::on_sequence_list_changed()
{
  update_sequence_chooser();
}

void
TimelinePanel::on_sequence_chosen()
{
  REQUIRE(timelineWidget);
  
  Gtk::TreeIter iter = sequenceChooser.get_active();
  if(iter)
    {
      weak_ptr<Sequence> sequence_ptr = 
        (*iter)[sequenceChooserColumns.sequenceColumn];
      shared_ptr<Sequence> sequence(sequence_ptr.lock());
      if(sequence)
        {
          shared_ptr<timeline::TimelineState> old_state(
            timelineWidget->get_state());
          REQUIRE(old_state);
            
          if(sequence != old_state->get_sequence())
            timelineWidget->set_state(load_state(sequence));
        }
    }
    
  update_zoom_buttons();
}

void
TimelinePanel::update_sequence_chooser()
{
  REQUIRE(sequenceChooserModel);

  // Block the event handler
  sequenceChooserChangedConnection.block();

  // Repopulate the sequence chooser
  sequenceChooserModel->clear();
  
  shared_ptr<timeline::TimelineState> state =
    timelineWidget->get_state();
  REQUIRE(state);    
  
  BOOST_FOREACH( shared_ptr< model::Sequence > sequence,
    workspace.get_project().get_sequences() )
    {
      Gtk::TreeIter iter = sequenceChooserModel->append();
      Gtk::TreeModel::Row row = *iter;
      row[sequenceChooserColumns.sequenceColumn] = sequence;
      row[sequenceChooserColumns.nameColumn] = sequence->get_name();
      
      if(state->get_sequence() == sequence)
        sequenceChooser.set_active(iter);
    }
    
  // Unblock the event handler
  sequenceChooserChangedConnection.unblock();
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
  REQUIRE(timelineWidget);

  timeline::TimelineViewWindow &viewWindow = 
    timelineWidget->get_state()->get_view_window();
  
  zoomIn.set_sensitive(viewWindow.get_time_scale() != 1);
  zoomOut.set_sensitive(viewWindow.get_time_scale()
    != TimelineWidget::MaxScale);
}

void
TimelinePanel::play()
{   
  workspace.get_controller().get_playback_controller().play();
}

void
TimelinePanel::pause()
{
  workspace.get_controller().get_playback_controller().pause();
}

bool
TimelinePanel::is_playing() const
{
  return workspace.get_controller().get_playback_controller().
    is_playing();
}

void
TimelinePanel::set_tool(timeline::ToolType tool)
{
  REQUIRE(timelineWidget);
  
  if(updatingToolbar) return;
  
  currentTool = tool;
  timelineWidget->set_tool(tool);
  update_tool_buttons();
}

void
TimelinePanel::show_time(gavl_time_t time)
{
  timeIndicator.set_text(lumiera_tmpbuf_print_time(time));
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

shared_ptr<timeline::TimelineState>
TimelinePanel::load_state(weak_ptr<Sequence> sequence)
{
  if(contains(timelineStates, sequence))
    return timelineStates[sequence];
  
  shared_ptr<Sequence> shared_sequence = sequence.lock();
  if(shared_sequence)
    return shared_ptr< timeline::TimelineState >(
      new timeline::TimelineState(shared_sequence));
  return shared_ptr< timeline::TimelineState >();
}

}   // namespace panels
}   // namespace gui
