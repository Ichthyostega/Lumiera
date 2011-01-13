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

#include <boost/rational.hpp>

using boost::rational_cast;
using std::string;


namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  namespace { // implementation helpers...
    
    ///////////TODO superfluous??
    
  }//(End) implementation helpers
  
  
  
  Quantiser::~Quantiser() { } // hint to emit the VTable here...
  
  
  /** */
  QuTime::QuTime (TimeValue raw, Symbol gridID)
    : Time(raw)          /////////////////////////////////////////////////TODO fetch quantiser
    { }
  
  
  /** */
  QuTime::QuTime (TimeValue raw, Quantiser const& quantisation_to_use)
    : Time(raw)
    , quantiser_(&quantisation_to_use)
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
  FixedFrameQuantiser::gridAlign (TimeValue const& rawTime)
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
  FixedFrameQuantiser::gridPoint (TimeValue const& rawTime)
  {
    return lumiera_quantise_frames (_raw(rawTime), _raw(origin_), _raw(raster_));
  }

  
  
  
  LUMIERA_ERROR_DEFINE (UNKNOWN_GRID, "referring to an undefined grid or scale in value quantisation");
  
  
  
}} // lib::time

