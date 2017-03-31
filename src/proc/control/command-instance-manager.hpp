/*
  COMMAND-INSTANCE-MANAGER.hpp  -  Service to manage command instances for actual invocation

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file command-instance-manager.hpp
 ** Service to support forming and invocation of command instances for use by the UI.
 ** A *Proc-Layer command* is a functor, which can be parametrised with concrete arguments.
 ** Typically, these arguments are to be picked up from the actual usage context in the GUI.
 ** This creates the specific twist that possible command instances for invocation can and will
 ** be formed during an extended time period, non-deterministically -- since the actual context
 ** depends on the user interactions. Within the UI, there is a dedicated mechanism to form such
 ** command invocations similar to forming sentences of a language (with subject, predication and
 ** possibly some further objects). The UI manages several InteractionStateManager instances to
 ** observe and pick up contextual state, finally leading to a complete parametrisation of a command.
 ** The CommandInstanceManager is a service to support this process; it prepares command instances
 ** and provides dedicated instance IDs, which can be stored in the UI and later used to retrieve
 ** those instances for invocation. These IDs are created by decorating a base command ID, allowing
 ** for several competing invocations to exist at the same time. When finally a given invocation is
 ** about to happen, a corresponding registration handle is transfered to the ProcDispatcher, where
 ** it is enqueued for execution.  
 ** 
 ** @see command-setup.cpp service implementation
 ** @see command.hpp
 ** @see command-def.hpp
 ** @see command-setup.hpp
 ** @see command-accessor.hpp
 ** @see CommandInstanceManager_test
 **
 */



#ifndef CONTROL_COMMAND_INSTANCE_MANAGER_H
#define CONTROL_COMMAND_INSTANCE_MANAGER_H

#include "lib/error.hpp"
#include "proc/control/command-dispatch.hpp"
#include "lib/symbol.hpp"
//#include "proc/common.hpp"

#include <boost/noncopyable.hpp>
#include <string>




namespace proc {
namespace control {
  
  using std::string;
  using lib::Symbol;
  //using std::shared_ptr;
  
  
  
  /**
   * @todo write type comment
   */
  class CommandInstanceManager
    : boost::noncopyable
    {
      CommandDispatch& dispatcher_;
      
    public:
      CommandInstanceManager (CommandDispatch&);
     ~CommandInstanceManager();
      
      Symbol newInstance (Symbol prototypeID, string invocationID);
      void dispatch (Symbol instanceID);
      
      bool contains (Symbol instanceID)  const;
      
    private:
    };
  
  
  
  
  /** */
  
  
  
  
}} // namespace proc::control
#endif /*CONTROL_COMMAND_INSTANCE_MANAGER_H*/
