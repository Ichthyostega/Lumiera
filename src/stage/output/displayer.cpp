/*
  Displayer  -  base class for displaying video

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


/** @file displayer.cpp
 ** Implementation of a displayer object, intended for creating
 ** a video display in the UI. This class was created as part of
 ** an initial draft of the user interface.
 ** @warning as of 2016 it is not clear, if this code will be
 **          evolved into the actual display facility, or be
 **          replaced and rewritten, when we're about to 
 **          create a functional video display connected
 **          to the render engine. 
 */


#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"
#include "stage/output/xvdisplayer.hpp"
#include "stage/output/gdkdisplayer.hpp"

namespace stage {
namespace output {
  
  bool
  Displayer::usable()
  {
    return false;
  }
  
  DisplayerInput
  Displayer::format()
  {
    return DISPLAY_NONE;
  }
  
  int
  Displayer::preferredWidth()
  {
    return imageWidth;
  }
  
  int
  Displayer::preferredHeight()
  {
    return imageHeight;
  }
  
  void
  Displayer::calculateVideoLayout(
          int widget_width, int widget_height,
          int image_width, int image_height,
          int &video_x, int &video_y, int &video_width, int &video_height )
  {
    REQUIRE (widget_width >= 0);
    REQUIRE (widget_height >= 0);
    REQUIRE (image_width >= 0);
    REQUIRE (image_height >= 0);

    double ratio_width = ( double ) widget_width / ( double ) image_width;
    double ratio_height = ( double ) widget_height / ( double ) image_height;
    double ratio_constant = ratio_height < ratio_width ?
                           ratio_height : ratio_width;
    video_width = ( int ) ( image_width * ratio_constant + 0.5 );
    video_height = ( int ) ( image_height * ratio_constant + 0.5 );
    video_x = ( widget_width - video_width ) / 2;
    video_y = ( widget_height - video_height ) / 2;
    
    ENSURE (video_x >= 0 && video_x < widget_width);
    ENSURE (video_y >= 0 && video_y < widget_height);
    ENSURE (video_width <= widget_width);
    ENSURE (video_width <= widget_width);
  }
  
  
}} // namespace stage::output
