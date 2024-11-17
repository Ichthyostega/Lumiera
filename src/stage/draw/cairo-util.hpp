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
 ** Utility functions for working with elements from the Cairo
 ** vector drawing library.
 */

#include <cairomm/cairomm.h>

#ifndef STAGE_DRAW_CAIRO_H
#define STAGE_DRAW_CAIRO_H

using Cairo::RefPtr;
using Cairo::SolidPattern;


namespace stage {
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
  
  
}}// namespace stage::draw
#endif /*STAGE_DRAW_CAIRO_H*/
