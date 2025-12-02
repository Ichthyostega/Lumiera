/*
  NOTIFICATION-SERVICE.hpp  -  public service allowing to push information into the GUI

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file notification-service.hpp
 ** A public service provided by the GUI, implementing the stage::GuiNotification facade interface.
 ** The purpose of this service is to push state update and notification of events from the lower
 ** layers into the Lumiera GUI. Typically, this happens asynchronously and triggered either by
 ** events within the lower layers, or as result of invoking commands on the session.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** stage::GuiNotification#facade to access this service. This header here defines the interface
 ** used to _provide_ this service, not to access it.
 ** 
 ** @see stage::GuiFacade launching the Lumiera UI
 ** @see facade.hpp RAII holder to start this service and open the interface
 ** @see stage::ctrl::UiManager::performMainLoop() exposes all UI façade interfaces
 */


#ifndef STAGE_NOTIFICATION_SERVICE_H
#define STAGE_NOTIFICATION_SERVICE_H


#include "include/gui-notification-facade.h"
#include "common/instancehandle.hpp"
#include "stage/ctrl/bus-term.hpp"

#include <memory>



namespace lib {
  namespace diff { class GenNode; }
}
namespace stage {
  
  namespace ctrl {
    class UiManager;
    class UiDispatcher;
  }
  
  
  
  /**************************************************//**
   * Actual implementation of the GuiNotification service
   * within the Lumiera GTK GUI. Creating an instance of
   * this class automatically registers the interface
   * with the Lumiera Interface/Plugin system and creates
   * a forwarding proxy within the application core to
   * route calls through this interface.
   * 
   * @note the ctor of this class establishes an "up-link"
   *       connection to the [UI-Bus](\ref ui-bus.hpp), which
   *       enables the service implementation to talk to
   *       other facilities within the UI.
   */
  class NotificationService
    : public GuiNotification
    , public ctrl::BusTerm
    {
      
      std::unique_ptr<ctrl::UiDispatcher> dispatch_;
      ctrl::UiManager& uiManager_;
      
      void dispatchMsg (ID, lib::diff::GenNode&&);
      
      
      /* === Interface Lifecycle === */
      
      using ServiceInstanceHandle = lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 0)
                                                           , GuiNotification
                                                           > ;
      ServiceInstanceHandle serviceInstance_;
      
      
    public:
     ~NotificationService();
      NotificationService (ctrl::BusTerm& upLink, ctrl::UiManager& uiManager);
      
      /* === Implementation of the Facade Interface === */
      
      void displayInfo (NotifyLevel,string const& text) override;
      void markError (ID uiElement, string const& text) override;
      void markNote  (ID uiElement, string const& text) override;
      void mark      (ID uiElement, GenNode&&)          override;
      void mutate (ID uiElement, MutationMessage&&)     override;
      void triggerGuiShutdown (string const& cause)     override;
    };
  
  
  
} // namespace stage
#endif
