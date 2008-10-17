/*
  xvdisplayer.hpp  -  Defines the base class for XVideo display
 
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
/** @file xvdisplayer.hpp
 ** This file contains the definition of XvDisplayer, the XVideo
 ** video output implementation
 ** @see xvdisplayer.cpp
 ** @see displayer.hpp
 */

#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>

#include "displayer.hpp"

#ifndef XVDISPLAYER_HPP
#define XVDISPLAYER_HPP

namespace Gtk {
  class Widget;
}

namespace gui {
namespace output {

  class XvDisplayer : public Displayer
  {
  public:
    XvDisplayer( Gtk::Widget *drawing_area, int width, int height );
    ~XvDisplayer();

    void put( void *image );
  
  protected:
    bool usable();

  private:
	  bool gotPort;
	  int grabbedPort;
	  Gtk::Widget *drawingArea;
	  Display *display;
	  Window window;
	  GC gc;
	  XGCValues values;
	  XvImage *xvImage;
	  unsigned int port;
	  XShmSegmentInfo shmInfo;
	  char pix[ MAX_WIDTH * MAX_HEIGHT * 4 ];
  };

}   // namespace output
}   // namespace gui

#endif // XVDISPLAYER_HPP
