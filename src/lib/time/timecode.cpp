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

#include <tr1/functional>

using std::string;
using util::unConst;
using util::isSameObject;
using util::floorwrap;


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
     *  into hours, minutes, seconds and frame offset.
     */
    void
    Smpte::rebuild (SmpteTC& tc, QuantR quantiser, TimeValue const& rawTime)
    {
      tc.clear();
      tc.frames = quantiser.gridPoint (rawTime);
      // will automatically wrap over to the secs, minutes and hour fields
    }
    
    /** calculate the time point denoted by this SMPTE timecode,
     *  by summing up the timecode's components */
    TimeValue 
    Smpte::evaluate (SmpteTC const& tc, QuantR quantiser)
    {
      uint frameRate = tc.getFps();
      long gridPoint = tc.frames
                     + tc.secs   * frameRate
                     + tc.mins   * frameRate * 60 
                     + tc.hours  * frameRate * 60 * 60;
      return quantiser.timeOf (tc.sgn * gridPoint);
    }
    
    /** yield the Framerate in effect at that point.
     *  Especially Timecode in SMPTE format exposes a "frames" field
     *  to contain the remainder of frames in addition to the h:m:s value.
     *  Obviously this value has to be kept below the number of frames for
     *  a full second and wrap around accordingly.
     * @note SMPTE format assumes this framerate to be constant. Actually,
     *       in this implementation the value returned here neither needs
     *       to be constant (independent of the given rawTime), nor does
     *       it need to be the actual framerate used by the quantiser.
     *       Especially in case of NTSC drop-frame, the timecode
     *       uses 30fps here, while the quantisation uses 29.97
     * @todo this design just doesn't feel quite right... 
     */ 
    uint
    Smpte::getFramerate (QuantR quantiser_, TimeValue const& rawTime)
    {
      long refCnt = quantiser_.gridPoint(rawTime);
      long newCnt = quantiser_.gridPoint(Time(0.5,1) + rawTime);
      long effectiveFrames = newCnt - refCnt;
      ENSURE (1000 > effectiveFrames);
      ENSURE (0 < effectiveFrames);
      return uint(effectiveFrames);
    }
    
    
    /** handle the limits of SMPTE timecode range.
     *  This is an extension and configuration point to control how
     *  to handle values beyond the official SMPTE timecode range of
     *  0:0:0:0 to 23:59:59:##. When this strategy function is invoked,
     *  the frames, seconds and minutes fields have already been processed
     *  under the assumption the overall value stays in range. After returning
     *  from this strategy function, the rawHours value will be returned to be
     *  stored into the hours field without any further adjustments. 
     * @note currently the range is extended "naturally" (i.e. mathematically).
     *       The representation is flipped around the zero point and the value
     *       of the hours is just allowed to increase beyond 23
     * @todo If necessary, this extension point should be converted into a
     *       configurable strategy. Possible variations
     *       - clip values beyond the boundaries
     *       - wrap around from 23:59:59:## to 0:0:0:0
     *       - just make the hour negative, but continue with the same
     *         orientation (0:0:0:0 - 1sec = -1:59:59:0)
     */
    void
    Smpte::rangeLimitStrategy (SmpteTC& tc, int& rawHours)
    {
      if ((rawHours^tc.sgn) >= 0) return;
      
      tc.sgn = rawHours; // transfer sign into the sign field
      rawHours = abs(rawHours);
      
      REQUIRE (0 <= tc.frames && uint(tc.frames) < tc.getFps());
      REQUIRE (0 <= tc.secs   && tc.secs   < 60 );
      REQUIRE (0 <= tc.mins   && tc.mins   < 60 );
      
        // sign flip was detected:
       //  switch orientation of all timecode fields
      //   i.e. -h + (m+s+f) becomes - (h+m+s+f)
      uint fr   (tc.getFps() - tc.frames);
      uint secs (60 - tc.secs);
      uint mins (60 - tc.mins);
      
      ASSERT (fr <= tc.getFps());
      ASSERT (0 < secs);
      if (fr < tc.getFps())
        --secs;
      else
        fr = 0;
      
      ASSERT (secs <= 60);
      ASSERT (0 < mins);
      if (secs < 60)
        --mins;
      else
        secs = 0;
      
      ASSERT (mins <= 60);
      ASSERT (0 < rawHours);
      if (mins < 60)
        --rawHours;
      else
          mins = 0;
      
      tc.frames.setValueRaw (fr);
      tc.secs.setValueRaw (secs);
      tc.mins.setValueRaw (mins);
    }
  }
  
  
  namespace { // Timecode implementation details
    
    typedef util::IDiv<int> Div;
    
    int
    wrapFrames  (SmpteTC* thisTC, int rawFrames)
    {
      Div scaleRelation = floorwrap<int> (rawFrames, thisTC->getFps());
      thisTC->secs += scaleRelation.quot;
      return scaleRelation.rem;
    }
    
    int
    wrapSeconds (SmpteTC* thisTC, int rawSecs)
    {
      Div scaleRelation = floorwrap (rawSecs, 60);
      thisTC->mins += scaleRelation.quot;
      return scaleRelation.rem;
    }
    
    int
    wrapMinutes (SmpteTC* thisTC, int rawMins)
    {
      Div scaleRelation = floorwrap (rawMins, 60);
      thisTC->hours += scaleRelation.quot;
      return scaleRelation.rem;
    }
    
    int
    wrapHours   (SmpteTC* thisTC, int rawHours)
    {
      format::Smpte::rangeLimitStrategy (*thisTC, rawHours);
      return rawHours;
    }
    
    
    using std::tr1::bind;
    using std::tr1::placeholders::_1;
    
    /** bind the individual Digxel mutation functors
     *  to normalise raw component values */
    inline void
    setupComponentNormalisation (SmpteTC * thisTC)
    {
      thisTC->hours.mutator  = bind (wrapHours,   thisTC, _1 );
      thisTC->mins.mutator   = bind (wrapMinutes, thisTC, _1 );
      thisTC->secs.mutator   = bind (wrapSeconds, thisTC, _1 );
      thisTC->frames.mutator = bind (wrapFrames,  thisTC, _1 );
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
    , effectiveFramerate_(Format::getFramerate (*quantiser_, quantisedTime))
    {
      setupComponentNormalisation (this);
      quantisedTime.castInto (*this);
    }
  
  
  SmpteTC::SmpteTC (SmpteTC const& o)
    : TCode(o)
    , effectiveFramerate_(o.effectiveFramerate_)
    {
      setupComponentNormalisation (this);
      sgn    = o.sgn;
      hours  = o.hours;
      mins   = o.mins;
      secs   = o.secs;
      frames = o.frames;
    }
  
  
  SmpteTC&
  SmpteTC::operator= (SmpteTC const& o)
    {
      if (!isSameObject (*this, o))
        {
          TCode::operator= (o);
          effectiveFramerate_ = o.effectiveFramerate_;
          sgn    = o.sgn;
          hours  = o.hours;
          mins   = o.mins;
          secs   = o.secs;
          frames = o.frames;
        }
      return *this;
    }
  
  
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
  SmpteTC::clear()
  {
    frames.setValueRaw(0);
    secs.setValueRaw  (0);
    mins.setValueRaw  (0);
    hours.setValueRaw (0);
    sgn.setValueRaw  (+1);
  }
  
  
  void
  SmpteTC::rebuild()
  {
    TimeValue point = Format::evaluate (*this, *quantiser_);
    Format::rebuild (*this, *quantiser_, point);
  }
  
  
  uint
  SmpteTC::getFps()  const
  {
    return effectiveFramerate_;    //////////////////////////////////TODO better design. Shouldn't Format::getFramerate(QuantR, TimeValue) be moved here?
  }
  
  
  string
  SmpteTC::show()  const
  {
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
    frames += sgn;
    return *this;
  }
  
  SmpteTC&
  SmpteTC::operator-- ()
  {
    frames -= sgn;
    return *this;
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
  
