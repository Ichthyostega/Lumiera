/*
  Quantiser  -  aligning time values to a time grid

   Copyright (C)
     2010,            Stefan Kangas <skangas@skangas.se>
     2010-2025,       Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file quantiser.cpp
 ** Implementation of support functions to create grid-aligned time and timecode values
 */


#include "lib/time/quantiser.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/util-quant.hpp"
#include "lib/rational.hpp"

using std::string;


namespace error = lumiera::error;

namespace lib {
namespace time {
  
  namespace { // implementation: basic frame quantisation....
    
    /**
     * Grid-align the raw time to a frame grid, which can be fractional.
     * @note for extreme values of framerate and framerate_divisor, the
     *       computation is approximative and will be µs-aligned.
     */
    inline int64_t
    calculate_quantisation (raw_time_64 time, uint64_t framerate, uint64_t framerate_divisor=1)
    {
      REQUIRE (framerate);
      REQUIRE (framerate_divisor);
      
      const uint64_t MAXRANGE = std::numeric_limits<raw_time_64>::max();
      const int64_t limit_num = MAXRANGE / framerate;
      const int64_t limit_den = MAXRANGE / framerate_divisor;
      const int64_t microScale {lib::time::TimeValue::SCALE};
      
      // protect against numeric overflow
      if (abs(time) < limit_num and microScale < limit_den)
        {
          // safe to calculate "time * framerate"
          return util::floordiv (time*int64_t(framerate), microScale*int64_t(framerate_divisor));
        }
      else
        {
          // direct calculation will overflow;
          // thus approximate the grid spacing by re-quantisation into µ-tick scale
          // (since in the end we will alias to the microScale anyway)...
          const uint64_t LIM{MAXRANGE/microScale};
          if (framerate > LIM or framerate_divisor > LIM)
            {
              framerate = util::reQuant (framerate,framerate_divisor, LIM);
              framerate_divisor = LIM;
            }
           // Note :  frameDuration ≡ microScale / (framerate/framerate_divisor)
          raw_time_64 frameDuration = util::reQuant (framerate_divisor,framerate, microScale);
          return util::floordiv (time,frameDuration);
        }
    }
  }
  
  /**
   * Quantise the given time into a fixed grid, relative to the origin.
   * The time grid used for quantisation is comprised of equally spaced intervals,
   * rooted at the given origin. The interval starting with the origin is numbered
   * as zero. Each interval includes its lower bound, but excludes its upper bound.
   * @param grid spacing of the grid intervals, measured in TimeValue::Scale (µ-ticks)
   * @return number of the grid interval containing the given time.
   */
  int64_t
  FixedFrameQuantiser::grid_aligned (TimeValue const& time, TimeValue const& grid)
  {
    return util::floordiv (_raw(time), _raw(grid));
  }
  
  
  
  PQuant
  getDefaultGridFallback()
  {
    static PQuant globalDefaultGrid (new FixedFrameQuantiser(1));
    return globalDefaultGrid;                                                    ///////////////////////TICKET #810
  };
  
  
  
  
  
  Grid::~Grid() { } // hint to emit the VTable here...
  
  
  /* Note: the ctor QuTime(TimeValue, Symbol) and the static function
   *       PQuant Quantiser::retrieve (Symbol) are defined in common-services.cpp
   *       To use this special convenience shortcuts, you need to link against liblumieracore.so
   *       This allows to use the Advice-system to retrieve grid definitions directly from the session
   */
  
  
  /**
   * build a quantised time value by explicitly specifying a
   * grid alignment facility and without any hidden reference
   * to the Lumiera session. This is mainly intended for
   * debugging and unit testing.
   */
  QuTime::QuTime (TimeValue raw, PQuant quantisation_to_use)
    : Time(raw)
    , quantiser_(quantisation_to_use)
    { }
  
  
  
  /** Create a quantiser based on a fixed constant spaced grid, rooted at the reference point
   *  as origin of the scale. Quantisation then means to determine the grid interval containing
   *  a given raw time value. Here, the grid interval number zero \em starts at the origin;
   *  each interval includes its lower bound and excludes its upper bound.*/
  FixedFrameQuantiser::FixedFrameQuantiser (FrameRate const& frames_per_second, TimeValue referencePoint)
    : origin_(referencePoint)
    , raster_(__ensure_nonzero(frames_per_second.duration()))
    { }
  
  FixedFrameQuantiser::FixedFrameQuantiser (Duration const& frame_duration,     TimeValue referencePoint)
    : origin_(referencePoint)
    , raster_(__ensure_nonzero(frame_duration))
    { }
  
  
  
  
  /** convenience shortcut: \em materialise a raw time value
   *  based on this grid or time axis, but returning a raw time value.
   *  Implemented as combination of the #gridPoint and #timeOf operations,
   *  i.e. we quantise into this scale, but transform the result back onto
   *  the global raw time value scale.
   * @warning this operation incurs information loss. Values may be rounded
   *          and / or clipped, according to the grid used. And, contrary to
   *          a QuTime value, the information about the actual grid is
   *          discarded. Please resist the temptation to abuse this
   *          operation to "just get a simple number", without
   *          understanding what quantisation means!
   */
  TimeValue
  Quantiser::materialise  (TimeValue const& raw)  const
  {
    return timeOf (gridPoint (raw));
  }
  
  
  
