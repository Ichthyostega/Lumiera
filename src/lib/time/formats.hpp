/*
  FORMATS.hpp  -  formats for displaying and specifying time

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

*/


#ifndef LIB_TIME_FORMATS_H
#define LIB_TIME_FORMATS_H

#include "lib/time/timevalue.hpp"

//#include <boost/operators.hpp>
#include <string>


namespace lib {
namespace time {
  
  
  /**
   * descriptor for a time representation format (ABC).
   * A time format describes how to specify time; it allows
   * to format a time value and to parse a textual representation.
   * @note while Format is an generic descriptor, the actual
   *       TCode (timecode) values are time values, which \em
   *       use a specific format, given as template parameter
   * 
   * @todo WIP-WIP-WIP
   */
  class Format
    {
      
    public:
      virtual ~Format();
    };
  
  namespace format {
    
    /** 
     * Frame count as timecode format.
     * An integral number used to count frames
     * can be used as a simple from of time code.
     * Indeed the Lumiera backend mostly relies on
     * these frame counts. As with any timecode, the
     * underlying framerate/quantisation remains implicit.
     */
    class Frames
      : public Format
      {
        
      };
    
    
    /**
     * Widely used standard media timecode format.
     * A SMPTE timestamp addresses individual frames,
     * by specifying time as hour-minute-second plus the
     * frame number within the actual second.
     */
    class Smpte
      : public Format
      {
        
      };
    
    
    /**
     * The informal hours-minutes-seconds-millisecond timecode.
     * As such, this timecode is quantisation agnostic, but usually
     * it is used to address some frame or block or otherwise quantised
     * entity in time. HMS-Timecode is similar to SMPTE, but uses a
     * floating point milliseconds value instead of the frame count
     */
    class Hms
      : public Format
      {
        
      };
    
    
    /**
     * Simple timecode specification as fractional seconds.
     * Similar to HMS, a specification of seconds is quantisation agnostic,
     * but usually some implicit quantisation is used anyway, be it on actual
     * data frames, audio frames, or just on some smaller time interval, e.g.
     * full milliseconds.
     * @note Seconds is implemented as rational number and thus uses
     *       decimal format, not the usual sexagesimal time format
     */
    class Seconds
      : public Format
      {
        
      };
    
    
    
    /* ==== predefined constants for specifying the format to use ==== */
      
    static const Seconds SECONDS;
    static const Frames  FRAMES;
    static const Smpte   SMPTE;
    static const Hms     HMS;
  
  
}}} // lib::time::format
#endif
