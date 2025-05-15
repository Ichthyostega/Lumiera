/*
  CAIRO-UTIL.hpp  -  utility functions for Cairo

   Copyright (C)
     2010,            Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file cairo-util.hpp
 ** Utility functions for working with elements from the Cairo vector drawing library.
 */


#ifndef STAGE_DRAW_CAIRO_H
#define STAGE_DRAW_CAIRO_H

#include "stage/gtk-base.hpp"
#include "lib/util.hpp"


namespace stage{
namespace draw {
  
  using Cairo::SolidPattern;
  using Cairo::RefPtr;
  
  
  inline RefPtr<SolidPattern>
  pattern_set_red (RefPtr<SolidPattern> const& color, double red)
  {
    double _red_ignored;
    double green;
    double blue;
    double alpha;
    
    color->get_rgba (_red_ignored, green, blue, alpha);
    return Cairo::SolidPattern::create_rgba (red, green, blue, alpha);
  }
  
  
  inline RefPtr<SolidPattern>
  pattern_set_green (RefPtr<SolidPattern> const& color, double green)
  {
    double red;
    double _green_ignored;
    double blue;
    double alpha;
    
    color->get_rgba (red, _green_ignored, blue, alpha);
    return Cairo::SolidPattern::create_rgba (red, green, blue, alpha);
  }
  
  
  inline RefPtr<SolidPattern>
  pattern_set_blue (RefPtr<SolidPattern> const& color, double blue)
  {
    double red;
    double green;
    double _blue_ignored;
    double alpha;
    
    color->get_rgba (red, green, _blue_ignored, alpha);
    return Cairo::SolidPattern::create_rgba (red, green, blue, alpha);
  }
  
  
  inline RefPtr<SolidPattern>
  pattern_set_alpha (RefPtr<SolidPattern> const& color, double alpha)
  {
    double red;
    double green;
    double blue;
    double _alpha_ignored;
    
    color->get_rgba (red, green, blue, _alpha_ignored);
    
    return Cairo::SolidPattern::create_rgba (red, green, blue, alpha);
  }
  
  
  inline bool
  pt_in_rect (Gdk::Point const& point, Gdk::Rectangle const& rect)
  {
    return (point.get_x() >= rect.get_x()
       and  point.get_x() <  rect.get_x() + rect.get_width()
       and  point.get_y() >= rect.get_y()
       and  point.get_y() <  rect.get_y() + rect.get_height());
  }
  
  inline bool
  rects_overlap (Gdk::Rectangle const& recA, Gdk::Rectangle const& recB)
  {
    return (
      util::max (recA.get_x(), recB.get_x())
        < util::min (recA.get_x() + recA.get_width(), recB.get_x() + recB.get_width())
      and
      util::max (recA.get_y(), recB.get_y())
        < util::min (recA.get_y() + recA.get_height(), recB.get_y() + recB.get_height())
      );
  }
  
}}// namespace stage::draw
#endif /*STAGE_DRAW_CAIRO_H*/
