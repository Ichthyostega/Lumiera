/*
  TIMELINE-PANEL.hpp  -  Dockable panel to hold the main timeline view            

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

*/

/** @file timeline-panel-obsolete.hpp
 ** This file contains the definition of the timeline panel
 ** @deprecated rework of timeline widget iminent
 */


#ifndef GUI_PANEL_TIMELINE_PANEL_OBSOLETE_H
#define GUI_PANEL_TIMELINE_PANEL_OBSOLETE_H

#include "gui/panel/panel.hpp"
#include "gui/widget/timecode-widget.hpp"
#include "gui/widget/timeline-widget.hpp"
#include "gui/widget/timeline/timeline-zoom-scale.hpp"
#include "gui/model/project.hpp"
#include "gui/ui-bus.hpp"   ////////////TODO for the obsolete Controller

#include "lib/time/timevalue.hpp"
#include "lib/util-coll.hpp"

#include <memory>
#include <map>

using namespace gui::widget;      /////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace gui  {
namespace model{
    class Sequence;
  }
  namespace panel {
  
  using std::shared_ptr;
  using std::weak_ptr;
  using lib::time::Time;
  using model::Project;
  using controller::Controller;
  
  
  
  /**
   * Dockable panel to hold timeline widget.
   */
  class TimelinePanelObsolete
    : public Panel
    {
    public:
      /**
       * @param panel_manager The owner panel manager widget.
       * @param dock_item The GdlDockItem that will host this panel.
       */
      TimelinePanelObsolete(workspace::PanelManager&, Gdl::DockItem&);
      
      
      static const char* getTitle();
      static const gchar* getStockID();
      
      
    private:
      //----- Event Handlers -----//
      
      void on_play_pause();
      void on_stop();
      
      void on_arrow_tool();
      void on_ibeam_tool();
      
      void on_zoom (double time_scale_ratio);
      void on_zoom_in();
      void on_zoom_out();
      
      void on_time_pressed();
        
      void on_mouse_hover (Time);
      void on_playback_period_drag_released();
      
      /** event handler for change of sequences list */
      void on_sequence_list_changed();
      
      /** event handler when choosing a new sequence for display */
      void on_sequence_chosen();
      
    private:
      
      void updateSequenceChooser();
      
      void updatePlaybackButtons();
      void updateToolButtons();
      void updateZoomButtons();
      
      void play();
      
      void pause();
      
      bool is_playing();
      
      void setTool (gui::widget::timeline::ToolType tool);
      
      void showTime (Time);
        
      shared_ptr<widget::timeline::TimelineState>
      loadState (weak_ptr<model::Sequence> sequence);
      
    private:
      /**
       * sequence chooser combo box columns
       */
      class SequenceChooserColumns
        : public Gtk::TreeModel::ColumnRecord
        {
        public:
          SequenceChooserColumns()
            {
              add(nameColumn);
              add(sequenceColumn);
            }
          
          /** invisible column which will be used
           *  to identify the sequence of a row. */
          Gtk::TreeModelColumn<weak_ptr<model::Sequence>> sequenceColumn;
          
          /** column to use as the label for the combo box widget items */
          Gtk::TreeModelColumn<uString> nameColumn;
        };
      
      
    private:
      
      //----- Data -----//
      
      // the obsolete and defunct UI model
      Project    obsoleteProject_;
      Controller obsoleteController_;                                 /////////////////////////////////////////TICKET #1048 GtkLumiera must not be a singleton

      
      // Grip Widgets
      ButtonBar toolbar;
      
      // Sequence Chooser
      SequenceChooserColumns sequenceChooserColumns;
      Glib::RefPtr<Gtk::ListStore> sequenceChooserModel;
      Gtk::ComboBox sequenceChooser;
      sigc::connection sequenceChooserChangedConnection;
      
      // Body Widgets
      std::unique_ptr<TimelineWidget> timelineWidget;
      
      std::map< weak_ptr<model::Sequence>
              , shared_ptr<widget::timeline::TimelineState>
              , ::util::WeakPtrComparator
              > 
        timelineStates;
      
      // Toolbar Widgets
      TimeCode timeCode;
      
      MiniButton previousButton;
      MiniButton rewindButton;
      MiniButton playPauseButton;
      MiniButton stopButton;
      MiniButton forwardButton;
      MiniButton nextButton;
      
      MiniToggleButton arrowTool;
      MiniToggleButton iBeamTool;
      
      Gtk::SeparatorToolItem separator1;
      
      MiniButton zoomIn;
      MiniButton zoomOut;
      gui::widget::timeline::TimelineZoomScale zoomScale;
      
      Gtk::SeparatorToolItem separator2;
      
      // Internals
      bool updatingToolbar;
      gui::widget::timeline::ToolType currentTool;
      
    private:
      // TEST CODE
      bool on_frame();
      
      //----- Constants -----//
    private:
      static const int ZoomToolSteps;
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_TIMELINE_PANEL_OBSOLETE_H*/
