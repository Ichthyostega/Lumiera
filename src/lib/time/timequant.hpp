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
   * fixed format time specification.
   * 
   * @todo WIP-WIP-WIP
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
