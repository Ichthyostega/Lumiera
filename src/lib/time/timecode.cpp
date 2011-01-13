/*
  Timecode  -  implementation of fixed grid aligned time specifications

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


#include "lib/lumitime.hpp"
#include "lib/time/timecode.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/formats.hpp"
#include "lib/time.h"


using std::string;


namespace lib {
namespace time {
  
  
  Format::~Format() { }  // emit VTable here....
  TCode::~TCode()   { }
  
  namespace format {
    
    /** build up a frame count
     *  by quantising the given time value 
     */
    void
    Frames::rebuild (FrameNr& framecnt, Quantiser const& quantiser, TimeValue const& rawTime)
    {
      framecnt.setValueRaw(quantiser.gridPoint (rawTime));
    }
  }
  
  
  /** */
  SmpteTC::SmpteTC (QuTime const& quantisedTime)
    : tpoint_(quantisedTime)           /////////////////////////////TODO eternal bullshit
    { }
  
  
  /** */
  HmsTC::HmsTC (QuTime const& quantisedTime)
    : tpoint_(quantisedTime)           /////////////////////////////TODO bullshit
    { }
  
  
  /** */
  Secs::Secs (QuTime const& quantisedTime)
    : sec_(TimeVar(quantisedTime) / GAVL_TIME_SCALE)   /////////////TODO bullshit
    { }
  
  
  
  /** */
  int
  SmpteTC::getSecs() const
  {
    return lumiera_time_seconds (tpoint_);
  }
  
  /** */
  int
  SmpteTC::getMins() const 
  {
    return lumiera_time_minutes (tpoint_);
  }
  
  /** */
  int
  SmpteTC::getHours() const 
  {
    return lumiera_time_hours (tpoint_);
  }
  
  /** */
  int
  SmpteTC::getFrames() const
  {
    UNIMPLEMENTED ("Frame-Quantisation");
  }
  
  /** */
  int
  HmsTC::getSecs() const
  {
    return lumiera_time_seconds (tpoint_);
  }
  
  /** */
  int
  HmsTC::getMins() const 
  {
    return lumiera_time_minutes (tpoint_);
  }
  
  /** */
  int
  HmsTC::getHours() const 
  {
    return lumiera_time_hours (tpoint_);
  }
  
  /** */
  double
  HmsTC::getMillis() const
  {
    TODO ("Frame-Quantisation");
    return lumiera_time_millis (tpoint_);
  }
  
  /** */

  
  
  
}} // lib::time
  
