/*
  displayer.hpp  -  Defines the base class for displaying video
 
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
/** @file displayer.hpp
 ** This file contains the defintion of Displayer; the base class
 ** of all video display implementations
 ** @see displayer.cpp
 */

#ifndef DISPLAYER_HPP
#define DISPLAYER_HPP

namespace lumiera {
namespace gui {
namespace output {

  /** Supported Displayer formats
  */

  typedef enum {
      DISPLAY_NONE,
      DISPLAY_YUV,
      DISPLAY_RGB,
      DISPLAY_BGR,
      DISPLAY_BGR0,
      DISPLAY_RGB16
  }
  DisplayerInput;

  /**	A Displayer is a class which is responsible for rendering an image in some
	  way (ie: Xvideo, GDK, OpenGL etc).
   
	  All Displayer classes must extend the Displayer class and minimally rewrite:
	
	  + usable() - to indicate if the object can be used, 
	  + format() - to indicate what type of input the put method expects
	  + put( void * ) - deal with an image of the expected type and size
   
	  By default, all images will be delivered to the put method in a resolution
	  of IMG_WIDTH * IMG_HEIGHT. If another size is required, then the rewrite
	  the methods:
   
	  + preferredWidth 
	  + preferredHeight
   
	  If the widget being written to doesn't need a fixed size, then rewrite
	  the two other put methods as required.
  */
  class Displayer
  {
  public:

    /** Indicates the format required by the abstract put method.
    */
    virtual DisplayerInput format();

    /** Expected width of input to put.
    */
	  virtual int preferredWidth();

    /** Expected height of input to put.
    */
	  virtual int preferredHeight();

    /** Put an image of a given width and height with the expected input
	    format (as indicated by the format method).
     
	    \param image	image of correct format and specified width/height
	    \param width	width of image
	    \param height	height of image
    */
	  virtual void put( void *image, int width, int height );

    virtual bool usable();
  
  protected:
	  int imageWidth;
	  int imageHeight;

	  virtual void put( void * ) = 0;
  };

}   // namespace output
}   // namespace gui
}   // namespace lumiera

#endif // DISPLAYER_HPP
