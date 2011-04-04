/*
  TIMEQUANT.hpp  -  quantised (grid aligned) time values

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


#ifndef LIB_TIME_TIMEQUQNT_H
#define LIB_TIME_TIMEQUQNT_H

#include "lib/time/timevalue.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/time/timecode.hpp"
#include "lib/symbol.hpp"

//#include <boost/operators.hpp>
#include <string>


namespace lib {
namespace time {
  
  using lib::Symbol;
  
  
  /**
   * grid aligned time specification, referring to a specific scale.
   * A quantised time value allows to access the time specification
   * as numeric value in one of the supported timecode formats, and
   * relative to the defined time scale. Usually this time scale
   * exists already in the Lumiera session and is referred simply
   * by symbolic ID, it will be fetched on demand through the
   * \link advice.hpp advice system.\endlink
   * 
   * By creating a QuTime value, the relation to such a predefined
   * time scale is made explicit. This doesn't change the internal
   * time value, but the actual creation of a timecode formatted
   * value (#formatAs) usually implies to quantise or grid align
   * the time to the frame grid specific to this time scale.  
   */
  class QuTime
    : public Time
    {
      PQuant quantiser_;
      
    public:
      QuTime (TimeValue raw, Symbol gridID);
      QuTime (TimeValue raw, PQuant quantisation_to_use);
      
      operator PQuant()  const;
          
      template<class FMT>
      bool supports()  const;
      
      template<class FMT>
      typename format::Traits<FMT>::TimeCode
      formatAs()  const;
      
      template<class TC>
      void
      castInto (TC& timecode)  const;
      
      /** receive change message, which
          might cause re-quantisation */
      void accept (Mutation const&);
    };
  
  
  
  /* == implementation == */
  
  inline
  QuTime::operator PQuant()  const
  {
    ASSERT (quantiser_);
    return quantiser_;
  }

  template<class FMT>
  inline bool
  QuTime::supports()  const
  {
    return quantiser_->supports<FMT>();
  }
  
  
  template<class FMT>
  inline typename format::Traits<FMT>::TimeCode
  QuTime::formatAs()  const
  {
    typedef typename format::Traits<FMT>::TimeCode TC; 
    return TC(*this);
  }
  
  
  template<class TC>
  inline void
  QuTime::castInto (TC& timecode)  const
  {
    typedef typename TC::Format Format;
    REQUIRE (supports<Format>());
    
    Format::rebuild (timecode, *quantiser_, TimeValue(*this));
  }
  
  
}} // lib::time
#endif
