/*
  displayer.cpp  -  Implements the base class for displaying video
 
  Copyright (C)         Lumiera.org
    2000,               Arne Schirmacher <arne@schirmacher.de>
    2001-2007,          Dan Dennedy <dan@dennedy.org>
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include "displayer.hpp"

namespace lumiera {
namespace gui {
namespace output {



  DisplayerInput Displayer::format()
  {
	  return DISPLAY_NONE;
  }

  int Displayer::preferredWidth()
  {
	  return imageWidth;
  }

  int Displayer::preferredHeight()
  {
	  return imageHeight;
  }

  void Displayer::put( void *image, int width, int height )
  {
	  if ( width == preferredWidth() && height == preferredHeight() )
	  {
		  put( image );
	  }
	  else
	  {
		  reformat( format(), format(), image, width, height );
		  put( pixels );
	  }
  }



}   // namespace output
}   // namespace gui
}   // namespace lumiera
