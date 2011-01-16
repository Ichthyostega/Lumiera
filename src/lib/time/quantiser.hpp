/*
  QUANTISER.hpp  -  aligning time values to a time grid

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


#ifndef LIB_TIME_QUANTISER_H
#define LIB_TIME_QUANTISER_H

#include "lib/error.hpp"
#include "lib/time/grid.hpp"
#include "lib/time/formats.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/iter-adapter.hpp"

//#include <boost/operators.hpp>
#include <vector>
#include <string>
#include <cmath>


namespace lib {
namespace time {
  
  LUMIERA_ERROR_DECLARE (UNKNOWN_GRID); ///< referring to an undefined grid or scale in value quantisation
  
  
  namespace { // stashed here for later
    
    template<typename NUM>
    struct ValTrait;
    
    template<>
    struct ValTrait<int>
      {
        static int    asInt    (int val) { return val; }
        static double asDouble (int val) { return val; }
      };
    
    template<>
    struct ValTrait<double>
      {
        static int    asInt    (double val) { return std::floor(0.5+val); }  ///< in accordance with Lumiera's time handling RfC
        static double asDouble (double val) { return val; }
      };
    
  }
  
  
  /**
   * Facility to create grid-aligned time values.
   * Effectively, a quantiser exposes the value Grid API, but
   * additionally also manages a set of supported (display) formats or
   * "time code" formats. Plus there is an static API to fetch a suitable
   * quantiser instance by-name; actually this utilises a hidden link to
   * the Lumiera session. Time quantisation and timecode handling explicitly
   * relies on this Quantiser interface.
   * 
   * @todo WIP-WIP-WIP
   */
  class Quantiser
    : public Grid
    {
    protected:
      format::Supported supportedFormats_;
      
      Quantiser()
        : supportedFormats_(format::SupportStandardTimecode())
        { }
      
    public:
      template<class FMT>
      bool
      supports()  const
        {
          return supportedFormats_.check<FMT>(); 
        }
      
      
      //------Grid-API----------------------------------------------
      virtual long      gridPoint (TimeValue const& raw)  const   =0;
      virtual TimeValue gridAlign (TimeValue const& raw)  const   =0;
      virtual TimeValue timeOf    (long gridPoint)        const   =0;
      virtual TimeValue timeOf    (FSecs, int =0)         const   =0;
    };
  
  
  
  
  
  /**
   * Simple stand-alone Quantiser implementation based on a constant sized gird.
   * This is a self-contained quantiser implementation without any implicit referral
   * to the Lumiera session. As such it is suited for simplified unit testing.
   * @warning real GUI and Proc-Layer code should always fetch a quantiser from the
   * Session, referring to a pre defined TimeGrid. Basically, the overall purpose of
   * the time-quantisation framework is to enforce such a link to a distinct time scale
   * and quantisation, so to prevent "wild and uncoordinated" rounding attempts.
   */
  class FixedFrameQuantiser
    : public Quantiser
    {
      Time     origin_;
      Duration raster_;
      
    public:
      FixedFrameQuantiser (FrameRate const& frames_per_second, TimeValue referencePoint  =TimeValue(0));
      FixedFrameQuantiser (Duration const& frame_duration,     TimeValue referencePoint  =TimeValue(0));
      
      long      gridPoint (TimeValue const&)  const;
      TimeValue gridAlign (TimeValue const&)  const;
      TimeValue timeOf    (long gridPoint)    const;
      TimeValue timeOf    (FSecs, int =0)     const;
      
    };
  
  
  
}} // lib::time
#endif
