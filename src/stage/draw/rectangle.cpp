/*
  Rectangle  -  utility functions for GDK rectangles

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


/** @file rectangle.cpp
 ** Implementation of functions to work with rectangular screen ranges
 */

#include "stage/draw/rectangle.hpp"
#include <algorithm>


using std::max;
using std::min;

namespace stage {
namespace draw{
  
  bool
  pt_in_rect(const Gdk::Point &point, const Gdk::Rectangle &rect)
  {
    return (point.get_x() >= rect.get_x() &&
      point.get_x() < rect.get_x() + rect.get_width() &&
      point.get_y() >= rect.get_y() &&
      point.get_y() < rect.get_y() + rect.get_height());
  }
  
  bool
  rects_overlap (Gdk::Rectangle const& a, Gdk::Rectangle const& b)
  {
    return (
      max(a.get_x(), b.get_x()) <
      min(a.get_x() + a.get_width(), b.get_x() + b.get_width())
      &&
      max(a.get_y(), b.get_y()) <
      min(a.get_y() + a.get_height(), b.get_y() + b.get_height())
      );
  }
  
  
}}// namespace stage::draw
