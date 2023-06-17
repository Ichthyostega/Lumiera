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


/** @file timecode.cpp
 ** Implementation parts of the timecode handling library.
 ** @todo a started implementation exists since 2010,
 **       yet crucial parts still need to be filled in as of 2016
 */


#include "lib/time/timecode.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"
#include "lib/time/formats.hpp"
#include "lib/time.h"
#include "lib/util.hpp"
#include "lib/util-quant.hpp"

#include <regex>
#include <functional>
#include <boost/lexical_cast.hpp>

using util::unConst;
using util::isSameObject;
using util::floorwrap;
using std::string;
using std::regex;
using std::smatch;
using std::regex_search;
using boost::lexical_cast;

namespace error = lumiera::error;

namespace lib {
namespace time {
  
  
  namespace format {  /* ================= Timecode implementation details ======== */ 
    
    LUMIERA_ERROR_DEFINE (INVALID_TIMECODE, "timecode format error, illegal value encountered");
    
    
    /** try to parse a frame number specification
     * @param frameNumber string containing an integral number with trailing '#'
     * @param frameGrid   coordinate system (and thus framerate) to use for the conversion
     * @return (opaque internal) lumiera time value of the given frame's start position
     * @throw error::Invalid in case of parsing failure
     * @note the string may contain any additional content, as long as a
     *       regular-expression search is able to pick out a suitable value
     */
    TimeValue
    Frames::parse (string const& frameNumber, QuantR frameGrid)
    {
      static regex frameNr_parser{"(?:^|[^\\d\\.\\-])(\\-?\\d+)#"};   // no leading [.-\d],  digit+'#'
      smatch match;                                                   // note: ECMA regexp does not support lookbehind
      if (regex_search (frameNumber, match, frameNr_parser))
        return frameGrid.timeOf (lexical_cast<FrameCnt> (match[1]));
      else
        throw error::Invalid ("unable to parse framecount \""+frameNumber+"\""
                             , LERR_(INVALID_TIMECODE));
    }
    
    
    TimeValue
    Smpte::parse (string const&, QuantR)
    {
      UNIMPLEMENTED("parsing SMPTE timecode");
    }
    
    
    TimeValue
    Hms::parse (string const&, QuantR)
    {
      UNIMPLEMENTED("parse a hours:mins:secs time specification");
    }
    
    
    /** try to parse a time specification in seconds or fractional seconds.
     * The value is interpreted relative to the origin of a the given time grid
     * This parser recognises full seconds, fractional seconds and both together.
     * In any case, the actual number is required to end with a trailing \c 'sec'
     * @par Example specifications
\verbatim
       12sec       -->  12     * TimeValue::SCALE
       -4sec       --> -4      * TimeValue::SCALE
       5/4sec      -->  1.25   * TimeValue::SCALE
       -5/25sec    --> -0.2    * TimeValue::SCALE
       1+1/2sec    -->  1.5    * TimeValue::SCALE
       1-1/25sec   -->  0.96   * TimeValue::SCALE
       -12-1/4sec  --> -11.75  * TimeValue::SCALE
\endverbatim
     * @param seconds string containing a time spec in seconds
     * @param grid coordinate system the parsed value is based on
     * @return the corresponding (opaque internal) lumiera time value
     * @throw error::Invalid in case of parsing failure
     * @note the string may contain any additional content, as long as a
     *       regular-expression search is able to pick out a suitable value
     */
    TimeValue
    Seconds::parse (string const& seconds, QuantR grid)
    {
      static regex fracSecs_parser ("(?:^|[^\\./\\d\\-])(\\-?\\d+)(?:([\\-\\+]\\d+)?/(\\d+))?sec");
                                  //__no leading[./-\d]   number      [+-]  number '/' number 'sec'   
      
      #define SUB_EXPR(N) lexical_cast<int> (match[N])
      smatch match;
      if (regex_search (seconds, match, fracSecs_parser))
        if (match[2].matched)
          {
            // complete spec with all parts
            FSecs fractionalPart (SUB_EXPR(2), SUB_EXPR(3));
            int fullSeconds (SUB_EXPR(1));
            return grid.timeOf (fullSeconds + fractionalPart);
          }
        else
        if (match[3].matched)
          {
            // only a fractional part was given
            return grid.timeOf (FSecs (SUB_EXPR(1), SUB_EXPR(3)));
          }
        else
          {
            // just simple non-fractional seconds
            return grid.timeOf (FSecs (SUB_EXPR(1)));
          }
      else
        throw error::Invalid ("unable to parse \""+seconds+"\" as (fractional)seconds"
                             , LERR_(INVALID_TIMECODE));
    }
    
    
    
    
    /** build up a frame count
     *  by quantising the given time value 
     */
    void
    Frames::rebuild (FrameNr& frameNr, QuantR quantiser, TimeValue const& rawTime)
    {
      frameNr.setValueRaw (quantiser.gridPoint (rawTime));
    }
    
    /** calculate the time point denoted by this frame count */
    TimeValue 
    Frames::evaluate (FrameNr const& frameNr, QuantR quantiser)
    {
      return quantiser.timeOf (frameNr);
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
      uint frameRate = tc.getFps();
      int64_t gridPoint(tc.frames);
      gridPoint += int64_t(tc.secs)  * frameRate;
      gridPoint += int64_t(tc.mins)  * frameRate * 60;
      gridPoint += int64_t(tc.hours) * frameRate * 60 * 60;
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
      FrameCnt refCnt = quantiser_.gridPoint(rawTime);
      FrameCnt newCnt = quantiser_.gridPoint(Time(0,1) + rawTime);
      FrameCnt effectiveFrames = newCnt - refCnt;
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
    Smpte::applyRangeLimitStrategy (SmpteTC& tc)
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
      format::Smpte::applyRangeLimitStrategy (*thisTC);
    }
    
    
    using std::bind;
    using std::placeholders::_1;
    
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
  
