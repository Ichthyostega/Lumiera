/*
  DISPLAY-HANDLES.hpp  -  opaque handle types for playback and display

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
 ** @deprecated this is part of prototyping code; as of 5/2025 it is clear
 **   that we will not use any of these interface schemes (yet something similar)
 **
 ** @see stage::DisplayService
 ** 
 */


#ifndef LUMIERA_DISPLAY_HANDLES_H
#define LUMIERA_DISPLAY_HANDLES_H

#include <cstddef>


using DummyFrame = std::byte *;







namespace lumiera {

  /** Supported Displayer formats */
  enum DisplayerInput {
      DISPLAY_NONE,
      DISPLAY_YUV,
      DISPLAY_RGB,
      DISPLAY_BGR,
      DISPLAY_BGR0,
      DISPLAY_RGB16
    };
} // namespace lumiera
#endif /*LUMIERA_DISPLAY_HANDLES_H*/
