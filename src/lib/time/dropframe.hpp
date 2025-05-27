/*
  DROPFRAME.hpp  -  drop-frame timecode conversions

   Copyright (C)
     2010,            Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

/** @file dropframe.hpp
 ** Calculations to support mapping into NTSC drop frame timecode.
 ** This is a scheme for mapping frame numbers to a time display in SMPTE format,
 ** i.e. with hours, minutes, seconds and a frame count. It was introduced in USA
 ** at the time when colour television with the NTSC standard was introduced.
 ** Since the existing black and white television broadcast system based on analogue
 ** signal processing had initially be fixed to the same frequency as the power grid,
 ** which runs with 60 Hz in USA, the addition of the additional carriers for the
 ** colour information was confronted with problems of signal cross-talk, due to
 ** some overtones of the used carriers falling into the same frequency band.
 ** As a pragmatic workaround, the frame rate was reduced to 29.97 fps.
 ** 
 ** This fix of one problem however caused secondary problems for the production
 ** of content for television broadcast, since at this frame rate, one hour of
 ** content would not sum up cleanly to a simple number of frames. Since the
 ** purpose of time code is to label each frame with a time stamp, as a solution,
 ** the mapping of time stamps to frame numbers is manipulated:
 ** - every minute, there is one second which is mapped to 28 frames only
 ** - with the exception of every 10 minutes, where no such gap is applied
 ** This scheme adds up to allocating 108 frames less per hour, as would be used
 ** with a full frame rate of 30 fps. It should be noted though that applying such
 ** a »drop event« every minute does not mean to _omit actual content frames_ —
 ** rather it is the dime display in the time stamp which suddenly skips ahead by
 ** +2 frames every minute (but not every 10th minute).
 ** 
 ** @todo 2025 this header provides some calculation functions to implement such
 **       a mapping scheme (thereby documenting the know-how). But the project
 **       never got to the point of integrating this functionality into an
 **       actual timecode implementation. Doing so will be future work.
 **                                                       ///////////////////////////////////////////////////TICKET #751
 ** 
 ** @see time-dropframe-test.cpp
 ** @see timequant.hpp
 ** @see lib::time::Time
 ** @see TimeValue_test
 ** 
 */


#ifndef LUMIERA_TIME_DROPFRAME_H
#define LUMIERA_TIME_DROPFRAME_H

#include "lib/time/timevalue.hpp"



namespace lib {
namespace time {
  
  
  /**
   * Compute the consecutive frame number from a given time,
   * which is interpreted at the NTSC drop frame timecode grid.
   */
  int64_t calculate_ntsc_drop_frame_number (raw_time_64 time);
  
  
  /**
   * Build effective time from a NTSC drop frame timecode.
   * @warning take care not to specify time codes that are illegal NTSC drop-frame times.
   */
  raw_time_64 build_time_from_ntsc_drop_frame (uint frames, uint secs, uint mins, uint hours);
  
  
  
}} // lib::time
#endif /*LUMIERA_TIME_DROPFRAME_H*/
