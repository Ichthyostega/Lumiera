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
      // will automatically wrap over to the seconds, minutes and hour fields
    }
    
    /** calculate the time point denoted by this SMPTE timecode,
     *  by summing up the timecode's components */
    TimeValue 
    Smpte::evaluate (SmpteTC const& tc, QuantR quantiser)
    {
      long frameRate = tc.getFps();
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
     *  the frames, seconds, minutes and hours fields have already been processed
     *  and stored into the component digxels, under the assumption the overall
     *  value stays in range. 
     * @note currently the range is extended "naturally" (i.e. mathematically).
     *       The representation is flipped around the zero point and the value
     *       of the hours is just allowed to increase beyond 23
     * @todo If necessary, this extension point should be converted into a
     *       configurable strategy. Possible variations
     *       - clip values beyond the boundaries
     *       - throw an exception on illegal values
     *       - wrap around from 23:59:59:## to 0:0:0:0
     *       - just make the hour negative, but continue with the same
     *         orientation (0:0:0:0 - 1sec = -1:59:59:0)
     */
    void
    Smpte::rangeLimitStrategy (SmpteTC& tc)
    {
      if (tc.hours < 0)
        tc.invertOrientation();
    }
  }
  
  
  namespace { // Timecode implementation details
    
    typedef util::IDiv<int> Div;
    
    void
    wrapFrames  (SmpteTC* thisTC, int rawFrames)
    {
      Div scaleRelation = floorwrap<int> (rawFrames, thisTC->getFps());
      thisTC->frames.setValueRaw (scaleRelation.rem);
      thisTC->secs += scaleRelation.quot;
    }
    
    void
    wrapSeconds (SmpteTC* thisTC, int rawSecs)
    {
      Div scaleRelation = floorwrap (rawSecs, 60);
      thisTC->secs.setValueRaw (scaleRelation.rem);
      thisTC->mins += scaleRelation.quot;
    }
    
    void
    wrapMinutes (SmpteTC* thisTC, int rawMins)
    {
      Div scaleRelation = floorwrap (rawMins, 60);
      thisTC->mins.setValueRaw (scaleRelation.rem);
      thisTC->hours += scaleRelation.quot;
    }
    
    void
    wrapHours   (SmpteTC* thisTC, int rawHours)
    {
      thisTC->hours.setValueRaw (rawHours);
      format::Smpte::rangeLimitStrategy (*thisTC);
    }
    
    
    using std::tr1::bind;
    using std::tr1::placeholders::_1;
    
    /** bind the individual Digxel mutation functors
     *  to normalise raw component values */
    inline void
    setupComponentNormalisation (SmpteTC& thisTC)
    {
      thisTC.hours.installMutator (wrapHours,   thisTC);
      thisTC.mins.installMutator  (wrapMinutes, thisTC);
      thisTC.secs.installMutator  (wrapSeconds, thisTC);
      thisTC.frames.installMutator(wrapFrames,  thisTC);
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
      setupComponentNormalisation (*this);
      quantisedTime.castInto (*this);
    }
  
  
  SmpteTC::SmpteTC (SmpteTC const& o)
    : TCode(o)
    , effectiveFramerate_(o.effectiveFramerate_)
    {
      setupComponentNormalisation (*this);
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
  
  
  /** flip the orientation of min, sec, and frames.
   *  Besides changing the sign, this will flip the
   *  meaning of the component fields, which by
   *  definition are always oriented towards zero.
   *  
   *  Normalised value fields are defined positive,
   *  with automatic overflow to next higher field.
   *  This might cause the hours to become negative.
   *  When invoked in this case, the meaning changes
   *  from -h + (m+s+f) to -(h+m+s+f)
   */
  void
  SmpteTC::invertOrientation()
  {
    int fr (getFps()); 
    int f (fr - frames); // revert orientation
    int s (60 - secs);  //  of the components
    int m (60 - mins); //   
    int h = -hours;   //  assumed to be negative
    sgn *= -1;       //   flip sign field
    
    if (f < fr) --s; else f -= fr;
    if (s < 60) --m; else s -= 60;
    if (m < 60) --h; else m -= 60;
    
    hours.setValueRaw(h);
    mins   = m;  // invoking setters
    secs   = s; //  ensures normalisation 
    frames = f;
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
  