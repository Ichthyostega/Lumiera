/*
  timeline-clip.cpp  -  Implementation of the timeline clip object
 
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

#include "timeline-clip.hpp"

namespace gui {
namespace widgets {
namespace timeline {

Clip::Clip(boost::shared_ptr<model::Clip> clip)
  : model_clip(clip)
{
  REQUIRE(model_clip);
}

void
Clip::draw_clip(Cairo::RefPtr<Cairo::Context> cr,
    TimelineViewWindow* const window) const
{
  REQUIRE(cr);
  REQUIRE(window);

  int x = window->time_to_x(1000000);
  int width = window->time_to_x(2000000) - window->time_to_x(1000000);
  
  // Draw a rectangle for the clip
  cr->rectangle(x, 1, width, 100-2); 

  // TODO: get duration from the model::Clip
  // TODO: get height from the Timeline::Track

  cr->set_source_rgb(0.4, 0.4, 0.4);
  cr->fill_preserve();
  
  cr->set_source_rgb(0.25, 0.25, 0.25);
  cr->stroke();

  // Show the clip name
  cr->rectangle(x, 1, width, 100-2); 
  cr->clip();

  cr->move_to(x + 3, 15);
  cr->set_source_rgb(1.0, 1.0, 1.0);
  Cairo::RefPtr<Cairo::ToyFontFace> font =
    Cairo::ToyFontFace::create("Bitstream Charter",
                               Cairo::FONT_SLANT_NORMAL,
                               Cairo::FONT_WEIGHT_NORMAL);
  cr->set_font_face(font);
  cr->set_font_size(11);
  cr->show_text("Track");  // TODO: get clip name from model

  // TODO: Show thumbnails for clip
}


}   // namespace timeline
}   // namespace widgets
}   // namespace gui

