/*
  PixbufDisplayer  -  displaying video via bitmap image

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file pixbuf-displayer.cpp
 ** Implementation of fallback functionality for video display.
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 */


#include "stage/gtk-base.hpp"
#include "stage/output/pixbuf-displayer.hpp"
#include "lib/format-cout.hpp"

#if false  ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <gdk/gdkx.h>
#endif     ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
#include <iostream>



namespace stage {
namespace output {
  
  PixbufDisplayer::PixbufDisplayer (Gtk::Image* drawing_area,
                              int width, int height)
    : Displayer{width,height}
    , drawingArea_{drawing_area}
    {
      REQUIRE (drawing_area);
      REQUIRE (width > 0);
      REQUIRE (height > 0);
      cout << "USING PixbufDisplayer" <<endl;
    }
  
  bool
  PixbufDisplayer::usable()
  {
    return true;  /////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
  }
  
  void
  PixbufDisplayer::put (void* const image)
  {  
    int video_x = 0,
        video_y = 0,
        video_width = 0,
        video_height = 0;
    
    calculateVideoLayout(
      drawingArea_->get_width(),
      drawingArea_->get_height(),
      videoWidth, videoHeight,
      video_x, video_y, video_width, video_height);
  
    GdkWindow *window = drawingArea_->get_window()->gobj();
    REQUIRE (window != NULL);
    
    if (not init_)
      {
        auto iconSet = Gtk::IconSet::lookup_default (Gtk::StockID("panel_play"));
        drawingArea_->set(iconSet, Gtk::IconSize(Gtk::ICON_SIZE_DIALOG));
        init_ = true;
        cout << "INIT Pixbuf"<< iconSet.get() <<endl;
      }
    drawingArea_->queue_draw();
    int pixSiz = drawingArea_->property_pixel_size();
    cout << "bong.."<<pixSiz<<endl;
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
