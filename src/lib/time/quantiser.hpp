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
#include "lib/time/timevalue.hpp"
#include "lib/time/formats.hpp"
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
   * 
   * @todo WIP-WIP-WIP
   */
  class Quantiser
    {
      typedef std::vector<Format*>         _FormatTable;
      typedef _FormatTable::const_iterator _SrcIter;
      typedef lib::PtrDerefIter<_SrcIter>  _Iter;
      
    public:
      virtual ~Quantiser();  ///< this is an ABC
      
      
      template<class FMT>
      bool supports()  const;
      
      typedef _Iter iterator;
      iterator getSupportedFormats()  const;
      
      virtual TimeValue gridAlign (TimeValue const& raw)   =0;
      virtual long      gridPoint (TimeValue const& raw)   =0;
      
    };
  
  
  /** 
   * smart reference
   * for accessing an existing quantiser 
   */
  class QuantiserRef
    {
      size_t hashID_;
      
    public:
      QuantiserRef (Quantiser const&);
      
      // using standard copy;
      
      
      Quantiser const&
      operator-> ()
        {
          UNIMPLEMENTED ("how to manage and address the existing quantisers");
        }
    };
  
  
  
  /**
   * Simple stand-alone Quantiser implementation for debugging and test.
   * This is a self-contained quantiser implementation without any implicit
   * referral to the Lumiera session. It is mainly intended for simplified unit testing.
   * @warning real GUI and Proc-Layer code should always prefer to build a real quantiser,
   * which referres some TimeGrid definition within the session. Basically, the overall
   * purpose of the time-quantisation framework is to enforce such a link to a specific
   * time and quantisation scale and to prevent "wild and uncoordinated" rounding attempts.
   */
  class FixedFrameQuantiser
    : public Quantiser
    {
      Time     origin_;
      Duration raster_;
      
    public:
      FixedFrameQuantiser (FrameRate const& frames_per_second, TimeValue referencePoint  =TimeValue(0));
      FixedFrameQuantiser (Duration const& frame_duration,     TimeValue referencePoint  =TimeValue(0));
      
      
      TimeValue gridAlign (TimeValue const&);
      long      gridPoint (TimeValue const&);
    };
  
  
  
}} // lib::time
#endif
