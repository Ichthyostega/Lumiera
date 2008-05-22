/*
  timeline.hpp  -  Declaration of the timeline widget
 
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
/** @file timeline.hpp
 ** This file contains the definition of timeline widget
 */

#ifndef TIMELINE_WIDGET_HPP
#define TIMELINE_WIDGET_HPP

#include <gtkmm.h>

namespace lumiera {
namespace gui {
namespace widgets {

  class TimelineWidget : public Gtk::Table
    {
    public:
      TimelineWidget();

      /* ===== Overrides ===== */
    protected:
      Gtk::Adjustment horizontalAdjustment, verticalAdjustment;
      Gtk::HScrollbar horizontalScroll;
      Gtk::VScrollbar verticalScroll;

      Gtk::Viewport rowHeaderViewport;
      Gtk::VBox rowHeaderBox;
      Gtk::Label label1, label2, label3, label4;
      Gtk::Label label5, label6, label7, label8;
      Gtk::Label label9, label10, label11, label12;
      Gtk::Label ruler;
    };

}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

#endif // TIMELINE_WIDGET_HPP
