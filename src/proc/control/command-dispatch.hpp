/*
  COMMAND-DISPATCH.hpp  -  Interface to enqueue and dispatch command messages

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file command-dispatch.hpp
 ** Interface to abstract the DispatcherLoop's ability to handle command messages.
 ** //TODO 
 **
 ** @see proc-dispatcher.hpp
 ** @see session-command-service.hpp
 ** @see DispatcherLoop
 **
 */



#ifndef PROC_CONTROL_COMMAND_DISPATCH_H
#define PROC_CONTROL_COMMAND_DISPATCH_H

//#include "lib/symbol.hpp"
#include "common/subsys.hpp"
#include "lib/depend.hpp"
#include "lib/sync.hpp"

#include <memory>
//#include <functional>



namespace proc {
namespace control {
  
//  using lib::Symbol;
  using std::unique_ptr;
//  using std::bind;
  using lumiera::Subsys;
  
  
  class DispatcherLoop;
  
  
  /**
   * @todo Type-comment
   */
  class ProcDispatcher
    : public lib::Sync<>
    {
      unique_ptr<DispatcherLoop> runningLoop_;
      bool active_{false};
      
      
    public:
      static lib::Depend<ProcDispatcher> instance;
      
      bool start (Subsys::SigTerm);
      bool isRunning();
      void requestStop();
      
      void activate();
      void deactivate();
      void clear();
      
      
      /* == diagnostics == */
      
//    size_t size() const ;
      bool empty()  const ;
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_COMMAND_DISPATCH_H*/
