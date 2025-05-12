/*
  XV-DISPLAYER.hpp  -  XVideo display

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file xv-displayer.hpp
 ** Implementation of video output via XVideo
 ** @todo WIP as of 5/2025 -- attempt to port this component to GTK-3 ///////////////////////////////////////TICKET #1403
 ** @see displayer.hpp
 */


#ifndef STAGE_OUTPUT_XV_DISPLAYER_H
#define STAGE_OUTPUT_XV_DISPLAYER_H


#include "stage/output/displayer.hpp"
#include "stage/gtk-base.hpp"

#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>


namespace Gtk {
  class Widget;
}

namespace stage {
namespace output {
  
  /**
   * XvDisplayer is a class which is responsible for rendering  a video
   * image via XVideo.
   */
  class XvDisplayer 
    : public Displayer
    {
    public:
      /**
       * Constructor
       * @param drawing_area The widget into which the video image will be drawn.
       * @param width  of the video image image to be displayed, in pixels.
       * @param height of the video image in pixels to be displayed.
       */
      XvDisplayer (Gtk::Widget& drawing_area, uint width, uint height);
      
     ~XvDisplayer();
      
      
    private:
      /**
       * Put an image of a given width and height with the expected input
       * format (as indicated by the format method).
       * @param[in] image The video image array to draw.
       */
      void put (void* const image)  override;
      
      /** Indicates if this object can be used to render images on the running system. */
      bool usable()  override;
      
      DisplayerInput format() override
        {
          return lumiera::DISPLAY_YUV;
        }
      
      
      /**
       * Specifies whether the object is currently attached to an XVideo port.
       * @remarks This value is false until the constructor has finished successfully.
       */
      bool gotPort;
      
      /**
       * The current port being used.
       * @remarks This value is meaningless unless gotPort is true.
       */
      uint grabbedPort;
      
      /**
       * The widget that video will be drawn into.
       * @remarks This value must be a valid pointer.
       */
      Gtk::Widget& drawingArea_;
      
      /**
       * The display that video will be drawn into.
       */
      Display* display;
      
      /**
       * The X11 window that video will be drawn into.
       */
      Window window;
      
      /**
       * The graphics context which will be used when rendering video.
       */
      GC gc;
      
      /**
       * The shared memory image object which video will be written into.
       */
      XvImage* xvImage;
      
      /**
       * Info about the shared memory segment.
       * @remarks shmInfo.shmaddr is set to NULL, when the SHM is detached.
       */
      XShmSegmentInfo shmInfo;
    };
  
  
}}   // namespace stage::output
#endif /*STAGE_OUTPUT_XV_DISPLAYER_H*/
