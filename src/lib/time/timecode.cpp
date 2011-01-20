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
#include "lib/util.hpp"


using util::unConst;
using std::string;


namespace lib {
namespace time {
  
  
  TCode::~TCode()   { }  // emit VTable here....
  
  namespace format {
    
    /** build up a frame count
     *  by quantising the given time value 
     */
    void
    Frames::rebuild (FrameNr& framecnt, QuantR quantiser, TimeValue const& rawTime)
    {
      framecnt.setValueRaw(quantiser.gridPoint (rawTime));
    }
    
    /** calculate the time point denoted by this frame count */
    TimeValue 
    Frames::evaluate (FrameNr const& framecnt, QuantR quantiser)
    {
      return quantiser.timeOf (framecnt);
    }

    
    /** build up a SMPTE timecode
     *  by quantising the given time value and then splitting it
     *  into hours, minutes, seconds and frame offset 
     */
    void
    Smpte::rebuild (SmpteTC& tc, QuantR quantiser, TimeValue const& rawTime)
    {
//    framecnt.setValueRaw(quantiser.gridPoint (rawTime));
      UNIMPLEMENTED("build smpte components from raw time");
    }
    
    /** calculate the time point denoted by this SMPTE timecode,
     *  by summing up the timecode's components */
    TimeValue 
    Smpte::evaluate (SmpteTC const& tc, QuantR quantiser)
    {
//    return quantiser.timeOf (framecnt);
      UNIMPLEMENTED("calculate time from smpte");
    }

  }
  
  namespace { // Timecode implementation details
    
    
    int
    wrapFrames  (SmpteTC& thisTC, int rawFrames)
    {
      int fps = 25; ////////////////////////////////TODO outch
      thisTC.secs += rawFrames / fps;
      return rawFrames % fps;
                  //////////////////////////////////TODO need floorwrap-util
    }
    
    int
    wrapSeconds (SmpteTC& thisTC, int rawSecs)
    {
        
    }
    
    int
    wrapMinutes (SmpteTC& thisTC, int rawMins)
    {
        
    }
    
    int
    wrapHours   (SmpteTC& thisTC, int rawHours)
    {
        
    }
    
    
  }//(End)implementation details
  
  
  /** */
  FrameNr::FrameNr (QuTime const& quantisedTime)
    : TCode(quantisedTime)
    , CountVal()
    {
      quantisedTime.castInto (*this);
    }
  

  /** */
  SmpteTC::SmpteTC (QuTime const& quantisedTime)
    : TCode(quantisedTime)
    { }
  
  
  /** */
  HmsTC::HmsTC (QuTime const& quantisedTime)
    : TCode(quantisedTime)
//  : tpoint_(quantisedTime)           /////////////////////////////TODO bullshit
    { }
  
  
  /** */
  Secs::Secs (QuTime const& quantisedTime)
    : TCode(quantisedTime)
//  : sec_(TimeVar(quantisedTime) / GAVL_TIME_SCALE)   /////////////TODO bullshit
    { }
  
  
  
  void
  SmpteTC::rebuild()  const
  {
    TimeValue point = Format::evaluate(*this, *quantiser_);
    Format::rebuild(unConst(*this), *quantiser_, point);
  }
  
  
  string
  SmpteTC::show()  const
  {
    rebuild();
    string tc;
           tc.reserve(15);
           tc += sgn.show();
           tc += hours.show();
           tc += ':';
           tc += mins.show();
           tc += ':';
           tc += secs.show();
           tc += ':';
           tc += frames.show();
    return tc;
  }
  
  SmpteTC&
  SmpteTC::operator++ ()
  {
    UNIMPLEMENTED ("SMPTE unit increment");
  }
  
  SmpteTC&
  SmpteTC::operator-- ()
  {
    UNIMPLEMENTED ("SMPTE unit decrement");
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
  
