/*
  DISPATCHER.hpp  -  translating calculation streams into frame jobs

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


#ifndef PROC_ENGINE_DISPATCHER_H
#define PROC_ENGINE_DISPATCHER_H

#include "proc/common.hpp"
//#include "proc/state.hpp"
#include "proc/mobject/model-port.hpp"
#include "proc/engine/time-anchor.hpp"
#include "proc/engine/frame-coord.hpp"
#include "proc/engine/job-ticket.hpp"
#include "lib/time/timevalue.hpp"

#include <boost/noncopyable.hpp>



namespace proc {
namespace engine {
  
  using mobject::ModelPort;
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * @todo 11/11 extremely fuzzy at the moment
   */
  class Dispatcher
    : boost::noncopyable
    {
      struct CoordBuilder
        {
          Dispatcher& dispatcher_;
          ModelPort modelPort_;
          uint channel_;
          
          FrameCoord relativeFrameLocation (TimeAnchor refPoint, uint frameCountOffset);    
        };
      
    public:
      virtual ~Dispatcher();  ///< this is an interface
      
      CoordBuilder onCalcStream (ModelPort modelPort, uint channel);
      
      JobTicket& accessJobTicket (FrameCoord const&);
      
    protected:      
      virtual FrameCoord locateFrameNext (uint frameCountOffset, TimeAnchor refPoint)   =0;
    };
  
  
  
}} // namespace proc::engine
#endif
