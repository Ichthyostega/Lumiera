/*
  SESSION-COMMAND-FACADE.h  -  access point for invoking commands on the session

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

/** @file session-command-facade.h
 ** Major public Interface to the Session subsystem of Lumiera GUI.
 ** This interface describes the ability of the Session to trigger the execution
 ** of pre-defined commands, outfitted with suitably arguments and parameters.
 ** Triggering of these commands typically happens in response of some messages
 ** being sent over the UI-Bus. Likewise, external entities (e.g. plug-ins) may
 ** invoke commands over this interface to alter the session.
 **
 ** @see notification-service.hpp implementation
 ** @see gui::GuiFacade
 ** @see main.cpp
 */


#ifndef PROC_CONTROL_SESSION_COMMAND_H
#define PROC_CONTROL_SESSION_COMMAND_H



#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "include/interfaceproxy.hpp"

#include <string>


namespace proc {
namespace control {
  
  using std::string;
  
  
  /*****************************************************************//**
   * Global access point to push state update and notification of events
   * from the lower layers into the Lumiera GUI. Typically, this happens
   * asynchronously and triggered by events within the lower layers.
   * 
   * This is a layer separation facade interface. Clients should use
   * the embedded #facade factory, which yields a proxy routing any 
   * calls through the lumieraorg_GuiNotification interface
   * @throws lumiera::error::State when interface is not opened
   */
  class SessionCommand
    {
    public:
      static lumiera::facade::Accessor<SessionCommand> facade;
      
      /** @todo dummy placeholder, actual operation to be defined soon (12/16) */
      virtual void bla_TODO (string const& text)      =0;
      
      /** @todo dummy placeholder, actual operation to be defined soon (12/16) */
      virtual void blubb_TODO (string const& cause)   =0;
      
      
    protected:
      virtual ~SessionCommand() {}
    };
    
  
  
}} // namespace proc::control


extern "C" {
#endif /* =========================== CL Interface ===================== */ 

  
#include "common/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_SessionCommand, 0,
                           LUMIERA_INTERFACE_SLOT (void, bla_TODO,    (const char*)),
                           LUMIERA_INTERFACE_SLOT (void, blubb_TODO,  (const char*)),
);


#ifdef __cplusplus
}
#endif
#endif /*PROC_CONTROL_SESSION_COMMAND_H*/
