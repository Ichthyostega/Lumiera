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

using namespace gui::widgets;

namespace gui {

namespace model {
class Sequence;
}
  
namespace panels {

/**
 * The definition of the timeline panel class, which holds timeline
 * widgets.
 */
class TimelinePanel : public Panel
{
public:
  /**
   * Contructor.
   * @param panel_manager The owner panel manager widget.
   * @param dock_item The GdlDockItem that will host this panel.
   **/
  TimelinePanel(workspace::PanelManager &panel_manager,
    GdlDockItem *dock_item);

  /**
   * Destructor 
   **/
  ~TimelinePanel();
  
  /**
   * Get the title of the panel.
   * @return Returns a pointer to the string title of the panel.
   **/
  static const char* get_title();
  
  /**
   * Get the stock id for this type panel.
   * @return Returns a pointer to the string stock id of the panel.
   **/
  static const gchar* get_stock_id();

private:
  //----- Event Handlers -----//
  
  void on_play_pause();
  void on_stop();
  
  void on_arrow_tool();
  void on_ibeam_tool();
  
  void on_zoom_in();
  void on_zoom_out();
  
  void on_time_pressed();
    
  void on_mouse_hover(gavl_time_t time);
  void on_playback_period_drag_released();
  
  /**
   * An event handler for when the list of sequences changes.
   **/
  void on_sequence_list_changed();
  
  /**
   * An event handler for when a new sequence is chosen in the
   * sequenceChooser.
   **/
  void on_sequence_chosen();
  
private:

  void update_sequence_chooser();
  
  void update_playback_buttons();
  void update_tool_buttons();
  void update_zoom_buttons();
  
  void play();
  
  void pause();
  
  bool is_playing();
  
  void set_tool(gui::widgets::timeline::ToolType tool);
  
  void show_time(gavl_time_t time);
    
  boost::shared_ptr<widgets::timeline::TimelineState> load_state(
    boost::weak_ptr<model::Sequence> sequence);
  
private:

  /**
   * The definition of the sequence chooser combo box columns
   **/
  class SequenceChooserColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    /**
     * Constructor
     **/
    SequenceChooserColumns()
      { add(nameColumn); add(sequenceColumn);  }

    /**
     * An invisible column which will be used to identify the sequence
     * of a row.
     **/
    Gtk::TreeModelColumn< boost::weak_ptr<model::Sequence> >
      sequenceColumn;
      
    /**
     * The column to use as the label for the combo box widget items.
     **/
    Gtk::TreeModelColumn< Glib::ustring > nameColumn;
  };

private:

  //----- Data -----//

  // Grip Widgets
  ButtonBar toolbar;
  
  // Sequence Chooser
  SequenceChooserColumns sequenceChooserColumns;
  Glib::RefPtr<Gtk::ListStore> sequenceChooserModel;
  Gtk::ComboBox sequenceChooser;
  sigc::connection sequenceChooserChangedConnection;
  
  // Body Widgets
  boost::scoped_ptr<TimelineWidget> timelineWidget;
  
  std::map< boost::weak_ptr<model::Sequence>,
    boost::shared_ptr<widgets::timeline::TimelineState> >
    timelineStates;
  
  // Toolbar Widgets
  Gtk::Label timeIndicator;
  Gtk::Button timeIndicatorButton;
  
  MiniButton previousButton;
  MiniButton rewindButton;
  MiniButton playPauseButton;
  MiniButton stopButton;
  MiniButton forwardButton;
  MiniButton nextButton;
    
  MiniToggleButton arrowTool;
  MiniToggleButton iBeamTool;
  
  Gtk::SeparatorToolItem seperator1;
  
  MiniButton zoomIn;
  MiniButton zoomOut;
  
  Gtk::SeparatorToolItem seperator2;
    
  // Internals
  bool updatingToolbar;
  gui::widgets::timeline::ToolType currentTool;
  
private:
  // TEST CODE
  bool on_frame();
  
  //----- Constants -----//
private:
  static const int ZoomToolSteps;
};

}   // namespace panels
}   // namespace gui

#endif // TIMELINE_PANEL_H
