/*
  PIXBUF-DISPLAYER.hpp  -  displaying video via bitmap image

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file pixbuf-displayer.hpp
 ** Display video as bitmap image with the UI toolkit.
 ** 
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 ** @see displayer.hpp
 */

#ifndef STAGE_OUTPUT_PIXBUF_DISPLAYER_H
#define STAGE_OUTPUT_PIXBUF_DISPLAYER_H

#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"

namespace Gtk {
  class Widget;
}

namespace stage {
namespace output {

/**
 * PixbufDisplayer is a class which is responsible for rendering a video
 * image via GDK.
 *
 * @todo WIP as of 5/2025 attempt to accommodate to GTK-3   /////////////////////////////////////////////////TICKET #1403
 */
class PixbufDisplayer
  : public Displayer
  {
    Gtk::Image& drawingArea_;
    
  public:
    
    /**
     * Constructor
     * @param[in] drawing_area The widget into which the video image will
     * be drawn. This value must not be NULL.
     * @param[in] width The width of the video image in pixels. This value
     * must be greater than zero.
     * @param[in] height The height of the video image in pixels. This
     * value must be greater than zero.
     */
    PixbufDisplayer (Gtk::Image& drawing_area, uint width, uint height );
    
  private:
    /**
     * Put an image of a given width and height with the expected input
     * format (as indicated by the format method).
     * @param[in] image The video image array to draw.
     */
    void put (void* const image);
    
    bool usable()  override;
    
    DisplayerInput format() override
      {
        return lumiera::DISPLAY_RGB;
      }
  };
  
  
  
}}   // namespace stage::output
#endif /*STAGE_OUTPUT_PIXBUF_DISPLAYER_H*/
