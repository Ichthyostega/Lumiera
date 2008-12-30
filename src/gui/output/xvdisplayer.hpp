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

/**
 * XvDisplayer is a class which is responsible for rendering  a video
 * image via XVideo.
 **/
class XvDisplayer : public Displayer
{
public:
  /**
   * Constructor
   * @param drawing_area The widget into which the video image will be
   * drawn. This value must not be NULL.
   * @param width The width of the video image in pixels. This value
   * must be greater than zero.
   * @param height The height of the video image in pixels. This value
   * must be greater than zero.
   **/
  XvDisplayer( Gtk::Widget *drawing_area, int width, int height );
  
  /**
   * Destructor
   **/
  ~XvDisplayer();

  /**
   * Put an image of a given width and height with the expected input
   * format (as indicated by the format method).
   * @param[in] image The video image array to draw.
   */
  void put( const void* image );

  /** 
   * Indicates if this object can be used to render images on the
   * running system.
   */
  bool usable();

private:

  /**
   * Specifies whether the object is currently attached to an XVideo
   * port.
   * @remarks This value is false until the constructor has finished
   * successfully.
   **/
  bool gotPort;
  
  /**
   * The current port being used.
   * @remarks This value is meaninless unless gotPort is true.
   **/
  unsigned int grabbedPort;
  
  /**
   * The widget that video will be drawn into.
   * @remarks This value must be a valid pointer.
   **/
  Gtk::Widget *drawingArea;
  
  /**
   * The display that video will be drawn into.
   **/
  Display *display;
  
  /**
   * The X11 window that video will be drawn into.
   **/
  Window window;
  
  /**
   * The graphics context which will be used when rednering video.
   **/
  GC gc;
  
  /**
   * The shared memory image object which video will be written into.
   **/
  XvImage *xvImage;
  
  /**
   * Info about the shared memory segment.
   * @remarks shmInfo.shmaddr is set to NULL, when the SHM is detached.
   **/
  XShmSegmentInfo shmInfo;
};

}   // namespace output
}   // namespace gui

#endif // XVDISPLAYER_HPP
