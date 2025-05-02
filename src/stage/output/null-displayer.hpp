/*
  NULL-DISPLAYER.hpp  -  fallback video displayer to not display video at all

   Copyright (C)
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file null-displayer.hpp
 ** Passive deactivated video displayer.
 ** 
 ** @deprecated obsolete since GTK-3
 ** @todo WIP as of 5/2025 attempt to accommodate to GTK-3   ////////////////////////////////////////////////TICKET #1403
 ** @see displayer.hpp
 */

#ifndef STAGE_OUTPUT_NULL_DISPLAYER_H
#define STAGE_OUTPUT_NULL_DISPLAYER_H

#include "stage/gtk-base.hpp"
#include "stage/output/displayer.hpp"

namespace Gtk {
  class Widget;
}

namespace stage {
namespace output {

/**
 * NullDisplayer implements the Displayer interface without any actual display.
 *
 * @todo the GdkDisplayer class is not supported anymore in Gtk3.
 *       This is due to Gtk3 only supporting drawing with Cairo
 *       ////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
 * @todo WIP as of 5/2025 attempt to accommodate to GTK-3   /////////////////////////////////////////////////TICKET #1403
 */
class NullDisplayer
  : public Displayer
  {
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
    NullDisplayer (Gtk::Widget* drawing_area, int width, int height );
    
    /**
     * Put an image of a given width and height with the expected input
     * format (as indicated by the format method).
     * @param[in] image The video image array to draw.
     */
    void put (void* const image);
    
  protected:
    
    /** 
     * Indicates if this object can be used to render images on the
     * running system.
     */
    bool usable();
    
  private:
    
    /**
     * The widget that video will be drawn into.
     * @remarks This value must be a valid pointer.
     */
    Gtk::Widget* drawingArea;
  };
  
  
  
}}   // namespace stage::output
#endif /*STAGE_OUTPUT_NULL_DISPLAYER_H*/
