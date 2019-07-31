/*
  GTK-CANVAS-EXPERIMENT.hpp  -  Explore capabilities of the GTK canvas widget

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


/** @file gtk-canvas-experiment.hpp
 ** A custom widget using a canvas to draw and to place widgets.
 ** @remark as of 10/2018 we start to build a new timeline widget,
 **         based on this technology demo.
 */


#ifndef RESEARCH_GTK_CANVAS_EXPERIMENT_H
#define RESEARCH_GTK_CANVAS_EXPERIMENT_H

#include "stage/gtk-base.hpp"

#include <vector>


namespace demo {
  
  
  /**
   * "experimental" child widget for investigation of Gtk::Layout
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
   * "experimental" custom canvas, based on Gtk::Layout.
   * In addition this customised widget supports direct drawing
   */
  class Canvas
    : public Gtk::Layout
    {
      bool shallDraw_;
      bool recalcExtension_ = false;
      
    public:
      void enableDraw (bool);
      void adjustSize();
      
    private:
      virtual bool on_draw (Cairo::RefPtr<Cairo::Context> const&)  override;
      
      void determineExtension();
    };
  
  
  
  /**
   * Dockable panel to hold timeline widget(s).
   * @todo build the actual implementation, after finishing the investigation
   * 
   * ## Investigation of gtk::GtkLayout
   * As of 10/2016, we start this task with an exploration of GTK behaviour
   * 
   * \par Plan of investigation
   *  1. place some simple widgets (Buttons) ✔
   *  2. learn how to draw ✔
   *  3. place a huge number of widgets, to scrutinise scrolling and performance
   *  4. place widgets overlapping and irregularly, beyond the scrollable area ✔
   *  5. bind signals to those widgets, to verify event dispatching ✔
   *  6. bind some further signal(s) to the ~GtkLayout container
   *  7. hide and re-show a partially and a totally overlapped widget
   *  8. find a way to move a widget ✔ and delete arbitrary widgets ✔
   *  9. expand an existing widget (text change) ✔
   *  10. build a custom "clip" widget ✘
   *  11. retrofit all preceding tests to use this "clip" widget ✘
   */
  class CanvasDemoPanel
    : public Gtk::Box
    {
    public:
      CanvasDemoPanel();
      
      
      
    private:
      Gtk::Box twoParts_;
      Gtk::ButtonBox buttons_;
      Gtk::Button button_1_;
      Gtk::Button button_2_;
      Gtk::Button button_3_;
      Gtk::Button button_4_;
      Gtk::Button button_5_;
      Gtk::CheckButton toggleDraw_;
      Gtk::Frame frame_;
      Gtk::ScrolledWindow scroller_;
      Canvas canvas_;
      
      ChildEx* makeChld();
      
      using ChildV = std::vector<ChildEx*>;
      ChildV childz_;
      
      void experiment_1();
      void experiment_2();
      void experiment_3();
      void experiment_4();
      void experiment_5();
    };
  
  
}// namespace demo
#endif /*RESEARCH_GTK_CANVAS_EXPERIMENT_H*/
