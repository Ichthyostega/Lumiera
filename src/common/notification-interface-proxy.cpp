/*
  Notification(Proxy)  -  public service to push information into the GUI

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/



/** @file notification-interface-proxy.cpp
 ** Interface-Proxy for the stage::GuiNotification facade.
 ** The purpose is to define a proxy implementation of stage::GuiNotification, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_GuiNotification"
 ** 
 ** @see gui-notification-facade.hpp
 ** @see notification-service.hpp actual implementation within the GUI
 */


#include "include/gui-notification-facade.h"
#include "include/interfaceproxy.hpp"

namespace stage {
  /** static storage for the facade access front-end */
  lib::Depend<GuiNotification> GuiNotification::facade;
}
namespace lumiera {
namespace facade {
  using stage::ID;
  using lib::diff::GenNode;
  using lib::diff::MutationMessage;
  
  
  
  /* ==================== SessionCommand =================================== */
  
  using Interface = LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 0);
  using Facade    = stage::GuiNotification;
  
  using IHandle   = InstanceHandle<Interface, Facade>;
  
  
  template<>
  class Proxy<IHandle>
    : public Binding<IHandle>
    {
      using Level = stage::NotifyLevel;
      
      //----Proxy-Implementation-of-GuiNotification--------
      
      void displayInfo (Level level, string const& text) override  { _i_.displayInfo (level, cStr(text));   }
      void markError (ID uiElement,  string const& text) override  { _i_.markError(&uiElement, cStr(text)); }
      void markNote  (ID uiElement,  string const& text) override  { _i_.markNote (&uiElement, cStr(text)); }
      void mark      (ID uiElement, GenNode&& stateMark) override  { _i_.mark     (&uiElement, &stateMark); }
      void mutate (ID uiElement, MutationMessage&& diff) override  { _i_.mutate   (&uiElement, &diff);      }
      void triggerGuiShutdown (string const& cause)      override  { _i_.triggerGuiShutdown (cStr(cause));  }
      
      
    public:
      using Binding<IHandle>::Binding;
    };
  
  
  /**  emit proxy code here... */
  template
  class Link<Interface,Facade>;
  
}} //namespace facade::lumiera
