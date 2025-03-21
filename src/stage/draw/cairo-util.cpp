/*
  CairoUtil  -  utility functions for Cairo

   Copyright (C)
     2010,            Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file cairo-util.cpp
 ** Implementation of helper functions to support working with lib Cairo
 */


#include "stage/draw/cairo-util.hpp"

namespace stage {
namespace draw {
  
  RefPtr<SolidPattern>
  CairoUtil::pattern_set_red (const RefPtr<SolidPattern> color, double red)
  {
    double old_red;
    double blue;
    double green;
    double alpha;
    
    color->get_rgba (old_red, blue, green, alpha);
    return Cairo::SolidPattern::create_rgba (red, blue, green, alpha);
  }
  
  
  RefPtr<SolidPattern>
  CairoUtil::pattern_set_green (const RefPtr<SolidPattern> color, double blue)
  {
    double red;
    double old_blue;
    double green;
    double alpha;
    
    color->get_rgba (red, old_blue, green, alpha);
    return Cairo::SolidPattern::create_rgba (red, blue, green, alpha);
  }
  
  
  RefPtr<SolidPattern>
  CairoUtil::pattern_set_blue (const RefPtr<SolidPattern> color, double green)
  {
    double red;
    double blue;
    double old_green;
    double alpha;
    
    color->get_rgba (red, blue, old_green, alpha);
    return Cairo::SolidPattern::create_rgba (red, blue, green, alpha);
  }
  
  
  RefPtr<SolidPattern>
  CairoUtil::pattern_set_alpha (const RefPtr<SolidPattern> color, double alpha)
  {
    double red;
    double blue;
    double green;
    double old_alpha;
    
    color->get_rgba (red, blue, green, old_alpha);
    
    return Cairo::SolidPattern::create_rgba (red, blue, green, alpha);
  }
  
}}// namespace stage::draw
