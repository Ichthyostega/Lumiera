/*
  DISPATCH-TABLE.hpp  -  implementation of frame job creation

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


/** @file dispatch-table.hpp
 ** Implementation details of render job generation.
 ** @todo draft from 2011, stalled, relevance not yet clear
 */


#ifndef PROC_ENGINE_DISPATCH_TABLE_H
#define PROC_ENGINE_DISPATCH_TABLE_H

#include "proc/common.hpp"
#include "proc/engine/dispatcher.hpp"



namespace proc {
namespace engine {
  
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * @todo 11/11 extremely fuzzy at the moment
   */
  class DispatchTable
    : public Dispatcher
    {
      
      /* ==== Dispatcher interface ==== */
      
      FrameCoord locateRelative (FrameCoord const&, FrameCnt frameOffset);
      bool       isEndOfChunk    (FrameCnt, ModelPort port);
      JobTicket& accessJobTicket (ModelPort, TimeValue nominalTime);
      
      
    protected:
      /** timerange covered by this RenderGraph */
      TimeSpan segment_;
      
    public:
      DispatchTable()
        : segment_(Time::ZERO, FSecs(5))
        {
          UNIMPLEMENTED ("anything regarding the Engine backbone");
        }
      
    };
  
  
  
}} // namespace proc::engine
#endif
