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
   * Constructor
   * @param workspace_window The window that owns this panel.
   */
  TimelinePanel(workspace::WorkspaceWindow &workspace_window);

  /**
   * Destructor 
   **/
  ~TimelinePanel();

private:
  //----- Event Handlers -----//
  
  void on_play_pause();
  void on_stop();
  
  void on_arrow_tool();
  void on_ibeam_tool();
  
  void on_zoom_in();
  void on_zoom_out();
  
  void on_time_pressed();
  
  void on_page_switched(GtkNotebookPage*, guint);
  
  void on_mouse_hover(gavl_time_t time);
  void on_playback_period_drag_released();
  
  void on_sequence_list_changed();
  
private:

  void update_notebook();

  void update_playback_buttons();
  void update_tool_buttons();
  void update_zoom_buttons();
  
  void play();
  
  void pause();
  
  bool is_playing() const;
  
  void set_tool(gui::widgets::timeline::ToolType tool);
  
  void show_time(gavl_time_t time);
  
  TimelineWidget* get_current_page();

private:

  //----- Data -----//

  // Widgets
  Gtk::Toolbar toolbar;
  Gtk::HBox toolStrip;
  
  // Body Widgets
  Gtk::Notebook notebook;
  std::map< const model::Sequence*, boost::shared_ptr<TimelineWidget> >
    notebook_pages;
  
  // Toolbar Widgets
  
  Gtk::Label timeIndicator;
  Gtk::ToolButton timeIndicatorButton;
  
  Gtk::ToolButton previousButton;
  Gtk::ToolButton rewindButton;
  Gtk::ToolButton playPauseButton;
  Gtk::ToolButton stopButton;
  Gtk::ToolButton forwardButton;
  Gtk::ToolButton nextButton;
    
  Gtk::ToggleToolButton arrowTool;
  Gtk::ToggleToolButton iBeamTool;
  
  Gtk::SeparatorToolItem seperator1;
  
  Gtk::ToolButton zoomIn;
  Gtk::ToolButton zoomOut;
  
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
