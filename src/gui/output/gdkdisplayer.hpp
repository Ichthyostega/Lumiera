/*
  gdkdisplayer.hpp  -  Defines the class for displaying video via GDK
 
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
 
*/
/** @file gdkdisplayer.hpp
 ** This file contains the definition of XvDisplayer, the XVideo
 ** video output implementation
 ** @see gdkdisplayer.cpp
 ** @see displayer.hpp
 */

#include "displayer.hpp"

#ifndef GDKDISPLAYER_HPP
#define GDKDISPLAYER_HPP

namespace Gtk {
  class Widget;
}

namespace lumiera {
namespace gui {
namespace output {

  class GdkDisplayer : public Displayer
  {
  public:
    GdkDisplayer( Gtk::Widget *drawing_area, int width, int height );

    void put( void *image );
  
  protected:
    bool usable();

  private:
    Gtk::Widget *drawingArea;
  };

}   // namespace output
}   // namespace gui
}   // namespace lumiera

#endif // GDKDISPLAYER_HPP
