/*
  DISPLAYER.hpp  -  base class for displaying video

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file displayer.hpp
 ** The Displayer serves as base of all video display implementations
 ** @warning as of 2016 it is not clear, if this code will be
 **          evolved into the actual display facility, or be
 **          replaced and rewritten, when we're about to 
 **          create a functional video display connected
 **          to the render engine. 
 */


#ifndef STAGE_OUTPUT_DISPLAYER_H
#define STAGE_OUTPUT_DISPLAYER_H


namespace stage {
namespace output {
  
  /** Supported Displayer formats */
  enum DisplayerInput {
      DISPLAY_NONE,
      DISPLAY_YUV,
      DISPLAY_RGB,
      DISPLAY_BGR,
      DISPLAY_BGR0,
      DISPLAY_RGB16
    };
  
  
  /**
   * A Displayer is a class which is responsible for rendering an image
   * in some way (ie: Xvideo, GDK, OpenGL etc).
   *
   * @remarks All Displayer classes must extend the Displayer class and
   * minimally rewrite:
   *
   * + usable() - to indicate if the object can be used, 
   * + format() - to indicate what type of input the put method expects
   * + put( void * ) - deal with an image of the expected type and size
   *
   * By default, all images will be delivered to the put method in a
   * resolution of IMG_WIDTH * IMG_HEIGHT. If another size is required,
   * then the rewrite the methods:
   *
   * + preferredWidth 
   * + preferredHeight
   *
   * If the widget being written to doesn't need a fixed size, then
   * rewrite the two other put methods as required.
   */
  class Displayer
    {
    protected:
      int imageWidth;
      int imageHeight;
      
    public:
      virtual ~Displayer() { }
      
      
      /** Indicates if this object can be used to render images on the running system. */
      virtual bool usable();
      
      /** Indicates the format required by the abstract put method. */
      virtual DisplayerInput format();
      
      /** Expected width of input to put. */
      virtual int preferredWidth();
      
      /** Expected height of input to put. */
      virtual int preferredHeight();
      
      /**
       * Put an image of a given width and height with the expected input
       * format (as indicated by the format method).
       */
      virtual void put (void* const)  =0;
      
      
    protected:
      /**
       * Calculates the coordinates for placing a video image inside a widget
       * 
       * @param[in] widget_width   The width of the display widget.
       * @param[in] widget_height  The height of the display widget.
       * @param[in] image_width    The width of the video image.
       * @param[in] image_height   The height of the video image.
       * @param[out] video_x       The x-coordinate of the top left
       *                           corner of the scaled video image.
       * @param[out] video_y       The y-coordinate of the top left
       *                           corner of the scaled video image.
       * @param[out] video_width   The width of the scale video image.
       * @param[out] video_height  The height of the scale video image.
       */
      static void calculateVideoLayout(
          int widget_width, int widget_height,
          int image_width, int image_height,
          int &video_x, int &video_y, int &video_width, int &video_height );
    };
    
    
  
}} // namespace stage::output
#endif /*STAGE_OUTPUT_DISPLAYER_H*/
