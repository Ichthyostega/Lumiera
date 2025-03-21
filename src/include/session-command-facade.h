/*
  SESSION-COMMAND-FACADE.h  -  access point for invoking commands on the session

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file session-command-facade.h
 ** Major public Interface to the Session subsystem of Lumiera GUI.
 ** This interface describes the ability of the Session to trigger the execution
 ** of pre-defined commands, outfitted with suitable arguments and parameters.
 ** Triggering of these commands typically happens in response to some messages
 ** being sent over the UI-Bus. Likewise, external entities (e.g. plug-ins) may
 ** invoke commands over this interface to alter the session.
 ** 
 ** For this reason, the operations exposed here are defined in terms matching
 ** the structure of binding and invocation messages. This goes so far as to
 ** accept the command arguments for binding packaged as `Record<GenNode>`.
 ** For each command, there needs to be a registration record within the
 ** Steam-Layer implementation. The service implementation backing this
 ** facade indeed retrieves the corresponding steam::control::Command
 ** handles to perform the binding operation and hands them over
 ** to the SteamDispatcher for invocation.
 ** 
 ** @see command.hpp
 ** @see session-command-service.hpp implementation
 ** @see steam::control::SteamDispatcher
 ** @see stage::ctrl::CoreService
 */


#ifndef STEAM_CONTROL_SESSION_COMMAND_H
#define STEAM_CONTROL_SESSION_COMMAND_H



#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "lib/depend.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/symbol.hpp"

#include <string>


namespace steam {
namespace control {
  
  using lib::Symbol;
  using std::string;
  
  
  /**********************************************************************//**
   * Global access point to invoke commands and cause edit operations within
   * the Session. Any changes to the session are effected by prepared functors
   * bound into a steam::control::Command registration. A command instance will
   * first be outfitted with suitable parameters to define the target and to
   * qualify and elaborate the action, before it can be actually triggered.
   * Commands then go through a queue to be invoked one by one.
   * 
   * The service exposed through this facade offers dedicated support for
   * the _standard command cycle_, as is typically performed from the UI.
   * Such a usage cycle starts with ["opening"](\ref #cycle) a local anonymous
   * clone copy from the global command definition, which is then used
   * in further calls to be outfitted with actual arguments and finally
   * to be handed over to the dispatcher for execution.
   * @warning this standard command cycle is intended for single-threaded
   *          use from the UI. It is not threadsafe. To the contrary, all
   *          operations with globally registered commands are threadsafe.
   * 
   * @remark This is a layer separation facade interface. Clients should
   * use the embedded #facade factory, which yields a proxy to route any
   * calls through the lumieraorg_SessionCommand interface
   * @throws lumiera::error::State when interface is not opened
   * @todo as of 2/2021 only the #trigger operation is used; if no case for
   *       an elaborated command cycle with binding step and deferred invocation
   *       shows up, we may consider dropping the other operations
   * @see [Command system](\ref command.hpp)
   * @see SessionCommandFunction_test
   */
  class SessionCommand
    {
    public:
      static lib::Depend<SessionCommand> facade;
      
      /** start next command cycle and "open" a new anonymous command instance */
      virtual Symbol cycle (Symbol cmdID, string const& invocationID) =0;
      
      /** bind the command's arguments and trigger command invocation immediately */
      virtual void trigger (Symbol cmdID, lib::diff::Rec const& args) =0;
      
      /** prepare command invocation: bind the command's arguments */
      virtual void bindArg (Symbol cmdID, lib::diff::Rec const& args) =0;
      
      /** trigger invocation of a prepared command */
      virtual void invoke (Symbol cmdID)                              =0;
      
      
    protected:
      virtual ~SessionCommand() {} ///< this is an interface
      friend class lib::DependencyFactory<SessionCommand>;
    };
    
  
  
}} // namespace steam::control


extern "C" {
#endif /* =========================== CL Interface ===================== */ 

  
#include "common/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_SessionCommand, 0,
                           LUMIERA_INTERFACE_SLOT (const char*, cycle, (const char*, const char*)),
                           LUMIERA_INTERFACE_SLOT (void,      trigger, (const char*, const void*)),
                           LUMIERA_INTERFACE_SLOT (void,      bindArg, (const char*, const void*)),
                           LUMIERA_INTERFACE_SLOT (void,       invoke, (const char*)),
);


#ifdef __cplusplus
}// extern "C"
#endif
#endif /*STEAM_CONTROL_SESSION_COMMAND_H*/
