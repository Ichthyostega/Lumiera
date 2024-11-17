/*
  Rectangle  -  utility functions for GDK rectangles

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file rectangle.cpp
 ** Implementation of functions to work with rectangular screen ranges
 ** @todo 3/23 this is leftover code from the old GKT-2 Lumiera GUI.
 **       This code should be consolidated into some kind of Cairo drawing utils!
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
