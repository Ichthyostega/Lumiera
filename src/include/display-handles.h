/*
  DISPLAY-HANDLES.h  -  opaque handle types for playback and display

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file display-handles.h
 ** Opaque handles and similar typedefs used to communicate via the
 ** lumiera::Display and lumiera::DummyPlayer facade interfaces.
 **
 ** @see stage::DisplayService
 ** 
 */


#ifndef LUMIERA_DISPLAY_HANDLES_H
#define LUMIERA_DISPLAY_HANDLES_H




typedef unsigned char * LumieraDisplayFrame;


struct lumiera_displaySlot_struct 
  {
    void (*put_)(lumiera_displaySlot_struct*, LumieraDisplayFrame);
  };
typedef struct lumiera_displaySlot_struct lumiera_displaySlot;
typedef lumiera_displaySlot* LumieraDisplaySlot;



struct lumiera_playprocess_struct { };
typedef struct lumiera_playprocess_struct lumiera_playprocess;
typedef lumiera_playprocess* LumieraPlayProcess;


#endif
