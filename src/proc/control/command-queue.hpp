/*
  COMMAND-QUEUE.hpp  -  organise ordering of commands within ProcDispatcher

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file command-queue.hpp
 ** Implementation building block of ProcDispatcher to organise commands.
 ** This is the actual implementation of the command queue and additionally
 ** supports some management tasks pertaining to the queue as a whole.
 ** 
 ** @todo WIP-WIP as of 12/2016 
 ** 
 ** @see CommandQueue_test
 ** @see proc-dispatcher.hpp
 ** @see session-command-service.hpp
 ** @see DispatcherLoop
 ** @see Looper
 **
 */



#ifndef PROC_CONTROL_COMMAND_QUEUE_H
#define PROC_CONTROL_COMMAND_QUEUE_H

#include "lib/error.hpp"   ////////TODO needed?
//#include "common/subsys.hpp"
//#include "lib/depend.hpp"

//#include <memory>
//#include <functional>



namespace proc {
namespace control {
  
//  using lib::Symbol;
//  using std::bind;
  
  
  
  /**
   * @todo Type-comment
   */
  class CommandQueue
    {
      
    public:
      CommandQueue()
      { }
      
      
      
      /* == diagnostics == */
      
      size_t
      size() const
        {
          TODO ("implement queue");
          return 0;
        }
      
      bool
      empty()  const
        {
          return 0 == size();
        }
      
    };
  ////////////////TODO 12/16 currently just fleshing  out the API....
  
  
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_COMMAND_QUEUE_H*/
