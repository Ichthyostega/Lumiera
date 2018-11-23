/*
  SessionCommandService  -  public service to invoke commands on the session

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

* *****************************************************/

/** @file session-command-service.cpp
 ** Implementation of command invocation on the Session interface.
 ** This is the actual service implementation and runs within Session subsystem.
 ** It is managed by the DispatcherLoop, which also starts the Session Loop Thread.
 ** Moreover, for actually perform any command, the command operation needs to be
 ** defined somewhere statically, as a function operating on the _current session,_
 ** plus an _state capturing_ and _UNDO_ function. And the concrete arguments of
 ** the command functor must be suitably bound.
 ** 
 */

#include "steam/control/session-command-service.hpp"
#include "steam/control/command.hpp"
#include "include/logging.h"
#include "lib/depend.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

extern "C" {
#include "common/interface-descriptor.h"
}

#include <string>




namespace steam {
namespace control {
  
  using lib::Symbol;
  using std::string;
  using util::cStr;
  
   
  Symbol
  SessionCommandService::cycle (Symbol cmdID, string const& invocationID)
  {
    return instanceManager_.newInstance(cmdID, invocationID);
  }
  
  
  /**
   * @param cmdID either the instanceID or the global cmdID
   * @throw error::Invalid when no suitable command definition exists
   * @throw error::State when the command's arguments are not suitably bound
   * @throw error::Logic when the instance is already dispatched
   * @note this function automatically _falls back_ on a global
   *       Command definition, in case the given ID is not known
   *       as a local command instance. This allows to use the
   *       SessionCommand service without explicit instantiation
   */
  void
  SessionCommandService::trigger (Symbol cmdID, Rec const& argSeq)
  {
    instanceManager_.getInstance(cmdID)
                    .bindArg(argSeq);
    instanceManager_.dispatch(cmdID);
  }
  
  
  /**
   * @param cmdID either the instanceID or the global cmdID
   * @throw error::Invalid when no suitable command definition exists
   * @throw error::Logic when the instance is already dispatched
   * @note this function automatically _falls back_ on a global
   *       Command definition, in case the given ID is not known
   *       as a local command instance. This allows to use the
   *       SessionCommand service without explicit instantiation
   */
  void 
  SessionCommandService::bindArg (Symbol cmdID, Rec const& argSeq)
  {
    instanceManager_.getInstance(cmdID)
                    .bindArg(argSeq);
  }
  
  
  /**
   * @param cmdID either the instanceID or the global cmdID
   * @throw error::Invalid when no suitable command definition exists
   * @throw error::State when the command's arguments are not bound
   * @throw error::Logic when the instance is already dispatched
   * @note similar to #bindArg, this function _falls back_ on a global
   *       Command definition, in case the given ID is not known locally.
   */
  void
  SessionCommandService::invoke (Symbol cmdID)
  {
    instanceManager_.dispatch(cmdID);
  }
  
  
  namespace { // facade implementation details
    
    
    /* ================== define an lumieraorg_GuiNotification instance ======================= */
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_interfacedescriptor, 0
                               ,lumieraorg_SessionCommandFacade_descriptor
                               , NULL, NULL, NULL
                               , LUMIERA_INTERFACE_INLINE (name,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "SessionCommand"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (brief,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Session Interface: invoke pre-defined commands to operate on the session"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (homepage,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "http://www.lumiera.org/develompent.html" ;}
                                                          )
                               , LUMIERA_INTERFACE_INLINE (version,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "0.3~pre"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (author,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Hermann Vosseler"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (email,
                                                           const char*, (LumieraInterface ifa),
                                                             { (void)ifa;  return "Ichthyostega@web.de"; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (copyright,
                                                           const char*, (LumieraInterface ifa),
                                                             {
                                                               (void)ifa;
                                                               return
                                                                 "Copyright (C)        Lumiera.org\n"
                                                                 "  2016               Hermann Vosseler <Ichthyostega@web.de>";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (license,
                                                           const char*, (LumieraInterface ifa),
                                                             {
                                                               (void)ifa;
                                                               return
                                                                 "This program is free software; you can redistribute it and/or modify\n"
                                                                 "it under the terms of the GNU General Public License as published by\n"
                                                                 "the Free Software Foundation; either version 2 of the License, or\n"
                                                                 "(at your option) any later version.\n"
                                                                 "\n"
                                                                 "This program is distributed in the hope that it will be useful,\n"
                                                                 "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                                                                 "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                                                 "GNU General Public License for more details.\n"
                                                                 "\n"
                                                                 "You should have received a copy of the GNU General Public License\n"
                                                                 "along with this program; if not, write to the Free Software\n"
                                                                 "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA";
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (state,
                                                           int, (LumieraInterface ifa),
                                                             {(void)ifa;  return LUMIERA_INTERFACE_EXPERIMENTAL; }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (versioncmp,
                                                           int, (const char* a, const char* b),
                                                             {(void)a;(void)b;  return 0;}  ////////////////////////////////////////////TODO define version ordering
                                                          )
                               );
    
    
    
    
    
    using lumiera::error::LERR_(LIFECYCLE);
    
    lib::Depend<SessionCommandService> _instance; ///< a backdoor for the C Language impl to access the actual SessionCommand implementation...
    
    
    
    LUMIERA_INTERFACE_INSTANCE (lumieraorg_SessionCommand, 0
                               ,lumieraorg_SessionCommandService
                               , LUMIERA_INTERFACE_REF(lumieraorg_interfacedescriptor, 0, lumieraorg_SessionCommandFacade_descriptor)
                               , NULL /* on  open  */
                               , NULL /* on  close */
                               , LUMIERA_INTERFACE_INLINE (cycle,
                                                           const char*, (const char* cmdID, const char* invocationID),
                                                             {
                                                               if (!_instance) 
                                                                 return lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, cmdID);
                                                               else
                                                                 return _instance().cycle(cmdID, invocationID);
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (trigger,
                                                           void, (const char* cmdID, const void* args),
                                                             {
                                                               if (!_instance) lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, cmdID);
                                                               else
                                                                 _instance().trigger(cmdID, *static_cast<Rec const *> (args));
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (bindArg,
                                                           void, (const char* cmdID, const void* args),
                                                             {
                                                               if (!_instance) lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, cmdID);
                                                               else
                                                                 _instance().bindArg(cmdID, *static_cast<Rec const *> (args));
                                                             }
                                                          )
                               , LUMIERA_INTERFACE_INLINE (invoke,
                                                           void, (const char* cmdID),
                                                             {
                                                               if (!_instance) lumiera_error_set (LUMIERA_ERROR_LIFECYCLE, cmdID);
                                                               else
                                                                 _instance().invoke(cmdID);
                                                             }
                                                          )
                               );
    
    
    
  } // (END) facade implementation details
  
  
  
  /** @remark actual instance created from within SteamDispatcher */
  SessionCommandService::SessionCommandService (CommandDispatch& dispatcherLoopInterface)
    : dispatcher_{dispatcherLoopInterface}
    , instanceManager_{dispatcher_}
    , serviceInstance_{ LUMIERA_INTERFACE_REF (lumieraorg_SessionCommand, 0, lumieraorg_SessionCommandService)}
    {
      INFO (stage, "SessionCommand Facade opened.");
    }
  
  
  
  
}} // namespace steam::control
