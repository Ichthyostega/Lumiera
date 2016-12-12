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

namespace proc {
namespace control {
  
  /** storage for the facade proxy factory
   *  used by client code to invoke through the interface */
  lumiera::facade::Accessor<SessionCommand> SessionCommand::facade;

}} // namespace proc::control



namespace lumiera {
namespace facade {
  
  typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_SessionCommand, 0)
                        , proc::control::SessionCommand
                        > IHandle_SessionCommand;
  
  
  template<>
  class Proxy<IHandle_SessionCommand>
    : public Holder<IHandle_SessionCommand>
    {
      //----Proxy-Implementation-of-SessionCommand--------
      
      void bla_TODO (string const& text)    override  { _i_.bla_TODO (cStr(text)); }
      void blubb_TODO (string const& cause) override  { _i_.blubb_TODO (cStr(cause)); }
      
      
    public:
      Proxy (IHandle const& iha) : THolder(iha) {}
    };
  
  
  template  void openProxy<IHandle_SessionCommand>  (IHandle_SessionCommand const&);
  template  void closeProxy<IHandle_SessionCommand> (void);
  
  
}} // namespace lumiera::facade
