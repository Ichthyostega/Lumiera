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
#include <vector>



namespace gui  {
namespace model{
    class Sequence;
  }
namespace panel {
  
//using std::shared_ptr;
  
  /**
   * "experimental" child widget for investigation of gtk::Layout
   */
  class ChildEx
    : public Gtk::Button
    {
      static uint childNo;
      
    public:
      ChildEx();
     ~ChildEx();
      
    private:
      void on_clicked()  override;
    };
  
  void __verifyDeadChildren();
  
  
  
  /**
   * Dockable panel to hold timeline widget(s).
   * @todo build the actual implementation, after finishing the investigation
   * 
   * ## Investigation of gtk::GtkLayout
   * As of 10/2016, we start this task with an exploration of GTK behaviour
   * 
   * \par Plan of investigation
   *  1. place some simple widgets (Buttons) ✔
   *  2. learn how to draw
   *  3. place a huge number of widgets, to scrutinise scrolling and performance
   *  4. place widgets overlapping and irregularily, beyond the scrollable area ✔
   *  5. bind signals to those widgets, to verify event dispatching
   *  6. bind some further signal(s) to the ~GtkLayout container
   *  7. hide and re-show a partially and a totally overlapped widget
   *  8. find a way to move a widget ✔
   *  9. expand an existing widget (text change)
   *  10. build a custom "clip" widget
   *  11. retrofit all preceding tests to use this "clip" widget
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
      
      
    private:
      Gtk::Box twoParts_;
      Gtk::ButtonBox buttons_;
      Gtk::Button button_1_;
      Gtk::Button button_2_;
      Gtk::Frame frame_;
      Gtk::ScrolledWindow scroller_;
      Gtk::Layout canvas_;
      
      ChildEx* makeChld();
      
      using ChildV = std::vector<ChildEx*>;
      ChildV childz_;
      
      void experiment_1();
      void experiment_2();
    };
  
  
}}// namespace gui::panel
#endif /*GUI_PANEL_TIMELINE_PANEL_H*/
