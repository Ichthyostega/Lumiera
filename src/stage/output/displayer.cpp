/*
  Displayer  -  base class for displaying video

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file displayer.cpp
 ** Implementation of a displayer object, intended for creating
 ** a video display in the UI. This class was created as part of
 ** an initial draft of the user interface.
 ** @todo as of 2016 it is not clear, if this code will be
 **          evolved into the actual display facility, or be
 **          replaced and rewritten, when we're about to
 **          create a functional video display connected
 **          to the render engine.
 ** @todo 5/2025 used in an experiment for video output
 */


#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"

#include <cmath>

namespace stage {
namespace output {
  
  DisplayerInput
  Displayer::format()
  {
    return DISPLAY_NONE;
  }
  
  void
  Displayer::calculateVideoLayout(
          int widgetWidth, int widgetHeight,
          int &imgOrg_x, int &imgOrg_y, int &imgWidth, int &imgHeight )
  {
    REQUIRE (0 < widgetWidth );
    REQUIRE (0 < widgetHeight);
    REQUIRE (0 < videoWidth  );
    REQUIRE (0 < videoHeight );

    auto ratioW = double(widgetWidth ) / videoWidth;
    auto ratioH = double(widgetHeight) / videoHeight;
    auto scale  = std::min (ratioW, ratioH);
    imgWidth  = std::lround (scale * videoWidth);
    imgHeight = std::lround (scale * videoHeight);
    imgOrg_x = (widgetWidth - imgWidth)   / 2;
    imgOrg_y = (widgetHeight - imgHeight) / 2;
    
    ENSURE (imgWidth <= widgetWidth);
    ENSURE (imgWidth <= widgetWidth);
    ENSURE (0 <= imgOrg_x and imgOrg_x < widgetWidth);
    ENSURE (0 <= imgOrg_y and imgOrg_y < widgetHeight);
  }
  
  
}} // namespace stage::output
