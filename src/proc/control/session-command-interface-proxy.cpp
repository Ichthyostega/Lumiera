/*
  SessionCommand(Proxy)  -  public service to invoke commands on the session

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



/** @file session-command-interface-proxy.cpp
 ** Interface-Proxy for the proc::control::SessionCommand facade.
 ** The purpose is to define a proxy implementation of proc::control::SessionCommand, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_SessionCommand"
 ** 
 ** @see session-command-facade.hpp
 ** @see session-command-service.hpp actual implementation within the GUI
 */


#include "include/session-command-facade.h"
#include "include/interfaceproxy.hpp"

namespace proc {
namespace control{
  /** static storage for the facade access front-end */
  lib::Depend<SessionCommand> SessionCommand::facade;
}}
namespace lumiera {
namespace facade {
  
  
  
  /* ==================== SessionCommand =================================== */
  
  using Interface = LUMIERA_INTERFACE_INAME(lumieraorg_SessionCommand, 0);
  using Facade    = proc::control::SessionCommand;
  
  using IHandle   = InstanceHandle<Interface, Facade>;
  
  
  template<>
  class Proxy<IHandle>
    : public Binding<IHandle>
    {
      //----Proxy-Implementation-of-SessionCommand--------
      
      Symbol cycle (Symbol cmdID, string const& invocID)      override { return _i_.cycle (cmdID, cStr(invocID));}
      void trigger (Symbol cmdID, lib::diff::Rec const& args) override { _i_.trigger (cmdID, &args); }
      void bindArg (Symbol cmdID, lib::diff::Rec const& args) override { _i_.bindArg (cmdID, &args); }
      void invoke  (Symbol cmdID)                             override { _i_.invoke  (cmdID);        }
      
      
    public:
      using Binding<IHandle>::Binding;
    };
  
  
  /**  emit proxy code here... */
  template
  class Link<Interface,Facade>;
  
}} //namespace facade::lumiera
