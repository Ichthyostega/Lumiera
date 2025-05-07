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
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 */


#include "stage/gtk-base.hpp"
#include "stage/output/null-displayer.hpp"
#include "lib/format-cout.hpp"


namespace stage {
namespace output {
  
  NullDisplayer::NullDisplayer (Gtk::Widget& drawing_area,
                                int width, int height)
    : Displayer{width,height}
    , drawingArea_{drawing_area}
    {
      REQUIRE (width > 0);
      REQUIRE (height > 0);
      cout << "NullDisplayer("<<&drawing_area<<"): "<<width<<" x "<<height<<endl;
    }
  
  void
  NullDisplayer::put (void* const image)
  {  
    int video_x = 0,
        video_y = 0,
        video_width = 0,
        video_height = 0;
    
    calculateVideoLayout(
      drawingArea_.get_width(),
      drawingArea_.get_height(),
      videoWidth, videoHeight,
      video_x, video_y, video_width, video_height);
  
    GdkWindow *window = drawingArea_.get_window()->gobj();
    REQUIRE (window != NULL);
    cout << "put("<<util::showAdr(image)<<")\t x="<<video_x<<" y="<<video_y<<" w:"<<video_width<<" h:"<<video_height<<endl;
  }
  
  
}}   // namespace stage::output
