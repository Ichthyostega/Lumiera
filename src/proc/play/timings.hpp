/*
  TIMINGS.hpp  -  timing specifications for a frame quantised data stream

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file timings.hpp
 ** How to define a timing specification or constraint.
 ** A timing spec is used to anchor an data stream with relation to a time axis or frame grid.
 ** There are two kinds of timing specs:
 ** - nominal timing specifications relate to the nominal time values
 **   of the frames in a data stream, i.e. the "should be" time values.
 **   These might be values derived from a timecode or just values in
 **   in relation to the timeline axis, but without any connection to
 **   the real wall clock time
 ** - actual timing specifications are always connected or related to
 **   an external time source, typically just wall clock time. For example,
 **   actual timing specs dictate the constraints for real time frame
 **   delivery to an external output connection.
 **   
 ** @todo WIP-WIP-WIP 8/2011
 ** @see output-slot.hpp  ////TODO
 */


#ifndef PROC_PLAY_TIMINGS_H
#define PROC_PLAY_TIMINGS_H


#include "lib/error.hpp"
//#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
//#include "proc/engine/buffer-provider.hpp"
//#include "lib/iter-source.hpp"
//#include "lib/sync.hpp"

//#include <boost/noncopyable.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>
//#include <boost/scoped_ptr.hpp>


namespace proc {
namespace play {

  using lib::time::Time;
//using std::string;

//using std::vector;
//using std::tr1::shared_ptr;
//using boost::scoped_ptr;
  
  
  
  
  /*****************************************************************************
   * Generic frame timing specification. Defines the expected delivery interval,
   * optionally also the expected quality-of-service
   * 
   * @note copyable value class
   * 
   * @todo write type comment
   */
  class Timings
    {
    public:
      
      //////////////TODO further accessor functions here
      
      Timings
      constrainedBy (Timings additionalConditions)
        {
          UNIMPLEMENTED ("how to combine timing constraints");
        }
    };
  
  
  
}} // namespace proc::play
#endif
