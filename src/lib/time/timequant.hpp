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


/** @file timequant.hpp
 ** Support library to represent grid-aligned time specifications
 ** This is part of Lumiera's time and time code handling framework.
 ** The QuTime entity represents the link between the internal precise
 ** time specifications and external representation formatted according
 ** to some well established time code format: QuTime both holds a precise
 ** internal time::TimeValue entry, plus a reference to the _time grid_ to
 ** be used, when it comes to _quantising_ (grid-aligning) those  values.
 ** And it offers a dedicated API to "materialise" this (still complete and
 ** precise) time value into an external representation.
 ** 
 ** # Collaborations
 ** 
 ** lib::time::Time is the ubiquitous yet opaque internal time "instant" used
 ** in Lumiera to designate time points (and by extension also a TimeSpan).
 ** What such an _internal time instant_ actually means, depends on the context,
 ** yet it is the implementation's (not the user's) responsibility to keep that
 ** meaning straight.
 ** 
 ** However, by creating a [Quantised Time](\ref lib::time::QuTime), the association
 ** to some time scale or time grid is made explicit; the time value within QuTime
 ** is stored in full precision though, without performing any "rounding" yet.
 ** 
 ** Only by building or casting into a _time code,_ the actual quantisation is
 ** performed, which means to align the time value to the next grid point and
 ** discard excess timing information. There are several [time formats](\ref formats.hpp)
 ** available to choose from, like Hour-Minute-Second, or SMPTE (h-m-s + frame) or
 ** fractional seconds or just a frame count with implicit frame rate.
 ** 
 ** How the _quantisation_ or _grid alignment_ is actually performed is up to the
 ** discretion of the lib::time::Quantiser; an actually Quantiser instance also
 ** represents a common notion of some specific time grid, together with a usage
 ** context. Notably, a "25 fps grid" means to start timing when the playback
 ** starts, and to proceed in steps of 1/25sec. Conceptually (not-yet-implemented
 ** as of 11/2022) a frame grid could also imply to anchor the time at some global
 ** time zone, allowing to integrate _live content_ into the timeline -- and the
 ** actual link to wall-clock time would be made explicit by some _placement_
 ** somewhere on the actual timeline.
 ** 
 */


#ifndef LIB_TIME_TIMEQUANT_H
#define LIB_TIME_TIMEQUANT_H

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
   * [advice system](\ref advice.hpp).
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
      QuTime (TimeValue raw, Symbol gridID);               ///< @note defined in common-services.cpp
      QuTime (TimeValue raw, PQuant quantisation_to_use);
      
      operator PQuant()  const;                            ///< @return `shared-ptr` to the associated time grid (or time scale)
      
      template<class FMT>
      bool supports()  const;                              ///< does our implicit time grid support building that timecode format?
      
      template<class FMT>
      typename format::Traits<FMT>::TimeCode
      formatAs()  const;                                   ///< create new time code instance, then #castInto
      
      template<class TC>
      void
      castInto (TC& timecode)  const;                      ///< quantise into implicit grid, then rebuild the timecode
      
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
    using TC = typename format::Traits<FMT>::TimeCode;
    return TC(*this);
  }
  
  
  /**
   * @tparam TC the kind of time code to use as target
   * @param  timecode existing instance of that timecode, to be overwritten
   * @remark this is the basic operation to convert an (internal) time value
   *         into a time code format. QuTime is already associated with some
   *         _time grid_ for quantisation, but the internal value is precise.
   *         This operation creates a quantised (frame aligned) transient copy,
   *         and uses this to adjust/modify the fields in the given timecode.
   */
  template<class TC>
  inline void
  QuTime::castInto (TC& timecode)  const
  {
    using Format = typename TC::Format;
    REQUIRE (supports<Format>());
    
    Format::rebuild (timecode, *quantiser_, TimeValue(*this));
  }
  
  
}} // lib::time
#endif