  /** grid quantisation (alignment).
   *  Determine the next lower grid interval start point,
   *  using a simple constant spaced time grid defined by
   *  origin and framerate stored within this quantiser.
   * @warning returned frame count might exceed the valid
   *      range when converting back into a TimeValue.
   * @see FixedFrameQuantiser::grid_aligned
   */
  FrameCnt
  FixedFrameQuantiser::gridPoint (TimeValue const& rawTime)  const
  {
    return grid_aligned (Offset{origin_, rawTime}, raster_);
  }
  
  
  /** transform into the local time scale grid aligned.
   *  The actual calculation first determines the number of the grid interval
   *  containing the given rawTime, followed by multiplying this interval number
   *  with the grid spacing.
   * @return time of the start point of the grid interval containing the rawTime,
   *         _relative to the origin_ of the time scale used by this quantiser.
   * @warning because the resulting value needs to be limited to fit into a 64bit long,
   *         the addressable time range can be considerably reduced. For example, if
   *         origin = Time::MIN, then all original time values above zero will be
   *         clipped, because the result, relative to origin, needs to be <= Time::MAX
   * @see FixedFrameQuantiser::grid_aligned
   */
  Offset
  FixedFrameQuantiser::gridLocal (TimeValue const& rawTime)  const
  {
    return raster_ * grid_aligned (Offset{origin_, rawTime}
                                  ,raster_);
  }
  
  
  /** calculate time value of a grid interval (frame) start point
   * @param gridPoint index number of the grid point (0 is at origin)
   * @return time point measured in Lumiera internal time
   * @warning returned time values are limited by the
   *      valid range of lumiera::Time
   */
  TimeValue
  FixedFrameQuantiser::timeOf (FrameCnt gridPoint)  const
  {
    return origin_ + gridPoint * raster_;
  }
  
  
  /** calculate time coordinates of a time spec relative to this quantised time scale
   * @param gridTime seconds relative to the origin of this scale
   * @param gridOffset additional offset in grid intervals (frames)
   * @return time point measured in Lumiera internal time
   * @warning returned time values are limited by the
   *      valid range of lumiera::Time
   */
  TimeValue
  FixedFrameQuantiser::timeOf (FSecs gridTime, int gridOffset)  const
  {
    Time gt(gridTime);
    TimeVar timePoint = gt + origin_;
    timePoint += gridOffset * Offset(raster_);
    return timePoint;
  }
  
  
  
  
  LUMIERA_ERROR_DEFINE (UNKNOWN_GRID, "referring to an undefined grid or scale in value quantisation");
  
  
  
  
  
  
  /* ===== NTSC drop-frame conversions ===== */

  namespace { // conversion parameters
  
    const uint FRAMES_PER_10min = 10*60 * 30000/1001;
    const uint FRAMES_PER_1min  =  1*60 * 30000/1001;
    const uint DISCREPANCY      = (1*60 * 30) - FRAMES_PER_1min;
  }
  
  
  /**
   * @remark This function reverses building the drop-frame timecode,
   *         and thus maps a time into consecutive frame numbers
   *         at NTSC framerate (i.e. without gaps)
   * @param  timecode represented as time value in µ-ticks
   * @return the absolute frame number as addressed by NTSC drop-frame
   * @todo 2011 I doubt this works correct for negative times!!
   */
  int64_t
  calculate_ntsc_drop_frame_number (raw_time_64 time)
  {
    int64_t frameNr = calculate_quantisation (time, 30000, 1001);
    
    // partition into 10 minute segments
    lldiv_t tenMinFrames = lldiv (frameNr, FRAMES_PER_10min);
    
    // ensure the drop-frame incidents happen at full minutes;
    // at start of each 10-minute segment *no* drop incident happens,
    // thus we need to correct discrepancy between nominal/real framerate once:
    int64_t remainingMinutes = (tenMinFrames.rem - DISCREPANCY) / FRAMES_PER_1min;
    
    int64_t dropIncidents = (10-1) * tenMinFrames.quot + remainingMinutes;
    return frameNr + 2*dropIncidents;
  }
  
  
  /**
   * @remark This is the mapping function to translate NTSC drop frame
   *         timecode specification into an actual time, with the necessary
   *         skip events every 1.-9. minute, thereby allocating 108 frames
   *         less per hour, than would be required for full 30 fps.
   * @return raw time value on a µ-tick scale
   */
  raw_time_64
  build_time_from_ntsc_drop_frame (uint frames, uint secs, uint mins, uint hours)
  {
    int64_t total_mins = 60 * hours + mins;
    int64_t total_frames  = 30*60*60 * hours
                          + 30*60 * mins
                          + 30 * secs
                          + frames
                          - 2 * (total_mins - total_mins / 10);
    raw_time_64 result = _raw (Offset{total_frames, FrameRate::NTSC});
    
    if (0 != result) // compensate for truncating down on conversion
      result += 1;  //  without this adjustment the frame number
    return result; //   would turn out off by -1 on back conversion
  }
  
}} // lib::time
