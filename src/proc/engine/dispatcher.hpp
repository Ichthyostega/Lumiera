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
#include "proc/state.hpp"
#include "lib/time/timevalue.hpp"



namespace proc {
namespace engine {
  
  using lib::time::TimeSpan;
  using lib::time::FSecs;
  using lib::time::Time;
//  
//  class ExitNode;
  
  /**
   * @todo
   */
  class Dispatcher
    {
    protected:
      /** timerange covered by this RenderGraph */
      TimeSpan segment_;
      
    public:
      Dispatcher()
        : segment_(Time::ZERO, FSecs(5))
        {
          UNIMPLEMENTED ("anything regarding the Engine backbone");
        }
      
    };
  
  
  
}} // namespace proc::engine
#endif
