/*
  GgdkDisplayer  -  displaying video via GDK

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file gdkdisplayer.cpp
 ** Dysfunctional implementation code, formerly used to
 ** create a video display based on GDK
 ** @deprecated obsolete since GTK-3
 */


#include "stage/gtk-base.hpp"
#include "stage/output/gdkdisplayer.hpp"

#if false  ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <gdk/gdkx.h>
#endif     ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <iostream>

using std::cerr;
using std::endl;


namespace stage {
namespace output {
  
  GdkDisplayer::GdkDisplayer (Gtk::Widget* drawing_area,
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
  GdkDisplayer::usable()
  {
    return false;  /////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
  }
  
  void
  GdkDisplayer::put (void* const image)
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
