/*
  timeline.cpp  -  Implementation of the timeline widget
 
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

#include <gdkmm/drawable.h>
#include <gdkmm/general.h>
#include <cairomm-1.0/cairomm/cairomm.h>

#include "timeline-widget.hpp"

using namespace Gtk;

namespace lumiera {
namespace gui {
namespace widgets {

TimelineWidget::TimelineWidget() :
  Table(2, 2),
  horizontalAdjustment(0, 200, 400),
  verticalAdjustment(0, 200, 400),
  horizontalScroll(horizontalAdjustment),
  verticalScroll(verticalAdjustment),
  rowHeaderViewport(),
  label1("label1"), label2("label2"), label3("label3"), label4("label4"),
  label5("label5"), label6("label6"), label7("label7"), label8("label8"),
  label9("label1"), label10("label10"), label11("label11"), ruler("ruler")
  {
    rowHeaderBox.pack_start(label1);
    rowHeaderBox.pack_start(label2);
    rowHeaderBox.pack_start(label3);
    rowHeaderBox.pack_start(label4);
    rowHeaderBox.pack_start(label5);
    rowHeaderBox.pack_start(label6);
    rowHeaderBox.pack_start(label7);
    rowHeaderBox.pack_start(label8);
    rowHeaderBox.pack_start(label9);
    rowHeaderBox.pack_start(label10);
    rowHeaderBox.pack_start(label11);
    rowHeaderViewport.add(rowHeaderBox);

    rowHeaderViewport.set_hadjustment(horizontalAdjustment);
    rowHeaderViewport.set_vadjustment(verticalAdjustment);

    attach(ruler, 1, 2, 0, 1, FILL|EXPAND, SHRINK);
    attach(rowHeaderViewport, 0, 1, 1, 2, SHRINK, FILL|EXPAND);
    attach(horizontalScroll, 1, 2, 2, 3, FILL|EXPAND, SHRINK);
    attach(verticalScroll, 2, 3, 1, 2, SHRINK, FILL|EXPAND);
  }



}   // namespace widgets
}   // namespace gui
}   // namespace lumiera

