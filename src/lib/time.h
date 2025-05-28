/*
  time.h  -  Utilities for handling time

   Copyright (C)
     2008,            Christian Thaeter <ct@pipapo.org>
     2010,            Stefan Kangas <skangas@skangas.se>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.
*/

/** @file time.h
 ** Common functions for handling of time values.
 ** Working with time values in sexagesimal format, quantising time and converting
 ** to/from common timecode formats can be tricky to get right. Thus the goal is
 ** to concentrate the actual bits of math for these operations into a small number
 ** of library functions, which are easy to test thoroughly in isolation.
 ** 
 ** Built on top of that, the actual time handling in the GUI and within the Lumiera
 ** session is mostly confined to use the opaque lib::time::Time wrapper objects.
 ** When time values actually need to be _quantised_ (aligned to a frame grid),
 ** this is expressed at the API through using the lib::time::QuTime type, which
 ** then in turn can be materialised into a number of _timecode formats_.
 ** These definitions ensure that whenever an actual quantisation (rounding)
 ** operation is performed, the link to the appropriate time grid is available,
 ** so that multiple output or rendering operations can use differing time origins
 ** and frame rates simultaneously on the same model.
 ** 
 ** The Lumiera Vault Layer functions mostly operate on raw frame counts, which in
 ** this model are defined to be a special kind of timecode, and thus dependent on
 ** a preceding time quantisation.
 ** 
 ** @deprecated 2025 this should not be a "simple" C library set aside from the Lumiera
 **             time handling framework, rather it should be clarified that these are
 **             implementation helpers and must not be used by any application code.
 **             It should be checked which of these functions actually need to be
 **             exposed through an interface header, since these are typically
 **             used to implement parts of the time handling framework.
 ** 
 ** @see lib::time::Time
 ** @see timequant.hpp
 ** @see TimeValue_test
 ** 
 */


#ifndef LUMIERA_TIME_H
#define LUMIERA_TIME_H

#include <inttypes.h>

#ifdef __cplusplus /*=================== C++ facilities ===================== */
#include "lib/time/timevalue.hpp"

using lib::time::raw_time_64;


/**
 * Converts a fraction of seconds to Lumiera's internal opaque time scale.
 * @param fractionalSeconds given as rational number
 * @note inconsistent with Lumiera's general quantisation behaviour,
 *       here negative fractional micro-ticks are truncated towards zero.
 *       This was deemed irrelevant in practice.
 */
raw_time_64
lumiera_rational_to_time (lib::time::FSecs const& fractionalSeconds);


/**
 * Converts a frame count into Lumiera's internal time scale.
 * based on a framerate given as rational number (e.g. NTSC)
 * @note handles only positive frame counts and assumes the
 *       origin to be at zero.
 */
raw_time_64
lumiera_framecount_to_time (uint64_t frameCount, lib::time::FrameRate const& fps);





extern "C" {    /* ===================== C interface ======================== */
#endif


/**
 * Build a time value by summing up the given components.
 * @param millis number of milliseconds
 * @param secs number of seconds
 * @param mins number of minutes
 * @param hours number of hours
 */
raw_time_64
lumiera_build_time (long millis, uint secs, uint mins, uint hours);




#ifdef __cplusplus
}//extern "C"
#endif
#endif

