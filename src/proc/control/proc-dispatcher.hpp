/*
  PROC-DISPATCHER.hpp  -  Proc-Layer command dispatch and execution

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


/** @file proc-dispatcher.hpp
 ** Dispatch and execute mutation operations on the High-level model.
 ** //TODO 
 **
 ** @see Command
 ** @see Session
 **
 */



#ifndef PROC_CONTROL_PROC_DISPATCHER_H
#define PROC_CONTROL_PROC_DISPATCHER_H

#include "common/subsys.hpp"
#include "lib/depend.hpp"
#include "lib/sync.hpp"

#include <memory>



namespace proc {
namespace control {
  
  using std::unique_ptr;
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
      void requestStop()  noexcept;
      
      void activate();
      void deactivate();
      void awaitDeactivation();
      void clear();
      
      bool empty()  const ;
      
    };
  
  
  
  
}} // namespace proc::control
#endif /*PROC_CONTROL_PROC_DISPATCHER_H*/
