/*
  TIMECODE.hpp  -  grid aligned and fixed format time specifications

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file timecode.hpp
 ** Timecode handling library
 ** This header defines the foundation interface TCode to represent a grid aligned
 ** time specification, given in some well-established time code format. It is complemented
 ** by implementations of the most relevant practical time code formats
 ** - frame numbers / frame counting
 ** - SMPTE (hours, minutes, seconds and frames) -- including drop-frame
 ** - HMS (hours, minutes, seconds and fractional seconds)
 ** - fractional seconds as rational number
 */


#ifndef LIB_TIME_TIMECODE_H
#define LIB_TIME_TIMECODE_H

#include "lib/time/timevalue.hpp"
#include "lib/time/formats.hpp"
#include "lib/time/digxel.hpp"
#include "lib/symbol.hpp"

#include <boost/operators.hpp>
#include <boost/lexical_cast.hpp> ///////////////TODO
#include <string>


namespace lib {
namespace time {
  
  using std::string;
  using lib::Literal;
  using boost::lexical_cast; /////////TODO
  
  
  /**
   * Interface: fixed format timecode specification.
   * @see time::format
   * @todo as of 2016 this is basically finished since years,
   *       but still not in any widespread practical use (not by bad intention,
   *       simply by lack of opportunities). So this core interface still needs
   *       some feedback from practice in order to be finalised. 
   */
  class TCode
    {
      
    public:
      virtual ~TCode() { }
      
      operator string()  const { return show(); }
      string describe()  const { return string(tcID()); }
      Time   getTime()   const { return Time(value()); }
      
    protected:
      TCode (PQuant const& quant)
        : quantiser_(quant)
        { }
      
      virtual string show()     const   =0;
      virtual Literal tcID()    const   =0;
      virtual TimeValue value() const   =0;
      
    protected:
      PQuant quantiser_;
    };
  
  
  class QuTime;
  
  /**
   * A frame counting timecode value.
   * This is the hard-coded standard representation of
   * format::Frames, and is defined such as to make FrameNr
   * values interchangeable with integral numbers.
   * Like any concrete TCode subclass, it can be created
   * based on a QuTime value. This way, not only the (raw) TimeValue
   * is provided, but also the (frame)-Grid to base the frame count on.
   * But contrary to a QuTime value, a FrameNr value is \em materialised
   * (rounded) into a definite integral number, stripping the excess
   * precision contained in the original (raw) TimeValue.
   * As framecount values are implemented as single display field for an
   * integral value (time::Digxel), they allow for simple presentation. 
   */
  class FrameNr
    : public TCode
    , public CountVal
    {
      
      string show()     const { return string(CountVal::show())+"#"; }
      Literal tcID()    const { return "Framecount"; } 
      TimeValue value() const { return Format::evaluate (*this, *quantiser_); }
      
    public:
      typedef format::Frames Format;
      
      FrameNr (QuTime const& quantisedTime);
      
      /** convenience shortcut: time grid to frame number */
      static FrameCnt quant (Time const&, Symbol gridID);// defined in common-services.cpp
      
      using TCode::operator string;
     // CountVal implicitly convertible to long      ///////////TICKET #882 : outch! should be a 64bit type!
    };
  
  
  
  /**
   * Classical Timecode value reminiscent to SMPTE format.
   * After quantisation, the resulting (materialised) time value is
   * decimated into a hours, a minutes, a seconds part and the remainder
   * is cast into a frame number relative to the seconds. Consequently,
   * a SmpteTC representation is always linked implicitly to a specific framerate.
   * 
   * \par range extensions
   * Historically, SMPTE timecode format was focused mainly on how to encode a
   * unique timestamp in a way allowing to 'piggyback' these timestamps into an
   * existing (analogue) media data format. As a side effect, quite tight limits
   * had to be imposed on the possible component values in such a fixed-length format.
   * This whole concern is completely beyond the scope of a typical computer based video
   * implementation; thus we can consider ways to extend the value range to be represented
   * in this SMPTE-like timecode format:
   * - we can allow time values below the zero point
   * - we can allow time values beyond 24 hours.
   * Several different schemes how to do this extensions could be devised (and in addition,
   * we could also wrap around the hours field, jumping from 23:59:59:## to 0:0:0:0).
   * Currently, we implement an extension, where the timecode representation is symmetrical
   * to the zero point and the hours field is just extended beyond 23 hours. To give an
   * example: \c 0:0:0:0 minus 1 frame yields \c -0:0:0:1
   * 
   * @todo the range extension scheme could be a configurable strategy
   */
  class SmpteTC
    : public TCode
    , boost::unit_steppable<SmpteTC>
    {
      uint effectiveFramerate_;
      
      virtual string show()     const ;
      virtual Literal tcID()    const { return "SMPTE"; }
      virtual TimeValue value() const { return Format::evaluate (*this, *quantiser_); }
      
      
    public:
      typedef format::Smpte Format;
      
      SmpteTC (QuTime const& quantisedTime);
      SmpteTC (SmpteTC const&);
      SmpteTC& operator= (SmpteTC const&);
      
      uint getFps()   const;
      
      void clear();
      void rebuild();
      void invertOrientation();

      
      HourDigit hours;
      SexaDigit mins;
      SexaDigit secs;
      SexaDigit frames;
      Signum    sgn;
      
      SmpteTC& operator++();
      SmpteTC& operator--();
    };
  
  
  
  /**
   * @warning missing implementation
   */        ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #736 implement HMS format
  class HmsTC
    : public TCode
    {
      TimeVar tpoint_;
      
      virtual string show()     const { return string(tpoint_); }
      virtual Literal tcID()    const { return "Timecode"; }        
      virtual TimeValue value() const { return tpoint_; }        
      
    public:
      typedef format::Hms Format;
      
      HmsTC (QuTime const& quantisedTime);
      
      double getMillis () const;
      int getSecs      () const; 
      int getMins      () const; 
      int getHours     () const; 
    };
  
  
  
  /**
   * @warning partially missing implementation
   */        ////////////////////////////////////////////////////////////////////////////////////////////////TICKET #736 implement Seconds format
  class Secs
    : public TCode
    {
      FSecs sec_;
      
      virtual string show()     const { return string(Time(sec_)); }
      virtual Literal tcID()    const { return "Seconds"; }
      virtual TimeValue value() const { return Time(sec_); }
      
    public:
      typedef format::Seconds Format;
      
      Secs (QuTime const& quantisedTime);
      
      operator FSecs()  const;
    };
  
    
  /** writes time value, formatted as HH:MM:SS:mmm */
  

  
}} // lib::time
#endif
