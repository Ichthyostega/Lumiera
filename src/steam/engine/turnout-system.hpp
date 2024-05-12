/*
  TURNOUT-SYSTEM.hpp  -  Encapsulation of the state corresponding to a render calculation

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file turnout-system.hpp
 ** Access point to the state of a frame rendering evaluation.
 ** The rendering of frames is triggered from a render job, and recursively
 ** retrieves the data from predecessor nodes. Some statefull aspects are involved
 ** into this recursive evaluation, beyond the data in the local call stack. Such
 ** additional statefull dependencies are problematic (regarding concurrency and
 ** throughput) and are thus abstracted from the actual processing operations
 ** with the help of the steam::engine::StateClosure interface
 ** @todo unfinished draft from 2009 regarding the render process
 */


#ifndef ENGINE_TURNOUT_SYSTEM_H
#define ENGINE_TURNOUT_SYSTEM_H


#include "steam/engine/state-closure.hpp"


namespace steam  {
namespace engine {



  class StateProxy
    : public StateClosure
    {
      
    private: /* === top-level implementation of the StateClosure interface === */
      
      BuffHandle allocateBuffer (const lumiera::StreamType*);              //////////////////////////TICKET #828
      
      void releaseBuffer (BuffHandle& bh);
      
      BuffHandle fetch (FrameID const& fID);
      
      void is_calculated (BuffHandle const& bh);
      
      FrameID const& genFrameID (NodeID const&, uint chanNo);

      BuffTableStorage& getBuffTableStorage();
      
      virtual StateClosure& getCurrentImplementation () { return *this; }
      
    };
    
    
    
}} // namespace steam::engine
#endif /*ENGINE_TURNOUT_SYSTEM_H*/
