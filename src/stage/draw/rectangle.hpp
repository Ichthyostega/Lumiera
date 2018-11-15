/*
  RECTANGLE.hpp  -  utility functions for GDK rectangles

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


/** @file rectangle.hpp
 ** Helper functions to work with rectangular screen ranges
 */


#ifndef STAGE_DRAW_RECTANGLE_H
#define STAGE_DRAW_RECTANGLE_H

#include "stage/gtk-base.hpp"

namespace gui {
namespace draw {
  
  /** determine whether point is within the bounding box of a rectangle */
  bool pt_in_rect(Gdk::Point const& point, Gdk::Rectangle const& rect);
  
  
  /** determine whether two rectangles overlap */
  bool rects_overlap(Gdk::Rectangle const& a, Gdk::Rectangle const& b);
  
  
}}// namespace gui::draw
#endif /*STAGE_DRAW_RECTANGLE_H*/
