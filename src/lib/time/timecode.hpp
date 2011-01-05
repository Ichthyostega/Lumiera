/*
  TIMECODE.hpp  -  grid aligned and fixed format time specifications

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


#ifndef LIB_TIME_TIMECODE_H
#define LIB_TIME_TIMECODE_H

#include "lib/time/timevalue.hpp"
#include "lib/time/formats.hpp"

//#include <iostream>
//#include <boost/operators.hpp>
#include <string>


namespace lib {
namespace time {
  
  using std::string;
  
  
  /**
   * Interface: fixed format timecode specification.
   * @see time::Format
   * @todo WIP-WIP-WIP
   */
  class TCode
    {
      
    public:
      virtual ~TCode();
      
      operator string()  const { return show(); }
      string describe()  const { return tcID(); }
      Time   getTime()   const { return Time(calc()); }
      
    protected:
      virtual string show()     const   =0;
      virtual string tcID()     const   =0;
      virtual TimeValue calc()  const   =0;
    };
  
  
  class QuTime;
  
  /**
   * A frame counting timecode value.
   * This is an hard-coded representation of
   * TCode<format::Frames>, with additional convenience
   * constructors and conversions, which basically make
   * FrameNr values interchangeable with integral numbers. 
   */
  class FrameNr
    : public TCode
    {
        
      virtual string show()     const ;
      virtual string tcID()     const ;
      virtual TimeValue calc()  const ;
      
    public:
      FrameNr (QuTime const& quantisedTime);
      
      operator long()  const;
    };
  
  
  
  /**
   * 
   */
  class SmpteTC
    : public TCode
    {
      
      virtual string show()     const ;
      virtual string tcID()     const ;
      virtual TimeValue calc()  const ;
      
    public:
      SmpteTC (QuTime const& quantisedTime);
      
      int getSecs   () const; 
      int getMins   () const; 
      int getHours  () const; 
      int getFrames () const;
    };
  
  
  
  /**
   * 
   */
  class HmsTC
    : public TCode
    {
      
      virtual string show()     const ;
      virtual string tcID()     const ;
      virtual TimeValue calc()  const ;
      
    public:
      HmsTC (QuTime const& quantisedTime);
      
      double getMillis () const;
      int getSecs      () const; 
      int getMins      () const; 
      int getHours     () const; 
    };
  
  
  
  /**
   * 
   */
  class Secs
    : public TCode
    {
      
      virtual string show()     const ;
      virtual string tcID()     const ;
      virtual TimeValue calc()  const ;
      
    public:
      Secs (QuTime const& quantisedTime);
      
      operator FSecs()  const;
    };
  
    
  /** writes time value, formatted as HH:MM:SS:mmm */
  

  
}} // lib::time
#endif
