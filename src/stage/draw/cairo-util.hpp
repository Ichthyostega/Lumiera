/*
  CAIRO-UTIL.hpp  -  utility functions for Cairo

  Copyright (C)         Lumiera.org
    2010,               Stefan Kangas <skangas@skangas.se>

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


/** @file cairo-util.hpp
 ** Utility functions for working with elements from the Cairo
 ** vector drawing library.
 */

#include <cairomm/cairomm.h>

#ifndef GUI_DRAW_CAIRO_H
#define GUI_DRAW_CAIRO_H

using Cairo::RefPtr;
using Cairo::SolidPattern;


namespace gui {
namespace draw {
  
  /**
   * @todo this is not a real entity; consider to turn this into a set of free functions!
   */ 
  class CairoUtil
    {
    public:
      /**
       * Make a new SolidPattern from an old one, changing the red component of the colour.
       * @param The new value for the red component of the colour.
       * @return The new pattern.
       */
      static RefPtr<SolidPattern>
      pattern_set_red (const RefPtr<SolidPattern> color, double red);
      
      /**
       * Make a new SolidPattern from an old one, changing the green component of the colour.
       * @param The new value for the green component of the colour.
       * @return The new pattern.
       */
      static RefPtr<SolidPattern>
      pattern_set_green (const RefPtr<SolidPattern>, double green);
      
      /**
       * Make a new SolidPattern from an old one, changing the blue component of the colour.
       * @param The new value for the blue component of the colour.
       * @return The new pattern.
       */
      static RefPtr<SolidPattern>
      pattern_set_blue (const RefPtr<SolidPattern>, double blue);
      
      /**
       * Make a new SolidPattern from an old one, changing the alpha component of the colour.
       * @param The new value for the alpha component of the colour.
       * @return The new pattern.
       */
      static RefPtr<SolidPattern>
      pattern_set_alpha (const RefPtr<SolidPattern>, double alpha);
    };
  
  
}}// namespace gui::draw
#endif /*GUI_DRAW_CAIRO_H*/
