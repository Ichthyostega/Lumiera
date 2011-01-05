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

using std::string;


namespace lib {
namespace time {
  
  
  /** */
  QuTime::QuTime (TimeValue raw, Symbol gridID)
    : Time(raw)          /////////////////////////////////////////////////TODO fetch quantiser
    { }
  
  
  /** */
  QuTime::QuTime (TimeValue raw, Quantiser const& quantisation_to_use)
    : Time(raw)          /////////////////////////////////////////////////TODO fetch quantiser
    { }
  
  
  
  /** */
  FixedFrameQuantiser::FixedFrameQuantiser (FSecs frames_per_second)
    : Quantiser()        /////////////////////////////////////////////////TODO we ought to do something
    { }

  
  
  
  
  LUMIERA_ERROR_DEFINE (UNKNOWN_GRID, "referring to an undefined grid or scale in value quantisation");
  
  
  
}} // lib::time
  
