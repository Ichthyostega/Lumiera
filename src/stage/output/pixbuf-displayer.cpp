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
  
  PixbufDisplayer::PixbufDisplayer (Gtk::Image& drawing_area,
                              uint width, uint height)
    : Displayer{width,height}
    , drawingArea_{drawing_area}
    {
      REQUIRE (width > 0);
      REQUIRE (height > 0);
      auto iconSet = Gtk::IconSet::lookup_default (Gtk::StockID("panel_play"));
      drawingArea_.set(iconSet, Gtk::IconSize(Gtk::ICON_SIZE_DIALOG));
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
    int orgX = 0,
        orgY = 0,
        destWidth = 0,
        destHeight = 0;
    
    calculateVideoLayout(
      drawingArea_.get_width(),
      drawingArea_.get_height(),
      orgX, orgY, destWidth, destHeight);
  
    GdkWindow *window = drawingArea_.get_window()->gobj();
    REQUIRE (window != NULL);
    
  #if false  ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
    GdkGC *gc = gdk_gc_new( window );
    REQUIRE(gc != NULL);
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data( (const guchar*)image, GDK_COLORSPACE_RGB, FALSE, 8,
      preferredWidth(), preferredHeight(), preferredWidth() * 3, NULL, NULL );
    REQUIRE(pixbuf != NULL);
      
    GdkPixbuf *scaled_image = gdk_pixbuf_scale_simple( pixbuf, destWidth, destHeight, GDK_INTERP_NEAREST );
    REQUIRE(scaled_image != NULL);
    
    gdk_draw_pixbuf( window, gc, scaled_image, 0, 0, orgX, orgY, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0 );
    
    g_object_unref( scaled_image );
    g_object_unref( pixbuf );
    g_object_unref( gc );
  #endif     ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
    auto* imageData = static_cast<const guint8*> (image);
    auto rawBuf = Gdk::Pixbuf::create_from_data (imageData
                                                ,Gdk::COLORSPACE_RGB
                                                ,false                 // has_alpha
                                                ,8                     // bits_per_sample
                                                ,videoWidth
                                                ,videoHeight
                                                ,0                     // rowstride (can be used to round up to even powers of two per row)                     
                                                );
    ASSERT (rawBuf);
    auto scaledBuf = rawBuf->scale_simple (destWidth, destHeight, Gdk::INTERP_NEAREST);
    drawingArea_.set (scaledBuf);
    drawingArea_.queue_draw();
    cout << "bong.."<<scaledBuf.get()<<endl;
  }
  
  
}}   // namespace stage::output
