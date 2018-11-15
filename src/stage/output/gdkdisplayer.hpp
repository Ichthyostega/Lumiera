/*
  GDKDISPLAYER.hpp  -  displaying video via GDK

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

*/


/** @file gdkdisplayer.hpp
 ** Display video via GDK
 ** 
 ** @deprecated obsolete since GTK-3
 ** @see displayer.hpp
 */

#ifndef GUI_OUTPUT_GDKDISPLAYER_H
#define GUI_OUTPUT_GDKDISPLAYER_H

#include "gui/gtk-base.hpp"
#include "gui/output/displayer.hpp"

namespace Gtk {
  class Widget;
}

namespace gui {
namespace output {

/**
 * GdkDisplayer is a class which is responsible for rendering a video
 * image via GDK.
 *
 * @todo the GdkDisplayer class is not supported anymore in Gtk3.
 *       This is due to Gtk3 only supporting drawing with Cairo
 *       /////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #950 : new solution for video display
 */
class GdkDisplayer
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
    GdkDisplayer( Gtk::Widget *drawing_area, int width, int height );
    
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
    Gtk::Widget *drawingArea;
  };
  
  
  
}}   // namespace gui::output
#endif /*GUI_OUTPUT_GDKDISPLAYER_H*/
