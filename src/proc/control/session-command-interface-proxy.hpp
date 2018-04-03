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



/** @file session-command-interface-proxy.hpp
 ** This is an implementation fragment, intended to be included into common/interfaceproxy.cpp
 ** 
 ** The purpose is to define a proxy implementation of proc::control::SessionCommand, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_SessionCommand"
 ** 
 ** @see session-command-facade.hpp
 ** @see session-command-service.hpp actual implementation within the GUI
 */





/* ==================== SessionCommand =================================== */

#include "include/session-command-facade.h"
#include "include/interfaceproxy.hpp"

namespace lumiera {
namespace facade {
  
  typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_SessionCommand, 0)
                        , proc::control::SessionCommand
                        > IHandle_SessionCommand;
  
  
  template<>
  class Proxy<IHandle_SessionCommand>
    : public Binding<IHandle_SessionCommand>
    {
      //----Proxy-Implementation-of-SessionCommand--------
      
      Symbol cycle (Symbol cmdID, string const& invocID)      override { return _i_.cycle (cmdID, cStr(invocID));}
      void trigger (Symbol cmdID, lib::diff::Rec const& args) override { _i_.trigger (cmdID, &args); }
      void bindArg (Symbol cmdID, lib::diff::Rec const& args) override { _i_.bindArg (cmdID, &args); }
      void invoke  (Symbol cmdID)                             override { _i_.invoke  (cmdID);        }
      
      
    public:
      using IBinding::IBinding;
    };
  
  
//  template  void openProxy<IHandle_SessionCommand>  (IHandle_SessionCommand const&);
//  template  void closeProxy<IHandle_SessionCommand> (void);
  
  
}} // namespace lumiera::facade
