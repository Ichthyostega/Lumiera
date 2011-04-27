/*
  Quantiser  -  aligning time values to a time grid

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


#include "lib/time/quantiser.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time.h"
#include "lib/advice.hpp"

#include <boost/rational.hpp>

using boost::rational_cast;
using std::string;


namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  namespace { // implementation helpers...
    
    PQuant
    retrieveQuantiser (Symbol gridID)
    {
      advice::Request<PQuant> query(gridID);
      PQuant grid_found = query.getAdvice();
      if (!grid_found)
        throw error::Logic ("unable to fetch the quantisation grid -- is it already defined?"
                           , LUMIERA_ERROR_UNKNOWN_GRID);
      return grid_found;
    }
    
  }//(End) implementation helpers
  
  
  
  Grid::~Grid() { } // hint to emit the VTable here...
  
  
  /** 
   * build a quantised time value, referring the time grid by-name.
   * This is the preferred standard way of establishing a quantisation,
   * but it requires an existing time scale defined in the Lumiera Session,
   * as TimeGrid (meta asset). Usually, such a time scale gets built based
   * on the format and parameters of an output bus.
   */
  QuTime::QuTime (TimeValue raw, Symbol gridID)
    : Time(raw)
    , quantiser_(retrieveQuantiser (gridID))
    { }
  
  
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
  
  
  
  /** Access an existing grid definition or quantiser, known by the given symbolic ID.
   *  Typically this fetches a meta::TimeGrid (asset) from the session.
   * @throw error::Logic if the given gridID wasn't registered
   * @return smart-ptr to the quantiser instance */
  PQuant
  Quantiser::retrieve (Symbol gridID)
  {
    return retrieveQuantiser (gridID);
  }
  
  
  /** alignment to a simple fixed size grid.
   *  The actual calculation first determines the number
   *  of the grid interval containing the given rawTime,
   *  then followed by multiplying this interval number
   *  with the grid spacing.
   * @return time of the start point of the grid interval
   *      containing the rawTime, relative to the origin
   *      of the time scale used by this quantiser.
   * @warning returned time values are limited by the
   *      valid range of lumiera::Time  
   * @see #lumiera_quantise_time
   */
  TimeValue
  FixedFrameQuantiser::gridAlign (TimeValue const& rawTime)  const
  {
    return TimeValue (lumiera_quantise_time (_raw(rawTime), _raw(origin_), _raw(raster_)));
  }
  
  
  /** grid quantisation (alignment).
   *  Determine the next lower grid interval start point,
   *  using a simple constant spaced time grid defined by
   *  origin and framerate stored within this quantiser.
   * @warning returned frame count might exceed the valid
   *      range when converting back into a TimeValue.
   * @see #lumiera_quantise_frames
   */
  long
  FixedFrameQuantiser::gridPoint (TimeValue const& rawTime)  const
  {
    return lumiera_quantise_frames (_raw(rawTime), _raw(origin_), _raw(raster_));
  }
  
  
  /** calculate time value of a grid interval (frame) start point
   * @return time point measured in Lumiera internal time 
   * @warning returned time values are limited by the
   *      valid range of lumiera::Time
   */
  TimeValue
  FixedFrameQuantiser::timeOf (long gridPoint)  const
  {
    return TimeValue (lumiera_time_of_gridpoint (gridPoint, _raw(origin_), _raw(raster_)));
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
    timePoint += gridOffset * raster_;
    return timePoint;
  }
  
  
  
  
  LUMIERA_ERROR_DEFINE (UNKNOWN_GRID, "referring to an undefined grid or scale in value quantisation");
  
  
  
}} // lib::time

