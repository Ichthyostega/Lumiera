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
 ** A *Steam-Layer command* is a functor, which can be parametrised with concrete arguments.
 ** Typically, these arguments are to be picked up from the actual usage context in the GUI.
 ** This creates the specific twist that possible command instances for invocation can and will
 ** be formed during an extended time period, non-deterministically -- since the actual context
 ** depends on the user interactions. Within the UI, there is a dedicated mechanism to form such
 ** command invocations similar to forming sentences of a language (with subject, predication and
 ** possibly some further objects). The UI manages several InteractionState instances to observe
 ** and pick up contextual state, finally leading to a complete parametrisation of a command.
 ** The CommandInstanceManager is a service to support this process; it prepares command instances
 ** and provides dedicated instance IDs, which can be stored in the UI and later used to retrieve
 ** those instances for invocation. These IDs are created by decorating a base command ID, allowing
 ** for several competing invocations to exist at the same time. When finally a given invocation is
 ** about to happen, a corresponding registration handle is transfered to the ProcDispatcher, where
 ** it is enqueued for execution.
 ** \par lifecycle
 ** There CommandInstanceManager is maintained by the SessionCommandService, which in turn is
 ** installed and removed by the implementation within ProcDispatcher. Its lifecycle is thus tied
 ** to the opening / closing of the Steam-Layer interface, as dictated by the Session lifecycle.
 ** When the current session is closed, all command instances "underway" will thus be discarded.
 ** 
 ** @see command-setup.cpp service implementation
 ** @see command.hpp
 ** @see command-def.hpp
 ** @see command-setup.hpp
 ** @see session-command-service.hpp
 ** @see CommandInstanceManager_test
 **
 */



#ifndef CONTROL_COMMAND_INSTANCE_MANAGER_H
#define CONTROL_COMMAND_INSTANCE_MANAGER_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/control/command.hpp"
#include "steam/control/command-dispatch.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/symbol.hpp"

#include <unordered_map>
#include <string>



namespace steam {
namespace control {
  
  using std::string;
  using lib::Symbol;
  using lib::diff::Rec;
  
  
  /**
   * Maintain a _current command instance_ for parametrisation.
   * The definition of a *Proc-Layer command* is used like a prototype.
   * For invocation, an anonymous clone copy is created from the definition
   * by calling #newInstance. Several competing usages of the same command can be
   * kept apart with the help of the `invocationID`, which is used to decorate the basic
   * command-ID to form a distinct _`instanceID`_. After #newInstance has "opened" an instance
   * this way and returned the instanceID, the actual \ref Command handle can be retrieved with
   * #getInstance. It represents an _anonymous instance_ kept alive solely by the CommandInstanceManager
   * (i.e. there is no registration of a command under that instanceID in the global CommandRegistry).
   * When done with the parametrisation, by calling #dispatch, this anonymous instance will be handed
   * over to the [Dispatcher](\ref CommandDispatch) (installed on construction). Typically, this will in fact
   * be the steam::control::ProcDispatcher, which runs in a dedicated thread ("session loop thread") and
   * maintains a queue of commands to be dispatched towards the current session. Since Command is a smart
   * handle, the enqueued instance will stay alive until execution and then go out of scope. But, after
   * #dispatch, it is no longer accessible from the CommandInstanceManger, and while it is still waiting
   * in the execution queue, the next instance for the same invocationID might already be opened.
   * @warning CommandInstanceManager is *not threadsafe*
   */
  class CommandInstanceManager
    : util::NonCopyable
    {
      CommandDispatch& dispatcher_;
      std::unordered_map<Symbol,Command> table_;
      
    public:
      CommandInstanceManager (CommandDispatch&);
     ~CommandInstanceManager();
      
      Symbol newInstance (Symbol prototypeID, string const& invocationID);
      Command getInstance(Symbol instanceID);
      void dispatch (Symbol instanceID);
      void bindAndDispatch (Symbol instanceID, Rec const& argSeq);
      
      bool contains (Symbol instanceID)  const;
      
    private:
      Command getCloneOrInstance (Symbol, bool);
      void handOver (Command&&);
    };
  
  
  
}} // namespace steam::control
#endif /*CONTROL_COMMAND_INSTANCE_MANAGER_H*/
