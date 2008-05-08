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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtkmm.h>
#include <gdk/gdkx.h>
#include <iostream>
using std::cerr;
using std::endl;

#include "xvdisplayer.hpp"

namespace lumiera {
namespace gui {
namespace output {

XvDisplayer::XvDisplayer( Gtk::Widget *drawing_area, int width, int height ) :
    xvImage( NULL )
{
  cerr << ">> Trying XVideo at " << width << "x" << height << endl;

  this->drawingArea = drawing_area;
  imageWidth = width;
  imageHeight = height;

  shmInfo.shmaddr = NULL;
  gotPort = false;

  Glib::RefPtr<Gdk::Window> area_window = drawing_area->get_window();

  window = gdk_x11_drawable_get_xid( area_window->gobj() );
  display = gdk_x11_drawable_get_xdisplay( area_window->gobj() );

  unsigned int  count;
  XvAdaptorInfo  *adaptorInfo;

  if ( XvQueryAdaptors( display, window, &count, &adaptorInfo ) == Success )
  {

    cerr << ">>> XvQueryAdaptors count: " << count << endl;
    for ( unsigned int n = 0; gotPort == false && n < count; ++n )
    {
      // Diagnostics
      cerr << ">>> Xv: " << adaptorInfo[ n ].name
      << ": ports " << adaptorInfo[ n ].base_id
      << " - " << adaptorInfo[ n ].base_id +
      adaptorInfo[ n ].num_ports - 1
      << endl;

      for ( port = adaptorInfo[ n ].base_id;
              port < adaptorInfo[ n ].base_id + adaptorInfo[ n ].num_ports;
              port ++ )
      {
        if ( XvGrabPort( display, port, CurrentTime ) == Success )
        {
          g_message("XvGrabPort == 0");
          int formats;
          XvImageFormatValues *list;

          list = XvListImageFormats( display, port, &formats );

          cerr << ">>> formats supported: " << formats << endl;

          for ( int i = 0; i < formats; i ++ )
          {
            fprintf( stderr, ">>>     0x%x (%c%c%c%c) %s\n",
                     list[ i ].id,
                     ( list[ i ].id ) & 0xff,
                     ( list[ i ].id >> 8 ) & 0xff,
                     ( list[ i ].id >> 16 ) & 0xff,
                     ( list[ i ].id >> 24 ) & 0xff,
                     ( list[ i ].format == XvPacked ) ? "packed" : "planar" );
            if ( list[ i ].id == 0x32595559 && !gotPort )
              gotPort = true;
          }

          if ( !gotPort )
          {
            XvUngrabPort( display, port, CurrentTime );
          }
          else
          {
            grabbedPort = port;
            break;
          }
        }
      }
    }

    if ( gotPort )
    {
      int num;
      unsigned int unum;
      XvEncodingInfo *enc;
      
      XvQueryEncodings( display, grabbedPort, &unum, &enc );
      for ( unsigned int index = 0; index < unum; index ++ )
      {
        fprintf( stderr, ">>> %d: %s, %ldx%ld rate = %d/%d\n", index, enc->name,
                enc->width, enc->height, enc->rate.numerator,
                enc->rate.denominator );
      }
      
      XvAttribute *xvattr = XvQueryPortAttributes( display, port, &num );
      for ( int k = 0; k < num; k++ )
      {
        if ( xvattr[k].flags & XvSettable ) 
        {
          if ( strcmp( xvattr[k].name, "XV_AUTOPAINT_COLORKEY") == 0 )
          {
            Atom val_atom = XInternAtom( display, xvattr[k].name, False );
            if ( XvSetPortAttribute( display, port, val_atom, 1 ) != Success )
              fprintf(stderr, "Couldn't set Xv attribute %s\n", xvattr[k].name);
          }
          else if (  strcmp( xvattr[k].name, "XV_COLORKEY") == 0 )
          {
            Atom val_atom = XInternAtom( display, xvattr[k].name, False );
            if ( XvSetPortAttribute( display, port, val_atom, 0x010102 ) != Success )
              fprintf(stderr, "Couldn't set Xv attribute %s\n", xvattr[k].name);
          }
        }
      }
    }

    if ( gotPort )
    {
      gc = XCreateGC( display, window, 0, &values );

      xvImage = ( XvImage * ) XvShmCreateImage( display, port, 0x32595559, 0, width, height, &shmInfo );

      shmInfo.shmid = shmget( IPC_PRIVATE, xvImage->data_size, IPC_CREAT | 0777 );
      if (shmInfo.shmid < 0) {
        perror("shmget");
        gotPort = false;
      } else {
        shmInfo.shmaddr = ( char * ) shmat( shmInfo.shmid, 0, 0 );
        xvImage->data = shmInfo.shmaddr;
        shmInfo.readOnly = 0;
        if ( !XShmAttach( gdk_display, &shmInfo ) )
        {
          gotPort = false;
        }
        XSync( display, false );
        shmctl( shmInfo.shmid, IPC_RMID, 0 );
#if 0
        xvImage = ( XvImage * ) XvCreateImage( display, port, 0x32595559, pix, width , height );
#endif
      }
    }
  }
  else
  {
    gotPort = false;
  }
}

XvDisplayer::~XvDisplayer()
{
  cerr << ">> Destroying XV Displayer" << endl;

  if ( gotPort )
  {
    XvUngrabPort( display, grabbedPort, CurrentTime );
  }

  if ( xvImage != NULL )
    XvStopVideo( display, port, window );

  if ( shmInfo.shmaddr != NULL )
  {
    XShmDetach( display, &shmInfo );
    shmctl( shmInfo.shmid, IPC_RMID, 0 );
    shmdt( shmInfo.shmaddr );
  }
  if ( xvImage != NULL )
    XFree( xvImage );
}

bool
XvDisplayer::usable()
{
  return gotPort;
}

void
XvDisplayer::put( void *image )
{
  g_assert(drawingArea != NULL);

  if(xvImage != NULL)
  {
    int video_x = 0, video_y = 0, video_width = 0, video_height = 0;
    CalculateVideoLayout(
      drawingArea->get_width(),
      drawingArea->get_height(),
      preferredWidth(), preferredHeight(),
      video_x, video_y, video_width, video_height );

    memcpy( xvImage->data, image, xvImage->data_size );

    XvShmPutImage( display, port, window, gc, xvImage,
                   0, 0, preferredWidth(), preferredHeight(),
                   video_x, video_y, video_width, video_height, false );
  }
}

}   // namespace output
}   // namespace gui
}   // namespace lumiera
