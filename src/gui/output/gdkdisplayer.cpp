/*
  gdkdisplayer.cpp  -  Implements the class for displaying video via GDK

  Copyright (C)         Lumiera.org
    2000,               Arne Schirmacher <arne@schirmacher.de>
    2001-2007,          Dan Dennedy <dan@dennedy.org>
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
#if 0

#include "gui/gtk-lumiera.hpp"
#include "gui/output/gdkdisplayer.hpp"

#include <gdk/gdkx.h>
#include <iostream>

using std::cerr;
using std::endl;


namespace gui {
namespace output {

GdkDisplayer::GdkDisplayer( Gtk::Widget *drawing_area, int width, int height ) :
    drawingArea( drawing_area )
{
  REQUIRE(drawing_area != NULL);
  REQUIRE(width > 0);
  REQUIRE(height > 0);
  
  imageWidth = width, imageHeight = height;
}

bool
GdkDisplayer::usable()
{
  return true;
}

void
GdkDisplayer::put( const void* image )
{  
  int video_x = 0, video_y = 0, video_width = 0, video_height = 0;
  calculateVideoLayout(
    drawingArea->get_width(),
    drawingArea->get_height(),
    preferredWidth(), preferredHeight(),
    video_x, video_y, video_width, video_height );

  GdkWindow *window = drawingArea->get_window()->gobj();
  REQUIRE(window != NULL);  
    
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
}

}   // namespace output
}   // namespace gui

#endif
