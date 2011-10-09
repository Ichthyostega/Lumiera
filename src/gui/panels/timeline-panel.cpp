/*
  timeline-panel.cpp  -  Implementation of the timeline panel

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


#include "gui/gtk-lumiera.hpp"
#include "gui/panels/timeline-panel.hpp"
#include "gui/widgets/timeline/timeline-zoom-scale.hpp"

#include "gui/workspace/workspace-window.hpp"
#include "gui/model/project.hpp"
#include "gui/controller/controller.hpp"
#include "lib/util.hpp"

#include <boost/foreach.hpp>

using namespace Gtk;
using namespace sigc;
using namespace gui::widgets;
using namespace gui::widgets::timeline;
using namespace gui::model;

using boost::shared_ptr;  ///////////////////////////////TICKET #796
using boost::weak_ptr;    ///////////////////////////////TICKET #796
using util::contains;

namespace gui {
namespace panels {
  
const int TimelinePanel::ZoomToolSteps = 2; // 2 seems comfortable


TimelinePanel::TimelinePanel (workspace::PanelManager &panel_manager,
                              GdlDockItem *dock_item) 
  : Panel(panel_manager, dock_item, get_title(), get_stock_id())
  , timeCode("sequence_clock", "timecode_widget", true)
  , previousButton(Stock::MEDIA_PREVIOUS)
  , rewindButton(Stock::MEDIA_REWIND)
  , playPauseButton(Stock::MEDIA_PLAY)
  , stopButton(Stock::MEDIA_STOP)
  , forwardButton(Stock::MEDIA_FORWARD)
  , nextButton(Stock::MEDIA_NEXT)
  , arrowTool(Gtk::StockID("tool_arrow"))
  , iBeamTool(Gtk::StockID("tool_i_beam"))
  , zoomIn(Stock::ZOOM_IN)
  , zoomOut(Stock::ZOOM_OUT)
  , zoomScale()
  , updatingToolbar(false)
  , currentTool(timeline::Arrow)
{
  // Hook up notifications
  get_project().get_sequences().signal_changed().connect(mem_fun(this,
      &TimelinePanel::on_sequence_list_changed));
  
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
  toolbar.append(timeCode);
  
  toolbar.append(previousButton);
  toolbar.append(rewindButton);
  toolbar.append(playPauseButton,
    mem_fun(this, &TimelinePanel::on_play_pause));
  toolbar.append(stopButton,
    mem_fun(this, &TimelinePanel::on_stop));
  toolbar.append(forwardButton);
  toolbar.append(nextButton);
  
  toolbar.append(separator1);
  
  toolbar.append(arrowTool,
    mem_fun(this, &TimelinePanel::on_arrow_tool));
  toolbar.append(iBeamTool,
    mem_fun(this, &TimelinePanel::on_ibeam_tool));
    
  toolbar.append(separator2);
  
  toolbar.append(zoomScale);
  zoomScale.signal_zoom().
    connect(mem_fun(this,&TimelinePanel::on_zoom));

  toolbar.show_all();
  panelBar.pack_start(toolbar, PACK_SHRINK);

  // Setup tooltips
  sequenceChooser     .set_tooltip_text(_("Change sequence"));

  previousButton  .set_tooltip_text(_("To beginning"));
  rewindButton    .set_tooltip_text(_("Rewind"));
  playPauseButton .set_tooltip_text(_("Start playback"));
  stopButton      .set_tooltip_text(_("Stop playback"));
  forwardButton   .set_tooltip_text(_("Forward"));
  nextButton      .set_tooltip_text(_("To end"));

  arrowTool       .set_tooltip_text(_("Selection tool"));
  iBeamTool       .set_tooltip_text(_("Marker tool"));

  zoomIn          .set_tooltip_text(_("Zoom in"));
  zoomOut         .set_tooltip_text(_("Zoom out"));
  zoomScale       .set_tooltip_text(_("Adjust timeline zoom scale"));

  // Setup the timeline widget
  shared_ptr<Sequence> sequence          ///////////////////////////////TICKET #796
    = *get_project().get_sequences().begin();  
  timelineWidget.reset(new TimelineWidget(load_state(sequence)));
  pack_start(*timelineWidget, PACK_EXPAND_WIDGET);

  // TimelineWidget is now initialized, lets set it in the zoomScale
  // and wire it with the timeline state changed signal
  zoomScale.set_view_window(timelineWidget->get_state()->get_view_window());
  zoomScale.wireTimelineState (timelineWidget->state_changed_signal());
  

  // Set the initial UI state
  update_sequence_chooser();
  update_tool_buttons();
  update_zoom_buttons();
  show_time (Time::ZERO);
}

const char*
TimelinePanel::get_title()
{
  return _("Timeline");
}

const gchar*
TimelinePanel::get_stock_id()
{
  return "panel_timeline";
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
  get_controller().get_playback_controller().stop();  
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
TimelinePanel::on_zoom(int64_t zoom_scale)
{
  REQUIRE(timelineWidget);
  timelineWidget->zoom_view(zoom_scale);
  update_zoom_buttons();
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
TimelinePanel::on_mouse_hover(Time)
{
  /* do nothing */
}

