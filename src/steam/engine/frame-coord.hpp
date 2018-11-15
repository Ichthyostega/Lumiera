/*
  FRAME-COORD.hpp  -  unique distinct coordinates of a frame to be calculated

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

*/


/** @file frame-coord.hpp
 ** Tuple data type to address distinct frames within the render engine calculations.
 */


#ifndef PROC_ENGINE_FRAME_COORD_H
#define PROC_ENGINE_FRAME_COORD_H

#include "steam/common.hpp"
#include "steam/mobject/model-port.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/time/timequant.hpp"

#include <limits>


namespace proc {
namespace engine {
  
  using mobject::ModelPort;
  using lib::time::FrameCnt;
  using lib::time::TimeVar;
  using lib::time::Time;
  
  
  /**
   * effective coordinates of a frame to be calculated.
   * Frame coordinates are produced as result of the Dispatcher call,
   * thus forming the foundation of an actual ProcNode invocation
   * A frame render job can be characterised by
   * - the nominal (timeline) time of the frame
   * - the corresponding frame-number
   * - a real wall-clock time deadline for delivery
   * - the actual node to pull data from, defined indirectly through
   *   ModelPort and channel number (as used within the Segmentation)
   * 
   * @remarks consider frame coordinates being "boiled down" to the actual values.
   *          There is no reference to any kind of time grid (or similar session internals).
   * 
   * @todo 1/12 WIP-WIP-WIP defining the invocation sequence and render jobs
   */
  struct FrameCoord
    {
      
      TimeVar  absoluteNominalTime;
      FrameCnt absoluteFrameNumber;
      
      TimeVar absoluteRealDeadline;
      
      ModelPort modelPort;
      uint      channelNr;
      
      
      /** build an \em undefined frame location */
      FrameCoord()
        : absoluteNominalTime(Time::NEVER)
        , absoluteFrameNumber(std::numeric_limits<FrameCnt>::max())
        , absoluteRealDeadline(Time::NEVER)
        , modelPort() // unconnected
        , channelNr(0)
        { }
      
      // using default copy operations
      
      /** @remarks sometimes we use NIL frame coordinate records
       *  to mark an exceptional condition, e.g. playback stop */
      bool
      isDefined()  const
        {
          return absoluteRealDeadline != Time::NEVER;
        }
    };
  
  
  
  /**
   * Facility for producing a sequence of FrameCoord.
   * This interface describes the essence of generating
   * a series of frame locations, which is necessary for
   * planning render jobs. To implement it, actually some
   * kind of [frame grid](\ref lib::time::Quantiser) is
   * necessary -- in practice we use a Dispatcher, which is
   * backed by the Segmentation (i.e. the render nodes network).
   */
  class FrameSequencer
    : util::NonCopyable
    {
      
    public:
      virtual ~FrameSequencer();  ///< this is an interface
      
      FrameCoord
      getNextFrame (FrameCoord refPoint)
        {
          return locateRelative (refPoint, +1 );
        }
      
    protected:
      virtual FrameCoord locateRelative (FrameCoord const&, FrameCnt frameOffset)   =0;
    };
  
  
  
}} // namespace proc::engine
#endif
