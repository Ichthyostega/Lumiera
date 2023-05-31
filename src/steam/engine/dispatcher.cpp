/*
  Dispatcher  -  translating calculation streams into frame jobs

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

* *****************************************************/


/** @file dispatcher.cpp
 ** Implementation parts of job generation within an ongoing render process
 ** @todo valid draft, unfortunately stalled in 2013
 */


#include "steam/engine/dispatcher.hpp"
//#include "lib/frameid.hpp"
//#include "steam/state.hpp"



namespace steam {
namespace engine {

  Dispatcher::~Dispatcher() { }  // emit VTables and Typeinfo here....
  FrameSequencer::~FrameSequencer() { }
  
  
  /** @todo WIP */
  Dispatcher::JobBuilder
  Dispatcher::onCalcStream (ModelPort modelPort, uint channel)
  {
    JobBuilder builder = {this, modelPort,channel};
    return builder;
  }
  
  
  /** @todo WIP */
  FrameCoord
  Dispatcher::JobBuilder::relativeFrameLocation (TimeAnchor& refPoint, FrameCnt frameOffset)
  {
    FrameCoord frame;
    frame.absoluteNominalTime = refPoint;
    frame.absoluteFrameNumber = refPoint.getStartFrame();
    frame.realTimeDeadline = refPoint.establishDeadlineFor (frameOffset);
    frame.modelPort = this->modelPort_;
    frame.channelNr = this->channel_;
    
    ENSURE (frame.isDefined());
    return dispatcher_->locateRelative (frame, frameOffset);
  }
  
  
  
  
  
}} // namespace engine
