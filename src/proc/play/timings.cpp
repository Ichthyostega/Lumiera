/*
  Timings  -  timing specifications for a frame quantised data stream

  Copyright (C)         Lumiera.org
    2012,               Hermann Vosseler <Ichthyostega@web.de>

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


#include "proc/play/timings.hpp"
#include "lib/time/formats.hpp"
#include "lib/time/timequant.hpp"



namespace proc {
namespace play {
  
  
  using lib::time::PQuant;
  
  namespace { // hidden local details of the service implementation....
    
    inline PQuant
    buildStandardGridForFramerate (FrameRate fps)
    {
      return PQuant (new lib::time::FixedFrameQuantiser (fps));
    }                         //////TODO maybe caching these quantisers? they are immutable and threadsafe
    
    
  } // (End) hidden service impl details
  
  
  /** Create a default initialised Timing constraint record.
   * Using the standard optimistic settings for most values,
   * no latency, no special requirements. The frame grid is
   * rooted at the "natural" time origin; it is not related
   * in any way to the current session.
   * @remarks this ctor is intended rather for testing purposes!
   *          Usually, when creating a play/render process,
   *          the actual timings \em are related to the timeline
   *          and the latency/speed requirements of the output.
   */
  Timings::Timings (FrameRate fps)
    : grid_(buildStandardGridForFramerate(fps))
    , playbackUrgency (ASAP)
    { }

  

}} // namespace proc::play