/*
  XvDisplayer  -  XVideo display

   Copyright (C)
     2000,            Arne Schirmacher <arne@schirmacher.de>
     2001-2007,       Dan Dennedy <dan@dennedy.org>
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file xv-displayer.cpp
 ** Implementation of video output via XVideo
 ** @todo WIP as of 5/2025 -- attempt to port this component to GTK-3 ///////////////////////////////////////TICKET #1403
 */


#include "stage/gtk-base.hpp"
#include "stage/output/xv-displayer.hpp"
#include "include/logging.h"
//#include "lib/format-cout.hpp"

#include <gdk/gdkx.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace stage {
namespace output {
  
  namespace {
    const uint32_t FORMAT_ID_YUY2 = 0x32595559;
  }
  
  XvDisplayer::XvDisplayer(Gtk::Widget& drawing_area
                          ,uint width, uint height)
    : Displayer{width,height}
    , gotPort{false}
    , drawingArea_{drawing_area}
    , xvImage{nullptr}
    {
      REQUIRE (videoWidth > 0);
      REQUIRE (videoHeight > 0);
      
      INFO(stage, "Trying XVideo at %d x %d", videoWidth, videoHeight);
    
      shmInfo.shmaddr = NULL;
    
      Glib::RefPtr<Gdk::Window> area_window = drawingArea_.get_window();
    
      window = GDK_WINDOW_XID (area_window->gobj());
      display = GDK_WINDOW_XDISPLAY (area_window->gobj());
    
      uint  count;
      XvAdaptorInfo* adaptorInfo;
    
      if (XvQueryAdaptors (display, window, &count, &adaptorInfo) == Success)
        {
          INFO(stage, "XvQueryAdaptors count: %d", count);
          for (uint n = 0; gotPort == false and n < count; ++n )
            {
              // Diagnostics
              INFO(stage, "%s, %lu, %lu", adaptorInfo[ n ].name,
                adaptorInfo[ n ].base_id, adaptorInfo[ n ].num_ports - 1);
    
              for (uint port = adaptorInfo[ n ].base_id;
                      port < adaptorInfo[ n ].base_id + adaptorInfo[ n ].num_ports;
                      port ++ )
                {
                  if ( XvGrabPort( display, port, CurrentTime ) == Success )
                    {
                      int formats;
                      XvImageFormatValues* list;
    
                      list = XvListImageFormats( display, port, &formats );
    
                      INFO(stage, "formats supported: %d", formats);
    
                      for ( int i = 0; i < formats; i ++ )
                        {
                          INFO(stage, "0x%x (%c%c%c%c) %s",
                                   list[ i ].id,
                                   ( list[ i ].id ) & 0xff,
                                   ( list[ i ].id >> 8 ) & 0xff,
                                   ( list[ i ].id >> 16 ) & 0xff,
                                   ( list[ i ].id >> 24 ) & 0xff,
                                   ( list[ i ].format == XvPacked ) ? "packed" : "planar" );
                          if ( list[ i ].id == FORMAT_ID_YUY2 and not gotPort )
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
              uint unum;
              XvEncodingInfo* enc;
              
              XvQueryEncodings( display, grabbedPort, &unum, &enc );
              for (uint index = 0; index < unum; ++index )
                {
                  INFO (stage, "%d: %s, %ldx%ld rate = %d/%d"
                             , index, enc->name
                             , enc->width, enc->height
                             , enc->rate.numerator
                             , enc->rate.denominator);
                }
              
              XvAttribute* xvattr = XvQueryPortAttributes (display, grabbedPort, &num);
              for (int k = 0; k < num; k++ )
                {
                  if ( xvattr[k].flags & XvSettable )
                    {
                      if (strcmp (xvattr[k].name, "XV_AUTOPAINT_COLORKEY") == 0 )
                        {
                          Atom val_atom = XInternAtom( display, xvattr[k].name, False );
                          if (XvSetPortAttribute(display, grabbedPort, val_atom, 1 ) != Success )
                            NOBUG_ERROR(stage, "Couldn't set Xv attribute %s\n", xvattr[k].name);
                        }
                      else
                      if (strcmp (xvattr[k].name, "XV_COLORKEY") == 0 )
                        {
                          Atom val_atom = XInternAtom( display, xvattr[k].name, False );
                          if ( XvSetPortAttribute( display, grabbedPort, val_atom, 0x010102 ) != Success )
                            NOBUG_ERROR(stage, "Couldn't set Xv attribute %s\n", xvattr[k].name);
                        }
                    }
                }
            }
          
          if (gotPort)
            {
//            XGCValues values;
//            memset(&values, 0, sizeof(XGCValues));
///////////////////////////////////////////////////////////////TODO actually pass these to XCreateGC to set line width or fill colour etc.              
              gc = XCreateGC( display, window, 0, NULL );
              
              xvImage = ( XvImage * ) XvShmCreateImage( display, grabbedPort, FORMAT_ID_YUY2, 0, videoWidth, videoHeight, &shmInfo );
              
              shmInfo.shmid = shmget( IPC_PRIVATE, xvImage->data_size, IPC_CREAT | 0777 );
              if (shmInfo.shmid < 0) {
                  perror("shmget");
                  gotPort = false;
                }
              else
                {
                  shmInfo.shmaddr = (char*) shmat (shmInfo.shmid, 0, 0);
                  xvImage->data = shmInfo.shmaddr;
                  shmInfo.readOnly = 0;
                  
                  if (!XShmAttach (display, &shmInfo))
                  {
                    gotPort = false;
                  }
                  
                  XSync( display, false );
                  shmctl( shmInfo.shmid, IPC_RMID, 0 );   // mark the segment as deleted
                }                                        //   -- it will be retained until the last client calls shmdt()
            }
        }
      else
        {
          gotPort = false;
        }
      if (not gotPort)
        ERROR (stage, "unable to use XVideo for display.");
    }
  
  
  XvDisplayer::~XvDisplayer()
  {
    NOBUG_ERROR(stage, "Destroying XV Displayer");
    
    if ( gotPort )
      {
        XvUngrabPort( display, grabbedPort, CurrentTime );
      }
    
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
  XvDisplayer::put (void* const image)
  {
    REQUIRE (image != NULL);
    
    if (xvImage != NULL)
      {
        REQUIRE (display != NULL);
        REQUIRE (drawingArea_.get_mapped());
        
        int org_x = 0, org_y = 0, destW = 0, destH = 0;
        calculateVideoLayout(
          drawingArea_.get_width(),
          drawingArea_.get_height(),
          org_x, org_y, destW, destH );
        
        auto spaceAlloc = drawingArea_.get_allocation();
        org_x += spaceAlloc.get_x();
        org_y += spaceAlloc.get_y();
        
        memcpy (xvImage->data, image, xvImage->data_size);
        
        XvShmPutImage (display, grabbedPort, window, gc, xvImage,
                       0, 0, videoWidth, videoHeight,
                       org_x, org_y, destW, destH, false);
        XFlush (display);
      }
  }
  
  
}}   // namespace stage::output
