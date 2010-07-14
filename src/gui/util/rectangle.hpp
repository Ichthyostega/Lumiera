/*
  rectangle.hpp  -  Declares utility functions for GDK rects
 
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
/** @file rectangle.hpp
 ** This file contains the declaration of the utility functions for
 ** GDK rects
 */

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "../gtk-lumiera.hpp"

namespace gui {
namespace util {

/**
 * Tests whether point is within the bounding box of a rectangle.
 * @param point The point to test.
 * @param rect The rect to test.
 * @return Returns true if the point is within the rectangle, false if
 * not.
 **/
bool pt_in_rect(const Gdk::Point &point, const Gdk::Rectangle &rect);

/**
 * Tests whether two rectangles overlap.
 * @param a The first rectangle.
 * @param b The second rectangle.
 **/
bool rects_overlap(const Gdk::Rectangle &a, const Gdk::Rectangle &b);

}   // util
}   // gui

#endif // RECTANGLE_HPP
