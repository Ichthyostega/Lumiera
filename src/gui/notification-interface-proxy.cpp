/*
  Notification(Proxy)  -  public service to push information into the GUI

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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



/** @file notification-interface-proxy.cpp
 ** Interface-Proxy for the gui::GuiNotification facade.
 ** The purpose is to define a proxy implementation of gui::GuiNotification, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_GuiNotification"
 ** 
 ** @see gui-notification-facade.hpp
 ** @see notification-service.hpp actual implementation within the GUI
 */





/* ==================== GuiNotification =================================== */
    
#include "include/gui-notification-facade.h"
#include "include/interfaceproxy.hpp"

namespace lumiera {
namespace facade {
  using gui::ID;
  using lib::diff::MutationMessage;
  
  typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 0)
                        , gui::GuiNotification
                        > IHandle_GuiNotification;
  
  
  template
  class Proxy<IHandle_GuiNotification>
    : public Binding<IHandle_GuiNotification>
    {
      using Level = gui::NotifyLevel;
      
      //----Proxy-Implementation-of-GuiNotification--------
      
      void displayInfo (Level level, string const& text) override  { _i_.displayInfo (level, cStr(text));                  }
      void markError (ID uiElement,  string const& text) override  { _i_.markError(uiElement.getHash().get(), cStr(text)); }
      void markNote  (ID uiElement,  string const& text) override  { _i_.markNote (uiElement.getHash().get(), cStr(text)); }
      void mutate (ID uiElement, MutationMessage&& diff) override  { _i_.mutate   (uiElement.getHash().get(), &diff);      }
      void triggerGuiShutdown (string const& cause)      override  { _i_.triggerGuiShutdown (cStr(cause));                 }
      
      
    public:
      using IBinding::IBinding;
    };
  
  
//  template  void openProxy<IHandle_GuiNotification>  (IHandle_GuiNotification const&);
//  template  void closeProxy<IHandle_GuiNotification> (void);
  
  
}} // namespace lumiera::facade
