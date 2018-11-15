/*
  BasicDrawStrategy  -  Implementation of a basic draw strategy

  Copyright (C)         Lumiera.org
    2010,               Stefan Kangas <skangas@skangas.se

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


/** @file draw-strategy.cpp
 ** @deprecated very likely to be completely rewritten.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/basic-draw-strategy.hpp"

namespace gui {
namespace widget {
namespace timeline {
  
  DrawStrategy::~DrawStrategy() { }  // emit VTable here
  
  
  void
  BasicDrawStrategy::draw (Entity const& entity
                          ,Cairo::RefPtr<Cairo::Context> cr
                          ,TimelineViewWindow* const window)  const
  {
    REQUIRE (cr);
    REQUIRE (window);
    
    int x = window->time_to_x(entity.getBegin());
    int width = window->time_to_x(
      entity.getEnd()) - window->time_to_x(entity.getBegin());
    
    // Draw a rectangle for the entity
    // TODO: get height from the timeline::Entity
    cr->rectangle(x, 1, width, 100-2);
    // if (entity.getSelected())
      cr->set_source(Cairo::SolidPattern::create_rgb (0.4, 0.4, 0.8));
    // else
      // cr->set_source(Cairo::SolidPattern::create_rgb (0.4, 0.4, 0.4));
    cr->fill_preserve();
    cr->set_source_rgb(0.25, 0.25, 0.25);
    cr->stroke();
    
    // Show the entities name
    cr->rectangle(x, 1, width, 100-2);
    cr->clip();
    cr->move_to (x + 3, 12);
    cr->set_source_rgb (1.0, 1.0, 1.0);
    cr->set_font_size (9);
    cr->show_text (entity.getName());
  }
  
  
}}}// namespace gui::widget::timeline