void
TimelinePanel::on_playback_period_drag_released()
{
  //----- TEST CODE - this needs to set the playback point via the
  // real backend
  
  REQUIRE(timelineWidget);
    
  timelineWidget->get_state()->setPlaybackPoint(
    timelineWidget->get_state()->getPlaybackPeriodStart());
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
  
  BOOST_FOREACH( shared_ptr< model::Sequence > sequence,
    get_project().get_sequences() )
    {
      Gtk::TreeIter iter = sequenceChooserModel->append();
      Gtk::TreeModel::Row row = *iter;
      row[sequenceChooserColumns.sequenceColumn] = sequence;
      row[sequenceChooserColumns.nameColumn] = sequence->get_name();
      
      if(state && state->get_sequence() == sequence)
        sequenceChooser.set_active(iter);
    }
    
  // If there's no active sequence, then unselect
  if(!state)
    sequenceChooser.set_active(-1);
    
  // Unblock the event handler
  sequenceChooserChangedConnection.unblock();
}

void
TimelinePanel::update_playback_buttons()
{
  if (is_playing())
    {
      playPauseButton.set_stock_id(Stock::MEDIA_PAUSE);
      playPauseButton.set_tooltip_text("Pause playback");
    }
  else
    {
      playPauseButton.set_stock_id(Stock::MEDIA_PLAY);
      playPauseButton.set_tooltip_text("Start playback");
    }

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

  const shared_ptr<timeline::TimelineState> state =
    timelineWidget->get_state();
  if(state)
    {
      timeline::TimelineViewWindow &viewWindow = 
        state->get_view_window();
      
      zoomIn.set_sensitive(viewWindow.get_time_scale() != 1);
      zoomOut.set_sensitive(viewWindow.get_time_scale()
        != TimelineWidget::MaxScale);
    }
}

void
TimelinePanel::play()
{   
  get_controller().get_playback_controller().play();
}

void
TimelinePanel::pause()
{
  get_controller().get_playback_controller().pause();
}

bool
TimelinePanel::is_playing()
{
  return get_controller().get_playback_controller().is_playing();
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
TimelinePanel::show_time(Time time)
{
  ////////////TODO integrate the Timecode Widget  
  
  // timeIndicator.set_text(string(time));
}

bool
TimelinePanel::on_frame()
{
  /////////TODO what happens here??
  return true;
}

shared_ptr<timeline::TimelineState>
TimelinePanel::load_state(weak_ptr<Sequence> sequence)
{
  if(contains(timelineStates, sequence))
    return timelineStates[sequence];
  
  shared_ptr<Sequence> shared_sequence = sequence.lock();
  if(shared_sequence)
    {
      shared_ptr<timeline::TimelineState> new_state(
        new timeline::TimelineState(shared_sequence));
      timelineStates[sequence] = new_state;
      return new_state;
    }
  
  return shared_ptr< timeline::TimelineState >();
}

}}   // namespace gui::panels
