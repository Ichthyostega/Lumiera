/*
  TimelinePanel  -  Dockable panel to hold the main timeline view

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


/** @file §§§
 ** TODO §§§
 */


#include "gui/gtk-lumiera.hpp"
#include "gui/panel/timeline-panel.hpp"
#include "gui/widget/timeline/timeline-zoom-scale.hpp"

#include "gui/workspace/workspace-window.hpp"
#include "gui/model/project.hpp"
#include "gui/ui-bus.hpp"

#include "lib/util.hpp"

#include <boost/foreach.hpp>

using namespace Gtk;
using namespace sigc;
using namespace gui::widget;
using namespace gui::widget::timeline;
using namespace gui::model;

using std::shared_ptr;
using std::weak_ptr;
using util::contains;


namespace gui {
namespace panel {
    
  const int TimelinePanel::ZoomToolSteps = 2; // 2 seems comfortable
  
  
  TimelinePanel::TimelinePanel (workspace::PanelManager& panelManager,
                                Gdl::DockItem& dockItem)
    : Panel(panelManager, dockItem, getTitle(), getStockID())
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
      getProject().get_sequences().signal_changed().connect(mem_fun(this,
          &TimelinePanel::on_sequence_list_changed));
      
      // Setup the sequence chooser
      sequenceChooserModel = Gtk::ListStore::create(sequenceChooserColumns);
      ENSURE(sequenceChooserModel);
      
      sequenceChooser.set_model(sequenceChooserModel);
      sequenceChooser.pack_start(sequenceChooserColumns.nameColumn);
      sequenceChooser.show_all();
      
      sequenceChooserChangedConnection = sequenceChooser.signal_changed().
        connect( sigc::mem_fun(*this, &TimelinePanel::on_sequence_chosen) );
      
      panelBar_.pack_start(sequenceChooser, PACK_SHRINK);
      
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
      panelBar_.pack_start(toolbar, PACK_SHRINK);
      
      // Setup tooltips
      sequenceChooser .set_tooltip_text(_("Change sequence"));
      
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
      shared_ptr<Sequence> sequence = *(getProject().get_sequences().begin());
      timelineWidget.reset(new TimelineWidget(loadState(sequence)));
      pack_start(*timelineWidget, PACK_EXPAND_WIDGET);
      
      // since TimelineWidget is now initialised,
      // wire the zoom slider to react on timeline state changes
      zoomScale.wireTimelineState (timelineWidget->get_state(),
                                   timelineWidget->state_changed_signal());
      
      // Set the initial UI state
      updateSequenceChooser();
      updateToolButtons();
      updateZoomButtons();
      showTime (Time::ZERO);
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
  
  void
  TimelinePanel::on_play_pause()
  {
    if (!is_playing())
      play();
    else
      pause();
    
    updatePlaybackButtons();
  }
  
  void
  TimelinePanel::on_stop()
  {
    getController().get_playback_controller().stop();
    updatePlaybackButtons();
  }
  
  void
  TimelinePanel::on_arrow_tool()
  {
    setTool(timeline::Arrow);
  }
  
  void
  TimelinePanel::on_ibeam_tool()
  {  
    setTool(timeline::IBeam);
  }
  
  void
  TimelinePanel::on_zoom (double time_scale_ratio)
  {
    REQUIRE(timelineWidget);
    timelineWidget->zoom_view(time_scale_ratio);
  }
  
  void
  TimelinePanel::on_zoom_in()
  {
    REQUIRE(timelineWidget);
    timelineWidget->zoom_view(ZoomToolSteps);
    updateZoomButtons();
  }
  
  void
  TimelinePanel::on_zoom_out()
  {
    REQUIRE(timelineWidget);
    timelineWidget->zoom_view(-ZoomToolSteps);
    updateZoomButtons();
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
    updateSequenceChooser();
  }
  
  
  /** @deprecated for #955 : move this callback \em into the model! */
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
            if(sequence != old_state->getSequence())
              timelineWidget->set_state(loadState(sequence));
          }
      }
    
    updateZoomButtons();
  }
  
  
  void
  TimelinePanel::updateSequenceChooser()
  {
    REQUIRE(sequenceChooserModel);
    
    // Block the event handler
    sequenceChooserChangedConnection.block();
    
    // Repopulate the sequence chooser
    sequenceChooserModel->clear();
    
    shared_ptr<timeline::TimelineState> state =
      timelineWidget->get_state();
    
    BOOST_FOREACH( shared_ptr< model::Sequence > sequence,
      getProject().get_sequences() )
      {
        Gtk::TreeIter iter = sequenceChooserModel->append();
        Gtk::TreeModel::Row row = *iter;
        row[sequenceChooserColumns.sequenceColumn] = sequence;
        row[sequenceChooserColumns.nameColumn] = sequence->get_name();
        
        if(state && state->getSequence() == sequence)
          sequenceChooser.set_active(iter);
      }
    
    // If there's no active sequence, then unselect
    if (!state)
      sequenceChooser.set_active(-1);
    
    // Unblock the event handler
    sequenceChooserChangedConnection.unblock();
  }
  
  
  void
  TimelinePanel::updatePlaybackButtons()
  {
    if (is_playing())
      {
        playPauseButton.setStockID (Stock::MEDIA_PAUSE);
        playPauseButton.set_tooltip_text("Pause playback");
      }
    else
      {
        playPauseButton.setStockID (Stock::MEDIA_PLAY);
        playPauseButton.set_tooltip_text("Start playback");
      }
  
  }
  
  
  void
  TimelinePanel::updateToolButtons()
  {    
    if (!updatingToolbar)
      {
        updatingToolbar = true;
        arrowTool.set_active(currentTool == timeline::Arrow);
        iBeamTool.set_active(currentTool == timeline::IBeam);
        updatingToolbar = false;
      }
  }
  
  
  void
  TimelinePanel::updateZoomButtons()
  {
  /* This function is no longer needed
   * TODO: Let the ZoomScaleWidget perform
   * the update on its own */
  }
  
  
  void
  TimelinePanel::play()
  {   
    getController().get_playback_controller().play();
  }
  
  void
  TimelinePanel::pause()
  {
    getController().get_playback_controller().pause();
  }
  
  bool
  TimelinePanel::is_playing()
  {
    return getController().get_playback_controller().is_playing();
  }
  
  void
  TimelinePanel::setTool (timeline::ToolType tool)
  {
    REQUIRE(timelineWidget);
    
    if (updatingToolbar) return;
    
    currentTool = tool;
    timelineWidget->set_tool(tool);
    updateToolButtons();
  }
  
  void
  TimelinePanel::showTime (Time time)
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
  TimelinePanel::loadState (weak_ptr<Sequence> sequence)
  {
    /* state exists */
    if (contains(timelineStates, sequence))
        return timelineStates[sequence];
    
    shared_ptr<Sequence> shared_sequence = sequence.lock();
    if (shared_sequence)
      {
        shared_ptr<timeline::TimelineState> new_state(new timeline::TimelineState(shared_sequence));
        timelineStates[sequence] = new_state;
        return new_state;
      }
    
    return shared_ptr< timeline::TimelineState >();
  }
  
}}   // namespace gui::panel
