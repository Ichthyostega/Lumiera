/*
  NullDisplayer.hpp  -  fallback video displayer to not display video at all

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file null-displayer.hpp
 ** Passive deactivated video displayer.
 ** @deprecated obsolete since GTK-3
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 */


#include "stage/gtk-base.hpp"
#include "stage/output/null-displayer.hpp"

#if false  ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <gdk/gdkx.h>
#endif     ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <iostream>

using std::cerr;
using std::endl;


namespace stage {
namespace output {
  
  NullDisplayer::NullDisplayer (Gtk::Widget* drawing_area,
                              int width, int height)
    : drawingArea( drawing_area )
    {
      REQUIRE (drawing_area != NULL);
      REQUIRE (width > 0);
      REQUIRE (height > 0);
      
      imageWidth = width,
      imageHeight = height;
    }
  
  bool
  NullDisplayer::usable()
  {
    return false;  /////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
  }
  
  void
  NullDisplayer::put (void* const image)
  {  
    int video_x = 0,
        video_y = 0,
        video_width = 0,
        video_height = 0;
    
    calculateVideoLayout(
      drawingArea->get_width(),
      drawingArea->get_height(),
      preferredWidth(), preferredHeight(),
      video_x, video_y, video_width, video_height);
  
    GdkWindow *window = drawingArea->get_window()->gobj();
    REQUIRE (window != NULL);
      
  #if false  ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
    GdkGC *gc = gdk_gc_new( window );
    REQUIRE(gc != NULL);
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data( (const guchar*)image, GDK_COLORSPACE_RGB, FALSE, 8,
      preferredWidth(), preferredHeight(), preferredWidth() * 3, NULL, NULL );
    REQUIRE(pixbuf != NULL);
      
    GdkPixbuf *scaled_image = gdk_pixbuf_scale_simple( pixbuf, video_width, video_height, GDK_INTERP_NEAREST );
    REQUIRE(scaled_image != NULL);
    
    gdk_draw_pixbuf( window, gc, scaled_image, 0, 0, video_x, video_y, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0 );
    
    g_object_unref( scaled_image );
    g_object_unref( pixbuf );
    g_object_unref( gc );
  #endif     ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
  }
  
  
}}   // namespace stage::output
